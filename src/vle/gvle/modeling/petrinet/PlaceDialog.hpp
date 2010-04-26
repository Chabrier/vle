/**
 * @file vle/gvle/modeling/petrinet/PlaceDialog.hpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2010 ULCO http://www.univ-littoral.fr
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


#ifndef VLE_GVLE_MODELING_PETRINET_PLACE_DIALOG_HPP
#define VLE_GVLE_MODELING_PETRINET_PLACE_DIALOG_HPP

#include <vle/gvle/modeling/petrinet/PetriNet.hpp>
#include <gtkmm/checkbutton.h>
#include <gtkmm/comboboxentrytext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <libglademm.h>

namespace vle { namespace gvle { namespace modeling { namespace petrinet {

class PlaceDialog
{
public:
    PlaceDialog(const Glib::RefPtr < Gnome::Glade::Xml >& xml,
                const PetriNet& petrinet, const Place* place = 0);
    virtual ~PlaceDialog();

    bool delay() const
    { return mDelayCheckbox->get_active(); }

    std::string delayValue() const
    { return mDelayEntry->get_text(); }

    std::string initialMarking() const
    { return mMarkingEntry->get_text(); }

    std::string name() const
    { return mNameEntry->get_text(); }

    bool output() const
    { return mOutputCheckbox->get_active(); }

    std::string outputPortName() const
    { return mOutputEntry->get_entry()->get_text(); }

    int run();

    bool valid() const
    {
        return not (mNameEntry->get_text().empty() or
            (mPetriNet.existPlace(name()) and
             mNameEntry->get_text() != mInitialName));
    }

private:
    void onChangeName();
    void onDelay();
    void onInput();
    void onOutput();
    void setStatus();

    Gtk::Dialog*            mDialog;
    Gtk::Entry*             mNameEntry;
    Gtk::Image*             mStatusName;
    // output
    Gtk::HBox*              mOutputHBox;
    Gtk::CheckButton*       mOutputCheckbox;
    Gtk::ComboBoxEntryText* mOutputEntry;
    // marking
    Gtk::Entry*             mMarkingEntry;
    // delay
    Gtk::CheckButton*       mDelayCheckbox;
    Gtk::Entry*             mDelayEntry;

    const PetriNet&         mPetriNet;
    const Place*            mPlace;
    std::string             mInitialName;

    std::list < sigc::connection > mList;
};

}}}} // namespace vle gvle modeling petrinet

#endif