#include <QCoreApplication>
#include <QSerialPort>
#include <QtCore>
#include "lm048.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString portName("ttyUSB0");
    if(argc > 1){
        portName = QString(argv[1]);
    }

    LM048 *lm048 = new LM048(&a, portName);
    QObject::connect(lm048, &LM048::finished,
                     &a, &QCoreApplication::quit);
    QTimer::singleShot(0, lm048, &LM048::run);

    return a.exec();
}
