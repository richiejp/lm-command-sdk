#ifndef LM048TESTS_H
#define LM048TESTS_H

#include <QObject>
#include <QSerialPort>
#include "../../../build/lm048lib.h"

class LM048 : public QObject
{
    Q_OBJECT
public:
    explicit LM048(QObject *parent = 0, QString portName = QString(""));

signals:
    void finished();

public slots:
    void run();

private slots:
    void transmit(const QByteArray data);
    QByteArray receive();
    LM048_STATUS transceive(int timeout);
    void abort();

private:
    QSerialPort *m_port;
    QString m_portName;
};

#endif // LM048TESTS_H
