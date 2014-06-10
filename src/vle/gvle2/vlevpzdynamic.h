/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#ifndef VLEVPZDYNAMIC_H
#define VLEVPZDYNAMIC_H

#include <QString>

class vleVpzDynamic
{
public:
    vleVpzDynamic(QString name = "", QString lib = "", QString package = "");

    void setName(QString name);
    void setLibrary(QString name);
    void setPackage(QString name);
    QString getName();
    QString getLibrary();
    QString getPackage();

private:
    QString mName;
    QString mLibrary;
    QString mPackage;
};

#endif // VLEVPZDYNAMIC_H
