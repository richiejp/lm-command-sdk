#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <QSerialPort>
#include <QThread>
#include "serialworker.h"

class SerialController : public QObject
{
    Q_OBJECT
public:
    explicit SerialController(QObject *parent = 0);

signals:
    void rxData(QString data);
    void txDataReady(QByteArray data);
    void error(QString message);
    void info(QString message, int level);
    void foundPorts(QVariantList ports);
    void foundBauds(QList<qint32> rates);
    void openedPort();
    void closedPort();
    void requestOpenPort(QString name, int baud);
    void requestClosePort();

public slots:
    void requestPorts();
    void requestBauds();
    void queueTxData(QString data);
    void processRxData(QByteArray data);

private slots:
    void workerOpened(bool success, QString errors);

private:
    char m_lineEnd;
    SerialWorker *m_worker;
    QThread m_workerThread;

};

#endif // SERIALCONTROLLER_H
