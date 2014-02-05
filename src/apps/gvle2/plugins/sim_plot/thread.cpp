/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "thread.h"

simPlotThread::simPlotThread(vle::vpz::Vpz *vpz)
{
    mOutputs = 0;
    mCurrentState = Wait;
    if (vpz)
    {
        // Make a copy of current VPZ before use it for sim
        mVpz = new vle::vpz::Vpz(*vpz);

        // Instanciate the root coordinator    ToDo : Plugins
        mRoot = new vle::devs::RootCoordinator(mLoadedPlugin);
    }
    else
    {
        mVpz  = 0;
        mRoot = 0;
    }
}

/**
 * @brief simPlotThread default destructor
 *
 */
simPlotThread::~simPlotThread()
{
    if (mRoot)
        delete mRoot;

    if (mVpz)
        delete mVpz;
}

/**
 * @brief simPlotThread::getOutputs
 *        Get the map of ouputs datas
 *
 * This method take place into -simulationView- thread!
 */
vle::value::Map *simPlotThread::getOutputs()
{
    if (mOutputs == 0)
        mOutputs = mRoot->outputs();

    return mOutputs;
}

vle::value::Matrix *simPlotThread::getMatrix(vle::value::Value *value)
{
    vle::value::Matrix *mat;
    mValueMutex.lock();
    mat = new vle::value::Matrix(value->toMatrix());
    mValueMutex.unlock();
    return mat;
}

/**
 * @brief simPlotThread::getOutputs
 *        Get the map of ouputs datas
 *
 * This method can be executed in both threads
 */
double simPlotThread::getCurrentTime()
{
    double currentTime = mRoot->getCurrentTime();

    return currentTime;
}

/**
 * @brief simPlotThread::setOutputPath
 *        Configure output plugins for a specific location
 * @param QString Path name
 *
 * This method take place into -simulationView- thread!
 */
void simPlotThread::setOutputPath(QString path)
{
    vle::vpz::Outputs::iterator itb;
    vle::vpz::Outputs::iterator ite;

    mOutputPath = path;

    if (mVpz == 0)
        return;

    itb = mVpz->project().experiment().views().outputs().begin();
    ite = mVpz->project().experiment().views().outputs().end();
    for (; itb!=ite; itb++)
    {
        vle::vpz::Output& output = itb->second;
        if (output.location().empty())
            output.setLocalStreamLocation(mOutputPath.toStdString());
    }
}

/**
 * @brief simPlotThread::isPaused
 *        Test if the simulation thread is in Pause state
 */
bool simPlotThread::isPaused()
{
    if (mCurrentState == Pause)
        return true;
    else
        return false;
}
/**
 * @brief simPlotThread::setPause
 *        Request the simulation thread to wait some time
 */
void simPlotThread::setPause(bool pause)
{
    if (pause)
        mCurrentState = Pause;
    else
        mCurrentState = Play;
}

/**
 * @brief simPlotThread::forceQuit
 *        Force the current state to Finish (this will end the run loop)
 *
 */
void simPlotThread::forceQuit()
{
    mCurrentState = Finish;
}

/**
 * @brief simPlotThread::run
 *        Do the sim !
 *
 * This method take place into -dedicated- thread!
 */
void simPlotThread::run()
{
    mCurrentState = Play;

    while((mCurrentState == Play) || (mCurrentState == Pause))
    {
        // If the simulation is paused ... just wait
        while (mCurrentState == Pause)
        {
            // QThread::msleep(100);
            QTimer::singleShot(800, this, SLOT(restart()));
            return;
        }

        try {
            mValueMutex.lock();
            if (mRoot->run() == false)
                mCurrentState = Finish;
            mValueMutex.unlock();
            // Notify the view
            emit step();
        } catch (const std::exception& e) {
            mErrorMessage = QString(e.what());
        }
    }
    emit end();
}

/**
 * @brief simPlotThread::restart
 *        Used by thread when in PAUSE state ; used to restart sim
 */
void simPlotThread::restart()
{
    if (mCurrentState == Pause)
        QTimer::singleShot(800, this, SLOT(restart()));
    else
        run();
}

/**
 * @brief simPlotThread::load
 *        Load the VPZ into devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simPlotThread::load()
{
    if ((mVpz == 0) || (mRoot == 0))
    {
        mErrorMessage = tr("Model or RootCoordinator missing");
        throw 10;
        return;
    }

    try {
        mRoot->load(*mVpz);
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 1;
    }
}

/**
 * @brief simPlotThread::init
 *        Initialize the devs coordinator
 *
 * This method take place into the -simulationView- thread!
 */
void simPlotThread::init()
{
    try {
        mRoot->init();
    } catch (const std::exception &e) {
        mErrorMessage = QString(e.what());
        throw 2;
    }
}

/**
 * @brief simPlotThread::finish
 *        Terminate devs coordinator jobs
 *
 * This method take place into the -simulationView- thread!
 */
void simPlotThread::finish()
{
    try {
        mRoot->finish();
    } catch (const std::exception& e) {
        mErrorMessage = QString(e.what());
        throw 3;
    }
}
