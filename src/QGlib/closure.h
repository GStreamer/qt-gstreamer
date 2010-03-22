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
#ifndef QGLIB_CLOSURE_H
#define QGLIB_CLOSURE_H

#include "value.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <QtCore/QDebug>

namespace QGlib {

class Closure : public RefCountedObject
{
    QGLIB_WRAPPER(Closure)
public:
    void setMarshaller(void (*marshaller)(GClosure*, GValue*, uint, const GValue*, void*, void*));

private:
    virtual void ref();
    virtual void unref();
};


class CppClosure
{
public:
    /** Constructs a new Closure that invokes the specified member function \a callback
     * on the specified \a object
     */
    template <typename T, typename R, typename... Args>
    static ClosurePtr newCppClosure(R (T::*callback)(Args...), T *object);

protected:
    /** \internal */
    struct ClosureDataBase
    {
        virtual ~ClosureDataBase() {}
    };

    /** This structure specifies the function that will be invoked by this C++ closure. */
    template <typename R, typename... Args>
    struct ClosureData : public ClosureDataBase
    {
        ClosureData(const boost::function<R (Args...)> & func) : function(func) {}
        boost::function<R (Args...)> function;
    };

    /** This is the C++ marshaller function.
     * It expects \a closure to be a closure created with newCppClosure(), as it invokes the
     * function specified in the ClosureData struct that was passed as argument to newCppClosure().
     * It transparently converts the given \a paramValues from GValue to the types expected by the
     * specified function and the function's return value back to GValue using the magic
     * ValueBase::get() / ValueBase::set() methods. Note that the template arguments must match
     * the ones of ClosureData, as they are used to determine the types of the function's arguments
     * and call the function in a type-safe manner.
     */
    template <typename R, typename... Args>
    static void genericCppMarshaller(GClosure *closure, GValue *returnValue, uint paramValuesCount,
                                     const GValue *paramValues, void *hint, void *data);

    /** Constructs a new Closure that invokes the function specified in \a closureData.
     * \note \a closureData MUST be an instance of the ClosureData struct, not just ClosureDataBase;
     * ClosureDataBase should be considered an implementation detail and should not be used directly.
     */
    static ClosurePtr newCppClosure(ClosureDataBase *closureData);

private:
    // callback to delete the ClosureData object that is pointed to by closure->data
    static void closureDestroyNotify(void *data, GClosure *closure);


    // helper functions to link genericCppMarshaller with the GObject world
    static ClosureDataBase *getClosureData(GClosure *closure);
    static QList<Value> convertParamValues(const GValue *values, uint number);


    // helper templates used by genericCppMarshaller
    typedef QList<Value>::const_iterator ArgsIterator;

    template <typename T, typename R, typename FirstArg, typename... Rest>
    static boost::function<R ()> unpackArguments(const T & function, ArgsIterator paramValuesBegin,
                                                 ArgsIterator paramValuesEnd);

    template <typename T, typename R>
    static boost::function<R ()> unpackArguments(const T & function, ArgsIterator paramValuesBegin,
                                                 ArgsIterator paramValuesEnd);

    template <typename R>
    static void invokeCallback(const boost::function<R ()> & callback, SharedValue & result);

    template <typename T, typename R, typename FirstArg, typename... Args>
    class Function
    {
    public:
        typedef R result_type;

        Function(const T & parentFunction, const FirstArg & boundArgument)
            : m_function(parentFunction), m_firstArg(boundArgument) {}

        R operator()(Args... args)
        {
            return m_function(m_firstArg, args...);
        }

    private:
        T m_function;
        FirstArg m_firstArg;
    };

};

template <typename T, typename R, typename... Args>
ClosurePtr CppClosure::newCppClosure(R (T::*callback)(Args...), T *object)
{
    auto function = Function<boost::function<R (T*,Args...)>, R, T*, Args...>(boost::mem_fn(callback),
                                                                              object);
    auto *data = new ClosureData<R, Args...>(function);

    ClosurePtr closure = newCppClosure(data);
    closure->setMarshaller(&genericCppMarshaller<R, Args...>);
    return closure;
}

template <typename R, typename... Args>
void CppClosure::genericCppMarshaller(GClosure *closure, GValue *returnValue, uint paramValuesCount,
                                      const GValue *paramValues, void *hint, void *data)
{
    Q_UNUSED(hint);
    Q_UNUSED(data);

    auto *cdata = static_cast< ClosureData<R, Args...>* >(getClosureData(closure));
    QList<Value> args = convertParamValues(paramValues, paramValuesCount);

    boost::function<R ()> callback =
                      unpackArguments<boost::function<R (Args...)>, R, Args...>(cdata->function,
                                                                                args.constBegin(),
                                                                                args.constEnd());
    SharedValue result(returnValue);
    invokeCallback<R>(callback, result);
}

template <typename T, typename R, typename FirstArg, typename... Rest>
boost::function<R ()> CppClosure::unpackArguments(const T & function, ArgsIterator paramValuesBegin,
                                                  ArgsIterator paramValuesEnd)
{
    FirstArg value;
    ArgsIterator newBegin(paramValuesBegin);
    if (paramValuesBegin != paramValuesEnd) {
        value = paramValuesBegin->get<FirstArg>();
        ++newBegin;
    } else {
        qWarning() << "QGlib::marshallerUnpackArguments:"
            << "More arguments are expected in the closure, but the signal doesn't provide them";
    }
    return unpackArguments< Function<T, R, FirstArg, Rest...>, R, Rest... >(
                                                Function<T, R, FirstArg, Rest...>(function, value),
                                                newBegin, paramValuesEnd);
}

template <typename T, typename R>
boost::function<R ()> CppClosure::unpackArguments(const T & function, ArgsIterator paramValuesBegin,
                                                  ArgsIterator paramValuesEnd)
{
    Q_UNUSED(paramValuesBegin);
    Q_UNUSED(paramValuesEnd);
    return function;
}

template <typename R>
void CppClosure::invokeCallback(const boost::function<R ()> & callback, SharedValue & result)
{
    R returnValue = callback();
    result.set(returnValue);
}

template <>
inline void CppClosure::invokeCallback<void>(const boost::function<void ()> & callback, SharedValue & result)
{
    Q_UNUSED(result);
    callback();
}

}

QGLIB_REGISTER_TYPE(QGlib::Closure)

#endif
