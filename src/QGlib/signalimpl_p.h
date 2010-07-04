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
#if !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

# ifndef IN_QGLIB_SIGNAL_H
#  error "This file must not be included directly"
# endif

# include "value.h"
# include "string.h"
# include <QtCore/QList>
# include <QtCore/QDebug>
# include <boost/type_traits.hpp>
# include <stdexcept>


namespace QGlib {
namespace Private {

template <typename Signature>
struct EmitImpl {};

} //namespace Private
} //namespace QGlib


# if QGLIB_HAVE_CXX0X

namespace QGlib {
namespace Private {

inline QList<Value> packArguments()
{
    return QList<Value>();
}

template <typename Arg1, typename... Args>
QList<Value> packArguments(Arg1 && a1, Args&&... args)
{
    typedef typename boost::add_const<
                typename boost::add_reference<Arg1>::type
            >::type ConstArg1Ref;

    QList<Value> result = packArguments(args...);
    //prepend, since we are packing from the last to the first argument
    result.prepend(Value(static_cast<ConstArg1Ref>(a1)));
    return result;
}

template <typename R, typename... Args>
struct EmitImpl<R (Args...)>
{
    static inline R emit(void *instance, const char *detailedSignal, Args&&... args)
    {
        Value && returnValue = Signal::emit(instance, detailedSignal, packArguments(args...));

        try {
            return returnValue.get<R>();
        } catch(const std::runtime_error &) {
            qCritical() << "Error during emission of signal" << detailedSignal
                        << "The returned value from emit is of different type than the one requested";
            return R();
        }
    }
};

template <typename... Args>
struct EmitImpl<void (Args...)>
{
    static inline void emit(void *instance, const char *detailedSignal, Args&&... args)
    {
        Value && returnValue = Signal::emit(instance, detailedSignal, packArguments(args...));

        if (returnValue.isValid()) {
            qWarning() << "Ignoring return value from emission of signal" << detailedSignal;
        }
    }
};

template <typename T, typename R, typename... Args>
class MemberFunction
{
public:
    inline MemberFunction(R (T::*fn)(Args...), T *obj)
        : m_function(fn), m_object(obj) {}

    inline R operator()(Args&&... args) const
    {
        return (m_object->*m_function)(args...);
    }

private:
    R (T::*m_function)(Args...);
    T *m_object;
};

template <typename T, typename R, typename... Args>
MemberFunction<T, R, Args...> mem_fn(R (T::*fn)(Args...), T *obj)
{
    return MemberFunction<T, R, Args...>(fn, obj);
}

} //namespace Private

template <typename R, typename... Args>
R Signal::emit(void *instance, const char *detailedSignal, Args&&... args)
{
    return QGlib::Private::EmitImpl<R (Args...)>::emit(instance, detailedSignal, args...);
}

template <typename T, typename R, typename... Args>
SignalHandler Signal::connect(void *instance, const char *detailedSignal,
                              T *receiver, R (T::*slot)(Args...), ConnectFlags flags)
{
    typedef QGlib::Private::MemberFunction<T, R, Args...> F;

    F && f = QGlib::Private::mem_fn(slot, receiver);
    ClosurePtr && closure = QGlib::Private::CppClosure<R (Args...), F>::create(f);
    return connect(instance, detailedSignal, closure, flags);
}

} //namespace QGlib

# else //QGLIB_HAVE_CXX0X

// include the second part of this file as many times as QGLIB_SIGNAL_MAX_ARGS specifies
#  define BOOST_PP_ITERATION_PARAMS_1 (3,(0, QGLIB_SIGNAL_MAX_ARGS, "QGlib/signalimpl_p.h"))
#  include BOOST_PP_ITERATE()
#  undef BOOST_PP_ITERATION_PARAMS_1

# endif //QGLIB_HAVE_CXX0X


#else // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

