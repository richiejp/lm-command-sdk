#include "serialcontroller.h"
#include <QSerialPortInfo>
#include <QDebug>

SerialController::SerialController(QObject *parent) : QObject(parent)
{
    m_lineEnd = 0x0D;
    m_worker = new SerialWorker();
    m_worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished,
            m_worker, &QObject::deleteLater);
    connect(m_worker, &SerialWorker::error,
            this, &SerialController::error);
    connect(this, &SerialController::requestOpenPort,
            m_worker, &SerialWorker::open);
    connect(m_worker, &SerialWorker::opened,
            this, &SerialController::workerOpened);
    connect(this, &SerialController::requestClosePort,
            m_worker, &SerialWorker::close);
    connect(m_worker, &SerialWorker::closed,
            this, &SerialController::closedPort);
    connect(this, &SerialController::txDataReady,
            m_worker, &SerialWorker::transmit);
    connect(m_worker, &SerialWorker::recieved,
            this, &SerialController::rxData);

    m_workerThread.start();
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
    QByteArray bytes = data.toLatin1();
    bytes.append(m_lineEnd);
    emit txDataReady(bytes);
}

void SerialController::processRxData(QByteArray data)
{
    emit rxData(QString(data));
}

void SerialController::validateInput(QString data)
{
}

void SerialController::workerOpened(bool success, QString errors)
{
    if(success){
        emit openedPort();
    }else{
        emit error(tr("Failed to open port: %1").arg(errors));
    }

}
