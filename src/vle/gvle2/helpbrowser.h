/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QHelpEngineCore>
#include <QTextBrowser>

class HelpBrowser : public QTextBrowser
{
    Q_OBJECT

public:
    HelpBrowser(QWidget *parent);
    void setHelp(QHelpEngineCore *h);
    void showHelpForKeyword(const QString &id);

private:
    QVariant loadResource(int type, const QUrl &name);

    QHelpEngineCore *mHelpEngine;
};

#endif // HELPBROWSER_H
