/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014 INRA
 *
 */
#include <QDebug>
#include "helpbrowser.h"

HelpBrowser::HelpBrowser(QWidget *parent)
    : QTextBrowser(parent)
{
    mHelpEngine = 0;
}

void HelpBrowser::setHelp(QHelpEngineCore *h)
{
    // Test validity of specified help
    if ( ! h->setupData())
    {
        qDebug() << "HelpBrowser::setHelp() " << tr("Specified help file is invalid");
        return;
    }

    mHelpEngine = h;
}

void HelpBrowser::showHelpForKeyword(const QString &id)
{
    if ( ! mHelpEngine)
        return;

    QMap<QString, QUrl> links = mHelpEngine->linksForIdentifier(id);
    if (links.count())
        setSource(links.constBegin().value());
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name)
{
    QByteArray ba;
    if (type < 4 && mHelpEngine) {
        QUrl url(name);
        if (name.isRelative())
            url = source().resolved(url);
        ba = mHelpEngine->fileData(url);
    }
    return ba;
}
