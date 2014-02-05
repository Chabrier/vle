/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>

class simPlotThread : public QObject
{
    Q_OBJECT

public:
    simPlotThread(vle::vpz::Vpz *vpz = 0);
    ~simPlotThread();
    QString getError()
    {
        return mErrorMessage;
    }
    bool   isPaused();
    void   setPause(bool pause);
    void   forceQuit();
    void   setOutputPath(QString path);
    double getCurrentTime();
    vle::value::Map    *getOutputs();
    vle::value::Matrix *getMatrix(vle::value::Value *value);
    void   load();
    void   init();
    void   finish();

public slots:
    void run();
    void restart();
signals:
     void step();
     void end();

private:
    enum State { Wait, Init, Play, Error, Finish, Close, Pause };
private:
    State                       mCurrentState;
    QString                     mErrorMessage;
    QString                     mOutputPath;
    QMutex                      mValueMutex;
    vle::vpz::Vpz              *mVpz;
    vle::devs::RootCoordinator *mRoot;
    vle::value::Map            *mOutputs;
    vle::utils::ModuleManager   mLoadedPlugin;
};

#endif // THREAD_H
