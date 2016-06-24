#include "tftpclientwindow.h"
#include "ui_tftpclientwindow.h"

tftpClientWindow::tftpClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tftpClientWindow)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(released()), this, SLOT(onBrowseFile()));
    connect(ui->executeButton, SIGNAL(released()), this, SLOT(onExecuteClick()));
    timeout = 10;
}

tftpClientWindow::~tftpClientWindow()
{
    delete ui;
}

quint16 tftpClientWindow::getSourcePort() const
{
    return sourcePort;
}

void tftpClientWindow::setSourcePort(const quint16 &value)
{
    sourcePort = value;
}

QHostAddress tftpClientWindow::getRemoteAddr() const
{
    return remoteAddr;
}

void tftpClientWindow::setRemoteAddr(const QHostAddress &value)
{
    remoteAddr = value;
}

quint16 tftpClientWindow::getRemotePort() const
{
    return remotePort;
}

void tftpClientWindow::setRemotePort(const quint16 &value)
{
    remotePort = value;
}

quint32 tftpClientWindow::getBlockNumber() const
{
    return blockNumber;
}

void tftpClientWindow::setBlockNumber(const quint32 &value)
{
    blockNumber = value;
}

quint8 tftpClientWindow::getTimeout() const
{
    return timeout;
}

void tftpClientWindow::setTimeout(const quint8 &value)
{
    timeout = value;
}

void tftpClientWindow::onBrowseFile()
{
    QString filename = QFileDialog::getOpenFileName(this,
        tr("Open File"), "", tr("All Files (*.*)"));
    ui->localFile->setText(filename);
}

void tftpClientWindow::onExecuteClick()
{
    ui->executeButton->setEnabled(false);
    ui->browseButton->setEnabled(false);
    ui->command->setEnabled(false);
    ui->localFile->setEnabled(false);

    ui->remoteServer->setEnabled(false);
    ui->remoteFile->setEnabled(false);

    if(ui->command->currentText() == "GET")
    {
        ui->logBrowser->append("GET called");
        getMethod();
    }
    else if(ui->command->currentText() == "PUT")
        ui->logBrowser->append("PUT called");
    else if(ui->command->currentText() == "LIST")
        ui->logBrowser->append("LIST called");

    ui->executeButton->setEnabled(true);
    ui->browseButton->setEnabled(true);
    ui->command->setEnabled(true);
    ui->localFile->setEnabled(true);

    ui->remoteServer->setEnabled(true);
    ui->remoteFile->setEnabled(true);
}


void tftpClientWindow::getMethod()
{
    sourcePort = bindUdpSocket(0);

    datagram.setFilename(ui->remoteFile->text().toLocal8Bit());
    datagram.setMode(ui->mode->currentText().toLower().toLocal8Bit());
    datagram.rrqOperation(timeout);
    datagram.writeDatagram(datagram.getDatagram(), QHostAddress(ui->remoteServer->text()), 69);
    printDatagram();

    datagram.disconnectFromHost();
    if (datagram.state() == QAbstractSocket::UnconnectedState ||
        datagram.waitForDisconnected(1000))
    {
        QString log = QString("Closed the UDP socket.");
        ui->statusBar->showMessage(log);
        ui->logBrowser->append(log);
    }
}

void tftpClientWindow::putMethod()
{

}

void tftpClientWindow::listMethod()
{

}

quint16 tftpClientWindow::bindUdpSocket(quint16 port)
{
    int i = 0;
    while(datagram.state() == QAbstractSocket::UnconnectedState)
    {
        datagram.bind(port);
        i++;
        if(i > 5)
        {
            QString log = QString("Unable to listen to any unused ports on any interface.");
            ui->statusBar->showMessage(log);
            ui->logBrowser->append(log);
            return 0;
        }
    }
    QString log = QString("UDP Socket listening on interface %1 at port %2.").arg(datagram.localAddress().toString(),
                                                                                  QString::number(datagram.localPort()));
    ui->statusBar->showMessage(log);
    ui->logBrowser->append(log);

    return datagram.localPort();
}

void tftpClientWindow::printDatagram()
{
    QByteArray datagramP = datagram.getDatagram();
    QString DataAsString = datagram.getDatagramString();
    ui->logBrowser->append(QString("\n"));
    ui->logBrowser->append(QString("Datagram: <code>%1</code>").arg(DataAsString));
    ui->logBrowser->append(QString("Length: %1").arg(QString::number(datagramP.length())));
}

void tftpClientWindow::sleep(int s)
{
        int ms = s * 1000;
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
}
