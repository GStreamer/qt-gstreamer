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
#include "valuetest.h"
#include <QGlib/Value>

void ValueTest::intTest()
{
    QGlib::Value v(10);
    QVERIFY(v.isValid());
    QCOMPARE(static_cast<GType>(v.type()), G_TYPE_INT);
    QCOMPARE(v.get<int>(), 10);
}

void ValueTest::stringTest()
{
    QGlib::Value v;
    QVERIFY(!v.isValid());
    v.init(QGlib::Type::String);
    v.set(QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(static_cast<GType>(v.type()), G_TYPE_STRING);
    QByteArray b = v.get<QByteArray>();
    QCOMPARE(QString::fromUtf8(b), QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(v.get<QString>(), QString::fromUtf8("Γειά σου κόσμε"));
}

void ValueTest::enumTest()
{
    QGlib::Value v;
    v.init(GST_TYPE_PAD_DIRECTION);
    v.set(GST_PAD_SINK);
    QVERIFY(v.isValid());
    QCOMPARE(static_cast<GType>(v.type()), GST_TYPE_PAD_DIRECTION);
    QCOMPARE(v.get<GstPadDirection>(), GST_PAD_SINK);
}

QTEST_APPLESS_MAIN(ValueTest)

#include "valuetest.moc"
#include "qgsttest.moc"
