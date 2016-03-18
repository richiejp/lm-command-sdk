/** Copyright LM Technologies 2016
 *
 * This file is licensed under the terms of the Adaptive Public License
 * set forth in licenses.txt in the root directory of the LM Command SDK
 * repository.
 *
 * Author(s): Richard Palethorpe <richard@lm-technologies.com>
 */
#ifndef PARSERWORKER_H
#define PARSERWORKER_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include "../../../build/lm951lib.h"

class ParserWorker : public QObject
{
    Q_OBJECT
public:
    explicit ParserWorker(QObject *parent = 0);

signals:
    void validatedInput(QVariantMap feedback);

public slots:
    void validateInput(QString data);

private:
    struct lm951_parser m_input_state;
};

#endif // PARSERWORKER_H
