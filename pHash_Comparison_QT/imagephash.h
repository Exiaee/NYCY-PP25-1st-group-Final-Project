#ifndef IMAGEPHASH_H
#define IMAGEPHASH_H

#include <QString>
#include <QImage>
#include <QVector>
#include <bitset>
#include <QtMath>
#include <algorithm>


class ImagePHash {
public:
    static QString computeHashCV(const QString &filePath);
    static int hammingDistance(const QString &hash1, const QString &hash2);

private:
    static void dct2D(double in[32][32], double out[32][32]);
};

#endif // IMAGEPHASH_H
