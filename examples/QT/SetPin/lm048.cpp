#include "lm048.h"
#include <QDebug>
#include <QSerialPortInfo>
#include <QThread>
#include <QCoreApplication>
#include "../../../build/lm048lib.h"

LM048::LM048(QObject *parent, QString portName)
    : QObject(parent)
{
    m_portName = portName;
}

void LM048::transmit(const QByteArray data)
{
    qint64 written = m_port->write(data);
    qint64 total = written;

    while(total < data.length()){
        if(written < 0){
            qDebug() << "tx error: " << m_port->errorString() << "\n";
            abort();
        }
        written = m_port->write(data.mid(total - 1));
        total += written;
    }

    m_port->flush();
    m_port->waitForBytesWritten(1000);

    qDebug() << "Transmitted (" << data.length() << "): " << data;
}

QByteArray LM048::receive()
{
    m_port->waitForReadyRead(1000);
    QByteArray data = m_port->readAll();

    //We wait here so that the LM048 has time to transmit more data. This is not necessary
    //for lm048_input's sake, but it does allow us to print the entire response at once.
    thread()->msleep(100);
    while(m_port->bytesAvailable() > 0){
        data += m_port->readAll();
        thread()->msleep(100);
    }

    if(data.length() > 0)
        qDebug() << "Received (" << data.length() << "): " << data;

    return data;
}

LM048_STATUS LM048::transceive(int timeout)
{
    char buf[LM048_MINIMUM_WRITE_BUFFER];
    size_t l = LM048_MINIMUM_WRITE_BUFFER;
    LM048_STATUS s = LM048_OK;

    //This takes the first command that we have queued and writes it to a specified buffer
    if((s = lm048_write_front_command(NULL, buf, &l)) != LM048_COMPLETED){
        qDebug() << "Could not write command, status: " << s << "\n";
    }

    //write front command does not null terminate the string because it is going over the wire
    //however in this particular case we can add it to conveniently print the generated command
    buf[l] = '\0';
    qDebug() << "Generated packet (" << strlen(buf) << "," << l
             << "): " << buf;
    //Send the generate command
    transmit(QByteArray::fromRawData(buf, l));
    s = LM048_OK;
    //This horrible code receives data and feeds it into the parser until either: a packet is parsed,
    //an error occurres or it times out.
    for(int c = 0; c < timeout && s == LM048_OK; c++){
        QByteArray recv = receive();
        l = recv.length();
        //This inner loop is necessary because lm048_input returns with LM048_OK when it parses the echo
        //of the command we just sent.
        while(l > 0 && s == LM048_OK){
            size_t sofar = recv.length() - l;
            s = lm048_input(recv.data() + sofar, &l);
        }
    }

    if(l > 0)
        qDebug() << "Exiting the receive loop while there is still data left to parse, it will be discarded";

    return s;
}

void LM048::run()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    if(ports.length() < 1){
        qDebug() << "No serial m_ports found";
        emit finished();
        return;
    }

    int i;
    for(i = 0; i < ports.length(); i++){
        if(m_portName == ports[i].portName()){
            break;
        }
    }

    if(i == ports.length()){
        qDebug() << m_portName
                 << " is not a valid serial m_port. Use one of these:";
        for(i = 0; i < ports.length(); i++){
            qDebug() << "\t" << ports[i].portName() << "\n";
        }
        emit finished();
        return;
    }

    m_port = new QSerialPort(this);

    m_port->setPortName(m_portName);
    m_port->setBaudRate((qint32)19200);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setFlowControl(QSerialPort::NoFlowControl);

    qDebug() << "Opening " << m_portName;

    if(!m_port->open(QIODevice::ReadWrite)){
        qDebug() << "Could not open serial m_port: " << m_port->errorString()
                 << "\n";
        emit finished();
        return;
    }

    //The lm048_packet structure holds information about an AT command or its response
    struct lm048_packet cmd, resp;

    enum LM048_STATUS s = LM048_OK;
    QByteArray payload = QByteArray("My Pin!");

    //This sets the payload storage buffer inside the packet structure. It just so happens
    //that the payload byte array contains valid data, but packet_init is not concerned by this,
    //it just sets the pointer and the capacity of the storage array.
    lm048_packet_init(&cmd, payload.data(), static_cast<size_t>(payload.size()));
    lm048_packet_init(&resp, payload.data(), static_cast<size_t>(payload.size()));

    qDebug() << "Sending AT ping/echo command to check RS232 connection is OK";
    cmd.type = LM048_AT_AT;
    resp.type = LM048_AT_OK;
    //Here we are telling the system that we are going to send AT and expect OK
    if(lm048_enqueue(NULL, cmd, resp) != LM048_COMPLETED){
        qDebug() << "Could not queue command\n";
        return;
    }

    //it is important to understand what transceive does, look at the definition above
    s = transceive(10);
    if(s == LM048_DEQUEUED){
        qDebug() << "Received OK as expected\n";
    }else if(s == LM048_UNEXPECTED){
        qDebug() << "Received unexpected response to AT\n";
    }else{
        qDebug() << "Error during lexing\n";
        return;
    }

    cmd.type = LM048_AT_PIN;
    cmd.modifier = LM048_ATM_SET;
    cmd.payload_length = static_cast<size_t>(payload.size());
    resp.type = LM048_AT_OK;
    //Here we are telling the system to send AT+PIN=My Pin! and expect OK in return
    lm048_enqueue(NULL, cmd, resp);
    qDebug() << "Sending the AT+PIN= command to set the pin";

    s = transceive(10);
    if(s == LM048_DEQUEUED){
        qDebug() << "Received OK as expected\n";
    }else if(s == LM048_UNEXPECTED){
        qDebug() << "Received unexpected response\n";
    }else{
        qDebug() << "Error during lexing\n";
        return;
    }

    //In this example we only queue up one command at a time and then do a send/receive
    //before continuing. Alternatively we could queue all the commands at once.

    cmd.type = LM048_AT_PIN;
    cmd.modifier = LM048_ATM_GET;
    resp.type = LM048_AT_VALUE_RESPONSE;
    resp.payload_length = static_cast<size_t>(payload.size());
    lm048_enqueue(NULL, cmd, resp);
    qDebug() << "Now Querying to check the correct pin was set with at+pin?";

    s = transceive(10);
    if(s == LM048_DEQUEUED){
        char buf[LM048_MINIMUM_WRITE_BUFFER];
        //This gets the most recently parsed packet payload which should be the
        //pin code we just set
        lm048_packet_payload(NULL, buf, LM048_MINIMUM_WRITE_BUFFER);
        qDebug() << "Received pin: " << buf << " as expected\n";
    }else if(s == LM048_UNEXPECTED){
        qDebug() << "Received unexpected response\n";
    }else{
        qDebug() << "Error during lexing\n";
        return;
    }

    m_port->close();
    qDebug() << "Fin.";
    emit finished();
}

void LM048::abort()
{
    m_port->close();
    QCoreApplication::quit();
}
