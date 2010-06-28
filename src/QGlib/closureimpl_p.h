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

# ifndef IN_QGLIB_CLOSURE_H
#  error "This file must not be included directly"
# endif

# include "value.h"
# include <QtCore/QList>
# include <stdexcept>
# include <boost/type_traits.hpp>


namespace QGlib {
namespace Private {

class ClosureDataBase
{
public:
    inline virtual ~ClosureDataBase() {}

    typedef void (*CppMarshaller)(SharedValue &, const QList<Value> &, ClosureDataBase*);
    CppMarshaller cppMarshaller;

protected:
    inline ClosureDataBase(CppMarshaller marshal) : cppMarshaller(marshal) {}
};


/*! This is the method that glues the c++ closure system with the GObject world.
 * It creates a GClosure, sets its data, marshaller and finalize notifier accordingly
 * and returns a ClosurePtr. This is the only exported symbol from this library that
 * allows you to create a GClosure and it should only be used through the
 * template CppClosure class.
 */
ClosurePtr createCppClosure(ClosureDataBase *data); //implemented in closure.cpp


template <typename Function, typename R>
struct invoker
{
    static inline void invoke(const Function & f, SharedValue & result) { result.set<R>(f()); }
};

template <typename Function>
struct invoker<Function, void>
{
    static inline void invoke(const Function & f, SharedValue &) { f(); }
};


template <typename Signature, typename Function>
struct CppClosure {};

} //namespace Private
} //namespace QGlib


# if QGLIB_HAVE_CXX0X

namespace QGlib {
namespace Private {

template <typename ParentFunction, typename R, typename Arg1, typename... Args>
class BoundArgumentFunction
{
public:
    inline BoundArgumentFunction(ParentFunction && fn, Arg1 && arg)
        : m_function(fn), m_firstArg(arg) {}

    inline R operator()(Args&&... args) const
    {
        return m_function(m_firstArg, args...);
    }

private:
    ParentFunction && m_function;
    Arg1 && m_firstArg;
};

template <typename F, typename R, typename Arg1, typename... Args>
inline BoundArgumentFunction<F, R, Arg1, Args...> partial_bind(F && f, Arg1 && a1)
{
    return BoundArgumentFunction<F, R, Arg1, Args...>(f, a1);
}


template <typename F, typename R>
inline void unpackAndInvoke(F && function, SharedValue & result,
                            QList<Value>::const_iterator &&,
                            QList<Value>::const_iterator &&)
{
    invoker<F, R>::invoke(function, result);
}

template <typename F, typename R, typename Arg1, typename... Args>
inline void unpackAndInvoke(F && function, SharedValue & result,
                            QList<Value>::const_iterator && argsBegin,
                            QList<Value>::const_iterator && argsEnd)
{
    typedef typename boost::remove_const<
                typename boost::remove_reference<Arg1>::type
            >::type CleanArg1;
    typedef BoundArgumentFunction<F, R, Arg1, Args...> F1;

    CleanArg1 && boundArg = argsBegin->get<CleanArg1>();
    F1 && f = partial_bind<F, R, Arg1, Args...>(function, boundArg);
    unpackAndInvoke< F1, R, Args... >(f, result, ++argsBegin, argsEnd);
}


template <typename F, typename R, typename... Args>
struct CppClosure<R (Args...), F>
{
    class ClosureData : public ClosureDataBase
    {
    public:
        inline ClosureData(CppMarshaller marshal, const F & func)
            : ClosureDataBase(marshal), function(func) {}
        F function;
    };

    static inline ClosurePtr create(const F & function)
    {
        return createCppClosure(new ClosureData(&marshaller, function));
    }

    static inline void marshaller(SharedValue & result, const QList<Value> & params, ClosureDataBase *data)
    {
        if (static_cast<unsigned int>(params.size()) < sizeof...(Args)) {
            throw std::runtime_error("The signal provides less arguments than what the closure expects");
        }

        ClosureData *cdata = static_cast<ClosureData*>(data);
        unpackAndInvoke<F, R, Args...>(cdata->function, result, params.constBegin(), params.constEnd());
    }
};

} //namespace Private
} //namespace QGlib

# else //QGLIB_HAVE_CXX0X

#  if !defined(QGLIB_CLOSURE_MAX_ARGS)
#   define QGLIB_CLOSURE_MAX_ARGS 9
#  endif

#  include <boost/function.hpp>
#  include <boost/preprocessor.hpp>
#  include <boost/bind.hpp>

// include the second part of this file as many times as QGLIB_CLOSURE_MAX_ARGS specifies
#  define BOOST_PP_ITERATION_PARAMS_1 (3,(0, QGLIB_CLOSURE_MAX_ARGS, "QGlib/closureimpl_p.h"))
#  include BOOST_PP_ITERATE()

