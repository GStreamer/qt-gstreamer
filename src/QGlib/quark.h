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
#ifndef QGLIB_QUARK_H
#define QGLIB_QUARK_H

#include <QtCore/QString>

namespace QGlib {

/*! \headerfile quark.h <QGlib/Quark>
 * \brief Wrapper class for GQuark
 *
 * Quarks are associations between strings and integer identifiers.
 * Given either the string or the Quark identifier it is possible to retrieve the other.
 *
 * \note This class is a thin wrapper around a quint32. There is no overhead in copying
 * it around, since it is just an integer.
 */
class Quark
{
public:
    inline Quark(quint32 gquark = 0) : m_quark(gquark) {}
    Quark(const char *str);
    Quark(const QString & str);

    QString toString() const;
    inline operator quint32() const { return m_quark; }

private:
    quint32 m_quark;
};

}

#endif // QGLIB_QUARK_H
