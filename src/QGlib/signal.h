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
#include <QtCore/QString>
#include <QtCore/QFlags>
#include <QtCore/QSharedData>

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
    inline SignalHandler()
        : m_instance(NULL), m_id(0) {}

    inline bool isValid() const { return m_instance != NULL; }

    bool isConnected() const;
    void disconnect();

    void block();
    void unblock();

private:
    friend class Signal;
    inline SignalHandler(void *instance, ulong id)
        : m_instance(instance), m_id(id) {}

    void *m_instance;
    ulong m_id;
};

/*! \headerfile signal.h <QGlib/Signal>
 * \brief Helper class providing integration with the GObject signal system
 *
 * Signals are a generic notification mechanism. Each signal is bound to a
 * certain instantiatable Type and can be emitted on any instance of this type.
 *
 * This class allows you to inspect, emit and connect to these signals. To inspect
 * the signals of a certain Type, you can use the lookup() and listSignals() methods.
 * To emit or connect a signal, use the emit() and connect() methods respectively.
 *
 * For more information, please read the relevant Glib documentation.
 */
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

    /*! These flags define options that can be passed to connect() to modify its behaviour. */
    enum ConnectFlag { //codegen: skip=true
        /*! If ConnectAfter is specified, the slot passed to connect() will be invoked after the
         * default signal handler of this signal has been called. See the Glib signals
         * documentation for more details on this parameter.
         */
        ConnectAfter = 1,
        /*! If PassSender is specified, the slot passed to connect() will receive as the first
         * argument a pointer to the sender of the signal. Thus, your slot should be defined
         * like this:
         * \code
         * void mySlot(const QGlib::ObjectPtr & sender, const Foo & firstArgument, ...);
         * \endcode
         */
        PassSender = 2
    };
    Q_DECLARE_FLAGS(ConnectFlags, ConnectFlag);

    Signal(const Signal & other);
    Signal & operator=(const Signal & other);
    virtual ~Signal();

    bool isValid() const;

    uint id() const;
    QString name() const;
    SignalFlags flags() const;

    Type instanceType() const;
    Type returnType() const;
    QList<Type> paramTypes() const;

    static Signal lookup(const char *name, Type type);
    static QList<Signal> listSignals(Type type);


