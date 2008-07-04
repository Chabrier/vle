/**
 * @file src/vle/vpz/View.cpp
 * @author The VLE Development Team
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment (http://vle.univ-littoral.fr)
 * Copyright (C) 2003 - 2008 The VLE Development Team
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */




#include <vle/vpz/View.hpp>
#include <vle/vpz/Views.hpp>
#include <vle/utils/Debug.hpp>
#include <glib/gmessages.h>

namespace vle { namespace vpz {

View::View(const std::string& name,
           View::Type type,
           const std::string& output,
           double timestep) :
    m_name(name),
    m_type(type),
    m_output(output),
    m_timestep(timestep)
{
    if (m_type == View::TIMED) {
        Assert(utils::SaxParserError, m_timestep > 0.0, boost::format(
                "Cannont define the View '%1%' with a timestep '%2%'") % m_name
            % m_timestep);
    }
}

void View::write(std::ostream& out) const
{
    out << "<view "
        << "name=\"" << m_name << "\" "
        << "output=\"" << m_output << "\" ";

    switch (m_type) {
    case View::EVENT:
        out << "type=\"event\"";
        break;
    case View::TIMED:
        out << "type=\"timed\" "
            << "timestep=\"" << m_timestep << "\"";
        break;
    case View::FINISH:
        out << "type=\"finish\"";
        break;
    }

    if (m_data.empty()) {
        out << " />\n";
    } else {
        out << ">\n"
            << "<![CDATA[\n"
            << m_data
            << "]]>\n"
            << "</view>\n";
    }
}

void View::setTimestep(double time)
{
    Assert(utils::SaxParserError, time > 0.0,
           (boost::format("Bad time step %1% for view %2%")
            % time % m_name));

    m_timestep = time;
}

void View::setEventView(const std::string& output)
{
    g_warning("Deprecated: Dont use View::setEventView function");

    setType(EVENT);
    setOutput(output);
}

void View::setTimedView(double timestep, const std::string& output)
{
    g_warning("Deprecated: Dont use View::setTimedView function");

    setType(TIMED);
    setTimestep(timestep);
    setOutput(output);
}

void View::setFinishView(const std::string& output)
{
    g_warning("Deprecated: Dont use View::setFinishView function");

    setType(FINISH);
    setOutput(output);
}

void View::setOutput(const std::string& output)
{
    g_warning("Deprecated: Dont use View::setOutput function");

    Assert(utils::SaxParserError, not output.empty(), boost::format(
                "Bad output '%1%' for the view '%2%'") % output % m_name);

    m_output = output;
}

}} // namespace vle vpz
