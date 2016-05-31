/*
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems.
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2014 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2014 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2014 INRA http://www.inra.fr
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


#include <vle/devs/StreamWriter.hpp>
#include <vle/devs/Simulator.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Path.hpp>
#include <vle/utils/Algo.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/version.hpp>

namespace vle { namespace devs {

oov::PluginPtr StreamWriter::plugin()
{
    if (not m_plugin) {
        throw utils::InternalError(
            _("Oov: The output plug-in is not initialized, "
              "may be the StreamWriter::open() function was nether called."));
    }

    return m_plugin;
}

void StreamWriter::open(const std::string& pluginname,
                        const std::string& package,
                        const std::string& location,
                        const std::string& file,
                        std::unique_ptr<value::Value> parameters,
                        const devs::Time& time)
{
    void *symbol = nullptr;

    try {
        /* If \e package is not empty we assume that library is the shared
         * library. Otherwise, we load the global symbol stores in \e
         * library/executable and we cast it into a \e
         * vle::oov::OovPluginSlot... Only useful for unit test or to
         * build executable with plugins.
         */
        if (not package.empty())
            symbol = m_modulemgr.get(package, pluginname, utils::MODULE_OOV);
        else
            symbol = m_modulemgr.get(pluginname);

        oov::OovPluginSlot fct(utils::functionCast<oov::OovPluginSlot>(symbol));
        oov::PluginPtr ptr(fct(location));
        m_plugin = ptr;
    } catch(const std::exception& e) {
        throw utils::InternalError(
            (fmt(_("Oov: Can not open the plug-in `%1%': %2%")) % pluginname %
             e.what()).str());
    }

    plugin()->onParameter(pluginname, location, file,
                          std::move(parameters), time);
}

void StreamWriter::processNewObservable(Simulator* simulator,
                                        const std::string& portname,
                                        const devs::Time& time,
                                        const std::string& view)
{
    plugin()->onNewObservable(simulator->getName(),
                              simulator->getParent(),
                              portname, view, time);
}

void StreamWriter::processRemoveObservable(Simulator* simulator,
                                           const std::string& portname,
                                           const devs::Time& time,
                                           const std::string& view)
{
    plugin()->onDelObservable(simulator->getName(),
                              simulator->getParent(),
                              portname, view, time);
}

void StreamWriter::process(Simulator* simulator,
                           const std::string& portname,
                           const devs::Time& time,
                           const std::string& view,
                           std::unique_ptr<value::Value> val)
{
    std::string name, parent;

    if (simulator) {
        name = simulator->getName();
        parent = simulator->getParent();
    }

    /* This is a strange behaviour. If the simulator is destroyed, we call
     * the onValue function with empty simulator and parent.
     */

    plugin()->onValue(name, parent, portname, view, time, std::move(val));
}

void StreamWriter::close(const devs::Time& time)
{
    plugin()->close(time);
}

std::unique_ptr<value::Matrix> StreamWriter::matrix() const
{
    if (m_plugin)
        return m_plugin->matrix();

    return {};
}

}} // namespace vle devs
