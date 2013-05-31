/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QGSTTEST_H
#define QGSTTEST_H

#include <QtTest/QtTest>
#include <QGlib/Value>
#include <QGst/ClockTime>
#include <QGst/Init>
#include <gst/gst.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
# define SkipSingle 0
# define SkipAll 0
# define QSKIP_PORT(m, a) QSKIP(m)
#else
# define QSKIP_PORT(m, a) QSKIP(m, a)
#endif

class QGstTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase() { QGst::init(); }
    void cleanupTestCase() { QGst::cleanup(); }
};

namespace QTest // teach QCOMPARE() printing of certain GStreamer values
{
template<> char *toString(const QGst::ClockTime &t)
{
    return toString(quint64(t));
}

template<> char *toString(const QGlib::Value &value)
{
    bool ok = false;
    QString text = value.type().name() + "(" + value.toString(&ok) + ")";
    return ok ? toString(text) : 0;
}
} // namespace QTest

#endif
