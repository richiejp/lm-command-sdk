#include "serialcontroller.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

SerialController::SerialController(QObject *parent) : QObject(parent)
{

}

void SerialController::requestPorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    if(ports.length() < 1){
        error(tr("No ports found"));
        qDebug() << "No ports found";
    }

    QVariantList list;

    for(int i = 0; i < ports.size(); ++i){
        QVariantMap p;
        p.insert("portName", ports.at(i).portName());
        p.insert("busy", ports.at(i).isBusy());
        list << p;
        qDebug() << "Found port " << ports.at(i).portName()
                 << " at " << ports.at(i).systemLocation();
    }

    emit foundPorts(list);
}

void SerialController::requestOpenPort(QString name)
{

}

void SerialController::requestBauds()
{
    QList<qint32> bauds = QSerialPortInfo::standardBaudRates();

    if(bauds.length() < 1){
        error(tr("No standard baud rates available"));
    }

    emit foundBauds(bauds);
}

void SerialController::queueTxData(QString data)
{

}
