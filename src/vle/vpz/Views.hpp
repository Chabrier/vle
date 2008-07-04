/**
 * @file src/vle/vpz/Views.hpp
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




#ifndef VLE_VPZ_VIEWS_HPP
#define VLE_VPZ_VIEWS_HPP

#include <list>
#include <vector>
#include <vle/vpz/Outputs.hpp>
#include <vle/vpz/Observables.hpp>
#include <vle/vpz/View.hpp>

namespace vle { namespace vpz {

    /**
     * @brief A list of View.
     */
    typedef std::map < std::string, View > ViewList;

    /**
     * @brief Views is a container based on ViewList to build a list of View
     * using the View's name as key. The Views class store Outputs and
     * Observables objects too.
     */
    class Views : public Base
    {
    public:
        Views()
        { }

        virtual ~Views()
        { }

        virtual void write(std::ostream& out) const;

        virtual Base::type getType() const
        { return VIEWS; }

        ////
        //// Manage outputs objects.
        ////

        Output& addLocalStreamOutput(const std::string& name,
                                     const std::string& location,
                                     const std::string& plugin);

        Output& addDistantStreamOutput(const std::string& name,
                                       const std::string& location,
                                       const std::string& plugin);

        void delOutput(const std::string& name);

        inline const Outputs& outputs() const
        { return m_outputs; }

        inline Outputs& outputs()
        { return m_outputs; }

        ////
        //// Manage observables objects.
        ////

        Observable& addObservable(const Observable& obs);

        Observable& addObservable(const std::string& name);

        void delObservable(const std::string& name);

        inline const Observables& observables() const
        { return m_observables; }

        inline Observables& observables()
        { return m_observables; }

        ////
        //// Manage views objects.
        ////

        inline const ViewList& viewlist() const
        { return m_list; }

        void add(const Views& views);

        View& add(const View& view);

        void clear();

        View& addEventView(const std::string& name,
                           const std::string& output);

        View& addTimedView(const std::string& name,
                           double timestep,
                           const std::string& output);

        View& addFinishView(const std::string& name,
                            const std::string& output);

        void del(const std::string& name);

        const View& get(const std::string& name) const;

        View& get(const std::string& name);

        inline bool exist(const std::string& name) const
        { return m_list.find(name) != m_list.end(); }

        /**
         * @brief Check if a View of the ViewList use the specific output.
         * @param name The name of the Output to check.
         * @return True if a View use an output with the, otherwise, return
         * false.
         */
        bool isUsedOutput(const std::string& name) const;

        ////
        //// Functors
        ////

        /** 
         * @brief A functor to add a ViewList::value_type of a Views. To use
         * with std::for_each.
         */
        struct AddViews {
            inline AddViews(Views& views) :
                m_views(views) { }

            inline void operator()(const ViewList::value_type& pair)
            { m_views.add(pair.second); }

            Views& m_views;
        };

        /** 
         * @brief A functor to test if a ViewList::value_type use the specific
         * output. To use with std::find_if.
         */
        struct UseOutput {
            inline UseOutput(const std::string& output) :
                m_output(output) { }

            inline bool operator()(const ViewList::value_type& pair) const
            { return pair.second.output() == m_output; }

            const std::string& m_output;
        };

    private:
        ViewList        m_list;
        Outputs         m_outputs;
        Observables     m_observables;
    };

}} // namespace vle vpz

#endif
