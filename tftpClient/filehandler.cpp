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
    file.seek(position);
    QByteArray block = file.read(blockSize);
    position += block.length();
    if((mode.toLower() == QString("netascii")) && (!block.contains("\r\n")))
    {
        block.replace("\n", "\r\n");
        if((block.length() + block.count("\n")) > blockSize)
        {
            int count = block.count("\n");
            position -= count;
            QByteArray temp = block.right(count);
            position += temp.count("\r");
        }
        block = block.left(blockSize);
    }
    file.seek(position);
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

bool FileHandler::eofReached()
{
    if(file.atEnd())
        return true;
    return false;
}

void FileHandler::closeFile()
{
    file.close();
}

FileHandler::FileHandler()
{
    position = 0;
}
