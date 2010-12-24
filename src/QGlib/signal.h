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

#if !QGLIB_HAVE_CXX0X && !defined(QGLIB_SIGNAL_MAX_ARGS)
# define QGLIB_SIGNAL_MAX_ARGS 9
#endif

//Qt's emit will clash
#if defined(emit)
# if defined(Q_CC_GNU)
#  warning "The emit keyword is defined and will be undefined here to compile QGlib::emit."
#  warning "It is recommended to compile your project with QT_NO_KEYWORDS defined."
# elif defined(Q_CC_MSVC)
#  pragma message("Warning: The emit keyword is defined and will be undefined here to compile QGlib::emit.")
#  pragma message("Warning: It is recommended to compile your project with QT_NO_KEYWORDS defined.")
# endif
# undef emit
# define QT_NO_EMIT //undocumented Qt macro that skips "#define emit" in qglobal.h
#endif

namespace QGlib {

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

private:
    Signal(uint id);

    struct Private;
    QSharedDataPointer<Private> d;
};

#if defined(DOXYGEN_RUN)

/*! Emits a signal on a specified \a instance with the specified arguments.
 *
 * This method will convert all the specified arguments to GValues using Value::set()
 * and will then call the non-templated emit() method, which is a wrapper for g_signal_emitv().
 * The returned value from the signal (if the signal returns a value) will be converted
 * from GValue to the type R using Value::get() and will be returned. If some argument
 * is not of the type that the signal expects, a warning will be printed to stderr at runtime
 * and the signal will not be emitted. If the return value is not of the type that the signal
 * returns, the signal will be emitted, but a default-constructed value for the type R will
 * be returned and a warning will be printed to stderr.
 *
 * Note that since the implementation uses Value::set() to convert the GValues into the
 * specified types, the same rules that apply to Value::set() apply here (i.e. you should
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
R emit(void *instance, const char *detailedSignal, const Args & ... args);

#endif //DOXYGEN_RUN

} //namespace QGlib

Q_DECLARE_OPERATORS_FOR_FLAGS(QGlib::Signal::SignalFlags)

#define IN_QGLIB_SIGNAL_H
# include "emitimpl.h"
#undef IN_QGLIB_SIGNAL_H

#if defined(QGLIB_SIGNAL_MAX_ARGS)
# undef QGLIB_SIGNAL_MAX_ARGS
#endif

#endif
