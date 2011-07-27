/*
 * @file examples/recursives/SimpleSimulation.cpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
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


#include <vle/devs/Dynamics.hpp>
#include <vle/vpz/Vpz.hpp>
#include <vle/manager/Run.hpp>
#include <vle/utils/Path.hpp>

namespace vle { namespace examples { namespace recursives {

class SimpleSimulation : public devs::Dynamics
{
public:
    SimpleSimulation(const devs::DynamicsInit& mdl,
                     const devs::InitEventList& lst)
        : devs::Dynamics(mdl, lst)
    {}

    virtual ~SimpleSimulation()
    {}

    devs::Time init(const devs::Time& /* time */)
    {
        vpz::Vpz file(utils::Path::path().getExampleFile("counter.vpz"));

        utils::ModuleManager man;
        manager::RunQuiet r(man);    // build a VLE manager
        r.start(file);          // exacute the local file simulation

        // get output of the simulation
        const oov::OutputMatrixViewList& result(r.outputs());

        // Show the results
        std::for_each(result.begin(), result.end(), ShowMatrix());

        // le modèle ne fait rien d'autre.
        return devs::Time::infinity;
    }

    /**
     * @brief A functor to show the matrix result.
     */
    struct ShowMatrix {
        void operator()(
            const oov::OutputMatrixViewList::value_type& x) const
        {
            std::cout << "view: " << x.first << "\n"
                << x.second.serialize()->writeToString() << "\n";
        }
    };
};

}}} // namespace vle examples recursives

DECLARE_DYNAMICS(vle::examples::recursives::SimpleSimulation)
