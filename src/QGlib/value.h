/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include <boost/type_traits.hpp>
#include <stdexcept>

namespace QGlib {

/*! This class serves as a base class for Value and SharedValue, providing all the common bits. */
class ValueBase
{
public:
    class InvalidValueException : public std::logic_error
    {
    public:
        inline InvalidValueException()
            : std::logic_error("This ValueBase instance has not been initialized") {}
    };

    class InvalidTypeException : public std::logic_error
    {
    public:
        inline InvalidTypeException()
            : std::logic_error("This ValueBase instance has been initialized to hold a different "
                                 "type of data than the one that you are trying to access") {}
    };

    bool isValid() const;
    void reset();

    template <typename T> T get() const;
    template <typename T> void set(const T & data);

    Type type() const;
    bool canTransformTo(Type type) const;
    Value transformTo(Type type) const;

    inline operator GValue*() { return m_value; }
    inline operator const GValue*() const { return m_value; }

protected:
    ValueBase(GValue *val);
    virtual ~ValueBase();
    Q_DISABLE_COPY(ValueBase)

    GValue *m_value;
};

/*! This class serves as a wrapper for GValue. GValue is a data type that can hold different
 * types of values inside, like a QVariant.
 * To set a value, you must first initialize this Value using one of the init() methods
 * (preferably the template one) in order to tell it what kind of value it is going to hold.
 * Once initialized to hold a specific type, you can use the set() and get() methods to set and
 * get values of this type only. Attempting to use another type will assert. To change the type
 * that this value holds, you can call the init() method again at any time. In this case, though,
 * any held value will be lost.
 * \note Unlike SharedValue, this class always keeps a private GValue internally which is
 * allocated in the constructors and freed from the destructor.
 * \sa SharedValue
 */
class Value : public ValueBase
{
public:
    Value();
    Value(const GValue & gvalue);
    template <typename T>
    inline Value(const T & data);
    Value(const SharedValue & other);
    Value(const Value & other);
    virtual ~Value();

    Value & operator=(const SharedValue & other);
    Value & operator=(const Value & other);

    void init(Type type);

