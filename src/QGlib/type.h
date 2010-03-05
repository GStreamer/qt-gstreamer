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

#include "gdeclarations.h"
#include "quark.h"

namespace QGlib {

template <class T> class RefPointer;

class Type
{
public:
    enum FundamentalType {
        Invalid = 0,
        None = 1<<2,
        Interface = 2<<2,
        Char = 3<<2,
        UChar = 4<<2,
        Boolean = 5<<2,
        Int = 6<<2,
        UInt = 7<<2,
        Long = 8<<2,
        ULong = 9<<2,
        Int64 = 10<<2,
        UInt64 = 11<<2,
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
    inline operator unsigned long() { return m_type; }

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


template <class T>
struct GetType { inline operator Type() { return T::type(); } };

template <class T>
struct GetType< RefPointer<T> > { inline operator Type() { return T::type(); } };

template <class T>
struct GetType<T*> { inline operator Type() { return Type::Pointer; } };

#define QGLIB_REGISTER_TYPE_DECLARATION(T) \
    template <> \
    struct GetType<T> { operator Type(); };

#define QGLIB_REGISTER_TYPE_IMPLEMENTATION(T, GTYPE) \
    template <> \
    GetType<T>::operator Type() { return (GTYPE); }

#define QGLIB_REGISTER_TYPE(T, GTYPE) \
    template <> \
    struct GetType<T> { inline operator Type() { return (GTYPE); } };

QGLIB_REGISTER_TYPE(char, Type::Char)
QGLIB_REGISTER_TYPE(unsigned char, Type::UChar)
QGLIB_REGISTER_TYPE(bool, Type::Boolean)
QGLIB_REGISTER_TYPE(int, Type::Int)
QGLIB_REGISTER_TYPE(unsigned int, Type::UInt)
QGLIB_REGISTER_TYPE(long, Type::Long)
QGLIB_REGISTER_TYPE(unsigned long, Type::ULong)
QGLIB_REGISTER_TYPE(qint64, Type::Int64)
QGLIB_REGISTER_TYPE(quint64, Type::UInt64)
QGLIB_REGISTER_TYPE(float, Type::Float)
QGLIB_REGISTER_TYPE(double, Type::Double)
QGLIB_REGISTER_TYPE(QString, Type::String)
QGLIB_REGISTER_TYPE(QByteArray, Type::String)
QGLIB_REGISTER_TYPE(char*, Type::String)
QGLIB_REGISTER_TYPE(GParamSpec*, Type::Param)

}

#endif // QGLIB_TYPE_H
