/**
 * @file vle/vpz/SaxParser.hpp
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


#ifndef VLE_VPZ_SAXPARSER_HPP
#define VLE_VPZ_SAXPARSER_HPP

#include <stack>
#include <libxml++/libxml++.h>
#include <vle/vpz/Base.hpp>
#include <vle/vpz/SaxStackValue.hpp>
#include <vle/vpz/SaxStackVpz.hpp>
#include <vle/value/Value.hpp>
#include <vle/value/Set.hpp>
#include <vle/value/Map.hpp>

namespace vle { namespace vpz {

    class Vpz;

    /**
     * @brief The SaxParser inherits from the xmlpp::SaxParser class of the
     * libxml++ library which implements a SAX2 parser.
     */
    class SaxParser : public xmlpp::SaxParser
    {
    public:
        /**
         * @brief Build a new SaxParser to fill the specific Vpz class.
         * @param vpz The Vpz class to fill when reading.
         */
        SaxParser(Vpz& vpz);

        /**
         * @brief Nothing to delete.
         */
        virtual ~SaxParser()
        {}

        /**
         * @brief Delete all information from Sax parser like stack, etc. Use it
         * before restart a new parse.
         */
        void clearParserState();

        /**
         * @brief When read a start document tag.
         */
        virtual void on_start_document();

        /**
         * @brief When read a end document tag.
         */
        virtual void on_end_document();

        /**
         * @brief When read a start element.
         * @param name the name of the element.
         * @param attributes the list of attributes.
         */
        virtual void on_start_element(const Glib::ustring& name,
                                      const AttributeList& attributes);

        /**
         * @brief When read an end element.
         * @param name the name if the element.
         */
        virtual void on_end_element(const Glib::ustring& name);

        /**
         * @brief When read characters in an element.
         * @param characters all data in characters.
         */
        virtual void on_characters(const Glib::ustring& characters);

        /**
         * @brief When read a comment.
         * @param text the comment.
         */
        virtual void on_comment(const Glib::ustring& text);

        /**
         * @brief When a warning appear in Sax parser.
         * @param text the comment.
         */
        virtual void on_warning(const Glib::ustring& text);

        /**
         * @brief When an error appear in Sax parser.
         * @param text the comment.
         */
        virtual void on_error(const Glib::ustring& text);

        /**
         * @brief When an fatal error appear in Sax parser.
         * @param text the comment.
         */
        virtual void on_fatal_error(const Glib::ustring& text);

        /**
         * @brief When a cdata is readed.
         * @param text the comment.
         */
        virtual void on_cdata_block(const Glib::ustring& text);

        /**
         * @brief When a validity error appear in Sax parser.
         * @param text the comment.
         */
        virtual void on_validity_error(const Glib::ustring& text);

        /**
         * @brief When an validity warning appear in Sax parser.
         * @param text the comment.
         */
        virtual void on_validity_warning(const Glib::ustring& text);

        /**
         * @brief Return true if the SaxParser have read a value.
         * @return true if the parser have read a value, false otherwise.
         */
        inline bool isValue() const
        { return m_isValue; }

        /**
         * @brief Return true if the SaxParser have read a vpz file.
         * @return true if the parser have read a vpz file, false otherwise.
         */
        inline bool isVpz() const
        { return m_isVPZ; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions.
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Get the content of the SaxStackValue.
         * @return A constant reference to the vector of value::Value.
         */
        const std::vector < value::Value* >& getValues() const;

        /**
         * @brief Get the content of the SaxParserError.
         * @return A reference to the vector of value::Value.
         */
        std::vector < value::Value* >& getValues();

        /**
         * @brief Get the content of a specific cells of the SaxStackValue.
         * @param pos The index of the value::Value.
         * @throw utils::SaxParserError if i is greater that size of the vector.
         * @return A constant reference to the value.
         */
        value::Value* getValue(const size_t pos) const;

        /**
         * @brief Get a constant reference to the Vpz.
         * @return A constant reference to the Vpz.
         */
        inline const vpz::Vpz& vpz() const
        { return m_vpz; }

    private:
        /**
         * @brief Get the last characters from internal buffer.
         * @return A characters buffer.
         */
        const Glib::ustring& lastCharactersStored() const;

        /**
         * @brief Clear the internal buffer for last characters.
         */
        void clearLastCharactersStored();

        void addToCharacters(const Glib::ustring& characters);

        SaxStackVpz                     m_vpzstack;
        ValueStackSax                   m_valuestack;
        Glib::ustring                   m_lastCharacters;
        Glib::ustring                   m_cdata;
        Vpz&                            m_vpz;

        bool                            m_isValue;
        bool                            m_isVPZ;

        typedef void (SaxParser::* startfunc)(const AttributeList&);
        typedef void (SaxParser::* endfunc)();
        typedef std::map < std::string, startfunc > StartFuncList;
        typedef std::map < std::string, endfunc > EndFuncList;

        StartFuncList   m_starts;
        EndFuncList     m_ends;

        void onBoolean(const AttributeList& att);
        void onInteger(const AttributeList& att);
        void onDouble(const AttributeList& att);
        void onString(const AttributeList& att);
        void onSet(const AttributeList& att);
        void onMatrix(const AttributeList& att);
        void onMap(const AttributeList& att);
        void onKey(const AttributeList& att);
        void onTuple(const AttributeList& att);
        void onTable(const AttributeList& att);
        void onXML(const AttributeList& att);
        void onNull(const AttributeList& att);
        void onVLEProject(const AttributeList& att);
        void onStructures(const AttributeList& att);
        void onModel(const AttributeList& att);
        void onIn(const AttributeList& att);
        void onOut(const AttributeList& att);
        void onPort(const AttributeList& att);
        void onSubModels(const AttributeList& att);
        void onConnections(const AttributeList& att);
        void onConnection(const AttributeList& att);
        void onOrigin(const AttributeList& att);
        void onDestination(const AttributeList& att);
        void onDynamics(const AttributeList& att);
        void onDynamic(const AttributeList& att);
        void onExperiment(const AttributeList& att);
        void onReplicas(const AttributeList& att);
        void onConditions(const AttributeList& att);
        void onCondition(const AttributeList& att);
        void onViews(const AttributeList& att);
        void onOutputs(const AttributeList& att);
        void onOutput(const AttributeList& att);
        void onView(const AttributeList& att);
        void onObservables(const AttributeList& att);
        void onObservable(const AttributeList& att);
        void onAttachedView(const AttributeList& att);
        void onClasses(const AttributeList& att);
        void onClass(const AttributeList& att);

        void onEndBoolean();
        void onEndInteger();
        void onEndDouble();
        void onEndString();
        void onEndSet();
        void onEndMatrix();
        void onEndMap();
        void onEndKey();
        void onEndTuple();
        void onEndTable();
        void onEndXML();
        void onEndNull();
        void onEndVLEProject();
        void onEndStructures();
        void onEndModel();
        void onEndIn();
        void onEndOut();
        void onEndPort();
        void onEndSubModels();
        void onEndConnections();
        void onEndConnection();
        void onEndOrigin();
        void onEndDestination();
        void onEndDynamics();
        void onEndDynamic();
        void onEndExperiment();
        void onEndReplicas();
        void onEndConditions();
        void onEndCondition();
        void onEndViews();
        void onEndOutputs();
        void onEndOutput();
        void onEndView();
        void onEndObservables();
        void onEndObservable();
        void onEndAttachedView();
        void onEndClasses();
        void onEndClass();

        void add(const std::string& name, startfunc fct, endfunc efct)
        {
            m_starts[name] = fct;
            m_ends[name] = efct;
        }

        void fillTagList()
        {
            add("boolean", &SaxParser::onBoolean, &SaxParser::onEndBoolean);
            add("integer", &SaxParser::onInteger, &SaxParser::onEndInteger);
            add("double", &SaxParser::onDouble, &SaxParser::onEndDouble);
            add("string", &SaxParser::onString, &SaxParser::onEndString);
            add("set", &SaxParser::onSet, &SaxParser::onEndSet);
            add("matrix", &SaxParser::onMatrix, &SaxParser::onEndMatrix);
            add("map", &SaxParser::onMap, &SaxParser::onEndMap);
            add("key", &SaxParser::onKey, &SaxParser::onEndKey);
            add("tuple", &SaxParser::onTuple, &SaxParser::onEndTuple);
            add("table", &SaxParser::onTable, &SaxParser::onEndTable);
            add("xml", &SaxParser::onXML, &SaxParser::onEndXML);
            add("null", &SaxParser::onNull, &SaxParser::onEndNull);
            add("vle_project", &SaxParser::onVLEProject,
                &SaxParser::onEndVLEProject);
            add("structures", &SaxParser::onStructures,
                &SaxParser::onEndStructures);
            add("model", &SaxParser::onModel, &SaxParser::onEndModel);
            add("in", &SaxParser::onIn, &SaxParser::onEndIn);
            add("out", &SaxParser::onOut, &SaxParser::onEndOut);
            add("port", &SaxParser::onPort, &SaxParser::onEndPort);
            add("submodels", &SaxParser::onSubModels,
                &SaxParser::onEndSubModels);
            add("connections", &SaxParser::onConnections,
                &SaxParser::onEndConnections);
            add("connection", &SaxParser::onConnection,
                &SaxParser::onEndConnection);
            add("origin", &SaxParser::onOrigin, &SaxParser::onEndOrigin);
            add("destination", &SaxParser::onDestination,
                &SaxParser::onEndDestination);
            add("dynamics", &SaxParser::onDynamics, &SaxParser::onEndDynamics);
            add("dynamic", &SaxParser::onDynamic, &SaxParser::onEndDynamic);
            add("experiment", &SaxParser::onExperiment,
                &SaxParser::onEndExperiment);
            add("replicas", &SaxParser::onReplicas, &SaxParser::onEndReplicas);
            add("conditions", &SaxParser::onConditions,
                &SaxParser::onEndConditions);
            add("condition", &SaxParser::onCondition, &SaxParser::onEndCondition);
            add("views", &SaxParser::onViews, &SaxParser::onEndViews);
            add("outputs", &SaxParser::onOutputs, &SaxParser::onEndOutputs);
            add("output", &SaxParser::onOutput, &SaxParser::onEndOutput);
            add("view", &SaxParser::onView, &SaxParser::onEndView);
            add("observables", &SaxParser::onObservables,
                &SaxParser::onEndObservables);
            add("observable", &SaxParser::onObservable,
                &SaxParser::onEndObservable);
            add("attachedview", &SaxParser::onAttachedView,
                &SaxParser::onEndAttachedView);
            add("classes", &SaxParser::onClasses, &SaxParser::onEndClasses);
            add("class", &SaxParser::onClass, &SaxParser::onEndClass);
        }
    };

    /**
     * @brief A template function de get a value of an attribute from an
     * attribute list. The value of the attribute found is casted in template by
     * the boost::lexical_cast function.
     * @param lst the list of attribute to search an attribute
     * @param name the name of the attribute to get value
     * @return the value of the attribute.
     * @throw utils::SaxParserError if the attribute was not found or if the
     * boost::lexical_cast failed.
     */
    template < typename T > T getAttribute(const AttributeList& lst,
                                           const Glib::ustring& name)
    {
        AttributeList::const_iterator it;
        it = std::find_if(lst.begin(), lst.end(),
                          xmlpp::SaxParser::AttributeHasName(name));
        Assert(utils::SaxParserError, it != lst.end(),
               (boost::format("Unknow attribute '%1%'") % name).str());

        T result;
        try {
            result = boost::lexical_cast < T >((*it).value);
        } catch(const std::exception& e) {
            Throw(utils::SaxParserError, (boost::format(
                        "Cannot convert '%1%' into desired type: %2%") %
                    name % e.what()).str());
        }
        return result;
    }

    /**
     * @brief Search an attribute in an attribute list.
     * @param lst the list of attribute to search an attribute.
     * @param name the attribute to search in attribute list.
     * @return true if the attribute exist in attribute list.
     */
    bool existAttribute(const AttributeList& lst, const Glib::ustring& name);

}} // namespace vle vpz

#endif