    template <typename T>
    inline void init();
};

/*! This class serves as a wrapper for shared GValue instances. Some functions in the GStreamer
 * API return a pointer to some internal GValue and expect you to change this internal instance,
 * not copy it and re-set it using some setter function (like all normal object-oriented APIs do),
 * so it is necessary to have way of accessing those instances. This class wraps a GValue without
 * copying it and without freeing it from the destructor, unlike Value, which always keeps a
 * GValue instance for itself.
 * \sa Value
 */
class SharedValue : public ValueBase
{
public:
    SharedValue(GValue *gvalue);
    SharedValue(const SharedValue & other);
    virtual ~SharedValue();
    SharedValue & operator=(const SharedValue & other);
};


/*! This class provides the implementation for the ValueBase::get() and ValueBase::set() methods.
 * If you want to provide support for a custom type, you should provide a template specialization
 * of this class that sets and gets data to the underlying GValue for your custom type.
 * The default implementation expects T to be an enum.
 * \sa QGLIB_REGISTER_VALUEIMPL
 */
template <typename T>
struct ValueImpl
{
    static inline T get(const ValueBase & value);
    static inline void set(ValueBase & value, const T & data);
};

/*! This macro declares a specialization for ValueImpl for the type \a T.
 * It should be used in a header, in combination with QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION
 * in the respective source file, which will define the implementation of this specialization.
 * \sa ValueImpl, QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION
 */
#define QGLIB_REGISTER_VALUEIMPL(T) \
    namespace QGlib { \
        template <> \
        struct ValueImpl<T> \
        { \
            static T get(const ValueBase & value); \
            static void set(ValueBase & value, T const & data); \
        }; \
    }

/*! This macro defines the implementation of a ValueImpl specialization for type \a T.
 * \param GET_IMPL should be an expression that evaluates to type T and extracts a value from
 * a const ValueBase refererence called 'value'
 * \param SET_IMPL should be an expression that evaluates to void and sets the data from a
 * const T reference called 'data' to a ValueBase reference called 'value'
 * \sa QGLIB_REGISTER_VALUEIMPL, ValueImpl, ValueBase
 */
#define QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(T, GET_IMPL, SET_IMPL) \
    namespace QGlib { \
        T ValueImpl<T>::get(const ValueBase & value) \
        { \
            return (GET_IMPL);\
        } \
        void ValueImpl<T>::set(ValueBase & value, T const & data) \
        { \
            (SET_IMPL);\
        } \
    }

// -- template implementations --

template <typename T>
T ValueBase::get() const
{
    if (!isValid()) {
        throw InvalidValueException();
    }
    if (!(GetType<T>() == type() || type().isA(GetType<T>()))) {
        throw InvalidTypeException();
    }

    return ValueImpl<T>::get(*this);
}

template <typename T>
void ValueBase::set(const T & data)
{
    if (!isValid()) {
        throw InvalidValueException();
    }
    if (!(GetType<T>() == type() || type().isA(GetType<T>()))) {
        throw InvalidTypeException();
    }

    ValueImpl<T>::set(*this, data);
}


template <typename T>
inline Value::Value(const T & data)
    : ValueBase(NULL)
{
    init<T>();
    set(data);
}

template <typename T>
inline void Value::init()
{
    init(GetType<T>());
}

// -- default ValueImpl implementation (handles enums) --

struct ValueImpl_Enum
{
    static int get(const ValueBase & value);
    static void set(ValueBase & value, int data);
};

template <typename T>
inline T ValueImpl<T>::get(const ValueBase & value)
{
    QGLIB_STATIC_ASSERT(boost::is_enum<T>::value,
                        "No QGlib::ValueImpl<T> specialization has been registered for this type");
    return static_cast<T>(ValueImpl_Enum::get(value));
}

template <typename T>
inline void ValueImpl<T>::set(ValueBase & value, const T & data)
{
    QGLIB_STATIC_ASSERT(boost::is_enum<T>::value,
                        "No QGlib::ValueImpl<T> specialization has been registered for this type");
    ValueImpl_Enum::set(value, static_cast<int>(data));
}

// -- ValueImpl specialization for QFlags --

struct ValueImpl_Flags
{
    static uint get(const ValueBase & value);
    static void set(ValueBase & value, uint data);
};

template <class T>
struct ValueImpl< QFlags<T> >
{
    static inline QFlags<T> get(const ValueBase & value)
    {
        return QFlags<T>(QFlag(ValueImpl_Flags::get(value)));
    }

    static inline void set(ValueBase & value, const QFlags<T> & data)
    {
        ValueImpl_Flags::set(value, static_cast<uint>(data));
    }
};

} //namespace QGlib

QDebug & operator<<(QDebug debug, const QGlib::ValueBase & value);

QGLIB_REGISTER_TYPE(QGlib::ValueBase) //codegen: GType=G_TYPE_VALUE
QGLIB_REGISTER_TYPE(QGlib::Value)
QGLIB_REGISTER_TYPE(QGlib::SharedValue) //codegen: GType=G_TYPE_VALUE

QGLIB_REGISTER_VALUEIMPL(bool)
QGLIB_REGISTER_VALUEIMPL(char)
QGLIB_REGISTER_VALUEIMPL(unsigned char)
QGLIB_REGISTER_VALUEIMPL(int)
QGLIB_REGISTER_VALUEIMPL(unsigned int)
QGLIB_REGISTER_VALUEIMPL(long)
QGLIB_REGISTER_VALUEIMPL(unsigned long)
QGLIB_REGISTER_VALUEIMPL(qint64)
QGLIB_REGISTER_VALUEIMPL(quint64)
QGLIB_REGISTER_VALUEIMPL(float)
QGLIB_REGISTER_VALUEIMPL(double)
QGLIB_REGISTER_VALUEIMPL(void*)
QGLIB_REGISTER_VALUEIMPL(QGlib::Type)
QGLIB_REGISTER_VALUEIMPL(const char*)
QGLIB_REGISTER_VALUEIMPL(QByteArray)
QGLIB_REGISTER_VALUEIMPL(QString)

#endif
