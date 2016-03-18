/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License
 * set forth in licenses.txt in the root directory of the LM Command SDK
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */
#ifndef SERIALWORKER_H
#define SERIALWORKER_H

#include <QObject>
#include <QString>
#include <QSerialPort>

class SerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit SerialWorker(QObject *parent = 0);

signals:
    void error(QString message);
    void opened(bool success, QString error);
    void closed();
    void transmitted(QByteArray data);
    void recieved(QByteArray data);

public slots:
    void open(QString portName, int baud);
    void close();
    void transmit(QByteArray data);
    void receive();

private:
    QSerialPort *m_port;
};

#endif // SERIALWORKER_H
