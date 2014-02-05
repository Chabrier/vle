/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef SIM_PLOT_H
#define SIM_PLOT_H

#include <QObject>
#include <QList>
#include <QSettings>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QThread>
#include "plugin_sim.h"
#include "logger.h"
#include "thread.h"
#include "simtab.h"
#include "widtoolbar.h"
#include "plotsignal.h"

class SimPlot : public QObject, public PluginSimulator
{
    Q_OBJECT
    Q_INTERFACES(PluginSimulator)

public:
    SimPlot();
    ~SimPlot();
    QString getname();
    QWidget *getWidget();
    QWidget *getWidgetToolbar();
    void setSettings(QSettings *s);
    void setLogger(Logger *logger);
    void setVpz(vle::vpz::Vpz *vpz);
    void *getVpz();
    void setPackage(vle::utils::Package *pkg);

public slots:
    void startStop();
    void addPlotSig(plotSignal *plot);
    void delPlotSig(plotSignal *plot);
    void simulationGetStep();
    void simulationFinished();

private:
    void log(QString message);
    void simulationStart();

private:
    QString name;
    QSettings             *  mSettings;
    Logger                *  mLogger;
    SimTab                *  mWidgetTab;
    widToolbar            *  mWidgetToolbar;
    vle::vpz::Vpz         *  mVpz;
    vle::utils::Package   *  mCurrPackage;
    QList<plotSignal *>      mPlotSignals;
    QThread               *  mThread;
    simPlotThread         *  mSimThread;
    vle::value::Map       *  mOutputs;
    QMap<QString, int>       mViewLastRow;
    bool                     mSimDataValid;
    int                      mStepCount;
    double                   mDuration;
};

#endif // SIM_PLOT_H
