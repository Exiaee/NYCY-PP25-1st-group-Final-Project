#include "imagephash.h"
#include <QVector>
#include <QDebug>
#include <bitset>
#include <QtMath>
#include <QImage>
#include <QtMath>
#include <algorithm>
#include <QFile>
#include <QCoreApplication>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <QProcess>


QString ImagePHash::computeHashCV(const QString &filePath) {
    QString pythonExe = "C:/Users/USER/anaconda3/python.exe";
    //process.start(pythonExe, QStringList() << pythonScript << filePath);

    QString pythonScript = QCoreApplication::applicationDirPath() + "/compute_phash.py";
    qDebug()<< pythonScript;
    QProcess process;
    //process.start("python", QStringList() << pythonScript <<" " <<filePath);
    process.start(pythonExe, QStringList() << pythonScript << filePath);
    process.waitForFinished();

    QString result = process.readAllStandardOutput().trimmed();
    if (result.startsWith("error")) {
        qWarning() << "[pHash Python Error]" << result;
        return "";
    }
    qDebug()<<result;
    return result;
}


/*QString ImagePHash::computeHashCV(const QString &filePath) {
    // 載入圖片並轉為灰階 32x32
    QImage img(filePath);
    if (img.isNull()) {
        qWarning() << "[ImagePHash] Failed to load image:" << filePath;
        return "";
    }
    QImage gray = img.convertToFormat(QImage::Format_Grayscale8);
    QImage scaled = gray.scaled(32, 32, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    // 將像素轉為 double
    double pixels[32][32];
    for (int y = 0; y < 32; ++y)
        for (int x = 0; x < 32; ++x)
            pixels[y][x] = qGray(scaled.pixel(x, y));

    // DCT
    double dct[32][32];
    dct2D(pixels, dct);

    // 取左上 8x8（排除 DC）
    QVector<double> dctVals;
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            if (x != 0 || y != 0)
                dctVals << dct[y][x];

    // 計算中位數
    std::sort(dctVals.begin(), dctVals.end());
    double median = std::accumulate(dctVals.begin(), dctVals.end(), 0.0) / dctVals.size();
    qDebug()<<median;
    // 轉為 hash
    QString hash;
    for (double v : dctVals)
        hash += (v > median ? '1' : '0');

    return hash;
}*/

void ImagePHash::dct2D(double in[32][32], double out[32][32]) {
    const int N = 32;
    double c = sqrt(2.0 / N);
    for (int u = 0; u < N; ++u) {
        for (int v = 0; v < N; ++v) {
            double sum = 0.0;
            for (int x = 0; x < N; ++x) {
                for (int y = 0; y < N; ++y) {
                    sum += in[x][y] *
                           qCos((M_PI * (2 * x + 1) * u) / (2.0 * N)) *
                           qCos((M_PI * (2 * y + 1) * v) / (2.0 * N));
                }
            }
            double cu = (u == 0) ? qSqrt(0.5) : 1.0;
            double cv = (v == 0) ? qSqrt(0.5) : 1.0;
            out[u][v] = c * cu * cv * sum;
        }
    }
}


int ImagePHash::hammingDistance(const QString &hex1, const QString &hex2) {

    /*if (hex1.length() != 16 || hex2.length() != 16)
        return -1;
    qDebug()<<hex1<<" "<<hex2;
    bool ok1, ok2;
    quint64 h1 = hex1.toULongLong(&ok1, 16);
    quint64 h2 = hex2.toULongLong(&ok2, 16);
    if (!ok1 || !ok2)
        return -1;

    std::bitset<64> b(h1 ^ h2);  // XOR 然後計算 1 的數量
    return b.count();*/
    if (hex1.length() != hex2.length()) return -1;
    int dist = 0;
    for (int i = 0; i < hex1.length(); ++i) {
        if (hex1[i] != hex2[i]) ++dist;
    }
    return dist;

}


