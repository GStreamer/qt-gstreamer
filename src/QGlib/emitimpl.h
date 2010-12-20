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
# include <QtCore/QList>
# include <QtCore/QDebug>
# include <stdexcept>
# include <boost/type_traits.hpp>


namespace QGlib {
namespace Private {

template <typename Signature>
struct EmitImpl {};

} //namespace Private
} //namespace QGlib


# if QGLIB_HAVE_CXX0X

namespace QGlib {
namespace Private {

//BEGIN ******** packArguments ********

inline QList<Value> packArguments()
{
    return QList<Value>();
}

template <typename Arg1, typename... Args>
QList<Value> packArguments(const Arg1 & a1, const Args & ... args)
{
    QList<Value> && result = packArguments(args...);
    Value v;
    v.init<Arg1>();
    ValueImpl<Arg1>::set(v, a1);
    //prepend, since we are packing from the last to the first argument
    result.prepend(v);
    return result;
}

//END ******** packArguments ********
//BEGIN ******** EmitImpl ********

template <typename R, typename... Args>
struct EmitImpl<R (Args...)>
{
    static inline R emit(void *instance, const char *detailedSignal, const Args & ... args)
    {
        try {
            Value && returnValue = Signal::emit(instance, detailedSignal, packArguments(args...));
            return ValueImpl<R>::get(returnValue);
        } catch(const std::exception & e) {
            qCritical() << "Error during emission of signal" << detailedSignal << ":" << e.what();
            return R();
        }
    }
};

template <typename... Args>
struct EmitImpl<void (Args...)>
{
    static inline void emit(void *instance, const char *detailedSignal, const Args & ... args)
    {
        try {
            Value && returnValue = Signal::emit(instance, detailedSignal, packArguments(args...));

            if (returnValue.isValid()) {
                qWarning() << "Ignoring return value from emission of signal" << detailedSignal;
            }
        } catch(const std::exception & e) {
            qCritical() << "Error during emission of signal" << detailedSignal << ":" << e.what();
        }
    }
};

//END ******** EmitImpl ********

} //namespace Private

//BEGIN ******** Signal::emit ********

template <typename R, typename... Args>
R Signal::emit(void *instance, const char *detailedSignal, const Args & ... args)
{
    return QGlib::Private::EmitImpl<R (Args...)>::emit(instance, detailedSignal, args...);
}

//END ******** Signal::emit ********

} //namespace QGlib

# else //QGLIB_HAVE_CXX0X

// include the second part of this file as many times as QGLIB_SIGNAL_MAX_ARGS specifies
#  define BOOST_PP_ITERATION_PARAMS_1 (3,(0, QGLIB_SIGNAL_MAX_ARGS, "QGlib/emitimpl.h"))
#  include BOOST_PP_ITERATE()
#  undef BOOST_PP_ITERATION_PARAMS_1

# endif //QGLIB_HAVE_CXX0X


#else // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

/*
    This part is included from BOOST_PP_ITERATE(). It defines specializations of struct EmitImpl
    with different number of arguments as well as the multiple implementations of the non-variadic
    Signal::emit. This part is included multiple times (QGLIB_SIGNAL_MAX_ARGS defines how many),
    and each time it defines those classes and functions with different number of arguments.
    The concept is based on the implementation of boost::function.
*/

# define QGLIB_SIGNAL_IMPL_NUM_ARGS  \
    BOOST_PP_ITERATION()

# define QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_PARAMS \
    BOOST_PP_ENUM_TRAILING_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, typename A)

# define QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS \
    BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, typename A)

# define QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_ARGS \
    BOOST_PP_ENUM_TRAILING_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, A)

# define QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS \
    BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, A)

# define QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS \
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, const A, & a)

# define QGLIB_SIGNAL_IMPL_FUNCTION_ARGS \
    BOOST_PP_ENUM_TRAILING_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, a)


namespace QGlib {
namespace Private {

//BEGIN ******** boostpp EmitImpl ********

# define QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP(z, n, list) \
    { \
        Value v; \
        v.init<A##n>(); \
        ValueImpl<A##n>::set(v, a##n); \
        list.append(v); \
    }

# define QGLIB_SIGNAL_IMPL_PACK_ARGS(list) \
    BOOST_PP_REPEAT(QGLIB_SIGNAL_IMPL_NUM_ARGS, QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP, list)

template <typename R QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_PARAMS>
struct EmitImpl<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
{
    static inline R emit(void *instance, const char *detailedSignal
                         QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS)
    {
        try {
            QList<Value> values;
            QGLIB_SIGNAL_IMPL_PACK_ARGS(values)
            Value returnValue = Signal::emit(instance, detailedSignal, values);
            return ValueImpl<R>::get(returnValue);
        } catch(const std::exception & e) {
            qCritical() << "Error during emission of signal" << detailedSignal << ":" << e.what();
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
        try {
            QList<Value> values;
            QGLIB_SIGNAL_IMPL_PACK_ARGS(values)
            Value returnValue = Signal::emit(instance, detailedSignal, values);
            if (returnValue.isValid()) {
                qWarning() << "Ignoring return value from emission of signal" << detailedSignal;
            }
        } catch(const std::exception & e) {
            qCritical() << "Error during emission of signal" << detailedSignal << ":" << e.what();
        }
    }
};

# undef QGLIB_SIGNAL_IMPL_PACK_ARGS
# undef QGLIB_SIGNAL_IMPL_PACK_ARGS_STEP

//END ******** boostpp EmitImpl ********

} //namespace Private

//BEGIN ******** boostpp Signal::emit ********

template <typename R QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_PARAMS>
R Signal::emit(void *instance, const char *detailedSignal QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS)
{
    return QGlib::Private::EmitImpl<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS)>
                             ::emit(instance, detailedSignal QGLIB_SIGNAL_IMPL_FUNCTION_ARGS);
}

//END ******** boostpp Signal::emit ********

} //namespace QGlib

# undef QGLIB_SIGNAL_IMPL_FUNCTION_ARGS
# undef QGLIB_SIGNAL_IMPL_FUNCTION_PARAMS
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS
# undef QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_ARGS
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS
# undef QGLIB_SIGNAL_IMPL_TRAILING_TEMPLATE_PARAMS
# undef QGLIB_SIGNAL_IMPL_NUM_ARGS

#endif // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING
