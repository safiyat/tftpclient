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
    ui->remoteServer->setText("0.0.0.0");
    ui->remoteFile->setText("");
    ui->localFile->setText("");
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
        getMethod();
    }
    else if(ui->command->currentText() == "PUT")
    {
        putMethod();
    }
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

    datagram.clearDatagram();
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

    FileHandler f;
    QString destFile = ui->localFile->text();
    if(destFile.isEmpty())
        destFile = QString(ui->remoteFile->text());

    f.setFilename(destFile);
    f.setMode(ui->mode->currentText());
    f.writeBlock(receivedData);

    f.closeFile();
    receivedData.clear();

    ui->logBrowser->append(QString("Wrote to %1").arg(destFile));

    this->disconnectSocket();
}

void tftpClientWindow::putMethod()
{
    sourcePort = bindUdpSocket(0);

    datagram.clearDatagram();
    datagram.setFilename(ui->remoteFile->text().toLocal8Bit());
    datagram.setMode(ui->mode->currentText().toLower().toLocal8Bit());
    datagram.setRemoteAddr(QHostAddress(ui->remoteServer->text()));
    datagram.setRemotePort(69);

    if(!datagram.wrqOperation())
    {
        ui->logBrowser->append("Unable to send WRQ.");
        this->disconnectSocket();
    }

    // Wait for the first ACK packet
    while(!datagram.waitForReadyRead(timeout))
    {
        ui->logBrowser->append("Response to WRQ timedout. Sending again...");
        datagram.rrqOperation();
    }

    // Receive the first ACK
    while(datagram.getOpcode() != OP_ACK)
    {
        while(!datagram.receiveAckOperation())
        {
            ui->logBrowser->append("Didn't receive. Waiting again...");
        }
        if(datagram.getOpcode() == OP_ERROR)
        {
            ui->logBrowser->append(QString("An ERROR instead of ACK."));
            this->disconnectSocket();
            return;
        }
        else if(datagram.getOpcode() != OP_ACK)
        {
            ui->logBrowser->append(QString("Recieved %1 instead of ACK.").arg(opcodeToString(datagram.getOpcode())));
        }
        else
        {
            ui->logBrowser->append(QString("Recieved %1.").arg(opcodeToString(datagram.getOpcode())));
        }
    }
    ui->logBrowser->append("Received first ACK");

    blockNumber = 1;
    FileHandler f;
    f.setFilename(ui->localFile->text());
    f.setMode((ui->mode->currentText()));
    f.setBlockSize(512);
    f.setBlockNumber(blockNumber);

    // Keep sending until available
    QByteArray b;
    do
    {
       b = f.readBlock();

       datagram.clearDatagram();
       datagram.setOpcode(OP_DATA);
       datagram.setBlockNumber(blockNumber);
       datagram.setBody(b);

       while(!datagram.sendDataOperation())
       {
           ui->logBrowser->append(QString("Failed to send data for block number %1. Trying again...").arg(blockNumber));
       }
       ui->logBrowser->append(QString("Sent data for block number %1.").arg(blockNumber));
       ui->logBrowser->insertHtml(QString("<br>%1").arg(printDatagram(datagram.getDatagram())));

       while(datagram.getOpcode() != OP_ACK)
       {
           while(!datagram.receiveAckOperation())
           {
               ui->logBrowser->append("Didn't receive. Waiting again...");
           }
           if(datagram.getOpcode() == OP_ERROR)
           {
               ui->logBrowser->append(QString("An ERROR instead of ACK."));

               datagram.clearDatagram();
               f.closeFile();

               this->disconnectSocket();
               return;
           }
           else if(datagram.getOpcode() != OP_ACK)
           {
               ui->logBrowser->append(QString("Recieved %1 instead of ACK.").arg(opcodeToString(datagram.getOpcode())));
           }
           else
           {
               ui->logBrowser->append(QString("Recieved %1.").arg(opcodeToString(datagram.getOpcode())));
               if(datagram.getBlockNumber() == blockNumber) // ACK for the block received
               {
                   blockNumber++;
                   f.setBlockNumber(blockNumber);
                   break;
               }
               else     // ACK received, but nor for current DATA
               {
                   ui->logBrowser->append(QString("Didn't receive ACK for block number %1. Trying again...").arg(blockNumber));
                   while(!datagram.sendDataOperation())
                   {
                       ui->logBrowser->append(QString("Failed to send data for block number %1. Trying again...").arg(blockNumber));
                   }
               }
           }
       }
    } while (!f.eofReached());
    if(b.length() == 512) // File size was a multiple of block size
    {
        ui->logBrowser->append(QString("Sent complete file. The size was a multiple of block size. Sending a zero packet."));
        datagram.clearDatagram();
        datagram.setOpcode(OP_DATA);
        datagram.setBlockNumber(blockNumber);

        datagram.sendDataOperation();
    }

    ui->logBrowser->append(QString("Block number: %1").arg(datagram.getBlockNumber()));

    this->disconnectSocket();
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
    return printDatagram(datagram.getDatagram());
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
    output.append(QString("Datagram: <br>%1").arg(DataAsString));
    output.append(QString("<br>Length: <b>%1</b><br>").arg(QString::number(datagramP.length())));
    return output;
}

QString tftpClientWindow::opcodeToString(quint16 o)
{
    if(o == OP_ACK)
        return QString("Acknowledgment");
    if(o == OP_DATA)
        return QString("Data");
    if(o == OP_ERROR)
        return QString("Error");
    if(o == OP_LSQ)
        return QString("List Request");
    if(o == OP_RRQ)
        return QString("Read Request");
    if(o == OP_WRQ)
        return QString("Write Request");
    return QString("Bad Request");
}

void tftpClientWindow::sleep(int s)
{
        int ms = s * 1000;
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
}
