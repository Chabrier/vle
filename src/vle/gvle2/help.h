/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef HELP_H
#define HELP_H

#include <QHelpEngineCore>
#include <QWidget>
#include <QTextBrowser>

namespace Ui {
class help;
}

class help : public QWidget
{
    Q_OBJECT
    
public:
    explicit help(QWidget *parent = 0);
    ~help();
    void loadResource();
    void loadHelp(QString topic);

protected:
    QString mLocaleName;
    
private:
    Ui::help *ui;
    QHelpEngineCore *mhelpEngine;
};

#endif // HELP_H
