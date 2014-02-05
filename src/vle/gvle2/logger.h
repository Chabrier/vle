#ifndef LOGGER_H
#define LOGGER_H

#include <QTime>
#include <QPlainTextEdit>

class Logger
{
public:
    Logger();
    void setWidget(QPlainTextEdit *widget);
    void log(QString message);
    void logExt(QString message, bool isError = false);

private:
    QPlainTextEdit *mWidget;
};

#endif // LOGGER_H
