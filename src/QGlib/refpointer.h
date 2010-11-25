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

#include "global.h"
#include "type.h"
#include <cstddef>
#include <boost/type_traits.hpp>

namespace QGlib {

/*! \headerfile refpointer.h <QGlib/RefPointer>
 * \brief Smart pointer class for working with wrapper classes that support reference counting
 *
 * Nearly all GObject and GStreamer classes are designed to work with reference counting.
 * This class provides a smart pointer for instances of those classes which takes care of
 * increasing and decreasing the reference count automatically when a new pointer is
 * constructed and destructed, respectively.
 *
 * All wrapper classes that wrap reference-counted objects must be used with RefPointer.
 * For convenience, this library provides typedefs for all the reference-counted wrappers,
 * which are in the form:
 * \code
 * typedef RefPointer<Foo> FooPtr;
 * \endcode
 * So, for example, if you want to use an instance of a QGst::Element, you should declare
 * a pointer to it like that:
 * \code
 * QGst::ElementPtr element;
 * \endcode
 *
 * For reference-counted classes that also have the concept of writability only when their
 * reference count is 1 (like GstMiniObject and GstCaps), this class also automatically
 * takes care of making a copy of the object when you are trying to access a non-const
 * method. This is similar to Qt's implicit sharing technique.
 */
template <class T>
class RefPointer
{
public:
    inline RefPointer();
    inline ~RefPointer();

    inline RefPointer(const RefPointer<T> & other);
    template <class X>
    inline RefPointer(const RefPointer<X> & other);
    inline RefPointer<T> & operator=(const RefPointer<T> & other);
    template <class X>
    inline RefPointer<T> & operator=(const RefPointer<X> & other);

    void clear();

    inline bool isNull() const;
    inline operator bool() const;
    inline bool operator!() const;
    inline T *operator->();
    inline const T *operator->() const;

    /*! Cast operator that implicitly casts the smart pointer to the pointer type
     * of the underlying C instance. For example, RefPointer<QGst::Element> will cast
     * to GstElement*. This is provided as a helper tool for working with native C
     * methods if needed.
     * \note the returned pointer does not have an increased reference count
     */
    inline operator typename T::CType*() const;

    /*! Makes a RefPointer out of a pointer to a native C instance. If \a increaseRef
     * is specified as false, the reference count is not increased on construction
     * (but it is decreased on destruction!).
     */
    static RefPointer<T> wrap(typename T::CType *nativePtr, bool increaseRef = true);

    /*! Statically casts this RefPointer to a RefPointer of another class. */
    template <class X>
    RefPointer<X> staticCast() const;

    /*! Dynamically casts this RefPointer to a RefPointer of another class.
     * This is equivalent to the built-in dynamic_cast, but it uses the Glib type system
     * to determine if the cast is allowed or not. If the cast fails, it returns a null
     * RefPointer.
     *
     * This method also allows you to cast an object to an interface that it implements.
     * For example, you can do:
     * \code
     * QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc");
     * QGst::UriHandlerPtr uriHandler = filesrc.dynamicCast<QGst::UriHandler>();
     * \endcode
     * because a "filesrc" element implements the GstUriHandler interface.
     */
    template <class X>
    RefPointer<X> dynamicCast() const;

private:
    template <class X> friend class RefPointer;
    template <class X>
    void assign(const RefPointer<X> & other);

    T *m_class;
};

/*! \headerfile refpointer.h <QGlib/RefPointer>
 * \brief Base class for all the reference-counted object wrappers.
 */
class RefCountedObject
{
protected:
    template <class T> friend class RefPointer;

    virtual ~RefCountedObject() {}

    virtual void ref(bool increaseRef) = 0;
    virtual void unref() = 0;

    template <class T>
    inline T* object();

    template <class T>
    inline T* object() const;

    void *m_object;
};

template <class T>
inline T* RefCountedObject::object()
{
    return static_cast<T*>(m_object);
}

template <class T>
inline T* RefCountedObject::object() const
{
    return static_cast<T* const>(m_object);
}


template <class T>
inline RefPointer<T>::RefPointer()
    : m_class(NULL)
{
}

template <class T>
inline RefPointer<T>::~RefPointer()
{
    clear();
}

template <class T>
template <class X>
inline RefPointer<T>::RefPointer(const RefPointer<X> & other)
    : m_class(NULL)
{
    assign(other);
}

template <class T>
inline RefPointer<T>::RefPointer(const RefPointer<T> & other)
    : m_class(NULL)
{
    assign(other);
}

template <class T>
template <class X>
inline RefPointer<T> & RefPointer<T>::operator=(const RefPointer<X> & other)
{
    clear();
    assign(other);
    return *this;
}

template <class T>
inline RefPointer<T> & RefPointer<T>::operator=(const RefPointer<T> & other)
{
    clear();
    assign(other);
    return *this;
}

template <class T>
template <class X>
void RefPointer<T>::assign(const RefPointer<X> & other)
{
    //T should be a base class of X
    QGLIB_STATIC_ASSERT((boost::is_base_of<T, X>::value),
                        "Cannot upcast a RefPointer without using dynamicCast()");

    if (!other.isNull()) {
        m_class = new T();
        m_class->m_object = other.m_class->m_object;
        static_cast<RefCountedObject*>(m_class)->ref(true);
    }
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
        static_cast<RefCountedObject*>(ptr.m_class)->ref(increaseRef);
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
inline T *RefPointer<T>::operator->()
{
    Q_ASSERT_X(!isNull() ,"RefPointer::operator->()",
               "Attempted to dereference a null pointer");
    return m_class;
}

template <class T>
inline const T *RefPointer<T>::operator->() const
{
    Q_ASSERT_X(!isNull(), "RefPointer::operator->() const",
               "Attempted to dereference a null pointer");
    return m_class;
}

template <class T>
inline RefPointer<T>::operator typename T::CType*() const
{
    return m_class ? static_cast<typename T::CType*>(m_class->m_object) : NULL;
}

template <class T>
template <class X>
RefPointer<X> RefPointer<T>::staticCast() const
{
    return isNull() ? RefPointer<X>()
        : RefPointer<X>::wrap(static_cast<typename X::CType*>(static_cast<X*>(m_class)->m_object));
}

template <class T>
template <class X>
RefPointer<X> RefPointer<T>::dynamicCast() const
{
    if (!isNull() && QGlib::Private::CanConvertTo<X>::from(m_class->m_object)) {
        return RefPointer<X>::wrap(static_cast<typename X::CType*>(m_class->m_object));
    } else {
        return RefPointer<X>();
    }
}

// trick GetType to return the same type for GetType<T>() and GetType< RefPointer<T> >()
template <class T>
struct GetTypeImpl< RefPointer<T> >
{
    inline operator Type() { return GetType<T>(); }
};

}

#endif
