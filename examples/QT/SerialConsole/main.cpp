/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License
 * set forth in licenses.txt in the root directory of the LM Command SDK
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */

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
