/**
 * @file vle/value/XML.cpp
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


#include <vle/value/XML.hpp>

namespace vle { namespace value {

void Xml::writeFile(std::ostream& out) const
{
    out << m_value;
}

void Xml::writeString(std::ostream& out) const
{
    out << m_value;
}

void Xml::writeXml(std::ostream& out) const
{
    out << "<xml>\n<![CDATA[" << m_value.c_str() << "\n]]>\n</xml>";
}

}} // namespace vle value

BOOST_CLASS_EXPORT(vle::value::Xml)

