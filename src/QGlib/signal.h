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
#ifndef QGLIB_SIGNAL_H
#define QGLIB_SIGNAL_H

#include "global.h"
#include "closure.h"
#include <QtCore/QString>
#include <QtCore/QFlags>
#include <QtCore/QSharedPointer>

#if !QGLIB_HAVE_CXX0X
//boost::bind restricts us to 9 arguments. if you need more,
//consider using a modern compiler with variadic template support ;)
# define QGLIB_SIGNAL_MAX_ARGS 9
# include <boost/preprocessor.hpp>
#endif

//Qt's emit will clash
#if defined(emit)
# if defined(Q_CC_GNU)
#  warning "The emit keyword is defined and will be undefined here to compile QGlib::Signal::emit."
#  warning "It is recommended to compile your project with QT_NO_KEYWORDS defined."
# elif defined(Q_CC_MSVC)
#  pragma message("Warning: The emit keyword is defined and will be undefined here to compile QGlib::Signal::emit.")
#  pragma message("Warning: It is recommended to compile your project with QT_NO_KEYWORDS defined.")
# endif
# undef emit
# define QT_NO_EMIT //undocumented Qt macro that skips "#define emit" in qglobal.h
#endif

namespace QGlib {

class SignalHandler
{
public:
    inline bool isValid() const { return m_instance != NULL; }

    bool isConnected() const;
    void disconnect();

    void block();
    void unblock();

private:
    friend class Signal;
    inline SignalHandler()
        : m_instance(NULL), m_id(0) {}
    inline SignalHandler(void *instance, ulong id)
        : m_instance(instance), m_id(id) {}

    void *m_instance;
    ulong m_id;
};

class Signal
{
public:
    enum SignalFlag {
        RunFirst = 1<<0,
        RunLast = 1<<1,
        RunCleanup = 1<<2,
        NoRecurse = 1<<3,
        Detailed = 1<<4,
        Action = 1<<5,
        NoHooks = 1<<6
    };
    Q_DECLARE_FLAGS(SignalFlags, SignalFlag);

    enum ConnectFlag { //codegen: skip=true
        ConnectAfter = 1
    };
    Q_DECLARE_FLAGS(ConnectFlags, ConnectFlag);

    Signal(const Signal & other);
    Signal & operator=(const Signal & other);
    virtual ~Signal();

    bool isValid() const;

    uint id() const;
    QGlib::String name() const;
    SignalFlags flags() const;

    Type instanceType() const;
    Type returnType() const;
    QList<Type> paramTypes() const;

    static Signal lookup(const QGlib::String & name, Type type);
    static QList<Signal> listSignals(Type type);


#if QGLIB_HAVE_CXX0X

    template <typename R, typename... Args>
    static R emit(void *instance, const QGlib::String & detailedSignal, Args&&... args);

    template <typename T, typename R, typename... Args>
    static SignalHandler connect(void *instance, const QGlib::String & detailedSignal,
                                 T *receiver, R (T::*slot)(Args...), ConnectFlags flags = 0);

#else //QGLIB_HAVE_CXX0X

    //versions that take no arguments
    template <typename R>
    static R emit(void *instance, const QGlib::String & detailedSignal);

    template <typename T, typename R>
    static SignalHandler connect(void *instance, const QGlib::String & detailedSignal,
                                 T *receiver, R (T::*slot)(), ConnectFlags flags = 0);

# define QGLIB_SIGNAL_TMPL_PARAMS(n) \
        BOOST_PP_ENUM_PARAMS(n, typename A)

# define QGLIB_SIGNAL_TMPL_ARGS(n) \
        BOOST_PP_ENUM_PARAMS(n, A)

# define QGLIB_SIGNAL_EMIT_DECLARATION(z, n, data) \
    template <typename R, QGLIB_SIGNAL_TMPL_PARAMS(n) > \
    static R emit(void *instance, const QGlib::String & detailedSignal, QGLIB_SIGNAL_TMPL_ARGS(n));

# define QGLIB_SIGNAL_CONNECT_DECLARATION(z, n, data) \
    template <typename T, typename R, QGLIB_SIGNAL_TMPL_PARAMS(n) > \
    static SignalHandler connect(void *instance, const QGlib::String & detailedSignal, \
                                 T *receiver, R (T::*slot)(QGLIB_SIGNAL_TMPL_ARGS(n)), \
                                 ConnectFlags flags = 0);

    //versions that take from 1 to QGLIB_SIGNAL_MAX_ARGS arguments
    BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(QGLIB_SIGNAL_MAX_ARGS), QGLIB_SIGNAL_EMIT_DECLARATION, dummy)
    BOOST_PP_REPEAT_FROM_TO(1, BOOST_PP_INC(QGLIB_SIGNAL_MAX_ARGS), QGLIB_SIGNAL_CONNECT_DECLARATION, dummy)

# undef QGLIB_SIGNAL_CONNECT_DECLARATION
# undef QGLIB_SIGNAL_EMIT_DECLARATION
# undef QGLIB_SIGNAL_TMPL_ARGS
# undef QGLIB_SIGNAL_TMPL_PARAMS

#endif //QGLIB_HAVE_CXX0X


    static Value emit(void *instance, const QGlib::String & detailedSignal, const QList<Value> & args);

    static SignalHandler connect(void *instance, const QGlib::String & detailedSignal,
                                 const ClosurePtr & closure, ConnectFlags flags = 0);

private:
    Signal(uint id);

    struct Private;
    QSharedPointer<Private> d;
};

} //namespace QGlib

Q_DECLARE_OPERATORS_FOR_FLAGS(QGlib::Signal::SignalFlags)

#define IN_QGLIB_SIGNAL_H
# include "signalimpl_p.h"
#undef IN_QGLIB_SIGNAL_H

#if defined(QGLIB_SIGNAL_MAX_ARGS)
# undef QGLIB_SIGNAL_MAX_ARGS
#endif

#endif
