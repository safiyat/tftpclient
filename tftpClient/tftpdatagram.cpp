#include "tftpdatagram.h"


quint16 tftpDatagram::getErrorNumber()
{
    errorNumber = this->readBlockNumberFromDatagram();
    return errorNumber;
}

QByteArray tftpDatagram::getErrorMessage()
{
    errorMessage = this->readDataFromDatagram();
    return errorMessage;
}

void tftpDatagram::addOpcodeToDatagram(quint16 o)
{
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
    QDataStream d(&datagram, QIODevice::Append);
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
    quint16 b;
    QByteArray temp(datagram);
    QDataStream d(temp);
    d >> b;
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

void tftpDatagram::setDatagram(const QByteArray &value)
{
    datagram = value;
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

void tftpDatagram::clearDatagram()
{
    datagram.clear();
}

bool tftpDatagram::rrqOperation()
{
    opcode = OP_RRQ;
    datagram.clear();

    addOpcodeToDatagram(opcode);
    addFilenameToDatagram(filename);
    addZeroToDatagram();
    addModeToDatagram(mode);
    addZeroToDatagram();

    this->writeDatagram(datagram, remoteAddr, remotePort);

    return true;
}

bool tftpDatagram::wrqOperation()
{
    opcode = OP_WRQ;
    datagram.clear();

    addOpcodeToDatagram(opcode);
    addFilenameToDatagram(filename);
    addZeroToDatagram();
    addModeToDatagram(mode);
    addZeroToDatagram();

    this->writeDatagram(datagram, remoteAddr, remotePort);

    return true;
}

bool tftpDatagram::errorOperation()
{
    return true;
}

bool tftpDatagram::lsqOperation()
{
    opcode = OP_LSQ;
    datagram.clear();

    addOpcodeToDatagram(opcode);

    this->writeDatagram(datagram, remoteAddr, remotePort);
    return true;
}

bool tftpDatagram::sendDataOperation()
{
    opcode = OP_DATA;
    datagram.clear();

    addOpcodeToDatagram(opcode);
    addBlockNumberToDatagram(blockNumber);
    addDataToDatagram(body);
    this->writeDatagram(datagram, remoteAddr, remotePort);
    return true;
}

bool tftpDatagram::receiveDataOperation()
{
    QByteArray data;
    data.resize(this->pendingDatagramSize());
    qint64 status = this->readDatagram(data.data(), data.size(), &remoteAddr, &remotePort);
    datagram = data;
    body = this->readDataFromDatagram();
    blockNumber = this->readBlockNumberFromDatagram();
    if(status != -1)
        return true;
    else
        return false;
}

bool tftpDatagram::sendAckOperation()
{
    opcode = OP_ACK;
    datagram.clear();

    addOpcodeToDatagram(opcode);
    addBlockNumberToDatagram(blockNumber);

    this->writeDatagram(datagram, remoteAddr, remotePort);
    return true;
}

bool tftpDatagram::receiveAckOperation()
{
    QByteArray data;
    data.resize(this->pendingDatagramSize());
    qint64 status = this->readDatagram(data.data(), data.size(), &remoteAddr, &remotePort);
    datagram = data;
    opcode = this->readOpcodeFromDatagram();
    blockNumber = this->readBlockNumberFromDatagram();
    if(status != -1)
        return true;
    else
        return false;
}
