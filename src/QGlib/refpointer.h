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
#ifndef QGLIB_REFPOINTER_H
#define QGLIB_REFPOINTER_H

#include "type.h"
#include <stddef.h>

namespace QGlib {

template <class T>
class RefPointer
{
public:
    inline RefPointer();
    RefPointer(const RefPointer<T> & other);
    ~RefPointer();

    RefPointer<T> & operator=(const RefPointer<T> & other);
    void clear();

    inline bool isNull() const;
    inline operator bool() const;
    inline bool operator!() const;
    inline T *operator->() const;
    inline operator typename T::CType*() const;

    static RefPointer<T> wrap(typename T::CType *nativePtr, bool increaseRef = true);

    template <class X>
    RefPointer<X> staticCast() const;

    template <class X>
    RefPointer<X> dynamicCast() const;

private:
    T *m_class;
};


class RefCountedObject
{
protected:
    template <class T> friend class RefPointer;

    virtual ~RefCountedObject() {}

    virtual void ref() = 0;
    virtual void unref() = 0;

    void *m_object;
};


template <class T>
inline RefPointer<T>::RefPointer()
    : m_class(NULL)
{
}

template <class T>
RefPointer<T>::RefPointer(const RefPointer<T> & other)
    : m_class(NULL)
{
    operator=(other);
}

template <class T>
RefPointer<T>::~RefPointer()
{
    clear();
}

template <class T>
RefPointer<T> & RefPointer<T>::operator=(const RefPointer<T> & other)
{
    clear();

    if (!other.isNull()) {
        m_class = new T();
        m_class->m_object = other.m_class->m_object;
        static_cast<RefCountedObject*>(m_class)->ref();
    }

    return *this;
}

template <class T>
void RefPointer<T>::clear()
{
    if (!isNull()) {
        static_cast<RefCountedObject*>(m_class)->unref();
        delete m_class;
        m_class = NULL;
    }
}

//static
template <class T>
RefPointer<T> RefPointer<T>::wrap(typename T::CType *nativePtr, bool increaseRef)
{
    RefPointer<T> ptr;
    if (nativePtr != NULL) {
        ptr.m_class = new T();
        ptr.m_class->m_object = nativePtr;
        if (increaseRef) {
            static_cast<RefCountedObject*>(ptr.m_class)->ref();
        }
    }
    return ptr;
}

template <class T>
inline bool RefPointer<T>::isNull() const
{
    return m_class == NULL;
}

template <class T>
inline RefPointer<T>::operator bool() const
{
    return m_class != NULL;
}

template <class T>
inline bool RefPointer<T>::operator!() const
{
    return m_class == NULL;
}

template <class T>
inline T *RefPointer<T>::operator->() const
{
    return m_class;
}

template <class T>
inline RefPointer<T>::operator typename T::CType*() const
{
    return static_cast<typename T::CType*>(m_class->m_object);
}

template <class T>
template <class X>
RefPointer<X> RefPointer<T>::staticCast() const
{
    return isNull() ? RefPointer<X>()
                    : RefPointer<X>::wrap(static_cast<typename X::CType*>(m_class->m_object));
}

template <class T> struct GetType;

template <class T>
template <class X>
RefPointer<X> RefPointer<T>::dynamicCast() const
{
    if (!isNull() && Type::fromInstance(m_class->m_object).isA(GetType<X>())) {
        return RefPointer<X>::wrap(static_cast<typename X::CType*>(m_class->m_object));
    } else {
        return RefPointer<X>();
    }
}

}

#endif
