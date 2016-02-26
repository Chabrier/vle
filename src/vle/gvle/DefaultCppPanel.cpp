/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2014-2015 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and
 * contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "DefaultCppPanel.h"

namespace vle {
namespace gvle {



DefaultCppPanel::DefaultCppPanel():
            PluginMainPanel(), m_edit(0), m_file("")
{

    m_edit = new QTextEdit();
    QObject::connect(m_edit, SIGNAL(undoAvailable(bool)),
                     this, SLOT(onUndoAvailable(bool)));

}

DefaultCppPanel::~DefaultCppPanel()
{

}

void
DefaultCppPanel::init(QString& relPath, utils::Package* pkg, Logger* /*log*/,
        gvle_plugins* /*plugs*/)
{
    QString basepath = pkg->getDir(vle::utils::PKG_SOURCE).c_str();

    m_file = basepath+"/"+relPath;
    QFile cppFile (m_file);

    if (!cppFile.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << " Error DefaultCppPanel::init ";
    }

    QTextStream in(&cppFile);
    m_edit->setText(in.readAll());
}

QString
DefaultCppPanel::canBeClosed()
{
    return "";
}

void
DefaultCppPanel::save()
{
    if (m_file != "") {
        QFile file(m_file);
        if (file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
            file.write(m_edit->toPlainText().toStdString().c_str()) ;
            file.flush();
            file.close();
        }
    }
    emit undoAvailable(false);
}

QString
DefaultCppPanel::getname()
{
    return "Default";
}

QWidget*
DefaultCppPanel::leftWidget()
{
    return m_edit;
}

QWidget*
DefaultCppPanel::rightWidget()
{
    return 0;
}

void
DefaultCppPanel::undo()
{
}

void
DefaultCppPanel::redo()
{

}

void
DefaultCppPanel::onUndoAvailable(bool b)
{
    emit undoAvailable(b);
}

}} //namespaces