#if QGLIB_HAVE_CXX0X

    /*! Emits a signal on a specified \a instance with the specified arguments.
     *
     * This method will convert all the specified arguments to GValues using ValueBase::set()
     * and will then call the non-templated emit() method, which is a wrapper for g_signal_emitv().
     * The returned value from the signal (if the signal returns a value) will be converted
     * from GValue to the type R using ValueBase::get() and will be returned. If some argument
     * is not of the type that the signal expects, a warning will be printed to stderr at runtime
     * and the signal will not be emitted. If the return value is not of the type that the signal
     * returns, the signal will be emitted, but a default-constructed value for the type R will
     * be returned and a warning will be printed to stderr.
     *
     * Note that since the implementation uses ValueBase::set() to convert the GValues into the
     * specified types, the same rules that apply to ValueBase::set() apply here (i.e. you should
     * only use the types of the bindings and not the C types, which means QGst::ObjectPtr instead
     * of GstObject*, etc...).
     *
     * Emitting a signal is useful for the so-called Action signals. These are meant to be emitted
     * from the application and not connected to. They are more like dynamic methods that can be
     * identified with a string.
     *
     * \note This method makes use of C++0x features (namely, variadic templates and rvalue
     * references). If your compiler does not support them, a hacky implementation using boost's
     * preprocessor, function and bind libraries will be compiled instead. That version has a
     * limit of 9 arguments.
     *
     * \param instance The instance of the object on which the signal will be emitted. You can pass
     * a RefPointer as an instance without any problems; it will automatically cast to void*.
     * \param detailedSignal The name of the signal that you want to emit, with an optional
     * detail if the signal is detailed. The detail may be specified with the following syntax:
     * "signal::detail".
     * \param args The arguments that will be passed to the signal.
     * \returns The return value of the signal.
     */
    template <typename R, typename... Args>
    static R emit(void *instance, const char *detailedSignal, Args&&... args);

    /*! Connects a signal to a specified \a slot.
     *
     * This method will generate a set of template functions and classes that bind on a GClosure.
     * When the signal is emitted, this GClosure will be invoked and its templated marshaller
     * function will take care of converting the parameters of the signal (which are given as
     * GValues) to the types that the \a slot expects. In case the types do not match, a warning
     * will be printed to stderr at runtime and the \a slot will not be invoked. You are
     * responsible for defining the \a slot with the correct arguments!
     *
     * Note that since the implementation uses ValueBase::get() to convert the GValues into the
     * specified types, the same rules that apply to ValueBase::get() apply here (i.e. you should
     * only use the types of the bindings and not the C types, which means QGst::ObjectPtr instead
     * of GstObject*, etc...).
     *
     * \note
     * \li You can use const references for the arguments of the slot to avoid unnecessary
     * copying of objects. The marshaller will always hold one copy of them during the execution
     * of your \a slot.
     * \li This method makes use of C++0x features (namely, variadic templates and rvalue
     * references). If your compiler does not support them, a hacky implementation using boost's
     * preprocessor, function and bind libraries will be compiled instead. That version has a
     * limit of 9 slot arguments.
     *
     * \param instance The instance of the object that emits this signal. You can pass
     * a RefPointer as an instance without any problems; it will automatically cast to void*.
     * \param detailedSignal The name of the signal that you want to connect to, with an optional
     * detail if the signal is detailed. The detail may be specified with the following syntax:
     * "signal::detail".
     * \param receiver The instance of the class on which \a slot will be invoked.
     * \param slot A pointer to a member function that will be invoked when the signal is emitted.
     * \param flags See ConnectFlag.
     * \returns A SignalHandler instance, which can be used to disconnect or block this handler.
     * Note that the return type of this function is subject to change before a stable release is made.
     */
    template <typename T, typename R, typename... Args>
    static SignalHandler connect(void *instance, const char *detailedSignal,
                                 T *receiver, R (T::*slot)(Args...), ConnectFlags flags = 0);

#else //QGLIB_HAVE_CXX0X

    //versions that take no arguments
    template <typename R>
    static R emit(void *instance, const char *detailedSignal);

    template <typename T, typename R>
    static SignalHandler connect(void *instance, const char *detailedSignal,
                                 T *receiver, R (T::*slot)(), ConnectFlags flags = 0);

# define QGLIB_SIGNAL_TMPL_PARAMS(n) \
        BOOST_PP_ENUM_PARAMS(n, typename A)

# define QGLIB_SIGNAL_TMPL_ARGS(n) \
        BOOST_PP_ENUM_PARAMS(n, A)

# define QGLIB_SIGNAL_EMIT_DECLARATION(z, n, data) \
    template <typename R, QGLIB_SIGNAL_TMPL_PARAMS(n) > \
    static R emit(void *instance, const char *detailedSignal, QGLIB_SIGNAL_TMPL_ARGS(n));

# define QGLIB_SIGNAL_CONNECT_DECLARATION(z, n, data) \
    template <typename T, typename R, QGLIB_SIGNAL_TMPL_PARAMS(n) > \
    static SignalHandler connect(void *instance, const char *detailedSignal, \
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

    /*! \internal This method is used internally from the templated emit() method. */
    static Value emit(void *instance, const char *detailedSignal, const QList<Value> & args);

    /*! \internal This method is used internally from the templated connect() method. */
    static SignalHandler connect(void *instance, const char *detailedSignal,
                                 const ClosurePtr & closure, ConnectFlags flags = 0);

private:
    Signal(uint id);

    struct Private;
    QSharedDataPointer<Private> d;
};

} //namespace QGlib

Q_DECLARE_OPERATORS_FOR_FLAGS(QGlib::Signal::SignalFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGlib::Signal::ConnectFlags)

#define IN_QGLIB_SIGNAL_H
# include "emitimpl.h"
# include "connectimpl.h"
#undef IN_QGLIB_SIGNAL_H

#if defined(QGLIB_SIGNAL_MAX_ARGS)
# undef QGLIB_SIGNAL_MAX_ARGS
#endif

#endif
