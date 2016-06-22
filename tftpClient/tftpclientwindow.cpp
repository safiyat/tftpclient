#include "tftpclientwindow.h"
#include "ui_tftpclientwindow.h"

tftpClientWindow::tftpClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tftpClientWindow)
{
    ui->setupUi(this);
    connect(ui->browseButton, SIGNAL(released()), this, SLOT(onBrowseFile()));
    connect(ui->executeButton, SIGNAL(released()), this, SLOT(onExecuteClick()));
            //    ui->logBrowser->setText();
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
        ui->logBrowser->append("GET called");
    else if(ui->command->currentText() == "PUT")
        ui->logBrowser->append("PUT called");
    else if(ui->command->currentText() == "LIST")
        ui->logBrowser->append("LIST called");

    for(int i = 0; i < 5; i++)
    {
        QString text = QString(i + 65);
        ui->logBrowser->append(text);
        ui->outputBrowser->setText(text);
        int ms = 1000;
        struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
        nanosleep(&ts, NULL);
        ui->statusBar->showMessage(QString(i + 49));
    }

    ui->executeButton->setEnabled(true);
    ui->browseButton->setEnabled(true);
    ui->command->setEnabled(true);
    ui->localFile->setEnabled(true);

    ui->remoteServer->setEnabled(true);
    ui->remoteFile->setEnabled(true);
}


void tftpClientWindow::getMethod()
{

}

void tftpClientWindow::putMethod()
{

}

void tftpClientWindow::listMethod()
{

}
