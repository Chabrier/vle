/**
 * @file vle/oov/NetStreamReader.cpp
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


#include <vle/oov/NetStreamReader.hpp>
#include <vle/oov/Plugin.hpp>
#include <vle/utils/Socket.hpp>
#include <vle/utils/Trace.hpp>
#include <vle/value.hpp>
#include <vle/utils/Debug.hpp>
#include <boost/format.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/export.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <config.h>

#ifdef HAVE_CAIRO
#   include <vle/oov/CairoPlugin.hpp>
#endif


namespace vle { namespace oov {

NetStreamReader::NetStreamReader(int port) :
    m_port(port),
    m_server(new utils::net::Server(port)),
    m_image(0)
{
    setBufferSize(4096);
    TraceAlways(boost::format("Build a NetStreamReader on port %1%") % port);

    // TODO: why segfault without this... Very very strange.
    value::Map* m = new value::Map;
    value::Set* s = new value::Set;
    value::Integer* i = new value::Integer;
    value::Double* d = new value::Double;
    value::Boolean* b = new value::Boolean;
    value::String* s2 = new value::String;
    value::Xml* x = new value::Xml;
    value::Null* n = new value::Null;
    value::Table* t = new value::Table;
    value::Tuple* t2 = new value::Tuple;
    value::Matrix* m2 = new value::Matrix;

    delete m;
    delete s;
    delete i;
    delete d;
    delete b;
    delete s2;
    delete x;
    delete n;
    delete t;
    delete t2;
    delete m2;
}

NetStreamReader::~NetStreamReader()
{
    if (m_server) {
        delete m_server;
    }
}

void NetStreamReader::process()
{
    for (; ;) {
        waitConnection();
        readConnection();
        closeConnection();
    }
}

void NetStreamReader::waitConnection()
{
    m_server->accept_client("vle");
    TraceAlways(boost::format(
            "NetStreamReader connection with client %1%") % "vle");
}

void NetStreamReader::readConnection()
{
    bool stop = false;

    while (not stop) {
        try {
            guint32 val = m_server->recv_int("vle");

            Assert(utils::InternalError, val > 0,
                   "NetStreamReader: bad size for package");

            m_buffer = m_server->recv_buffer("vle", val);

            std::istringstream in(m_buffer, std::istringstream::binary);
            boost::archive::binary_iarchive ia(in);
            value::Set frame;
            ia >> (value::Set&)frame;

            stop = dispatch(frame);
        } catch (const std::exception& e) {
            Throw(utils::InternalError, boost::format(
                    "NetStreamReader: %1%") % e.what());
        }
    }
}

bool NetStreamReader::dispatch(value::Set& frame)
{
    bool end = false;

    switch (frame.get(0).toInteger().value()) {
    case 0:
        onParameter(frame.get(1).toString().value(),
                    frame.get(2).toString().value(),
                    frame.get(3).toString().value(),
                    frame.get(4).toXml().value(),
                    frame.get(5).toDouble().value());
        break;
    case 1:
        onNewObservable(frame.get(1).toString().value(),
                        frame.get(2).toString().value(),
                        frame.get(3).toString().value(),
                        frame.get(4).toString().value(),
                        frame.get(5).toDouble().value());
        break;
    case 2:
        onDelObservable(frame.get(1).toString().value(),
                        frame.get(2).toString().value(),
                        frame.get(3).toString().value(),
                        frame.get(4).toString().value(),
                        frame.get(5).toDouble().value());
        break;
    case 3:
        onValue(frame.get(1).toString().value(),
                frame.get(2).toString().value(),
                frame.get(3).toString().value(),
                frame.get(4).toString().value(),
                frame.get(5).toDouble().value(),
                frame.give(6));
        break;
    case 4:
        serializePlugin();
        onClose(frame.get(1).toDouble().value());
        end = true;
        break;
    case 5:
        serializePlugin();
        break;
    default:
        Throw(utils::InternalError, boost::format(
                "NetStreamReader: unknow tag '%1%'") %
            frame.get(0).toInteger().value());
    }
    return end;
}

void NetStreamReader::closeConnection()
{
    m_server->close_client("vle");
}

void NetStreamReader::close()
{
    delete m_server;
    m_server = 0;
}

void NetStreamReader::onValue(const std::string& simulator,
                              const std::string& parent,
                              const std::string& port,
                              const std::string& view,
                              const double& time,
                              value::Value* value)
{
    plugin()->onValue(simulator, parent, port, view, time, value);

#ifdef HAVE_CAIRO
    //if (plugin()->isCairo()) {
    //CairoPluginPtr plg = toCairoPlugin(plugin());
    //if (plg and plg->context() and plg->context()->get_target()) {
    //plg->context()->get_target()->write_to_png((boost::format(
    //"%1%_%2$05d.png") % plg->location() % m_image).str());
    //m_image++;
    //}
    //}
#endif
}

void NetStreamReader::serializePlugin()
{
    if (plugin()->isSerializable()) {
        m_server->send_buffer("vle", "ok");
        m_server->recv_string("vle");

        value::Set* vals = value::Set::create();
        vals->add(value::String::create(plugin()->name()));
        vals->add(plugin()->serialize());

        std::ostringstream out(std::ostringstream::binary);
        boost::archive::binary_oarchive oa(out);
        oa << (const value::Set&)*vals;
        delete vals;

        m_server->send_int("vle", out.str().size());
        m_server->recv_string("vle");
        m_server->send_buffer("vle", out.str());
        m_server->recv_string("vle");
    } else {
        m_server->send_buffer("vle", "no");
    }
}

void NetStreamReader::setBufferSize(size_t buffer)
{
    Assert(utils::InternalError, buffer != 0,
           "Cannot build a buffer with an empty size");

    Assert(utils::InternalError, buffer < m_buffer.max_size(), boost::format(
            "Cannot allocate a buffer of size %1%") % buffer);

    m_buffer.reserve(buffer);
}

}} // namespace vle oov
