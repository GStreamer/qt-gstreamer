/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
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
#ifndef QGLIB_VALUE_H
#define QGLIB_VALUE_H

#include "global.h"
#include "type.h"
#include "refpointer.h"
#include <boost/mpl/if.hpp>
#include <boost/type_traits.hpp>
#include <stdexcept>
#include <QtCore/QString>
#include <QtCore/QDebug>

namespace QGlib {

/*! This structure holds the set and get methods that are used internally
 * by Value to handle data of a specific type. If you want to provide
 * support for a custom type, you need to write two such methods, create
 * a new ValueVTable instance that holds pointers to them and register it
 * using Value::registerValueVTable().
 * \sa \ref value_design
 */
struct ValueVTable
{
    typedef void (*SetFunction)(Value & value, const void *data);
    typedef void (*GetFunction)(const Value & value, void *data);

    inline ValueVTable() : set(NULL), get(NULL) {}
    inline ValueVTable(SetFunction s, GetFunction g) : set(s), get(g) {}

    SetFunction set;
    GetFunction get;
};


/*! \headerfile value.h <QGlib/Value>
 * \brief Wrapper class for GValue
 *
 * This class serves as a wrapper for GValue. GValue is a data type that can hold different
 * types of values inside, like a QVariant.
 * To set a value, you must first initialize this Value using one of the init() methods
 * (preferably the template one) in order to tell it what kind of value it is going to hold.
 * Once initialized to hold a specific type, you can use the set() and get() methods to set and
 * get values of this type only. Attempting to use another type will assert. To change the type
 * that this value holds, you can call the init() method again at any time. In this case, though,
 * any held value will be lost.
 */
class Value
{
public:
    Value();

    explicit Value(const GValue *gvalue);

    template <typename T>
    inline Value(const T & data);

    Value(const Value & other);

    virtual ~Value();

    Value & operator=(const Value & other);

    void init(Type type);

    template <typename T>
    inline void init();

    /*! \returns whether this Value instance wraps a valid GValue instance or not */
    bool isValid() const;

    /*! Clears the current value in this GValue instance and resets it to the
     * default value (as if the GValue had just been initialized). */
    void reset();

    /*! Retrieves the value stored in this GValue instance, as the specified type T.
     * The bindings take care of calling the appropriate g_value_get_* method depending
     * on the type T. Note though that this is only meant to be used with the types of
     * the bindings, not their C types. This means that if for example the GValue has
     * been initialized to hold a GstObject pointer, you can use:
     * \code
     * QGst::ObjectPtr object = value.get<QGst::ObjectPtr>();
     * \endcode
     * but \em not this:
     * \code
     * GstObject *object = value.get<GstObject*>(); //will cause compilation error
     * \endcode
     * If the underlying stored value is not of the type T, this method will attempt
     * to convert it to type T. If this is not possible, a default constructed value
     * will be returned.
     */
    template <typename T> T get() const;

    /*! Sets this GValue instance to hold the specified \a data.
     * As with get(), the bindings take care of calling the appropriate g_value_set_*
     * method depending on the type T, but note that this is only meant to be used
     * with the types of the bindings.
     *
     * If this GValue instance has been initialized to hold a different type of data
     * than T, a conversion to the correct type() will be attempted. If the conversion
     * fails, the GValue will remain unaffected.
     * \sa get()
     */
    template <typename T> void set(const T & data);

    /*! \returns the type that this GValue instance has been initialized to hold */
    Type type() const;

    /*! \returns whether it is possible to transform this GValue to a GValue of another type. */
    bool canTransformTo(Type type) const;

    /*! Transforms the current value into a value of the specified \a type, if possible.
     * Possible transformations are, for example, int to float, int to string, etc... */
    Value transformTo(Type type) const;

    inline operator GValue*() { return m_value; }
    inline operator const GValue*() const { return m_value; }

    /*! Registers the given ValueVTable \a vtable for the given \a type.
     * This is provided to let you add support for a custom type, if necessary.
     * You should normally not need to do that, as most types are handled
     * by the handlers of their parent types.
     * \sa \ref value_design
     */
    static void registerValueVTable(Type type, const ValueVTable & vtable);

private:
    template <typename T>
    friend struct ValueImpl;

    /*! Retrieves the data from this GValue and places it into the memory position
     * pointed to by \a data. \a dataType indicates the actual data type of \a data
     * and is used, among others, to cast \a data back to the actual C++ type that
     * it points to and assign it.
     * \note This method should only be accessed from ValueImpl.
     * \sa \ref value_design
     */
    void getData(Type dataType, void *data) const;

    /*! Sets the data of this GValue to be the data pointed to by \a data.
     * \a dataType indicates the actual data type of \a data and is used,
     * among others, to cast \a data back to the actual C++ type that
     * it points to and retrieve its value.
     * \note This method should only be accessed from ValueImpl.
     * \sa \ref value_design
     */
    void setData(Type dataType, const void *data);

    GValue *m_value;
};


/*! This struct provides the implementation for the Value::get() and Value::set() methods.
 * If you want to provide support for a custom type, you may want to provide a template
 * specialization of this class to handle your type in a different way than the default
 * implementation. You should normally not need to be concerned at all with this.
 * \note this struct is declared as friend in Value and as a result it has access to
 * Value::setData() and Value::getData()
 * \sa \ref value_design
 */
template <typename T>
struct ValueImpl
{
    static inline T get(const Value & value);
    static inline void set(Value & value, const T & data);
};

// -- template implementations --

template <typename T>
inline Value::Value(const T & data)
    : m_value(NULL)
{
    init<T>();
    set(data);
}

template <typename T>
inline void Value::init()
{
    init(GetType<T>());
}

template <typename T>
T Value::get() const
{
    try {
        return ValueImpl<T>::get(*this);
    } catch (const std::exception & e) {
        qWarning() << "QGlib::Value::get:" << e.what();
        return T();
    }
}

template <typename T>
void Value::set(const T & data)
{
    try {
        ValueImpl<T>::set(*this, data);
    } catch (const std::exception & e) {
        qWarning() << "QGlib::Value::set:" << e.what();
    }
}

// -- default ValueImpl implementation --

template <typename T>
inline T ValueImpl<T>::get(const Value & value)
{
    //Use int for enums, T for everything else
    typename boost::mpl::if_<
        boost::is_enum<T>,
        int, T
    >::type result;

    value.getData(GetType<T>(), &result);
    return static_cast<T>(result);
}

template <typename T>
inline void ValueImpl<T>::set(Value & value, const T & data)
{
    //Use const int for enums, const T for everything else
    typename boost::mpl::if_<
        boost::is_enum<T>,
        const int, const T &
    >::type dataRef = data;

    value.setData(GetType<T>(), &dataRef);
}

// -- ValueImpl specialization for QFlags --

template <class T>
struct ValueImpl< QFlags<T> >
{
    static inline QFlags<T> get(const Value & value)
    {
        uint flags;
        value.getData(GetType< QFlags<T> >(), &flags);
        return QFlags<T>(QFlag(flags));
    }

    static inline void set(Value & value, const QFlags<T> & data)
    {
        uint flags = data;
        value.setData(GetType< QFlags<T> >(), &flags);
    }
};

// -- ValueImpl specialization for RefPointer --

template <class T>
struct ValueImpl< RefPointer<T> >
{
    static inline RefPointer<T> get(const Value & value)
    {
        typename T::CType *gobj;
        value.getData(GetType<T>(), &gobj);
        return RefPointer<T>::wrap(gobj);
    }

    static inline void set(Value & value, const RefPointer<T> & data)
    {
        typename T::CType *gobj = static_cast<typename T::CType*>(data);
        value.setData(GetType<T>(), &gobj);
    }
};

// -- ValueImpl specialization for string literals --

template <int N>
struct ValueImpl<char[N]>
{
    //No get method, obviously.

    static inline void set(Value & value, const char (&data)[N])
    {
        QByteArray str = QByteArray::fromRawData(data, N);
        value.setData(Type::String, &str);
    }
};

// -- ValueImpl specialization for const char* --

template <>
struct ValueImpl<const char*>
{
    //No get method, obviously.

    static inline void set(Value & value, const char *data)
    {
        QByteArray str = QByteArray::fromRawData(data, qstrlen(data));
        value.setData(Type::String, &str);
    }
};

// -- ValueImpl specialization for QString --

template <>
struct ValueImpl<QString>
{
    static inline QString get(const Value & value)
    {
        QByteArray str;
        value.getData(Type::String, &str);
        return QString::fromUtf8(str);
    }

    static inline void set(Value & value, const QString & data)
    {
        QByteArray str = data.toUtf8();
        value.setData(Type::String, &str);
    }
};

// -- Exceptions thrown from getData/setData --

namespace Private {

class InvalidValueException : public std::logic_error
{
public:
    inline InvalidValueException()
        : std::logic_error("This Value instance has not been initialized") {}
};

class InvalidTypeException : public std::logic_error
{
public:
    inline InvalidTypeException(const std::string & dataType, const std::string & valueType)
        : std::logic_error("Unable to handle value type \"" + dataType +
                           "\". This Value instance has been initialized to hold values of type \""
                           + valueType + "\" and no conversion is possible") {}
};

class UnregisteredTypeException : public std::logic_error
{
public:
    inline UnregisteredTypeException(const std::string & typeName)
        : std::logic_error("Unable to handle unregistered type \"" + typeName + "\"") {}
};

class TransformationFailedException : public std::runtime_error
{
public:
    inline TransformationFailedException(const std::string & srcTypeName,
                                         const std::string & destTypeName)
        : std::runtime_error("Failed to transform value from type \""
                             + srcTypeName + "\" to type \"" + destTypeName + "\"") {}
};

} //namespace Private

// -- QDebug operator --

/*! \relates QGlib::Value */
QDebug & operator<<(QDebug debug, const Value & value);

} //namespace QGlib

QGLIB_REGISTER_TYPE(QGlib::Value)

#endif
