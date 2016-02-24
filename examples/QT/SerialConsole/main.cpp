#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "serialcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    qmlRegisterType<SerialController>("lm", 1, 0, "SerialController");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
