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
#ifndef QGLIB_TYPE_H
#define QGLIB_TYPE_H

#include "global.h"

namespace QGlib {

/*! \headerfile type.h <QGlib/Type>
 * \brief Wrapper class for GType
 *
 * A GType is a numerical value which represents the unique identifier
 * of a registered type. The GType API is the foundation of the GObject system.
 * It provides the facilities for registering and managing all fundamental data types,
 * user-defined object and interface types.
 *
 * As a wrapper, this class provides only methods for querying existing types,
 * not registering new ones. You should never need to use it directly, although
 * it might come in handy in some cases.
 *
 * To retrieve a Type for a certain class, use GetType().
 *
 * \note This class is a thin wrapper around an unsigned long. There is no overhead
 * in copying it around, since it is just an integer.
 */
class Type
{
public:
    enum FundamentalType {
        Invalid = 0,
        None = 1<<2,
        Interface = 2<<2,
        Char = 3<<2,
        Uchar = 4<<2,
        Boolean = 5<<2,
        Int = 6<<2,
        Uint = 7<<2,
        Long = 8<<2,
        Ulong = 9<<2,
        Int64 = 10<<2,
        Uint64 = 11<<2,
        Enum = 12<<2,
        Flags = 13<<2,
        Float = 14<<2,
        Double = 15<<2,
        String = 16<<2,
        Pointer = 17<<2,
        Boxed = 18<<2,
        Param = 19<<2,
        Object = 20<<2
    };

    inline Type() : m_type(0) {}
    inline Type(unsigned long gtype) : m_type(gtype) {}
    inline Type(FundamentalType ftype) : m_type(ftype) {}
    inline Type(const Type & other) : m_type(other.m_type) {}

    inline Type & operator=(Type other);
    inline bool operator==(Type other) const;
    inline operator unsigned long() const { return m_type; }

    template<class T>
    static Type fromInstance(const RefPointer<T> & instance);
    static Type fromInstance(void *nativeInstance);
    static Type fromName(const char *name);

    QString name() const;
    Quark qname() const;

    bool isAbstract() const;
    bool isDerived() const;
    bool isFundamental() const;
    bool isValueType() const;
    bool hasValueTable() const;
    bool isClassed() const;
    bool isInstantiatable() const;
    bool isDerivable() const;
    bool isDeepDerivable() const;
    bool isInterface() const;

