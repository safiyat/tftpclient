#ifndef TFTPCLIENTWINDOW_H
#define TFTPCLIENTWINDOW_H

#include <QMainWindow>

namespace Ui {
class tftpClientWindow;
}

class tftpClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit tftpClientWindow(QWidget *parent = 0);
    ~tftpClientWindow();

private:
    Ui::tftpClientWindow *ui;
};

#endif // TFTPCLIENTWINDOW_H
