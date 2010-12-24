/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#ifndef QGLIB_CONNECT_H
#define QGLIB_CONNECT_H

#include "global.h"
#include "quark.h"
#include <QtCore/QObject>
#include <QtCore/QSharedPointer>
#include <QtCore/QFlags>
#include <QtCore/QHash>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace QGlib {

/*! These flags define options that can be
 * passed to connect() to modify its behaviour.
 */
enum ConnectFlag { //codegen: skip=true
    /*! If ConnectAfter is specified, the slot passed to connect()
     * will be invoked after the default signal handler of this signal
     * has been called. See the Glib signals documentation for more
     * details on this parameter.
     */
    ConnectAfter = 1,
    /*! If PassSender is specified, the slot passed to connect()
     * will receive as the first argument a pointer to the sender
     * of the signal. Thus, your slot should be defined like this:
     * \code
     * void mySlot(const QGlib::ObjectPtr & sender, const Foo & firstArgument, ...);
     * \endcode
     */
    PassSender = 2
};
Q_DECLARE_FLAGS(ConnectFlags, ConnectFlag);
Q_DECLARE_OPERATORS_FOR_FLAGS(ConnectFlags)

#if defined(DOXYGEN_RUN)

/*! Connects a signal to a specified \a slot.
 *
 * This method will generate a set of template functions and classes that bind on a GClosure.
 * When the signal is emitted, this GClosure will be invoked and its templated marshaller
 * function will take care of converting the parameters of the signal (which are given as
 * GValues) to the types that the \a slot expects. In case the types do not match, a warning
 * will be printed to stderr at runtime and the \a slot will not be invoked. You are
 * responsible for defining the \a slot with the correct arguments!
 *
 * Note that since the implementation uses Value::get() to convert the GValues into the
 * specified types, the same rules that apply to Value::get() apply here (i.e. you should
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
 * \returns whether the connection was successfully made or not
 */
template <typename T, typename R, typename... Args>
bool connect(void *instance, const char *detailedSignal,
             T *receiver, R (T::*slot)(Args...), ConnectFlags flags = 0);

//Fake disconnect() declaration.
//Doxygen should document a version with optional arguments. In reality we have to use
//two versions to avoid having to type the template parameters in case the user wants
//to use NULL for the receiver and slot arguments. Also, a version that takes void*
//for everything is not possible since member function pointers do not cast to void*.

/*! Disconnects \a detailedSignal of \a instance from the \a slot of \a receiver. */
template <typename T, typename R, typename... Args>
bool disconnect(void *instance, const char *detailedSignal = 0,
                T *receiver = 0, R (T::*slot)(Args...) = 0);

#else //DOXYGEN_RUN

namespace Private {

/* This interface specifies the methods that will be used to connect/disconnect a
 * signal receiver to/from a slot that should be called when the receiver is destroyed.
 * This notification is used to disconnect the signal automatically.
 */
class DestroyNotifierIface
{
public:
    virtual ~DestroyNotifierIface() {}
    virtual bool connect(void *receiver, QObject *notificationReceiver, const char *slot) = 0;
    virtual bool disconnect(void *receiver, QObject *notificationReceiver) = 0;
};

typedef QSharedPointer<DestroyNotifierIface> DestroyNotifierIfacePtr;

/* This is DestroyNotifierIface that works for signal receivers that inherit QObject. */
class QObjectDestroyNotifier : public DestroyNotifierIface
{
public:
    static DestroyNotifierIfacePtr instance();

    virtual bool connect(void *receiver, QObject *notificationReceiver, const char *slot);
    virtual bool disconnect(void *receiver, QObject *notificationReceiver);
};

/* This is provided for future expansion.
 * It should implement operator DestroyNotifierIfacePtr() and return
 * the appropriate DestroyNotifierIface for the given type T
 * (i.e. the signal receiver is of type T)
 */
template <typename T, typename Enable = void>
struct GetDestroyNotifier
{
};

/* Partial specialization for QObjects (T inherits QObject) */
template <typename T>
struct GetDestroyNotifier<T, typename boost::enable_if< boost::is_base_of<QObject, T> >::type>
{
    inline operator DestroyNotifierIfacePtr() { return QObjectDestroyNotifier::instance(); }
};


/* This method is used internally from QGlib::connect(). */
ulong connect(void *instance, const char *signal, Quark detail,
              void *receiver, const DestroyNotifierIfacePtr & notifier,
              uint slotHash, const ClosurePtr & closure, ConnectFlags flags);

/* This method is used internally from QGlib::disconnect(). */
bool disconnect(void *instance, const char *signal, Quark detail,
                void *receiver, uint slotHash, ulong handlerId);


/* This is a helper that returns a hash value for a member function pointer.
 * Because of the nature of member function pointers, it is not possible to cast
 * them to void* or any integral type and as a result we need to create a hash value
 * of their data to be able to store them in the connections store. This value is
 * only used for disconnection, so storing the real pointer is not necessary.
 */
template <typename T>
inline typename boost::enable_if< boost::is_member_function_pointer<T>, uint >::type
hashMfp(const T & mfp)
{
    const char *data = reinterpret_cast<const char*>(&mfp);
    return qHash(QByteArray::fromRawData(data, sizeof(T)));
}

template <typename T>
inline typename boost::enable_if< boost::is_integral<T>, uint >::type
hashMfp(const T & mfp)
{
    Q_ASSERT(mfp == 0);
    return 0;
}

} //namespace Private


//The real QGlib::disconnect

inline bool disconnect(void *instance, const char *detailedSignal = 0, void *receiver = 0)
{
    return Private::disconnect(instance, detailedSignal, Quark(), receiver, 0, 0);
}

template <typename T>
inline bool disconnect(void *instance, const char *detailedSignal, void *receiver, T slot)
{
    return Private::disconnect(instance, detailedSignal, Quark(), receiver, Private::hashMfp(slot), 0);
}

#endif //DOXYGEN_RUN

} //namespace QGlib

#define IN_QGLIB_CONNECT_H
# if !QGLIB_HAVE_CXX0X
//boost::bind restricts us to 9 arguments. if you need more,
//consider using a modern compiler with variadic template support ;)
#  define QGLIB_CONNECT_MAX_ARGS 9
# endif
# include "connectimpl.h"
#undef IN_QGLIB_CONNECT_H

#endif //QGLIB_CONNECT_H