    FundamentalType fundamental() const;
    Type parent() const;
    uint depth() const;
    Type nextBase(Type rootType) const;
    bool isA(Type is_a_type) const;
#if 0
    QList<Type> children() const;
    QList<Type> interfaces() const;
    QList<Type> interfacePrequisities() const;
#endif
    void *quarkData(const Quark & qname) const;
    void setQuarkData(const Quark & qname, void *data);

private:
    unsigned long m_type;
};

inline Type & Type::operator=(Type other)
{
    m_type = other.m_type;
    return *this;
}

inline bool Type::operator==(Type other) const
{
    return m_type == other.m_type;
}

template <class T>
Type Type::fromInstance(const RefPointer<T> & instance)
{
    return fromInstance(static_cast<void*>(static_cast<typename T::CType*>(instance)));
}

//***************
// -- GetType --
//***************

/* Used to provide the implementation for GetType() */
template <class T>
struct GetTypeImpl
{
//If we have static_assert(), use it to show a more friendly error message to the developer.
//The check is dummy and is expected to evaluate to false. It is just used to trick the
//compiler to delay the evaluation of the expression until the instantiation of this template
//(where T becomes a known type). static_assert(false, "foo"); fails even before instantiation.
#if defined(QGLIB_HAVE_CXX0X_STATIC_ASSERT)
private:
    template <class X> struct FailStruct { static const bool value = false; };
    static_assert(FailStruct<T>::value, "Type T has not been registered with the QGlib type system");
#endif
};

/*! This template function retrieves the QGlib::Type (aka GType) of a given type T.
 * Type T must have been registered with the QGlib type system using QGLIB_REGISTER_TYPE,
 * otherwise this function will fail to compile.
 * \relates QGlib::Type
 * \sa QGLIB_REGISTER_TYPE
 */
template <class T>
inline Type GetType()
{
    return GetTypeImpl<T>();
}

/*! \addtogroup macros Internal macros */
//@{

/*! \internal
 * This macro is used to register a class with the QGlib type system. It forward-declares
 * a specialization for struct GetTypeImpl and serves as a keyword for codegen, our code generator,
 * which then uses QGLIB_REGISTER_TYPE_IMPLEMENTATION to provide the compiled implementation.
 * \note this macro must be used outside of any namespace scope
 */
#define QGLIB_REGISTER_TYPE(T) \
    namespace QGlib { \
        template <> \
        struct GetTypeImpl<T> { operator Type(); }; \
    }

/*! \internal Used by codegen only */
#define QGLIB_REGISTER_TYPE_IMPLEMENTATION(T, GTYPE) \
    namespace QGlib { \
        GetTypeImpl<T>::operator Type() { return (GTYPE); } \
    }

//@}

//***********************
// -- private helpers --
//***********************

namespace Private {

/* These are used in RefPointer::dynamicCast and ValueBase::set / ValueBase::get
 * to retrieve if it is possible to convert from a given Type to the type T
 * or vice versa. It is provided as a template to be able to be specialized
 * for strange cases like the GstMessage subclasses, where the subclasses
 * have the same GType as their superclass.
 * The names "from" and "to" are just to be able to type this in a nice syntax:
 * CanConvertTo<T>::from(mytype) or CanConvertFrom<T>::to(mytype)
 */

template <typename T>
struct CanConvertTo
{
    /* This is used from RefPointer::dynamicCast only.
     * It takes an instance so that specializations can actually query the instance itself. */
    static inline bool from(void *instance)
    {
        return from(Type::fromInstance(instance));
    }

    /* This is used from ValueBase::get only. */
    static inline bool from(Type t)
    {
        return t.isA(GetType<T>());
    }
};

template <typename T>
struct CanConvertFrom
{
    /* This is used from ValueBase::set only. */
    static inline bool to(Type t)
    {
        return GetType<T>().isA(t);
    }
};

} //namespace Private
} //namespace QGlib

//**************************
// -- type registrations --
//**************************

#define QGLIB_REGISTER_NATIVE_TYPE(T, GTYPE) \
    namespace QGlib { \
        template <> \
        struct GetTypeImpl<T> { \
            inline operator Type() { return (GTYPE); }; \
        }; \
    }

QGLIB_REGISTER_NATIVE_TYPE(bool, Type::Boolean)
QGLIB_REGISTER_NATIVE_TYPE(char, Type::Char)
QGLIB_REGISTER_NATIVE_TYPE(unsigned char, Type::Uchar)
QGLIB_REGISTER_NATIVE_TYPE(int, Type::Int)
QGLIB_REGISTER_NATIVE_TYPE(unsigned int, Type::Uint)
QGLIB_REGISTER_NATIVE_TYPE(long, Type::Long)
QGLIB_REGISTER_NATIVE_TYPE(unsigned long, Type::Ulong)
QGLIB_REGISTER_NATIVE_TYPE(qint64, Type::Int64)
QGLIB_REGISTER_NATIVE_TYPE(quint64, Type::Uint64)
QGLIB_REGISTER_NATIVE_TYPE(float, Type::Float)
QGLIB_REGISTER_NATIVE_TYPE(double, Type::Double)
QGLIB_REGISTER_NATIVE_TYPE(void*, Type::Pointer)
QGLIB_REGISTER_NATIVE_TYPE(const char*, Type::String)
QGLIB_REGISTER_NATIVE_TYPE(QByteArray, Type::String)
QGLIB_REGISTER_NATIVE_TYPE(QString, Type::String)

#undef QGLIB_REGISTER_NATIVE_TYPE

QGLIB_REGISTER_TYPE(QGlib::Type) //codegen: GType=G_TYPE_GTYPE

#endif // QGLIB_TYPE_H
