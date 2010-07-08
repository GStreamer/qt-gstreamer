/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "closure.h"
#include "quark.h"
#include <glib-object.h>
#include <QtCore/QDebug>

namespace QGlib {

void Closure::ref()
{
    g_closure_ref(static_cast<GClosure*>(m_object));
}

void Closure::unref()
{
    g_closure_unref(static_cast<GClosure*>(m_object));
}


namespace Private {

static void c_marshaller(GClosure *closure, GValue *returnValue, uint paramValuesCount,
                         const GValue *paramValues, void *hint, void *data)
{
    Q_UNUSED(data);

    ClosureDataBase *cdata = static_cast<ClosureDataBase*>(closure->data);

    QList<Value> params;
    for(uint i=0; i<paramValuesCount; ++i) {
        params.append(Value(&paramValues[i]));
    }

    try {
        SharedValue result(returnValue);
        (*cdata->cppMarshaller)(result, params, cdata);
    } catch (const std::logic_error & e) {
        QString signalName;
        if (hint != NULL) {
            GSignalInvocationHint *ihint = static_cast<GSignalInvocationHint*>(hint);

            GSignalQuery query;
            g_signal_query(ihint->signal_id, &query);
            signalName = QString::fromUtf8(query.signal_name);

            if (ihint->detail != 0) {
                Quark q(ihint->detail);
                signalName.append(QLatin1String("::"));
                signalName.append(q.toString());
            }
        }

        QString instanceName;
        const Value & instanceValue = params.at(0);
        if (instanceValue.type().isInstantiatable() && instanceValue.canTransformTo(Type::String)) {
            //instances can be transformed to strings for debugging purposes
            instanceName = instanceValue.transformTo(Type::String).get<QString>();
        }

        //attempt to determine the cause of the failure
        QString msg;
        try {
            //dynamic_cast will throw an std::bad_cast if it fails
            dynamic_cast<const ValueBase::InvalidTypeException &>(e);
            //cast succeded, e is indeed an InvalidTypeException
            msg = QLatin1String("One or more of the arguments of the signal are of different "
                                "type than the type that the closure expects");
        } catch (...) {
            try {
                dynamic_cast<const ValueBase::InvalidValueException &>(e);
                //cast succeded, e is indeed an InvalidValueException
                //this is most likely to happen because the signal returns void
                //but the closure returns something non-void. check this first.
                if (returnValue == NULL) {
                    msg = QLatin1String("The signal is defined to return void but the "
                                        "closure returns something non-void");
                } else {
                    msg = QLatin1String("One of the arguments of the signal was not a valid GValue. "
                                        "This is most likely a bug in the code that invoked the signal.");
                }
            } catch (...) {
                msg = QString::fromAscii(e.what());
            }
        }

        qCritical() << "Error during invocation of closure connected to signal"
                    << signalName << "from object" << instanceName << ":" << msg;
    }
}

static void closureDestroyNotify(void *data, GClosure *closure)
{
    Q_UNUSED(data);
    delete static_cast<ClosureDataBase*>(closure->data);
}

ClosurePtr createCppClosure(ClosureDataBase *closureData)
{
    GClosure *closure = g_closure_new_simple(sizeof(GClosure), closureData);
    g_closure_set_marshal(closure, &c_marshaller);
    g_closure_add_finalize_notifier(closure, NULL, &closureDestroyNotify);
    ClosurePtr result = ClosurePtr::wrap(closure);
    g_closure_sink(closure);
    return result;
}

} //namespace Private
} //namespace QGlib
