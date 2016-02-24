#ifndef SERIALCONTROLLER_H
#define SERIALCONTROLLER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class SerialController : public QObject
{
    Q_OBJECT
public:
    explicit SerialController(QObject *parent = 0);

signals:
    void rxData(QString data);
    void error(QString message);
    void info(QString message, int level);
    void foundPorts(QVariantList ports);
    void foundBaudRates(QVariantList rates);
    void openedPort();
    void closedPort();

public slots:
    void requestPorts();
    void requestOpenPort(QString name);
    void queueTxData(QString data);
};

#endif // SERIALCONTROLLER_H
