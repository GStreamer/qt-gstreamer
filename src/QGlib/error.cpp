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
#include "error.h"
#include <glib/gerror.h>
#include <QtCore/QDebug>

namespace QGlib {

Error::Error(GError *error)
    : std::exception()
{
    m_error = error;
}

Error::Error(Quark domain, int code, const char *message)
    : std::exception()
{
    m_error = g_error_new_literal(domain, code, message);
}

Error::Error(const Error & other)
    : std::exception()
{
    m_error = g_error_copy(other.m_error);
}

Error & Error::operator=(const Error & other)
{
    g_error_free(m_error);
    m_error = g_error_copy(other.m_error);
    return *this;
}

Error::~Error() throw()
{
    g_error_free(m_error);
}

const char* Error::what() const throw()
{
    return m_error->message;
}

Quark Error::domain() const
{
    return m_error->domain;
}

int Error::code() const
{
    return m_error->code;
}

QString Error::message() const
{
    return QString::fromUtf8(m_error->message);
}

} //namespace QGlib

QDebug operator<<(QDebug dbg, const QGlib::Error & error)
{
    return dbg << error.message();
}
