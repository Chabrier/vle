#include <QFileDialog>
#include <QStyleFactory>
#include <QActionGroup>
#include <QMessageBox>

#include "gvle2win.h"
#include "ui_gvle2win.h"
#include "plugin_sim.h"
#include "help.h"
#include "aboutbox.h"
#include "filevpzview.h"
#include "simulation.h"
#include "vlevpz.h"
#include <QtDebug>

GVLE2Win::GVLE2Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GVLE2Win)
{
    mVpz = 0;
    mTimer = 0;
    mLogger = 0;
    mSimOpened = false;
    mOpenedPackage = false;
    mCurrentSimPlugin = 0;
    // VLE init
    mCurrPackage.refreshPath();

    // GUI init
    ui->setupUi(this);
    // Open the configuration file
    std::string configFile = vle::utils::Path::path().getHomeFile("gvle2.conf");
    mSettings = new QSettings(QString(configFile.c_str()),QSettings::IniFormat);
    menuRecentProjectRefresh();

    // Connect menubar handlers
    QObject::connect(ui->actionNewProject,  SIGNAL(triggered()), this, SLOT(onNewProject()));
    QObject::connect(ui->actionOpenProject,  SIGNAL(triggered()), this, SLOT(onOpenProject()));
    QObject::connect(ui->actionRecent1,      SIGNAL(triggered()), this, SLOT(onProjectRecent1()));
    QObject::connect(ui->actionRecent2,      SIGNAL(triggered()), this, SLOT(onProjectRecent2()));
    QObject::connect(ui->actionRecent3,      SIGNAL(triggered()), this, SLOT(onProjectRecent3()));
    QObject::connect(ui->actionRecent4,      SIGNAL(triggered()), this, SLOT(onProjectRecent4()));
    QObject::connect(ui->actionRecent5,      SIGNAL(triggered()), this, SLOT(onProjectRecent5()));
    QObject::connect(ui->actionCloseProject, SIGNAL(triggered()), this, SLOT(onCloseProject()));
    QObject::connect(ui->actionQuit,         SIGNAL(triggered()), this, SLOT(onQuit()));
    QObject::connect(ui->actionConfigureProject, SIGNAL(triggered()), this, SLOT(onProjectConfigure()));
    QObject::connect(ui->actionBuildProject, SIGNAL(triggered()), this, SLOT(onProjectBuild()));
    QObject::connect(ui->actionLaunchSimulation, SIGNAL(triggered()), this, SLOT(onLaunchSimulation()));
    QObject::connect(ui->actionSimNone,      SIGNAL(toggled(bool)),   this, SLOT(onSelectSimulator(bool)));
    QObject::connect(ui->actionHelp,         SIGNAL(triggered()), this, SLOT(onHelp()));
    QObject::connect(ui->actionAbout,        SIGNAL(triggered()), this, SLOT(onAbout()));
    // Handle -click- on project tree
    QObject::connect(ui->treeProject, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), \
                     this,              SLOT(onTreeDblClick(QTreeWidgetItem*, int)));
    //
    QObject::connect(ui->tabWidget,   SIGNAL(currentChanged(int)),    this, SLOT(onTabChange(int)));
    QObject::connect(ui->tabWidget,   SIGNAL(tabCloseRequested(int)), this, SLOT(onTabClose(int)));
    // Buttons and bar
    QObject::connect(ui->statusTitleToggle,  SIGNAL(clicked()),   this, SLOT(onStatusToggle()));
    mProjectTree = new WidgetProjectTree();
    // Initiate an mutual exclusive selection on simulators menu
    mMenuSimGroup = new QActionGroup(this);
    ui->actionSimNone->setActionGroup(mMenuSimGroup);

    mLogger = new Logger();
    mLogger->setWidget(ui->statusLog);

    // Update window title
    setWindowTitle("GVLE");
    //
    QList<int> sizes;
    sizes.append(200);
    sizes.append(500);
    sizes.append(200);
    ui->splitter->setSizes(sizes);

    loadSimulationPluggins();
}

GVLE2Win::~GVLE2Win()
{
    mSettings->sync();
    delete mSettings;
    if (mProjectTree)
        delete mProjectTree;
    if (mVpz)
        delete mVpz;

    ui->treeProject->clear();

    while(ui->tabWidget->count())
    {
        QWidget *w = ui->tabWidget->widget(0);

        ui->tabWidget->removeTab(0);
        delete w;
    }
    delete ui;
}

