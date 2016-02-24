#include "serialcontroller.h"
#include <QSerialPortInfo>
#include <QDebug>

SerialController::SerialController(QObject *parent) : QObject(parent)
{
    m_port = new QSerialPort(this);
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

void SerialController::requestOpenPort(QString name, int baud)
{
    m_port->setPortName(name);
    m_port->setBaudRate((qint32)baud);

    if(!m_port->open(QIODevice::ReadWrite)){
        error(tr("Failed to open port"));
    }else{
        emit openedPort();
    }
}

void SerialController::requestClosePort()
{
    m_port->close();

    emit closedPort();
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
