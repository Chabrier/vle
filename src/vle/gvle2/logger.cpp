#include "logger.h"

Logger::Logger()
{
    mWidget = 0;
}

void Logger::setWidget(QPlainTextEdit *widget)
{
    mWidget = widget;
}

void Logger::log(QString message)
{
    if (mWidget == 0)
        return;

    QTime currentTime = QTime::currentTime();
    QString logTime = QString("[%1] ").arg(currentTime.toString("hh:mm:ss"));

    QString logLine = logTime + message;
    mWidget->appendPlainText(logLine);
}

void Logger::logExt(QString message, bool isError)
{
    if (mWidget == 0)
        return;

    QString logLine;
    if (isError)
        logLine = "<font color=#FF0000><pre>" + message + "</pre></font>";
    else
        logLine = "<font color=#0000FF><pre>" + message + "</pre></font>";
    mWidget->appendHtml(logLine);
}
