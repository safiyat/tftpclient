#ifndef DATAGRAM_H
#define DATAGRAM_H

#include <QUdpSocket>

class Datagram : public QUdpSocket
{
    Q_OBJECT
public:
    explicit Datagram(QObject *parent = 0);

signals:

public slots:

};

#endif // DATAGRAM_H
