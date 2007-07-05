/** 
 * @file MatrixTranslator.hpp
 * @brief 
 * @author The vle Development Team
 * @date jeu, 05 jui 2007 17:12:53 +0200
 */

/*
 * Copyright (C) 2007 - The vle Development Team
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP
#define VLE_TRANSLATOR_MATRIXTRANSLATOR_HPP

#include <vle/vpz/Translator.hpp>
#include <vle/utils/XML.hpp>
#include <string>
#include <vector>

namespace vle { namespace translator {

    class MatrixTranslator : public vpz::Translator
    {
    public:
        MatrixTranslator(const vpz::Project& prj);

        virtual ~MatrixTranslator();

        virtual void translate(const std::string& buffer);

    protected:
        bool existModel(unsigned int i, unsigned int j = 0);
        std::string getDynamics(unsigned int i, unsigned int j = 0);
        std::string getName(unsigned int i, unsigned int j = 0) const;

        virtual void parseXML(const std::string& buffer);
        virtual void translateStructures();
        virtual void translateDynamics();
        virtual void translateTranslators() { }
        virtual void translateConditions();
        virtual void translateViews() { }
        virtual void translateFinish() { }

        typedef enum { VON_NEUMANN, MOORE, LINEAR } connectivity_type;

        xmlpp::DomParser m_parser;
        xmlpp::Element* m_root;
        unsigned int m_dimension;
        std::map < unsigned int, unsigned int > m_size;
        connectivity_type m_connectivity;
        bool m_multipleLibrary;
        std::string m_library;
        std::map < unsigned int , std::string > m_libraries;
        std::string m_prefix;
        unsigned int* m_init;
        std::map < std::string , vpz::AtomicModel* > m_models;
    };

}} // namespace vle translator

#endif
