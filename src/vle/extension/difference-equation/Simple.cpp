/**
 * @file vle/extension/difference-equation/Simple.cpp
 * @author The VLE Development Team
 * See the AUTHORS or Authors.txt file
 */

/*
 * VLE Environment - the multimodeling and simulation environment
 * This file is a part of the VLE environment
 * http://www.vle-project.org
 *
 * Copyright (C) 2003-2009 ULCO http://www.univ-littoral.fr
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


#include <vle/extension/difference-equation/Simple.hpp>

namespace vle { namespace extension { namespace DifferenceEquation {

using namespace devs;
using namespace value;

Simple::Simple(const DynamicsInit& model,
               const InitEventList& events,
               bool control) :
    Base(model, events, control), mPortName("update"),
    mInitValue(false), mSize(DEFAULT_SIZE)
{
    if (events.exist("name")) {
        mVariableName = toString(events.get("name"));
    } else {
        if (getModel().existOutputPort("update") or
            getModel().getOutputPortNumber() == 0) {
            mVariableName = getModelName();
        } else {

            if (getModel().getOutputPortNumber() > 1) {
                throw utils::ModellingError(
                    fmt(_("[%1%] DifferenceEquation::Simple: invalid "  \
                          "number of output port")) % getModelName());
            }

            mVariableName = getModel().getOutputPortList().begin()->first;
            mPortName = mVariableName;
        }
    }

    if (events.exist("value")) {
        mInitialValue = toDouble(events.get("value"));
        mInitValue = true;
    }

    if (events.exist("init")) {
        const value::Set& init = toSetValue(events.get("init"));
        unsigned int i;

        for (i = 0; i < init.size(); ++i) {
            addValue(toDouble(init.get(i)), mVariableName);
        }
    }
}

void Simple::addValue(double value, const std::string& /* name */)
{
    mValues.push_front(value);
    if (mSize > 0
        and mValues.size() > (unsigned int)mSize) {
        mValues.pop_back();
    }
    mSetValue = true;
}

void Simple::initValues(const vle::devs::Time& time)
{
    if (mInitValue) {
        addValue(mInitialValue, mVariableName);
    } else {
        addValue(initValue(time), mVariableName);
        mInitValue = true;
    }
}

void Simple::removeValue(const std::string& /* name */)
{
    mValues.pop_front();
}

void Simple::removeValues()
{
    mValues.pop_front();
}

void Simple::updateValues(const Time& time)
{
    mSetValue = false;
    addValue(compute(time), mVariableName);
}

void Simple::size(int size)
{
    if (size == 0) {
        throw utils::ModellingError(fmt(_(
                    "[%1%] DifferenceEquation::size - not null size")) %
            getModelName());
    }

    mSize = size;
}

double Simple::val() const
{
    if (not mSetValue) {
        throw utils::InternalError(
            fmt(_("[%1%] DifferenceEquation::getValue - forbidden to use " \
                  "%2%() before computing of %2%"))
            % getModelName() % mVariableName);
    }

    return mValues.front();
}

double Simple::val(int shift) const
{
    if (shift > 0) {
        throw utils::ModellingError(fmt(_(
                    "[%1%] DifferenceEquation::getValue - positive shift on %2%")) %
            getModelName() % mVariableName);
    }

    if (shift == 0 and not mSetValue) {
        throw utils::InternalError(
            fmt(_("[%1%] DifferenceEquation::getValue - forbidden to use " \
                  "%2%() before computing of %2%"))
            % getModelName() % mVariableName);
    }

    ++shift;
    if (shift == 0) {
        return mValues.front();
    } else {
        if ((int)(mValues.size() - 1) < -shift) {
            throw utils::InternalError(fmt(_(
                        "[%1%] - %2%[%3%] - shift too large")) %
                getModelName() % mVariableName % shift);
        }

        return mValues[-shift];
    }
}

void Simple::output(const Time& /* time */,
                    ExternalEventList& output) const
{
    if (mState == SEND_INIT and not mValues.empty()) {
        ExternalEvent* ee = new ExternalEvent(mPortName);

        Set* values = Set::create();
        std::deque < double>::const_iterator it = mValues.begin();

        while (it != mValues.end()) {
            values->addDouble(*it++);
        }
        ee << attribute("name", mVariableName);
        ee << attribute("init", values);
        output.addEvent(ee);
    } else {
        if (mActive and (mState == PRE_INIT or mState == PRE_INIT2
                         or mState == POST or mState == POST2)) {
            ExternalEvent* ee = new ExternalEvent(mPortName);

            ee << attribute("name", mVariableName);
            ee << attribute("value", val());
            output.addEvent(ee);
        }
    }
}

Value* Simple::observation(const ObservationEvent& event) const
{
    if (event.getPortName() != mVariableName) {
        throw utils::InternalError(fmt(_(
                    "[%1%] DifferenceEquation::observation: invalid variable" \
                    " name: %2%")) % getModelName() % event.getPortName());
    }
    return Double::create(val());
}

void Simple::request(const RequestEvent& event,
                     const Time& /*time*/,
                     ExternalEventList& output) const
{
    if (event.getStringAttributeValue("name") != mVariableName) {
        throw utils::InternalError(fmt(_(
                    "[%1%] DifferenceEquation::request - invalid variable" \
                    "name: %2%")) % getModelName() %
            event.getStringAttributeValue("name"));
    }

    ExternalEvent* ee = new ExternalEvent("response");

    ee << attribute("name", mVariableName);
    ee << attribute("value", val());
    output.addEvent(ee);
}

}}} // namespace vle extension DifferenceEquation