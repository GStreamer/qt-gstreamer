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
#ifndef QGLIB_ERROR_H
#define QGLIB_ERROR_H

#include "global.h"
#include "quark.h"
#include <exception>

namespace QGlib {

/*! Wrapper class for GError */
class Error : public std::exception
{
public:
    /*! Wraps an existing GError into an Error.
     * \note this method takes ownership of \a error */
    Error(GError *error);

    /*! Creates a new Error with the given \a domain, \a code and \a message */
    Error(Quark domain, int code, const char *message);

    Error(const Error & other);
    Error & operator=(const Error & other);
    virtual ~Error() throw();

    /*! Use message() instead. This method is provided
     * to implement the std::exception interface. */
    virtual const char* what() const throw();

    /*! Returns the domain of the error.
     * The domain indicates the module where the error has happened. */
    Quark domain() const;

    /*! Returns a code that describes the error. */
    int code() const;

    /*! Returns a human-readable message that describes the error. */
    QString message() const;

    inline operator GError *() { return m_error; }
    inline operator const GError *() const { return m_error; }

private:
    GError *m_error;
};

} //namespace QGlib

QDebug operator<<(QDebug dbg, const QGlib::Error & error);

#endif // QGLIB_ERROR_H
