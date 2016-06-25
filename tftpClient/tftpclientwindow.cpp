#include "tftpclientwindow.h"
#include "ui_tftpclientwindow.h"

tftpClientWindow::tftpClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tftpClientWindow)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(released()), this, SLOT(onBrowseFile()));
    connect(ui->executeButton, SIGNAL(released()), this, SLOT(onExecuteClick()));
    timeout = 2;
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
    datagram.setRemoteAddr(QHostAddress(ui->remoteServer->text()));
    datagram.setRemotePort(69);

    if(!datagram.rrqOperation())
    {
        ui->logBrowser->append("Unable to send RRQ.");
        this->disconnectSocket();
    }

    // Wait for the first data packet
    while(!datagram.waitForReadyRead(timeout))
    {
        ui->logBrowser->append("Response to RRQ timedout. Sending again...");
        datagram.rrqOperation();
    }

    // Receive first data packet
    while(!datagram.receiveDataOperation())
    {
        ui->logBrowser->append("Didn't receive. Waiting again...");
    }

    // Send first ACK
    if(datagram.sendAckOperation())
        ui->logBrowser->append(QString("Successfully sent ack for block: %1").arg(datagram.getBlockNumber()));
    else
        ui->logBrowser->append(QString("Failed to send ack for block: %1").arg(datagram.getBlockNumber()));
    ui->logBrowser->append(QString("ACK: %1").arg(datagram.getDatagramString()));
    ui->logBrowser->append(QString("LEN: %1").arg(datagram.getDatagram().length()));

    QByteArray first = datagram.getBody();
    QByteArray body;
    // Make note of size of first DATA packet
    int first_size = first.length();
    receivedData.append(first);

    // Keep receiving until available
    while(datagram.waitForReadyRead(timeout))
    {
        // Receive DATA packet
        while(!datagram.receiveDataOperation())
        {
            ui->logBrowser->append("Didn't receive. Waiting again...");
        }
        body = datagram.getBody();
        receivedData.append(body);

        // Send ACK
        if(datagram.sendAckOperation())
            ui->logBrowser->append(QString("Successfully sent ack for block: %1").arg(datagram.getBlockNumber()));
        else
            ui->logBrowser->append(QString("Failed to send ack for block: %1").arg(datagram.getBlockNumber()));
        ui->logBrowser->append(QString("ACK: %1").arg(datagram.getDatagramString()));
        if(body.length() < first_size)
            break;
    }

    ui->outputBrowser->setText(printDatagram(receivedData));
    ui->logBrowser->append(QString("Block number: %1").arg(datagram.getBlockNumber()));

    this->disconnectSocket();
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

void tftpClientWindow::disconnectSocket()
{
    datagram.disconnectFromHost();
    if (datagram.state() == QAbstractSocket::UnconnectedState ||
        datagram.waitForDisconnected(1000))
    {
        QString log = QString("Closed the UDP socket.");
        ui->statusBar->showMessage(log);
        ui->logBrowser->append(log);
    }
}

QString tftpClientWindow::printDatagram()
{
    QByteArray datagramP = datagram.getDatagram();
    QString DataAsString = datagram.getDatagramString();
    QString output = QString();
    output.append(QString("\n"));
    output.append(QString("Datagram: <code>%1</code>").arg(DataAsString));
    output.append(QString("Length: %1").arg(QString::number(datagramP.length())));
    return output;
}

QString tftpClientWindow::printDatagram(const QByteArray &datagramP)
{
    QString DataAsString;
    for(int i = 0; i < datagramP.length(); i++)
    {
        char c = datagramP.at(i);
        if(c < 10)
        {
            QString t = "<b>";
            t.append(c + 48);
            t.append("</b>");
            DataAsString.append(t);
        }
        else
            DataAsString.append(c);
    }
    QString output = QString();
    output.append(QString("\n"));
    output.append(QString("Datagram: <code>%1</code>").arg(DataAsString));
    output.append(QString("Length: %1").arg(QString::number(datagramP.length())));
    return output;
}

void tftpClientWindow::sleep(int s)
{
        int ms = s * 1000;
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
}
