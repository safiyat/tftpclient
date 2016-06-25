#include "filehandler.h"


QString FileHandler::getFilename() const
{
    return filename;
}

void FileHandler::setFilename(const QString &value)
{
    filename = value;
}

quint16 FileHandler::getBlockNumber() const
{
    return blockNumber;
}

void FileHandler::setBlockNumber(const quint16 &value)
{
    blockNumber = value;
}

quint16 FileHandler::getBlockSize() const
{
    return blockSize;
}

void FileHandler::setBlockSize(const quint16 &value)
{
    blockSize = value;
}

QString FileHandler::getMode() const
{
    return mode;
}

void FileHandler::setMode(const QString &value)
{
    mode = value;
}

QByteArray FileHandler::readBlock()
{
    if(!file.isOpen())
    {
        file.setFileName(filename);
        if(mode.toLower() == QString("netascii"))
            file.open(QIODevice::ReadOnly | QIODevice::Text);
        else
            file.open(QIODevice::ReadOnly);
    }
    qint64 pos = (blockNumber - 1) * blockSize;
    if(pos < 0)
        pos = 0;
    file.seek(pos);
    QByteArray block = file.read(blockSize);
    if(mode.toLower() == QString("netascii"))
        block.replace("\n", "\r\n");
    return block;
}

QByteArray FileHandler::readBlock(quint16 blockN)
{
    QFile f;
    f.setFileName(filename);
    if(mode.toLower() == QString("netascii"))
        f.open(QIODevice::ReadOnly | QIODevice::Text);
    else
        f.open(QIODevice::ReadOnly);

    qint64 pos = (blockN - 1) * blockSize;
    if(pos < 0)
        pos = 0;
    f.seek(pos);
    QByteArray block = f.read(blockSize);
    f.close();
    if(mode.toLower() == QString("netascii"))
        block.replace("\n", "\r\n");

    return block;
}

void FileHandler::writeBlock(QByteArray block)
{
    if(!file.isOpen())
    {
        file.setFileName(filename);
        if(mode.toLower() == QString("netascii"))
            file.open(QIODevice::WriteOnly | QIODevice::Text);
        else
            file.open(QIODevice::WriteOnly);
    }
    block.replace("\r\n", "\n");
    file.write(block);
}

void FileHandler::closeFile()
{
    file.close();
}

FileHandler::FileHandler()
{
}
