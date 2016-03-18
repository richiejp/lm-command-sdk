/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License
 * set forth in licenses.txt in the root directory of the LM Command SDK
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

#include "serialcontroller.h"
#include <QSerialPortInfo>
#include <QDebug>

SerialController::SerialController(QObject *parent) : QObject(parent)
{
    m_lineEnd = 0x0D;
    m_sworker = new SerialWorker();
    m_pworker = new ParserWorker();
    m_sworker->moveToThread(&m_sworkerThread);
    m_pworker->moveToThread(&m_pworkerThread);

    connect(&m_sworkerThread, &QThread::finished,
            m_sworker, &QObject::deleteLater);
    connect(&m_pworkerThread, &QThread::finished,
            m_pworker, &QObject::deleteLater);

    connect(m_sworker, &SerialWorker::error,
            this, &SerialController::error);
    connect(this, &SerialController::requestOpenPort,
            m_sworker, &SerialWorker::open);
    connect(m_sworker, &SerialWorker::opened,
            this, &SerialController::workerOpened);
    connect(this, &SerialController::requestClosePort,
            m_sworker, &SerialWorker::close);
    connect(m_sworker, &SerialWorker::closed,
            this, &SerialController::closedPort);
    connect(this, &SerialController::txDataReady,
            m_sworker, &SerialWorker::transmit);
    connect(m_sworker, &SerialWorker::recieved,
            this, &SerialController::rxData);

    connect(this, &SerialController::validateInput,
            m_pworker, &ParserWorker::validateInput);
    connect(m_pworker, &ParserWorker::validatedInput,
            this, &SerialController::validatedInput);

    m_sworkerThread.start();
    m_pworkerThread.start();
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

void SerialController::workerOpened(bool success, QString errors)
{
    if(success){
        emit openedPort();
    }else{
        emit error(tr("Failed to open port: %1").arg(errors));
    }

}
