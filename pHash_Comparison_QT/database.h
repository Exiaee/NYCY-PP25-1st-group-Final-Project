#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QPixmap>
#include <QStringList>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>

class Database {
public:
    static bool openFromFile(const QString &dbFilePath);
    static void getImageHash(const QString &name);
    static bool insertImage(const QString &name, const QString &filePath);
    static QPixmap loadImage(const QString &name);
    static QStringList listAllImages(const QString &like = "");
    static bool deleteImage(const QString &name);
    static QStringList findSimilarImages(const QString &imagePath, int maxDistance = 10);
    static QStringList findSimilarImagesOpenMP(const QString &imagePath, int maxDistance = 10);
    static QStringList findSimilarImagesPthread(const QString &imagePath, int maxDistance = 10);
    static QStringList findSimilarImagesQT(const QString &imagePath);
    static QStringList findSimilarImagesQtOpenMP(const QString &imagePath);



private:
    static QSqlDatabase db;
};

#endif // DATABASE_H
