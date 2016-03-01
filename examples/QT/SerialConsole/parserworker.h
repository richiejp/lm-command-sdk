#ifndef PARSERWORKER_H
#define PARSERWORKER_H

#include <QObject>

class ParserWorker : public QObject
{
    Q_OBJECT
public:
    explicit ParserWorker(QObject *parent = 0);

signals:

public slots:
};

#endif // PARSERWORKER_H
