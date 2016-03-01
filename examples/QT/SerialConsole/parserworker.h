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