/*
    This part is included from BOOST_PP_ITERATE(). It defines specializations of struct EmitImpl
    with different number of arguments as well as the multiple implementations of the non-variadic
    Signal::emit and Signal::connect. This part is included multiple times (QGLIB_SIGNAL_MAX_ARGS
    defines how many), and each time it defines those classes and functions with different number
    of arguments. The concept is based on the implementation of boost::function.
*/

# define QGLIB_SIGNAL_IMPL_NUM_ARGS  BOOST_PP_ITERATION()

# define QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS \
        BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, typename A)

# define QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS \
        BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, A)

# define QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS \
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, A, a)

# define QGLIB_SIGNAL_IMPL_FUNCTION_ARGS \
        BOOST_PP_ENUM_TRAILING_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, a)

# if QGLIB_SIGNAL_IMPL_NUM_ARGS > 0
#  define QGLIB_SIGNAL_IMPL_COMMA ,
# else
#  define QGLIB_SIGNAL_IMPL_COMMA
# endif

# define QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP(z, n, list) \
        list.append(Value(static_cast< typename boost::add_const< typename boost::add_reference<A ##n>::type >::type >(a ##n)));

# define QGLIB_SIGNAL_IMPL_PACK_ARGS(list) \
        BOOST_PP_REPEAT(QGLIB_SIGNAL_IMPL_NUM_ARGS, QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP, list)

# define QGLIB_SIGNAL_IMPL_BIND_ARGS \
        BOOST_PP_ENUM_SHIFTED_PARAMS(BOOST_PP_INC(QGLIB_SIGNAL_IMPL_NUM_ARGS), _)

namespace QGlib {
namespace Private {

template <typename R QGLIB_SIGNAL_IMPL_COMMA
                     QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
struct EmitImpl<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
{
    static inline R emit(void *instance, const char *detailedSignal
                         QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS)
    {
        QList<Value> values;
        QGLIB_SIGNAL_IMPL_PACK_ARGS(values)
        Value returnValue = Signal::emit(instance, detailedSignal, values);

        try {
            return returnValue.get<R>();
        } catch(const std::runtime_error &) {
            qCritical() << "Error during emission of signal" << detailedSignal
                        << "The returned value from emit is of different type than the one requested";
            return R();
        }
    }
};

template <QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
struct EmitImpl<void (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
{
    static inline void emit(void *instance, const char *detailedSignal
                            QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS)
    {
        QList<Value> values;
        QGLIB_SIGNAL_IMPL_PACK_ARGS(values)
        Value returnValue = Signal::emit(instance, detailedSignal, values);

        if (returnValue.isValid()) {
            qWarning() << "Ignoring return value from emission of signal" << detailedSignal;
        }
    }
};

} //namespace Private

template <typename R QGLIB_SIGNAL_IMPL_COMMA
                     QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
R Signal::emit(void *instance, const char *detailedSignal QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS)
{
    return QGlib::Private::EmitImpl<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
                             ::emit(instance, detailedSignal QGLIB_SIGNAL_IMPL_FUNCTION_ARGS);
}

template <typename T, typename R QGLIB_SIGNAL_IMPL_COMMA
                                 QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
SignalHandler Signal::connect(void *instance, const char *detailedSignal,
                              T *receiver, R (T::*slot)(QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS),
                              ConnectFlags flags)
{
    boost::function<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)> f
                = boost::bind(slot, receiver QGLIB_SIGNAL_IMPL_COMMA
                                             QGLIB_SIGNAL_IMPL_BIND_ARGS);

    ClosurePtr closure = QGlib::Private::CppClosure<
                                        R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS),
                                        boost::function<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
                                                   >::create(f);
    return connect(instance, detailedSignal, closure, flags);
}

} //namespace QGlib

# undef QGLIB_SIGNAL_IMPL_BIND_ARGS
# undef QGLIB_SIGNAL_IMPL_PACK_ARGS
# undef QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP
# undef QGLIB_SIGNAL_IMPL_COMMA
# undef QGLIB_SIGNAL_IMPL_FUNCTION_ARGS
# undef QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS
# undef QGLIB_SIGNAL_IMPL_NUM_ARGS

#endif // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING
