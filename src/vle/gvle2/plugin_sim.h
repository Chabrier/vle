#ifndef PLUGIN_SIM_H
#define PLUGIN_SIM_H

#include <QObject>
#include <QString>
#include <QWidget>
#include <vle/gvle2/logger.h>

#ifndef Q_MOC_RUN
#include <vle/utils/Package.hpp>
#include <vle/vpz/Vpz.hpp>
#endif

class PluginSimulator
{
public:
    virtual ~PluginSimulator() { };
    virtual QString  getname()   = 0;
    virtual QWidget *getWidget() = 0;
    virtual QWidget *getWidgetToolbar() = 0;
    virtual void  setSettings(QSettings *s) = 0;
    virtual void  setLogger(Logger *logger) = 0;
    virtual void  setVpz(vle::vpz::Vpz *vpz) = 0;
    virtual void *getVpz() = 0;
    virtual void  setPackage(vle::utils::Package *pkg) = 0;
};

Q_DECLARE_INTERFACE(PluginSimulator, "fr.inra.vle.gvle2.PluginSimulator/1.0")

#endif // PLUGIN_SIM_H
