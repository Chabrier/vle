/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include "vlevpzdynamic.h"

vleVpzDynamic::vleVpzDynamic(QString name, QString lib, QString package)
{
    mName    = name;
    mLibrary = lib;
    mPackage = package;
}

QString vleVpzDynamic::getName()
{
    return mName;
}

void vleVpzDynamic::setName(QString name)
{
    mName = name;
}

QString vleVpzDynamic::getLibrary()
{
    return mLibrary;
}

void vleVpzDynamic::setLibrary(QString lib)
{
    mLibrary = lib;
}

QString vleVpzDynamic::getPackage()
{
    return mPackage;
}

void vleVpzDynamic::setPackage(QString pkg)
{
    mPackage = pkg;
}
