#ifndef SIMULATIONVIEW_H
#define SIMULATIONVIEW_H

#include <QList>
#include <QWidget>
#include <QThread>
#include <QMutex>
#include <QSettings>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStandardItem>

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#include <vle/devs/RootCoordinator.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/ModuleManager.hpp>
#endif

#include "logger.h"
#include "ui_simulationrtool.h"

namespace Ui {
class simulationView;
class simulationRTool;
}

class simulationThread : public QObject
{
    Q_OBJECT
public:
    simulationThread(vle::vpz::Vpz *vpz = 0);
    ~simulationThread();
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

class simulationView : public QWidget
{
    Q_OBJECT
public:
    explicit simulationView(QWidget *parent = 0);
    ~simulationView();
    void setVpz(vle::vpz::Vpz *vpz);
    void setPackage(vle::utils::Package *pkg)
    {
        mCurrPackage = pkg;
    }
    void setLogger(Logger *logger)
    {
        mLogger = logger;
    }
    void setSettings(QSettings *s)
    {
        mSettings = s;
    }

    void simulationStart();
    void updateViews();
    QWidget *getTool()
    {
        return mWidgetTool;
    }

private:
    enum State { Wait, Init, Play, Error, Finish, Close };
    void insertViewPort(QString viewName, QString portName, QString portInfo);
    void log(QString message)
    {
        if (mLogger)
            mLogger->log(message);
    }

private:
    QThread               *  mThread;
    Logger                *  mLogger;
    QSettings             *  mSettings;
    Ui::simulationView    *  ui;
    Ui::simulationRTool   *  uiTool;
    QWidget               *  mWidgetTool;
    simulationThread      *  mSimThread;
    QList<QTreeWidgetItem *> mViewsItems;
    vle::vpz::Vpz         *  mVpz;
    vle::utils::Package   *  mCurrPackage;
    vle::utils::ModuleManager mLoadedPlugin;
    vle::value::Map       *  mOutputs;
    bool                     mSimDataValid;
    double    mDuration;
    double    mCurrentTime;
    int       mStepCount;
    QMap<QString, int>       mViewLastRow;

public slots:
     void simulationGetStep();
     void simulationFinished();
     void onTreeItemSelected();

private slots:
    void onButtonGo();
};

#endif // SIMULATIONVIEW_H
