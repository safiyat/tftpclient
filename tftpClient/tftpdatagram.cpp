#include "tftpdatagram.h"

tftpDatagram::tftpDatagram(QObject *parent) :
    QUdpSocket(parent)
{
}

QHostAddress tftpDatagram::getSourceAddr() const
{
    return sourceAddr;
}

void tftpDatagram::setSourceAddr(const QHostAddress &value)
{
    sourceAddr = value;
}

quint16 tftpDatagram::getSourcePort() const
{
    return sourcePort;
}

void tftpDatagram::setSourcePort(const quint16 &value)
{
    sourcePort = value;
}

QHostAddress tftpDatagram::getRemoteAddr() const
{
    return remoteAddr;
}

void tftpDatagram::setRemoteAddr(const QHostAddress &value)
{
    remoteAddr = value;
}

quint16 tftpDatagram::getRemotePort() const
{
    return remotePort;
}

void tftpDatagram::setRemotePort(const quint16 &value)
{
    remotePort = value;
}

quint32 tftpDatagram::getBlockNumber() const
{
    return blockNumber;
}

void tftpDatagram::setBlockNumber(const quint32 &value)
{
    blockNumber = value;
}

quint16 tftpDatagram::getOpcode() const
{
    return opcode;
}

void tftpDatagram::setOpcode(const quint16 &value)
{
    opcode = value;
}

QByteArray tftpDatagram::getFilename() const
{
    return filename;
}

void tftpDatagram::setFilename(const QByteArray &value)
{
    filename = value;
}

QByteArray tftpDatagram::getMode() const
{
    return mode;
}

void tftpDatagram::setMode(const QByteArray &value)
{
    mode = value;
}

QByteArray tftpDatagram::getBody() const
{
    return body;
}

void tftpDatagram::setBody(const QByteArray &value)
{
    body = value;
}

QString tftpDatagram::prettyPrint() const
{
    QString output;
    return output;
}
