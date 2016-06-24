#include "tftpdatagram.h"

void tftpDatagram::addOpcodeToDatagram(quint16 o)
{
//    addZeroToDatagram();
//    datagram.append(o);
    QDataStream d(&datagram, QIODevice::WriteOnly);
    d << o;
}

void tftpDatagram::addFilenameToDatagram(QByteArray f)
{
    datagram.append(f);
}

void tftpDatagram::addZeroToDatagram()
{
    datagram.append(char(0));
}

void tftpDatagram::addModeToDatagram(QByteArray m)
{
    datagram.append(m);
}

void tftpDatagram::addBlockNumberToDatagram(quint16 b)
{
//    Needs fixing.
//    datagram.append(b);
    QDataStream d(&datagram, QIODevice::WriteOnly);
    d << b;
}

void tftpDatagram::addDataToDatagram(QByteArray d)
{
    datagram.append(d);
}

quint16 tftpDatagram::readOpcodeFromDatagram()
{
    quint16 o = 1;
    QDataStream d(datagram);
    d >> o;
    return o;
}

QByteArray tftpDatagram::readFilenameFromDatagram()
{
    QList<QByteArray> list;
    QByteArray f;
    list = datagram.split(char(0));
    f = list.first();
    f.remove(0, 2);
    return f;
}

QByteArray tftpDatagram::readModeFromDatagram()
{
    QList<QByteArray> list;
    QByteArray m;
    list = datagram.split(char(0));
    m = list.last();
    m.remove(0, 2);
    return m;
}

quint16 tftpDatagram::readBlockNumberFromDatagram()
{
    QList<QByteArray> list;
    quint16 b;
    QByteArray temp;
    list = datagram.split(char(0));
    temp = list.first();
    temp.remove(0, 2);
    QDataStream d(temp);
    d >> b;
    return b;
}

QByteArray tftpDatagram::readDataFromDatagram()
{
    QByteArray d(datagram);
    d.remove(0, 4);
    return d;
}

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

quint16 tftpDatagram::getBlockNumber() const
{
    return blockNumber;
}

void tftpDatagram::setBlockNumber(const quint16 &value)
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

QByteArray tftpDatagram::getDatagram() const
{
    return datagram;
}

QString tftpDatagram::getDatagramString() const
{
    QString value;
    for(int i = 0; i < datagram.length(); i++)
    {
        char c = datagram.at(i);
        if(c < 10)
        {
            QString t = "<b>";
            t.append(c + 48);
            t.append("</b>");
            value.append(t);
        }
        else
            value.append(c);
    }
    return value;
}

QString tftpDatagram::prettyPrint()
{
    QString output;
    quint16 o = this->readOpcodeFromDatagram();
    output.append(QString("<b><font color=\"red\">OPCODE:</font></b> 0%1").arg(o));
    return output;
}

bool tftpDatagram::rrqOperation(quint8 timeout)
{
//    QHostAddress sourceAddr;
//    quint16 sourcePort;

//    QHostAddress remoteAddr;
//    quint16 remotePort;

//    quint16 blockNumber;
//    quint16 opcode;

//    QByteArray filename;
//    QByteArray mode;
//    QByteArray body;
    opcode = OP_RRQ;
    datagram.clear();
//    datagram.append(char(0));   //Padding for opcode
//    quint16 o = 01;
//    datagram.append(o);
//    datagram.append(filename);
//    datagram.append(char(0));
//    datagram.append(mode);
//    datagram.append(char(0));
    addOpcodeToDatagram(OP_RRQ);
    addFilenameToDatagram(filename);
    addZeroToDatagram();
    addModeToDatagram(mode);
    addZeroToDatagram();

    timeout--;
    return true;
}

bool tftpDatagram::wrqOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::errorOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::listOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::sendDataOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::receiveDataOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::sendAckOperation(quint8 timeout)
{
    timeout--;
    return true;
}

bool tftpDatagram::receiveAckOperation(quint8 timeout)
{
    timeout--;
    return true;
}
