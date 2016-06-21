#include "tftpclientwindow.h"
#include "ui_tftpclientwindow.h"

tftpClientWindow::tftpClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tftpClientWindow)
{
    ui->setupUi(this);
}

tftpClientWindow::~tftpClientWindow()
{
    delete ui;
}
