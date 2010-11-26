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
# include "refpointer.h"
# include <QtCore/QList>
# include <stdexcept>
# include <boost/type_traits.hpp>


namespace QGlib {

//BEGIN ******** Closure ********

/* Wrapper class for GClosure. It contains no public methods, it is
 * only used internally to pass around GClosure pointers in a C++ fashion.
 */
class Closure : public RefCountedObject
{
    QGLIB_WRAPPER(Closure)

    virtual void ref(bool increaseRef);
    virtual void unref();
};

//END ******** Closure ********

namespace Private {

//BEGIN ******** Closure internals ********

class ClosureDataBase
{
public:
    inline virtual ~ClosureDataBase() {}
    virtual void marshaller(SharedValue &, const QList<Value> &) = 0;

    bool passSender; //whether to pass the sender instance as the first slot argument

protected:
    inline ClosureDataBase(bool passSender)
        : passSender(passSender) {}
};


/*! This is the method that glues the c++ closure system with the GObject world.
 * It creates a GClosure, sets its data, marshaller and finalize notifier accordingly
 * and returns a ClosurePtr. This is the only exported symbol from this library that
 * allows you to create a GClosure and it should only be used through the
 * template CppClosure class.
 */
ClosurePtr createCppClosure(ClosureDataBase *data); //implemented in signal.cpp


template <typename Signature, typename Function>
struct CppClosure {};

//END ******** Closure internals ********
//BEGIN ******** invoker ********

template <typename Function, typename R>
struct invoker
{
    static inline void invoke(const Function & f, SharedValue & result) { ValueImpl<R>::set(result, f()); }
};

template <typename Function>
struct invoker<Function, void>
{
    static inline void invoke(const Function & f, SharedValue &) { f(); }
};

//END ******** invoker ********

} //namespace Private
} //namespace QGlib


# if QGLIB_HAVE_CXX0X

namespace QGlib {
namespace Private {

//BEGIN ******** MemberFunction ********

template <typename T, typename R, typename... Args>
class MemberFunction
{
public:
    inline MemberFunction(R (T::*fn)(Args...), T *obj)
        : m_function(fn), m_object(obj) {}

    inline R operator()(Args&&... args) const
    {
        return (m_object->*m_function)(std::forward<Args>(args)...);
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

//END ******** MemberFunction ********
//BEGIN ******** BoundArgumentFunction ********

template <typename ParentFunction, typename R, typename Arg1, typename... Args>
class BoundArgumentFunction
{
public:
    inline BoundArgumentFunction(ParentFunction && fn, Arg1 && arg)
        : m_function(std::forward<ParentFunction>(fn)),
          m_firstArg(std::forward<Arg1>(arg)) {}

    inline R operator()(Args&&... args) const
    {
        return m_function(std::forward<Arg1>(m_firstArg), std::forward<Args>(args)...);
    }

private:
    ParentFunction && m_function;
    Arg1 && m_firstArg;
};

template <typename F, typename R, typename Arg1, typename... Args>
inline BoundArgumentFunction<F, R, Arg1, Args...> partial_bind(F && f, Arg1 && a1)
{
    return BoundArgumentFunction<F, R, Arg1, Args...>(std::forward<F>(f), std::forward<Arg1>(a1));
}

//END ******** BoundArgumentFunction ********
//BEGIN ******** unpackAndInvoke ********

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

    CleanArg1 && boundArg = ValueImpl<CleanArg1>::get(*argsBegin);
    F1 && f = partial_bind<F, R, Arg1, Args...>(std::forward<F>(function), std::forward<Arg1>(boundArg));

    unpackAndInvoke< F1, R, Args... >(std::forward<F1>(f), result,
                                      std::forward<QList<Value>::const_iterator>(++argsBegin),
                                      std::forward<QList<Value>::const_iterator>(argsEnd));
}

//END ******** unpackAndInvoke ********
//BEGIN ******** CppClosure ********

template <typename F, typename R, typename... Args>
struct CppClosure<R (Args...), F>
{
    class ClosureData : public ClosureDataBase
    {
    public:
        inline ClosureData(const F & func, bool passSender)
            : ClosureDataBase(passSender), m_function(func) {}

        virtual void marshaller(SharedValue & result, const QList<Value> & params)
        {
            if (static_cast<unsigned int>(params.size()) < sizeof...(Args)) {
                throw std::logic_error("The signal provides less arguments than what the closure expects");
            }

            unpackAndInvoke<F, R, Args...>(std::forward<F>(m_function), result,
                                           params.constBegin(), params.constEnd());
        }

    private:
        F m_function;
    };

