#include "database.h"
#include "imagephash.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QImage>
#include <QDebug>
#include <omp.h>
#include <pthread.h>

QSqlDatabase Database::db;
struct ThreadTask {
    int startIndex;
    int endIndex;
    QString queryHash;
    QVector<QPair<QString, QString>> dbImages;  // filename, phash
    int maxDistance;
    QStringList *results;
    pthread_mutex_t *lock;
};

bool Database::openFromFile(const QString &dbFilePath) {
    if (db.isOpen())
        db.close();
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbFilePath);
    if (!db.open()) {
        qWarning() << "Failed to open database:" << dbFilePath << db.lastError().text();
        return false;
    }
    QSqlQuery query;
    return query.exec("CREATE TABLE IF NOT EXISTS images ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "name TEXT UNIQUE, "
                      "data BLOB, "
                      "phash TEXT)");
}

void Database::getImageHash(const QString &name) {
    /*QSqlQuery query;
    query.prepare("SELECT phash FROM images WHERE name = :name");
    query.bindValue(":name", name);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return QString("N/A");*/
}


bool Database::insertImage(const QString &name, const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    QByteArray imageData = file.readAll();
    QImage img;
    img.loadFromData(imageData);
    //QString hash = ImagePHash::computeHash(img);

    QSqlQuery query;
    query.prepare("REPLACE INTO images (name, data, phash) VALUES (:name, :data, :phash)");
    query.bindValue(":name", name);
    query.bindValue(":data", imageData);
    //query.bindValue(":phash", hash);
    return query.exec();
}

QPixmap Database::loadImage(const QString &_filename) {
    QSqlQuery query;
    QString filename = _filename.section(" (", 0, 0).trimmed();
    query.prepare("SELECT data FROM images WHERE filename = :filename");
    query.bindValue(":filename", filename);
    if (!query.exec() || !query.next()) {
        qWarning() << "[loadImage] Failed to load image from images for:" << filename << query.lastError().text();
        return QPixmap();
    }

    QByteArray imageData = query.value(0).toByteArray();
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData)) {
        qWarning() << "[loadImage] Failed to decode image data for:" << filename;
        return QPixmap();
    }

    return pixmap;
}


QStringList Database::listAllImages(const QString &like) {
    QStringList names;
    QSqlQuery query;
    if (like.isEmpty()) {
        query.prepare("SELECT filename FROM images ORDER BY filename DESC");
    } else {
        query.prepare("SELECT filename FROM images WHERE filename LIKE :pattern ORDER BY id DESC");
        query.bindValue(":pattern", "%" + like + "%");
    }
    if (query.exec()) {
        while (query.next()) {
            names << query.value(0).toString();
        }
    }
    return names;
}

bool Database::deleteImage(const QString &name) {
    QSqlQuery query;
    query.prepare("DELETE FROM images WHERE name = :name");
    query.bindValue(":name", name);
    return query.exec();
}

QStringList Database::findSimilarImages(const QString &filePath, int maxDistance) {
    QStringList similar;
    QString queryHash = ImagePHash::computeHashCV(filePath);
    qDebug()<<queryHash;
    if (queryHash.isEmpty()) return similar;

    QSqlQuery query("SELECT filename, phash FROM images");
    while (query.next()) {
        QString dbFilename = query.value(0).toString();
        QString dbHash = query.value(1).toString();
        int dist = ImagePHash::hammingDistance(queryHash, dbHash);
        if (dist >= 0 && dist <= maxDistance) {
            similar << QString("%1 (dist: %2)").arg(dbFilename).arg(dist);
        }
    }
    return similar;
}

QStringList Database::findSimilarImagesOpenMP(const QString &filePath, int maxDistance) {
    QStringList similar;
    QString queryHash = ImagePHash::computeHashCV(filePath);
    if (queryHash.isEmpty()) return similar;

    struct Entry { QString filename; QString phash; };
    QVector<Entry> entries;

    // 先將資料拉出來（避免在 OpenMP 內使用 QSqlQuery）
    QSqlQuery query("SELECT filename, phash FROM images");
    while (query.next()) {
        entries.push_back({ query.value(0).toString(), query.value(1).toString() });
    }

    // OpenMP 平行區
    #pragma omp parallel
    {
        QStringList localSimilar;
        #pragma omp for nowait
        for (int i = 0; i < entries.size(); ++i) {
            const auto &e = entries[i];
            int dist = ImagePHash::hammingDistance(queryHash, e.phash);
            if (dist >= 0 && dist <= maxDistance) {
                QString result = QString("%1 (dist: %2)").arg(e.filename).arg(dist);
                localSimilar << result;
            }
        }

        // 合併 thread-local 結果
        #pragma omp critical
        similar.append(localSimilar);
    }

    return similar;
}

