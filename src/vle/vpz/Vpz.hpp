/**
 * @file vle/vpz/Vpz.hpp
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


#ifndef VLE_VPZ_VPZ_HPP
#define VLE_VPZ_VPZ_HPP

#include <vle/vpz/Project.hpp>
#include <vle/vpz/SaxParser.hpp>
#include <string>

namespace vle { namespace vpz {

    /**
     * @brief The vpz::Vpz class is the complete representation of the Vpz XML
     * file. To get reference to these objects, uses the project functions.
     */
    class Vpz : public Base
    {
    public:
        /**
         * @brief Build a empty Vpz XML file.
         */
        Vpz() :
            m_isGzip(true)
        {}

        /**
         * @brief Use the filename to build a Vpz XML file.
         * @param filename The filename to open.
         * @throw utils::ArgError if an error occured during loading.
         */
        Vpz(const std::string& filename);

        /**
         * @brief Copy constructor of the Vpz XML file.
         * @param vpz The source to copy.
         */
        Vpz(const Vpz& vpz);

        /**
         * @brief Nothing to delete.
         */
        virtual ~Vpz()
        {}

        /**
         * @brief Write into the output stream the XML representation of this
         * class. It write first, the xml tag and doctype before writing the
         * vpz::Project object.
         * @param out The output parameter where send XML representation.
         */
        virtual void write(std::ostream& out) const;

        /**
         * @brief Get the type of this object.
         * @return Return VPZ.
         */
        virtual Base::type getType() const
        { return VPZ; }

        /**
         * @brief Open a VPZ file project.
         * @param filename file to read.
         * @throw utils::ArgError if an error occured during loading.
         */
        void parseFile(const std::string& filename);

        /**
         * @brief Open a VPZ from a buffer.
         * @param buffer the buffer to parse XML.
         * @throw utils::ArgError if an error occured during loading.
         */
        void parseMemory(const std::string& buffer);

        /**
         * @brief Write file into the current VPZ filename open.
         */
        void write();

        /**
         * @brief Write file into the specified filename open.
         * @param filename file to write.
         */
        void write(const std::string& filename);

        /**
         * @brief Build an string with the write function.
         * @return A string representation of the XML.
         */
        std::string writeToString() const;

        /**
         * @brief Delete all information from this VPZ.
         */
        void clear();

        /**
         * @brief Return true if the file is compressed.
         * @return true if the file is compressed, false otherwise.
         */
        inline bool isGzip() const
        { return m_isGzip; }

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Static methods
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Parse the buffer to find a value.
         * @param buffer the buffer to translate.
         * @throw utils::ArgError if buffer is not a value.
         * @return value read from buffer.
         */
        static value::Value* parseValue(const std::string& buffer);

        /**
         * @brief Parse the buffer to find a list of values.
         * @param buffer the buffer to translate.
         * @throw utils::ArgError if buffer is not a value.
         * @return a vector of values read from buffer.
         */
        static std::vector < value::Value* > parseValues(
            const std::string& buffer);

        /**
         * @brief Add the vpz extension to filename if does not exist, If
         * correct filename is passed, no modification is apply.
         * @param filename string to change if no extension exist.
         */
        static void fixExtension(std::string& filename);

        /**
         * @brief Use the libxml++ DOM parser to check the validation of the VPZ
         * file.
         * @param filename the file name to test.
         * @throw std::exception on error.
         */
        static void validateFile(const std::string& filename);

        /**
         * @brief Use the libxml++ DOM parser to check the validation of the VPZ
         * buffer.
         * @param buffer the content of the XML.
         * @throw std::exception on error.
         */
        static void validateMemory(const Glib::ustring& buffer);

        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
         *
         * Get/Set functions
         *
         * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

        /**
         * @brief Assign a new filename when write file.
         * @param filename Assign a new filename when write file.
         */
        inline void setFilename(const std::string& filename)
        { m_filename.assign(filename); }

        /**
         * @brief Get the current filename.
         * @return The current filename.
         */
        inline const std::string& filename() const
        { return m_filename; }

        /**
         * @brief Get a constant reference to the vpz::Project.
         * @return A constant reference to the vpz::Project.
         */
        inline const Project& project() const
        { return m_project; }

        /**
         * @brief Get a reference to the vpz::Project.
         * @return A reference to the vpz::Project.
         */
        inline Project& project()
        { return m_project; }

    private:
        bool                m_isGzip;
        std::string         m_filename;
        vpz::Project        m_project;
    };

}} // namespace vle vpz

#endif
