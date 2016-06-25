#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <QString>
#include <QFile>
#include <QTextStream>

#define FILE_NETASCII 0
#define FILE_OCTET    1

class FileHandler
{
    QString filename;
    QFile file;
    quint64 position;
    quint16 blockNumber;
    quint16 blockSize;
    QString mode;

public:
    FileHandler();

    QString getFilename() const;
    void setFilename(const QString &value);
    quint16 getBlockNumber() const;
    void setBlockNumber(const quint16 &value);
    quint16 getBlockSize() const;
    void setBlockSize(const quint16 &value);
    QString getMode() const;
    void setMode(const QString &value);

    QByteArray readBlock();

    void writeBlock(QByteArray block);
    bool eofReached();
    void closeFile();
};

#endif // FILEHANDLER_H
