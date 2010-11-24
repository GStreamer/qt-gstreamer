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
#include "signal.h"
#include "quark.h"
#include <glib-object.h>
#include <QtCore/QStringList>
#include <QtCore/QDebug>

namespace QGlib {

//BEGIN ******** SignalHandler ********

bool SignalHandler::isConnected() const
{
    return g_signal_handler_is_connected(m_instance, m_id);
}

void SignalHandler::disconnect()
{
    g_signal_handler_disconnect(m_instance, m_id);
}

void SignalHandler::block()
{
    g_signal_handler_block(m_instance, m_id);
}

void SignalHandler::unblock()
{
    g_signal_handler_unblock(m_instance, m_id);
}

//END ******** SignalHandler ********
//BEGIN ******** Signal ********

struct Signal::Private : public QSharedData
{
    Private(uint i) : id(i), m_queryInitialized(false) {}

    uint id;
    GSignalQuery *query() const;

private:
    mutable GSignalQuery m_query;
    mutable bool m_queryInitialized;
};

GSignalQuery *Signal::Private::query() const
{
    if (!m_queryInitialized) {
        g_signal_query(id, &m_query);
        m_queryInitialized = true;
    }
    return &m_query;
}


Signal::Signal(uint id)
    : d(new Private(id))
{
}

Signal::Signal(const Signal & other)
    : d(other.d)
{
}

Signal & Signal::operator=(const Signal & other)
{
    d = other.d;
    return *this;
}

Signal::~Signal()
{
}

bool Signal::isValid() const
{
    return d->id != 0;
}

uint Signal::id() const
{
    return d->id;
}

QString Signal::name() const
{
    return QString::fromUtf8(d->query()->signal_name);
}

Signal::SignalFlags Signal::flags() const
{
    return QFlag(d->query()->signal_flags);
}

Type Signal::instanceType() const
{
    return d->query()->itype;
}

Type Signal::returnType() const
{
    return d->query()->return_type & ~G_SIGNAL_TYPE_STATIC_SCOPE;
}

QList<Type> Signal::paramTypes() const
{
    QList<Type> result;
    for(uint i=0; i<d->query()->n_params; ++i) {
        result.append(d->query()->param_types[i] & ~G_SIGNAL_TYPE_STATIC_SCOPE);
    }
    return result;
}

//static
Signal Signal::lookup(const char *name, Type type)
{
    return Signal(g_signal_lookup(name, type));
}

//static
QList<Signal> Signal::listSignals(Type type)
{
    QList<Signal> result;
    uint n_ids;
    uint *ids = g_signal_list_ids(type, &n_ids);
    for(uint i=0; i<n_ids; ++i) {
        result.append(Signal(ids[i]));
    }
    g_free(ids);
    return result;
}

//END ******** Signal ********
//BEGIN ******** Signal::emit ********

//static
Value Signal::emit(void *instance, const char *detailedSignal, const QList<Value> & args)
{
    Value result;
    Type itype = Type::fromInstance(instance);
    QStringList signalParts = QString::fromUtf8(detailedSignal).split(QLatin1String("::"));
    Quark detail;
    if (signalParts.size() > 1) {
        detail = Quark(signalParts[1]);
    }

    //initialize arguments array
    GValue *values = new GValue[args.size() + 1];
    memset(values, 0, sizeof(GValue) * (args.size() + 1));

    //set instance
    g_value_init(&values[0], itype);
    g_value_set_instance(&values[0], instance);

    try {
        //find the signal and perform sanity checks
        Signal signal = lookup(signalParts[0].toUtf8(), itype);
        if (!signal.isValid()) {
            throw QString(QLatin1String("Could not find any signal named %1 "
                                        "on this instance type")).arg(signalParts[0]);
        }

        QList<Type> paramTypes = signal.paramTypes();
        if (paramTypes.size() != args.size()) {
            throw QString(QLatin1String("The number of arguments that the signal accepts differ "
                                        "from the number of arguments provided to Signal::emit"));
        }

        //set arguments
        for(int i=0; i<args.size(); i++) {
            if (!paramTypes[i].isA(args[i].type())) {
                throw QString(QLatin1String("Argument %1 provided to Signal::emit is not of the "
                                            "type that the signal expects")).arg(i);
            } else {
                g_value_init(&values[i+1], args[i].type());
                g_value_copy(args[i], &values[i+1]);
            }
        }

        //initialize return value
        GValue returnValue = QGLIB_G_VALUE_INITIALIZER;
        if (signal.returnType() != Type::None) {
            g_value_init(&returnValue, signal.returnType());
        }

        //emit the signal
        g_signal_emitv(values, signal.id(), detail, &returnValue);

        if (G_IS_VALUE(&returnValue)) {
            result = SharedValue(&returnValue);
            g_value_unset(&returnValue);
        }
    } catch (const QString & msg) {
        QString instanceName;
        SharedValue instanceValue(&values[0]);
        if (instanceValue.type().isInstantiatable() && instanceValue.canTransformTo(Type::String)) {
            //instances can be transformed to strings for debugging purposes
            instanceName = instanceValue.transformTo(Type::String).get<QString>();
        }

        qCritical() << "Error during emission of signal" << detailedSignal
                    << "on object"<< instanceName << ":" << msg;
    }

    //cleanup
    for(int i=0; i<args.size() + 1; i++) {
        g_value_unset(&values[i]);
    }
    delete[] values;

    return result;
}

//END ******** Signal::emit ********
//BEGIN ******** Closure ********

void Closure::ref(bool increaseRef)
{
    if (increaseRef) {
        g_closure_ref(static_cast<GClosure*>(m_object));
    }
}

void Closure::unref()
{
    g_closure_unref(static_cast<GClosure*>(m_object));
}

//END ******** Closure ********

namespace Private {

//BEGIN ******** Closure internals ********

static void c_marshaller(GClosure *closure, GValue *returnValue, uint paramValuesCount,
                         const GValue *paramValues, void *hint, void *data)
{
    Q_UNUSED(data);

    ClosureDataBase *cdata = static_cast<ClosureDataBase*>(closure->data);

    QList<Value> params;
    //the signal sender is always the first argument. if we are instructed not to pass it
    //as an argument to the slot, begin converting from paramValues[1]
    for(uint i = cdata->passSender ? 0 : 1; i<paramValuesCount; ++i) {
        params.append(Value(&paramValues[i]));
    }

    try {
        SharedValue result(returnValue);
        cdata->marshaller(result, params);
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

//END ******** Closure internals ********

} //namespace Private

//BEGIN ******** Signal::connect ********

//static
SignalHandler Signal::connect(void *instance, const char *detailedSignal,
                              const ClosurePtr & closure, ConnectFlags flags)
{
    uint id = g_signal_connect_closure(instance, detailedSignal, closure,
                                       (flags & ConnectAfter) ? TRUE : FALSE);
    return SignalHandler(instance, id);
}

//END ******** Signal::connect ********

} //namespace QGlib