void*computeSimilarImages(void* arg) {
    ThreadTask *task = static_cast<ThreadTask*>(arg);

    for (int i = task->startIndex; i < task->endIndex; ++i) {
        const auto &pair = task->dbImages[i];
        const QString &filename = pair.first;
        const QString &phash = pair.second;

        int dist = ImagePHash::hammingDistance(task->queryHash, phash);
        if (dist >= 0 && dist <= task->maxDistance) {
            pthread_mutex_lock(task->lock);
            task->results->append(QString("%1 (dist: %2)").arg(filename).arg(dist));
            pthread_mutex_unlock(task->lock);
        }
    }
    return nullptr;
}


QStringList Database::findSimilarImagesPthread(const QString &filePath, int maxDistance) {
    QStringList result;
    QString queryHash = ImagePHash::computeHashCV(filePath);
    if (queryHash.isEmpty()) return result;

    QVector<QPair<QString, QString>> dbImages;

    QSqlQuery query("SELECT filename, phash FROM images");
    while (query.next()) {
        dbImages.append({query.value(0).toString(), query.value(1).toString()});
    }

    const int numThreads = 4;
    pthread_t threads[numThreads];
    ThreadTask tasks[numThreads];
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, nullptr);

    int chunk = dbImages.size() / numThreads;

    for (int i = 0; i < numThreads; ++i) {
        tasks[i].startIndex = i * chunk;
        tasks[i].endIndex = (i == numThreads - 1) ? dbImages.size() : (i + 1) * chunk;
        tasks[i].queryHash = queryHash;
        tasks[i].dbImages = dbImages;
        tasks[i].maxDistance = maxDistance;
        tasks[i].results = &result;
        tasks[i].lock = &lock;
        pthread_create(&threads[i], nullptr, computeSimilarImages, &tasks[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    pthread_mutex_destroy(&lock);
    return result;
}


QStringList Database::findSimilarImagesQT(const QString &filePath){
    QStringList similar;
    //QString queryHash = ImagePHash::computeHashCV(filePath);
    QImage imgFromFile(filePath);

    if (imgFromFile.isNull()) return similar;

    QSqlQuery query("SELECT filename, data FROM images");
    while (query.next()) {
        QString dbFilename = query.value(0).toString();
        QByteArray blob = query.value(1).toByteArray();
        QImage imgFromDB;
        imgFromDB.loadFromData(blob);
        if(imgFromDB.size()!=imgFromFile.size()) continue;
        QImage i1 = imgFromDB.convertToFormat(QImage::Format_ARGB32);
        QImage i2 = imgFromFile.convertToFormat(QImage::Format_ARGB32);

        //int dist = ImagePHash::hammingDistance(queryHash, dbHash);
        if(memcmp(i1.bits(),i2.bits(),i1.sizeInBytes())==0) {
            similar << QString("%1 (dist: 0)").arg(dbFilename);
        }
    }
    return similar;

}


QStringList Database::findSimilarImagesQtOpenMP(const QString &filePath) {
    QStringList similar;
    QImage imgFromFile(filePath);
    if (imgFromFile.isNull()) return similar;

    // 拉資料庫內容出來，避免 OpenMP 使用 QSqlQuery
    struct Entry { QString filename; QByteArray blob; };
    QVector<Entry> entries;

    QSqlQuery query("SELECT filename, data FROM images");
    while (query.next()) {
        entries.push_back({ query.value(0).toString(), query.value(1).toByteArray() });
    }

    QImage imgFileARGB = imgFromFile.convertToFormat(QImage::Format_ARGB32);
    const QSize fileSize = imgFromFile.size();

    QVector<QString> localResults;  // 最後回傳用
    #pragma omp parallel
    {
        QVector<QString> threadLocal;
    #pragma omp for nowait
        for (int i = 0; i < entries.size(); ++i) {
            const auto &e = entries[i];
            QImage imgFromDB;
            imgFromDB.loadFromData(e.blob);
            if (imgFromDB.size() != fileSize) continue;

            QImage i1 = imgFromDB.convertToFormat(QImage::Format_ARGB32);
            if (memcmp(i1.bits(), imgFileARGB.bits(), i1.sizeInBytes()) == 0) {
                threadLocal << QString("%1 (dist: 0)").arg(e.filename);
            }
        }
    #pragma omp critical
        localResults += threadLocal;
    }

    return QStringList::fromVector(localResults);
}
