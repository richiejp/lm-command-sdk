#include "parserworker.h"
#include "../../../build/lm951lib.h"

ParserWorker::ParserWorker(QObject *parent) : QObject(parent)
{
    lm951_init(&m_input_state);
}
 void ParserWorker::validateInput(QString data)
{
    QVariantMap feedback;
    QByteArray str = data.toLatin1();
    size_t l = (size_t)str.size();

    switch(lm951_inputs(&m_input_state, str.data(), &l)){
    case LM951_OK:
        l = 1;
        if(lm951_inputs(&m_input_state, LM951_COMMAND_DELIMETER, &l)
                != LM951_COMPLETED){
            feedback.insert("state", "ok");
            break;
        }
    case LM951_COMPLETED:
        feedback.insert("state", "completed");
        break;
    case LM951_ERROR:
        feedback.insert("state", "error");
        break;
    }

    lm951_restart(&m_input_state);

    emit validatedInput(feedback);
}