void GVLE2Win::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    statusWidgetClose();
}

void GVLE2Win::loadSimulationPluggins()
{
    //qDebug() << QCoreApplication::libraryPaths();
    // QCoreApplication::addLibraryPath

    QString basePath;
    std::string defPluginPath = vle::utils::Path::path().getHomeFile("plugins");
    QVariant path = mSettings->value("Plugins/path", defPluginPath.c_str());
    if (path.isValid())
        basePath = path.toString();
    else
        basePath = defPluginPath.c_str();

    qDebug() << "Load plugins path : " << basePath;
    QDir pluginsDir = QDir(basePath);
    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName) );
        QObject *plugin = loader.instance();
        if (loader.isLoaded())
        {
            PluginSimulator *sim = qobject_cast<PluginSimulator *>(plugin);
            if (sim) {
                qDebug() << "    " << sim->getname();
                mLogger->log(QString("Load simulator pluggin : %1").arg(sim->getname()));
                mSimulators << pluginsDir.absoluteFilePath(fileName);
                // Update Menu
                QAction *newAct = ui->menuSelectSimulator->addAction(sim->getname());
                newAct->setCheckable(true);
                newAct->setActionGroup(mMenuSimGroup);
                newAct->setObjectName(sim->getname());
                newAct->setData(pluginsDir.absoluteFilePath(fileName));
                QObject::connect(newAct, SIGNAL(toggled(bool)), this, SLOT(onSelectSimulator(bool)));
            }
        }
    }
    ui->menuSelectSimulator->setEnabled(true);
}

/**
 * @brief GVLE2Win::onOpenProject
 *        Handler for menu function : File > New Project
 */
void GVLE2Win::onNewProject()
{
    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setFileMode(QFileDialog::AnyFile);
    FileChooserDialog.setOptions(QFileDialog::ShowDirsOnly);
    FileChooserDialog.setLabelText(QFileDialog::LookIn,
            "Choose a directory");
    FileChooserDialog.setLabelText(QFileDialog::FileName,
            "Name of the VLE project");
    if (FileChooserDialog.exec())
        newProject(FileChooserDialog.selectedFiles().first());
}
/**
 * @brief GVLE2Win::onOpenProject
 *        Handler for menu function : File > Open Project
 */
void GVLE2Win::onOpenProject()
{
    QFileDialog FileChooserDialog(this);

    FileChooserDialog.setFileMode(QFileDialog::Directory);
    if (FileChooserDialog.exec())
        openProject(FileChooserDialog.selectedFiles().first());
}
void GVLE2Win::onProjectRecent1()
{
    QVariant path = mSettings->value("Projects/recent1");
    if (path.isValid())
        openProject(path.toString());
}
void GVLE2Win::onProjectRecent2()
{
    QVariant path = mSettings->value("Projects/recent2");
    if (path.isValid())
        openProject(path.toString());
}
void GVLE2Win::onProjectRecent3()
{
    QVariant path = mSettings->value("Projects/recent3");
    if (path.isValid())
        openProject(path.toString());
}
void GVLE2Win::onProjectRecent4()
{
    QVariant path = mSettings->value("Projects/recent4");
    if (path.isValid())
        openProject(path.toString());
}
void GVLE2Win::onProjectRecent5()
{
    QVariant path = mSettings->value("Projects/recent5");
    if (path.isValid())
        openProject(path.toString());
}

/**
 * @brief GVLE2Win::newProject
 *        Handler for menu function : File > New Project
 */
void GVLE2Win::newProject(QString pathName)
{
    QDir    dir(pathName);
    std::string basename = dir.dirName().toStdString ();

    if (mOpenedPackage)
        onCloseProject();
    mLogger->log(QString("New Project %1").arg(dir.dirName()));

    // Update window title
    setWindowTitle("GVLE - " + dir.dirName());

    dir.cdUp();
    QDir::setCurrent( dir.path() );
    mCurrPackage.select(basename);
    mCurrPackage.create();
    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);

    mOpenedPackage = true;
    mProjectPath = dir.dirName();
}

/**
 * @brief GVLE2Win::openProject
 *        Handler for menu function : File > Open Project
 */
void GVLE2Win::openProject(QString pathName)
{
    QDir    dir(pathName);
    std::string basename = dir.dirName().toStdString ();

    if (mOpenedPackage)
        onCloseProject();

    mLogger->log(QString("Open Project %1").arg(dir.dirName()));

    // Update window title
    setWindowTitle("GVLE - " + dir.dirName());

    dir.cdUp();
    QDir::setCurrent( dir.path() );

    mCurrPackage.select(basename);
    treeProjectUpdate();

    // Update the recent projects
    menuRecentProjectUpdate(pathName);
    menuRecentProjectRefresh();

    ui->actionCloseProject->setEnabled(true);
    ui->menuProject->setEnabled(true);

    mOpenedPackage = true;
    mProjectPath = dir.dirName();
}



/**
 * @brief GVLE2Win::onCloseProject
 *        Handler for menu function : File > Close Project
 */
void GVLE2Win::onCloseProject()
{
    // Clear the project tree
    ui->treeProject->clear();
    ui->treeProject->setColumnCount(0);
    // Update menus
    ui->actionCloseProject->setEnabled(false);
    ui->menuProject->setEnabled(false);
    // Update window title
    setWindowTitle("GVLE");

    int i;
    for (i = (ui->tabWidget->count() - 1); i >= 0; i--)
    {
        QWidget *w = ui->tabWidget->widget(i);
        QVariant rtool = w->property("wTool");
        if (rtool.isValid())
        {
            QWidget *wtool = ui->rightStack->widget( rtool.toInt() );
            ui->rightStack->removeWidget(wtool);
            delete wtool;
        }
        QVariant tabType = w->property("type");
        if (tabType.isValid())
            onTabClose(i);
    }

    mLogger->log(QString("Project closed"));

    mOpenedPackage = false;
}

/**
 * @brief GVLE2Win::onQuit
 *        Handler for menu function : File > Quit
 */
void GVLE2Win::onQuit()
{
    qApp->exit();
}

void GVLE2Win::onProjectConfigure()
{
    mLogger->log(tr("Project configuration started"));
    statusWidgetOpen();

    try {
        mCurrPackage.configure();
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project configuration failed"));
        return;
    }
    ui->actionConfigureProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectConfigureTimer()));
    mTimer->start(50);
}

void GVLE2Win::projectConfigureTimer()
{
    if (mCurrPackage.isFinish())
    {
        mTimer->stop();
        delete mTimer;
        mLogger->log(tr("Project configuration complete"));
        ui->actionConfigureProject->setEnabled(true);
    }
    std::string oo;
    std::string oe;
    bool ret = mCurrPackage.get(&oo, &oe);
    if (ret)
    {
        if(oe.length())
            mLogger->logExt(oe.c_str(), true);
        if (oo.length())
            mLogger->logExt(oo.c_str());
    }
}

void GVLE2Win::onProjectBuild()
{
    mLogger->log(tr("Project compilation started"));
    statusWidgetOpen();

    try {
        mCurrPackage.build();
    } catch (const std::exception &e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project compilation failed"));
        return;
    }
    ui->actionBuildProject->setEnabled(false);

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectBuildTimer()));
    mTimer->start(50);
}

void GVLE2Win::projectBuildTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if(oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimer->stop();
        delete mTimer;
        if (mCurrPackage.get(&oo, &oe)) {
            if(oe.length()) {
                mLogger->logExt(oe.c_str(), true);
            }
            if (oo.length()) {
                mLogger->logExt(oo.c_str());
            }
        }
        if (mCurrPackage.isSuccess()) {
            projectInstall();
            mLogger->log(tr("Project compilation complete"));
        } else {
            mLogger->log(tr("Project compilation failed"));
        }
        ui->actionBuildProject->setEnabled(true);
    }
}

void GVLE2Win::projectInstall()
{
    mLogger->log(tr("Project installation started"));

    try {
        mCurrPackage.install();
    } catch (const std::exception& e) {
        QString logMessage = QString("%1").arg(e.what());
        mLogger->logExt(logMessage, true);
        mLogger->log(tr("Project installation failed"));
        return;
    }

    mTimer = new QTimer();
    QObject::connect(mTimer, SIGNAL(timeout()), this, SLOT(projectInstallTimer()));
    mTimer->start(50);
}

