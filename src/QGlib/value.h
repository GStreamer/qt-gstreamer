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

namespace QGlib {

class ValueBase
{
public:
    void reset();

    bool isValid() const;
    Type type() const;

    template <typename T> T get() const;
    template <typename T> void set(const T & data);

    QString dumpContents() const;
    bool canTransformTo(Type type) const;
    Value transformTo(Type type) const;

    inline GValue *peekNativeObject() { return m_value; }
    inline const GValue *peekNativeObject() const { return m_value; }

#define QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(NICK, TYPE) \
    TYPE get ##NICK () const; \
    void set ##NICK (TYPE data)

    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Bool, bool);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Char, char);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Uchar, uchar);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Int, int);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Uint, uint);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Long, long);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Ulong, ulong);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Int64, qint64);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Uint64, quint64);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Float, float);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Double, double);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Enum, int);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Flags, uint);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Pointer, void*);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Boxed, void*);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(ParamSpec, GParamSpec*);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(Object, void*);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(String, const char*);
    QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION(GType, ulong);

#undef QGLIBVALUE_FUNDAMENTAL_GETSET_DECLARATION

protected:
    ValueBase(GValue *val);
    virtual ~ValueBase();
    Q_DISABLE_COPY(ValueBase)

    GValue *m_value;
};

class Value : public ValueBase
{
public:
    Value();
    Value(const GValue & gvalue);
    template <typename T> explicit Value(const T & data);
    Value(const Value & other);
    virtual ~Value();
    Value & operator=(const Value & other);

    void init(Type type);
};

class SharedValue : public ValueBase
{
public:
    SharedValue(GValue *gvalue);
    SharedValue(const SharedValue & other);
    virtual ~SharedValue();
    SharedValue & operator=(const SharedValue & other);
};

} //namespace QGlib

#define QGLIB_VALUEIMPL_H
# include "valueimpl/valueimpl.h"
# include "valueimpl/nativetypes.h"
#undef QGLIB_VALUEIMPL_H

namespace QGlib {

template <typename T>
T ValueBase::get() const
{
    Q_ASSERT(isValid());
    return ValueImpl<T>::get(*this);
}

template <typename T>
void ValueBase::set(const T & data)
{
    Q_ASSERT(isValid());
    ValueImpl<T>::set(*this, data);
}


template <typename T>
Value::Value(const T & data)
    : ValueBase(NULL)
{
    init(GetType<T>());
    set(data);
}

}

#endif