#  undef BOOST_PP_ITERATION_PARAMS_1
#  undef QGLIB_CLOSURE_MAX_ARGS

# endif //QGLIB_HAVE_CXX0X


#else // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

/*
    This part is included from BOOST_PP_ITERATE(). It defines a CppClosureN class
    (where N is the number of template arguments it takes) and a specialization for class
    CppClosure, so that the CppClosure<R (Args...), F> syntax is supported. This part is
    included multiple times (QGLIB_CLOSURE_MAX_ARGS defines how many), and each time
    it defines those classes with different number of arguments.
    The concept is based on the implementation of boost::function.
*/

# define QGLIB_CLOSURE_IMPL_NUM_ARGS  BOOST_PP_ITERATION()

# define QGLIB_CLOSURE_IMPL_TEMPLATE_PARAMS \
        BOOST_PP_ENUM_PARAMS(QGLIB_CLOSURE_IMPL_NUM_ARGS, typename A)

# define QGLIB_CLOSURE_IMPL_TEMPLATE_ARGS \
        BOOST_PP_ENUM_PARAMS(QGLIB_CLOSURE_IMPL_NUM_ARGS, A)

# define QGLIB_CLOSURE_IMPL_CPPCLOSUREN \
        BOOST_PP_CAT(CppClosure, QGLIB_CLOSURE_IMPL_NUM_ARGS)

# if QGLIB_CLOSURE_IMPL_NUM_ARGS > 0
#  define QGLIB_CLOSURE_IMPL_COMMA ,
# else
#  define QGLIB_CLOSURE_IMPL_COMMA
# endif

# define QGLIB_CLOSURE_IMPL_UNPACK_ARGS_STEP(z, n, list) \
    ,list.at(n).get< typename boost::remove_const< typename boost::remove_reference<A ##n>::type >::type >()

# define QGLIB_CLOSURE_IMPL_UNPACK_ARGS(list) \
    BOOST_PP_REPEAT(QGLIB_CLOSURE_IMPL_NUM_ARGS, QGLIB_CLOSURE_IMPL_UNPACK_ARGS_STEP, list)

namespace QGlib {
namespace Private {

template <typename F, typename R QGLIB_CLOSURE_IMPL_COMMA
                                 QGLIB_CLOSURE_IMPL_TEMPLATE_PARAMS>
struct QGLIB_CLOSURE_IMPL_CPPCLOSUREN
{
    class ClosureData : public ClosureDataBase
    {
    public:
        inline ClosureData(CppMarshaller marshal, const F & func)
            : ClosureDataBase(marshal), function(func) {}
        F function;
    };

    static ClosurePtr create(const F & function)
    {
        return createCppClosure(new ClosureData(&marshaller, function));
    }

    static void marshaller(SharedValue & result, const QList<Value> & params, ClosureDataBase *data)
    {
        if (params.size() < QGLIB_CLOSURE_IMPL_NUM_ARGS) {
            throw std::runtime_error("The signal provides less arguments than what the closure expects");
        }

        ClosureData *cdata = static_cast<ClosureData*>(data);
# if QGLIB_CLOSURE_IMPL_NUM_ARGS > 0
        boost::function<R ()> callback = boost::bind<R>(cdata->function
                                                        QGLIB_CLOSURE_IMPL_UNPACK_ARGS(params));
        invoker< boost::function<R ()>, R >::invoke(callback, result);
# else
        invoker< F, R >::invoke(cdata->function, result);
# endif
    }
};

//partial specialization of struct CppClosure to support the CppClosure<R (Args...), F> syntax
template <typename F, typename R  QGLIB_CLOSURE_IMPL_COMMA
                                  QGLIB_CLOSURE_IMPL_TEMPLATE_PARAMS>
struct CppClosure<R (QGLIB_CLOSURE_IMPL_TEMPLATE_ARGS), F>
    : public QGLIB_CLOSURE_IMPL_CPPCLOSUREN< F, R  QGLIB_CLOSURE_IMPL_COMMA
                                                   QGLIB_CLOSURE_IMPL_TEMPLATE_ARGS >
{
};

} //namespace Private
} //namespace QGlib

# undef QGLIB_CLOSURE_IMPL_UNPACK_ARGS
# undef QGLIB_CLOSURE_IMPL_UNPACK_ARGS_STEP
# undef QGLIB_CLOSURE_IMPL_COMMA
# undef QGLIB_CLOSURE_IMPL_CPPCLOSUREN
# undef QGLIB_CLOSURE_IMPL_TEMPLATE_ARGS
# undef QGLIB_CLOSURE_IMPL_TEMPLATE_PARAMS
# undef QGLIB_CLOSURE_IMPL_NUM_ARGS

#endif // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING
