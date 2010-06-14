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
#ifndef QGLIB_STRING_H
#define QGLIB_STRING_H

#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QDebug>

namespace QGlib {

/** This is a string class based on QByteArray. It is used instead of QString
 * to avoid the overhead of converting the data to/from UTF-16 when not necessary.
 * Since GLib/Gstreamer use entirely UTF-8 encoding, expect the data of this class
 * to be encoded as UTF-8. This means that if you want to display this string
 * somewhere, you should first convert it to QString with the toQString() method
 * or with operator QString().
 */
class String
{
public:
    inline String() {}
    inline String(const char *str) : m_str(str) {}
    inline String(const QByteArray & str) : m_str(str) {}
    inline String(const QString & str) : m_str(str.toUtf8()) {}

    inline QString toQString() const { return QString::fromUtf8(m_str.constData(), m_str.size()); }
    inline operator QString() const { return toQString(); }

    /** This is a special method that is used to wrap the return value of glib/gstreamer
     * methods that return a newly allocated gchar*. This method converts it to a String
     * and calls g_free() on the original string to free it.
     */
    static String fromGCharPtr(char *s);

    /** This is a special method that is used to pass this string as an argument to
     * glib/gstreamer methods that take a const gchar*. If the string is empty, it
     * returns NULL to hit the sanity checks of those methods.
     */
    inline const char *toGCharPtr() const { return m_str.isEmpty() ? NULL : m_str.constData(); }

    /** Behaves the same as toGCharPtr() */
    inline operator const char*() const { return toGCharPtr(); }

private:
    QByteArray m_str;
};

} //namespace QGlib

inline QDebug operator<<(QDebug dbg, const QGlib::String & str)
{
    return dbg << str.toQString();
}

#endif // QGLIB_STRING_H
