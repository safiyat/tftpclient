#ifndef TFTPCLIENTWINDOW_H
#define TFTPCLIENTWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "tftpdatagram.h"

namespace Ui {
class tftpClientWindow;
}

class tftpClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit tftpClientWindow(QWidget *parent = 0);
    ~tftpClientWindow();

    quint16 getSourcePort() const;
    void setSourcePort(const quint16 &value);

    QHostAddress getRemoteAddr() const;
    void setRemoteAddr(const QHostAddress &value);

    quint16 getRemotePort() const;
    void setRemotePort(const quint16 &value);

    quint32 getBlockNumber() const;
    void setBlockNumber(const quint32 &value);

    quint8 getTimeout() const;
    void setTimeout(const quint8 &value);

public slots:
    void onBrowseFile();
    void onExecuteClick();

private:
    Ui::tftpClientWindow *ui;

    quint16 sourcePort;

    QHostAddress remoteAddr;
    quint16 remotePort;

    quint32 blockNumber;

    quint8 timeout;

    tftpDatagram datagram;

    void getMethod();
    void putMethod();
    void listMethod();

    quint16 bindUdpSocket(quint16 port);

    void printDatagram();
    void sleep(int s);
};

#endif // TFTPCLIENTWINDOW_H