void GVLE2Win::projectInstallTimer()
{
    std::string oo, oe;

    if (mCurrPackage.get(&oo, &oe)) {
        if(oe.length()) {
            mLogger->logExt(oe.c_str(), true);
        }
        if (oo.length()) {
            mLogger->logExt(oo.c_str());
        }
    }

    if (mCurrPackage.isFinish()) {
        mTimer->stop();
        delete mTimer;
        if (mCurrPackage.isSuccess()) {
            mLogger->log(tr("Project installation complete"));
        } else {
            mLogger->log(tr("Project installation failed"));
        }
        ui->actionBuildProject->setEnabled(true);
    }
}

/**
 * @brief GVLE2Win::onLaunchSimulation
 *        Handler for menu function : Simulation > Launch Simulation
 */
void GVLE2Win::onLaunchSimulation()
{
    QWidget *w = ui->tabWidget->currentWidget();
    QVariant tabType = w->property("type");
    if (tabType.toString().compare("vpz"))
        return;
    if (mSimOpened)
        return;

    fileVpzView *vpzView = (fileVpzView *)w;

    if ( ! ui->actionSimNone->isChecked())
    {
        QObject *plugin = mCurrentSimPlugin->instance();
        if ( ! mCurrentSimPlugin->isLoaded())
            return;
        PluginSimulator *sim = qobject_cast<PluginSimulator *>(plugin);
        if ( ! sim)
            return;

        mCurrentSim = sim;

        sim->setSettings(mSettings);
        //sim->setLogger(mLogger);

        QWidget *newTab = sim->getWidget();
        if (newTab)
        {
            // Configure Pluggin for the requested VPZ
            newTab->setProperty("type",   QString("simulation"));
            newTab->setProperty("plugin", QString("yes"));
            newTab->setProperty("vpz",    QVariant::fromValue((void*)vpzView));
            vpzView->usedBySim(true);
            try {
                sim->setVpz(vpzView->vpz());
                sim->setPackage(&mCurrPackage);
                // Associate the pluggin widget with a new tab
                int n = ui->tabWidget->addTab(newTab, "Simulation");
                ui->tabWidget->setCurrentIndex(n);
                newTab->show();
            } catch(...) {
                qDebug() << "Simulation pluggin configuration error";
            }
        }

        // Search a toolbox widget from the plugin
        QWidget *newTool = sim->getWidgetToolbar();
        if (newTool)
        {
            try {
                int nid;
                nid = ui->rightStack->addWidget(newTool);
                ui->rightStack->setCurrentWidget(newTool);
                newTab->setProperty("wTool", nid);
            } catch (...) {
                qDebug() << "Simulation pluggin toolbar error";
            }
        }
    }
    else
    {
        // Create a new tab
        simulationView * newTab = new simulationView();
        newTab->setProperty("type", QString("simulation"));
        newTab->setProperty("vpz",  QVariant::fromValue((void*)vpzView));
        vpzView->usedBySim(true);
        newTab->setVpz(vpzView->vpz());
        newTab->setPackage(&mCurrPackage);
        newTab->setLogger(mLogger);
        newTab->setSettings(mSettings);
        int n = ui->tabWidget->addTab(newTab, "Simulation");
        ui->tabWidget->setCurrentIndex(n);
        newTab->show();

        // Create a new toolbox for the right column
        int nid;
        QWidget *newRTool = newTab->getTool();
        nid = ui->rightStack->addWidget(newRTool);
        ui->rightStack->setCurrentWidget(newRTool);
        newTab->setProperty("wTool", nid);
    }

    // Disable some menu entries when a tab is opened
    ui->actionLaunchSimulation->setEnabled(false);
    ui->menuSelectSimulator->setEnabled(false);
    mSimOpened = true;
}

void GVLE2Win::onSelectSimulator(bool isChecked)
{
    QAction *act = (QAction *)sender();

    if (isChecked)
    {
        if (mCurrentSimPlugin)
        {
            delete mCurrentSimPlugin;
            mCurrentSimPlugin = 0;
        }

        if (act->objectName() != "actionSimNone")
        {
            QString pluginPath = act->data().toString();

            mCurrentSimPlugin = new QPluginLoader(pluginPath);
            // Try to load instance, to test symbols
            mCurrentSimPlugin->instance();
            if ( ! mCurrentSimPlugin->isLoaded())
                return;
            // Test ok, unload the plugin to reduce memory usage
            qDebug() << mCurrentSimPlugin->unload();
        }
    }
}

/**
 * @brief GVLE2Win::onHelp Handler of menu Help > Help
 *        Open a new tab and display embedded help page
 */
void GVLE2Win::onHelp()
{
    bool  helpOpened = false;
    help *tabHelp = 0;
    int   idx = 0;

    // Get current widget to detect contextual help entry
    //QWidget *wid = QApplication::focusWidget();

    // Search if an Help tab is already open
    for (int i = (ui->tabWidget->count() - 1); i >= 0; i--)
    {
        QWidget *w = ui->tabWidget->widget(i);
        QVariant tabType = w->property("type");
        if (tabType.isValid() && (tabType.toString() == "help"))
        {
            tabHelp = (help *)w;
            idx = i;
            helpOpened = true;
            break;
        }
    }
    // If no help tab found, create a new one
    if ( ! helpOpened)
    {
        tabHelp = new help(ui->tabWidget);
        tabHelp->setProperty("type", QString("help"));
        idx = ui->tabWidget->addTab(tabHelp, "Help");
    }
    // Set the focus on help tab
    ui->tabWidget->setCurrentIndex(idx);
}

/**
 * @brief GVLE2Win::onAbout Handler of menu Help > About
 *        Open the about box as dialog and show it
 */
void GVLE2Win::onAbout()
{
    AboutBox box;

    box.exec();
}

/**
 * @brief GVLE2Win::menuRecentProjectRefresh
 *        Read the recent opened projects list and update menu
 */
void GVLE2Win::menuRecentProjectRefresh()
{
    QVariant v1 = mSettings->value("Projects/recent1");
    QVariant v2 = mSettings->value("Projects/recent2");
    QVariant v3 = mSettings->value("Projects/recent3");
    QVariant v4 = mSettings->value("Projects/recent4");
    QVariant v5 = mSettings->value("Projects/recent5");
    if (v1.isValid())
        menuRecentProjectSet(v1.toString(), ui->actionRecent1);
    else
        ui->actionRecent1->setText(tr("<none>"));

    if (v2.isValid())
        menuRecentProjectSet(v2.toString(), ui->actionRecent2);
    if (v3.isValid())
        menuRecentProjectSet(v3.toString(), ui->actionRecent3);
    if (v4.isValid())
        menuRecentProjectSet(v4.toString(), ui->actionRecent4);
    if (v5.isValid())
        menuRecentProjectSet(v5.toString(), ui->actionRecent5);
}
void GVLE2Win::menuRecentProjectSet(QString path, QAction *menu)
{
    QDir dir(path);
    menu->setText(dir.dirName());
    menu->setToolTip(path);
    menu->setEnabled(true);
    menu->setVisible(true);
}
void GVLE2Win::menuRecentProjectUpdate(QString path)
{
    QVariant v1 = mSettings->value("Projects/recent1");
    QVariant v2 = mSettings->value("Projects/recent2");
    QVariant v3 = mSettings->value("Projects/recent3");
    QVariant v4 = mSettings->value("Projects/recent4");
    QVariant v5 = mSettings->value("Projects/recent5");

    QSet <QString> recentList;
    recentList.insert(path);

    if (v1.isValid()) recentList.insert(v1.toString());
    if (v2.isValid()) recentList.insert(v2.toString());
    if (v3.isValid()) recentList.insert(v3.toString());
    if (v4.isValid()) recentList.insert(v4.toString());
    if (v5.isValid()) recentList.insert(v5.toString());

    QSetIterator<QString> i(recentList);
    if (i.hasNext()) mSettings->setValue("Projects/recent1", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent2", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent5", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent4", i.next());
    if (i.hasNext()) mSettings->setValue("Projects/recent3", i.next());

    mSettings->sync();
}

/* ---------- Manage the central tabs ---------- */

void GVLE2Win::onTabChange(int index)
{
    if (ui->tabWidget->count() == 0)
        return;

    QWidget *w = ui->tabWidget->widget(index);
    QVariant tabType = w->property("type");
    bool isVpz = (tabType.toString().compare("vpz") == 0);
    bool isSim = (tabType.toString().compare("simulation") == 0);

    if (tabType.isValid())
    {
        // Update Right Column
        if (isSim)
        {
            int toolId = w->property("wTool").toInt();
            ui->rightStack->setCurrentIndex(toolId);
        }
        else if (isVpz)
        {
            int toolId = w->property("wTool").toInt();
            ui->rightStack->setCurrentIndex(toolId);
        }
        else
            ui->rightStack->setCurrentIndex(0);

        // Update Simulation menu
        if (isVpz || isSim)
            ui->menuSimulation->setEnabled(true);
        else
            ui->menuSimulation->setEnabled(false);
    }
    else
    {
        ui->menuSimulation->setEnabled(false);
        ui->rightStack->setCurrentIndex(0);
    }
}

void GVLE2Win::onTabClose(int index)
{
    bool isSim = false;
    QWidget *wTool = 0;
    QWidget *w = ui->tabWidget->widget(index);
    if (w == 0)
        return;

    bool isPlugin = false;
    QVariant vIsPlugin = w->property("plugin");
    if (vIsPlugin.isValid())
        isPlugin = (vIsPlugin.toString().compare("yes") == 0);

    if (w->property("type").toString().compare("vpz") == 0)
    {
        fileVpzView *tabVpz = (fileVpzView *)w;
        int useReason;
        bool allowClose = false;
        if (tabVpz->isUsed(&useReason))
        {
            QMessageBox msgBox;
            if (useReason == 1)
            {
                msgBox.setText(tr("This tab can't be closed (used by a simulator)"));
                msgBox.exec();
                return;
            }
            if (useReason == 2)
            {
                msgBox.setText(tr("File modified ! Save before close ?"));
                msgBox.addButton(QMessageBox::Save);
                msgBox.addButton(QMessageBox::Discard);
                msgBox.addButton(QMessageBox::Cancel);
                int ret = msgBox.exec();
                switch (ret) {
                  case QMessageBox::Save:
                    tabVpz->save();
                    allowClose = true;
                    break;
                  case QMessageBox::Discard:
                    allowClose = true;
                    break;
                  case QMessageBox::Cancel:
                    allowClose = false;
                    break;
                  default:
                      // should never be reached
                      break;
                }
                if (! allowClose)
                    return;
            }
        }
    }

    ui->tabWidget->removeTab(index);

    if (w->property("type").toString().compare("simulation") == 0)
    {
        isSim = true;

        QVariant linkedVpz = w->property("vpz");
        if (linkedVpz.isValid())
        {
            void *ptr = linkedVpz.value<void *>();
            fileVpzView *currentVpz = (fileVpzView *)ptr;
            currentVpz->usedBySim(false);
        }

        ui->actionLaunchSimulation->setEnabled(true);
        ui->menuSelectSimulator->setEnabled(true);
        mSimOpened = false;

        int toolId = w->property("wTool").toInt();
        wTool = ui->rightStack->widget(toolId);
        ui->rightStack->removeWidget(wTool);
        if (isPlugin)
            mCurrentSim->delWidgetToolbar();
        else
            delete wTool;

        if (isPlugin)
        {
            mCurrentSim->delWidget();
            w = 0;
        }
    }
    if (w)
        delete w;

    if (isSim && isPlugin)
    {
        mCurrentSimPlugin->unload();
        //delete mCurrentSim;
        mCurrentSim = 0;
    }
}

/* ---------- Manage the status bar ---------- */

/**
 * @brief GVLE2Win::onProjectConfigure
 *        Handler for menu function : Project > Configure Project
 */
void GVLE2Win::onStatusToggle()
{
    if (ui->statusLog->isVisible())
        statusWidgetClose();
    else
        statusWidgetOpen();
}

void GVLE2Win::statusWidgetOpen()
{
    ui->statusLog->show();
    ui->statusWidget->setMinimumSize(0, 0);
    ui->statusWidget->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
}

void GVLE2Win::statusWidgetClose()
{
    ui->statusLog->hide();
    QSize titleSize = ui->statusTitleFrame->size();
    ui->statusWidget->setFixedHeight(titleSize.height());
    ui->statusWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
}

/* ---------- Manage the Project navigator ---------- */

void GVLE2Win::treeProjectUpdate()
{
    QTreeWidget *tree = ui->treeProject;
    QString      packageName( vle::utils::Path::filename(mCurrPackage.name()).c_str() );

    // Clear the current tree
    tree->clear();
    tree->setColumnCount(1);
    // Add package name at the top of the widget
    QStringList HeaderLabels;
    HeaderLabels << packageName;
    tree->setHeaderLabels(HeaderLabels);

    // Read directory content
    QDir dir(mCurrPackage.name().c_str());
    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::Name);
    QFileInfoList list = dir.entryInfoList();
    QListIterator<QFileInfo> files( list );
    QList<QTreeWidgetItem *> fileItems;
    QList<QTreeWidgetItem *> folderItems;
    while( files.hasNext() )
    {
        QFileInfo fileInfo = files.next();
        QString   fileName = fileInfo.fileName();
        if (fileInfo.isFile())
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setData(0, Qt::UserRole, QVariant(fileInfo.filePath()));
            item->setText(0, fileName);
            item->setIcon(0, *(new QIcon(":/icon/resources/page_white.png")));
            fileItems.append(item);
        }
        if (fileInfo.isDir())
        {
            if ( (fileName == ".") || (fileName ==".."))
                continue;
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, fileName);
            item->setIcon(0, *(new QIcon(":/icon/resources/folder.png")));
            folderItems.append(item);
            treeProjectUpdate(item, fileInfo.filePath());
        }
    }
    tree->insertTopLevelItems(0, fileItems);
    tree->insertTopLevelItems(0, folderItems);
    tree->setStyleSheet(QStyleFactory::keys().at(1));
}

void GVLE2Win::treeProjectUpdate(QTreeWidgetItem *base, QString folderName)
{
    QDir dir(folderName);
    dir.setFilter(QDir::Dirs | QDir::Files);
    dir.setSorting(QDir::Name);

    QFileInfoList list = dir.entryInfoList();
    QListIterator<QFileInfo> files( list );
    QList<QTreeWidgetItem *> fileItems;

    while( files.hasNext() )
    {
        QFileInfo fileInfo = files.next();
        QString   fileName = fileInfo.fileName();
        if (fileInfo.isFile())
        {
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setData(0, Qt::UserRole, QVariant(fileInfo.filePath()));
            item->setText(0, fileName);
            if (fileInfo.suffix() == "vpz")
                item->setIcon(0, *(new QIcon(":/icon/resources/map_color.png")));
            else
                item->setIcon(0, *(new QIcon(":/icon/resources/page_white.png")));
            fileItems.append(item);
        }
        if (fileInfo.isDir())
        {
            if ( (fileName == ".") || (fileName ==".."))
                continue;
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, fileName);
            item->setIcon(0, *(new QIcon(":/icon/resources/folder.png")));
            base->addChild(item);
            treeProjectUpdate(item, fileInfo.filePath());
        }
    }

    QListIterator<QTreeWidgetItem*> items( fileItems );
    while( items.hasNext() )
        base->addChild(items.next());
}

void GVLE2Win::onTreeDblClick(QTreeWidgetItem *item, int column)
{
    QString fileName = item->data(column, Qt::UserRole).toString();

    QFileInfo selectedFileInfo = QFileInfo(fileName);
    if (selectedFileInfo.suffix() != "vpz")
        return;

#ifdef QTVPZ
    vleVpz *selVpz;
    selVpz = new vleVpz(fileName);
    selVpz->setBasePath(mProjectPath);
#else
    vle::vpz::Vpz *selectedVpz;
    selectedVpz = new vle::vpz::Vpz(fileName.toStdString());
#endif

    // Search if the selected VPZ has already been opened
    int alreadyOpened = 0;
    int i;
    for (i = 0; i < ui->tabWidget->count(); i++)
    {
        QWidget *w = ui->tabWidget->widget(i);
        QVariant tabType = w->property("type");
        if (tabType.toString() != "vpz")
            continue;
        // Compare the tab title with the requested vpz name
        if (ui->tabWidget->tabText(i) == fileName)
        {
            alreadyOpened = i;
            break;
        }
    }

    if (alreadyOpened)
        // If the VPZ is opened, select his tab
        ui->tabWidget->setCurrentIndex(alreadyOpened);
    else
    {
        // Create a new Tab to display VPZ model
        fileVpzView * newTab = new fileVpzView();
        newTab->setProperty("type", QString("vpz"));
#ifdef QTVPZ
        newTab->setVpz(selVpz);
#else
        newTab->setVpz(selectedVpz);
#endif
        int n = ui->tabWidget->addTab(newTab, fileName);
        ui->tabWidget->setCurrentIndex(n);
        newTab->show();

        // Create a new toolbox for the right column
        int nid;
        QWidget *newRTool = newTab->getTool();
        nid = ui->rightStack->addWidget(newRTool);
        ui->rightStack->setCurrentWidget(newRTool);
        newTab->setProperty("wTool", nid);
    }
}
