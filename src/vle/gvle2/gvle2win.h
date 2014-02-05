#ifndef GVLE2WIN_H
#define GVLE2WIN_H

#include "vle/gvle2/logger.h"
#include "widgetprojecttree.h"
#include <QMainWindow>
#include <QTranslator>
#include <QTreeWidgetItem>
#include <QActionGroup>
#include <QSettings>
#include <QTimer>

#include "plugin_sim.h"

#ifndef Q_MOC_RUN
#include <vle/vpz/Vpz.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Package.hpp>
#include <vle/utils/Preferences.hpp>
#endif

namespace Ui {
class GVLE2Win;
}

class GVLE2Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit GVLE2Win(QWidget *parent = 0);
    ~GVLE2Win();

protected:
    void loadSimulationPluggins();
    void showEvent(QShowEvent *event);

private slots:
    void onOpenProject();
    void onProjectRecent1();
    void onProjectRecent2();
    void onProjectRecent3();
    void onProjectRecent4();
    void onProjectRecent5();
    void onCloseProject();
    void onQuit();
    void onProjectConfigure();
    void onLaunchSimulation();
    void onSelectSimulator(bool isChecked);
    void onHelp();
    void onAbout();
    void onTabChange(int index);
    void onTabClose(int index);
    void onStatusToggle();
    void onTreeDblClick(QTreeWidgetItem *item, int column);
    void projectConfigureTimer();

private:
    Ui::GVLE2Win *ui;
    bool               mOpenedPackage;
    Logger           * mLogger;
    QTimer           * mTimer;
    WidgetProjectTree* mProjectTree;
    QSettings        * mSettings;
    vle::vpz::Vpz    * mVpz;
    bool               mSimOpened;
    QActionGroup     * mMenuSimGroup;
    QList <QString>    mSimulators;
    PluginSimulator  * mCurrentSim;

protected:
    void openProject(QString pathName);
private:
    void menuRecentProjectRefresh();
    void menuRecentProjectSet(QString path, QAction *menu);
    void menuRecentProjectUpdate(QString path);
    void statusWidgetOpen();
    void statusWidgetClose();
    void treeProjectUpdate();
    void treeProjectUpdate(QTreeWidgetItem *base, QString folderName);
private:
    vle::utils::Package mCurrPackage;
};

#endif // GVLE2WIN_H
