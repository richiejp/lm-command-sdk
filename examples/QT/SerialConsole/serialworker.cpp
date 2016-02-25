#include "serialworker.h"
#include <QDebug>

SerialWorker::SerialWorker(QObject *parent) : QObject(parent)
{
    m_port = new QSerialPort(this);
    connect(m_port, &QSerialPort::readyRead,
            this, &SerialWorker::receive);
}

void SerialWorker::open(QString portName, int baud)
{
    m_port->setPortName(portName);
    m_port->setBaudRate((qint32)baud);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    qDebug() << "Opening " << portName
             << " " << baud;

    if(!m_port->open(QIODevice::ReadWrite)){
        emit opened(true, "");
    }else{
        emit opened(false, m_port->errorString());
    }
}

void SerialWorker::close()
{
    m_port->close();

    emit closed();
}

void SerialWorker::transmit(QByteArray data)
{
    qint64 written = m_port->write(data);
    qint64 total = written;

    while(total < data.length()){
        if(written < 0){
            emit error(tr("Tx error: %1").arg(m_port->errorString()));
            break;
        }
        written = m_port->write(data.mid(total - 1));
        total += written;
    }

    qDebug() << "Transmitted: " << data;

    emit transmitted(data);
}

void SerialWorker::receive()
{
    QByteArray data = m_port->readAll();

    while(m_port->bytesAvailable() > 0){
        data += m_port->readAll();
    }

    qDebug() << "Received: " << data;

    emit recieved(data);
}