    static inline ClosurePtr create(const F & function, bool passSender)
    {
        return createCppClosure(new ClosureData(function, passSender));
    }
};

//END ******** CppClosure ********

} //namespace Private

//BEGIN ******** Signal::connect ********

template <typename T, typename R, typename... Args>
SignalHandler Signal::connect(void *instance, const char *detailedSignal,
                              T *receiver, R (T::*slot)(Args...), ConnectFlags flags)
{
    typedef QGlib::Private::MemberFunction<T, R, Args...> F;

    F && f = QGlib::Private::mem_fn(slot, receiver);
    ClosurePtr && closure = QGlib::Private::CppClosure<R (Args...), F>::create(f, flags & PassSender);
    return connect(instance, detailedSignal, closure, flags);
}

//END ******** Signal::connect ********

} //namespace QGlib

# else //QGLIB_HAVE_CXX0X

#  include <boost/function.hpp>
#  include <boost/preprocessor.hpp>
#  include <boost/bind.hpp>

// include the second part of this file as many times as QGLIB_SIGNAL_MAX_ARGS specifies
#  define BOOST_PP_ITERATION_PARAMS_1 (3,(0, QGLIB_SIGNAL_MAX_ARGS, "QGlib/connectimpl_p.h"))
#  include BOOST_PP_ITERATE()

#  undef BOOST_PP_ITERATION_PARAMS_1
#  undef QGLIB_SIGNAL_MAX_ARGS

# endif //QGLIB_HAVE_CXX0X


#else // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING

/*
    This part is included from BOOST_PP_ITERATE(). It defines a CppClosureN class
    (where N is the number of template arguments it takes) and a specialization for class
    CppClosure, so that the CppClosure<R (Args...), F> syntax is supported. This part is
    included multiple times (QGLIB_SIGNAL_MAX_ARGS defines how many), and each time
    it defines those classes with different number of arguments.
    The concept is based on the implementation of boost::function.
*/

# define QGLIB_SIGNAL_IMPL_NUM_ARGS  BOOST_PP_ITERATION()

# define QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS \
        BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, typename A)

# define QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS \
        BOOST_PP_ENUM_PARAMS(QGLIB_SIGNAL_IMPL_NUM_ARGS, A)

# if QGLIB_SIGNAL_IMPL_NUM_ARGS > 0
#  define QGLIB_SIGNAL_IMPL_COMMA ,
# else
#  define QGLIB_SIGNAL_IMPL_COMMA
# endif


namespace QGlib {
namespace Private {

//BEGIN ******** boostpp CppClosure ********

# define QGLIB_SIGNAL_IMPL_CPPCLOSUREN \
        BOOST_PP_CAT(CppClosure, QGLIB_SIGNAL_IMPL_NUM_ARGS)

# define QGLIB_SIGNAL_IMPL_UNPACK_ARGS_STEP(z, n, list) \
    ,list.at(n).get< typename boost::remove_const< typename boost::remove_reference<A ##n>::type >::type >()

# define QGLIB_SIGNAL_IMPL_UNPACK_ARGS_STEP(z, n, list) \
    ,ValueImpl< typename boost::remove_const< typename boost::remove_reference<A ##n>::type >::type >::get(list.at(n))

# define QGLIB_SIGNAL_IMPL_UNPACK_ARGS(list) \
    BOOST_PP_REPEAT(QGLIB_SIGNAL_IMPL_NUM_ARGS, QGLIB_SIGNAL_IMPL_UNPACK_ARGS_STEP, list)

template <typename F, typename R QGLIB_SIGNAL_IMPL_COMMA
                                 QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
struct QGLIB_SIGNAL_IMPL_CPPCLOSUREN
{
    class ClosureData : public ClosureDataBase
    {
    public:
        inline ClosureData(const F & func, bool passSender)
            : ClosureDataBase(passSender), m_function(func) {}

        virtual void marshaller(SharedValue & result, const QList<Value> & params)
        {
            if (params.size() < QGLIB_SIGNAL_IMPL_NUM_ARGS) {
                throw std::logic_error("The signal provides less arguments than what the closure expects");
            }

# if QGLIB_SIGNAL_IMPL_NUM_ARGS > 0
            boost::function<R ()> callback = boost::bind<R>(m_function
                                                            QGLIB_SIGNAL_IMPL_UNPACK_ARGS(params));
            invoker< boost::function<R ()>, R >::invoke(callback, result);
# else
            invoker< F, R >::invoke(m_function, result);
# endif
        }

    private:
        F m_function;
    };

    static ClosurePtr create(const F & function, bool passSender)
    {
        return createCppClosure(new ClosureData(function, passSender));
    }
};

//partial specialization of struct CppClosure to support the CppClosure<R (Args...), F> syntax
template <typename F, typename R  QGLIB_SIGNAL_IMPL_COMMA
                                  QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS>
struct CppClosure<R (QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS), F>
    : public QGLIB_SIGNAL_IMPL_CPPCLOSUREN< F, R  QGLIB_SIGNAL_IMPL_COMMA
                                                   QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS >
{
};

# undef QGLIB_SIGNAL_IMPL_UNPACK_ARGS
# undef QGLIB_SIGNAL_IMPL_UNPACK_ARGS_STEP
# undef QGLIB_SIGNAL_IMPL_CPPCLOSUREN

//END ******** boostpp CppClosure ********

} //namespace Private

//BEGIN ******** bostpp Signal::connect ********

# define QGLIB_SIGNAL_IMPL_BIND_ARGS \
        BOOST_PP_ENUM_SHIFTED_PARAMS(BOOST_PP_INC(QGLIB_SIGNAL_IMPL_NUM_ARGS), _)

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
                                                   >::create(f, flags & PassSender);
    return connect(instance, detailedSignal, closure, flags);
}

# undef QGLIB_SIGNAL_IMPL_BIND_ARGS

//END ******** bostpp Signal::connect ********

} //namespace QGlib

# undef QGLIB_SIGNAL_IMPL_COMMA
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_ARGS
# undef QGLIB_SIGNAL_IMPL_TEMPLATE_PARAMS
# undef QGLIB_SIGNAL_IMPL_NUM_ARGS

#endif // !defined(BOOST_PP_IS_ITERATING) || !BOOST_PP_IS_ITERATING
