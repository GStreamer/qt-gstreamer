/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef _QTGSTREAMER_QGVALUE_H
#define _QTGSTREAMER_QGVALUE_H

#include "qgstdeclarations.h"
#include <QtCore/QVariant>

namespace QtGstreamer {

class QGValue
{
public:
    QGValue();
    QGValue(char val);
    QGValue(uchar val);
    QGValue(int val);
    QGValue(uint val);
    QGValue(qint64 val);
    QGValue(quint64 val);
    QGValue(bool val);
    QGValue(double val);
    QGValue(const char *val);
    QGValue(const QByteArray & val);
    QGValue(const QGstObjectPtr & val);
    QGValue(const QGstElementPtr & val);
    QGValue(const QGstPadPtr & val);
    QGValue(const QGstCapsPtr & val);
    QGValue(const QGValue & other);
    ~QGValue();

    QGValue & operator=(const QGValue & other);

    bool isValid() const;

    static QGValue fromGValue(const GValue *val);
    GValue *toGValue() const;

    static QGValue fromQVariant(const QVariant & variant, quint32 gtype = 0);
    QVariant toQVariant() const;

    template<class T> static QGValue fromValue(const T & value, quint32 gtype = 0);
    template<class T> T value() const;

private:
    GValue *m_value;
    friend QDebug operator<<(QDebug debug, const QGValue & qgvalue);
};

QDebug operator<<(QDebug debug, const QGValue & qgvalue);

template<class T>
QGValue QGValue::fromValue(const T & value, quint32 gtype)
{
    return QGValue::fromQVariant(QVariant::fromValue(value), gtype);
}

template<class T>
T QGValue::value() const
{
    return toQVariant().value<T>();
}

}

#endif
