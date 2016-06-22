#ifndef TFTPDATAGRAM_H
#define TFTPDATAGRAM_H

#define OP_RRQ   1
#define OP_WRQ   2
#define OP_DATA  3
#define OP_ACK   4
#define OP_ERROR 5
#define OP_LSQ   6

#include <QUdpSocket>

class tftpDatagram : public QUdpSocket
{
    Q_OBJECT
    QHostAddress sourceAddr;
    quint16 sourcePort;

    QHostAddress remoteAddr;
    quint16 remotePort;

    quint32 blockNumber;
    quint16 opcode;

    QByteArray filename;
    QByteArray mode;
    QByteArray body;

public:
    explicit tftpDatagram(QObject *parent = 0);

    QHostAddress getSourceAddr() const;
    void setSourceAddr(const QHostAddress &value);

    quint16 getSourcePort() const;
    void setSourcePort(const quint16 &value);

    QHostAddress getRemoteAddr() const;
    void setRemoteAddr(const QHostAddress &value);

    quint16 getRemotePort() const;
    void setRemotePort(const quint16 &value);

    quint32 getBlockNumber() const;
    void setBlockNumber(const quint32 &value);

    quint16 getOpcode() const;
    void setOpcode(const quint16 &value);

    QByteArray getFilename() const;
    void setFilename(const QByteArray &value);

    QByteArray getMode() const;
    void setMode(const QByteArray &value);

    QByteArray getBody() const;
    void setBody(const QByteArray &value);

    QString prettyPrint() const; // For logging and output

signals:

public slots:

};

#endif // TFTPDATAGRAM_H
