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
#include "qgsttest.h"
#include <QGlib/Value>
#include <QGst/Object>

class ValueTest : public QGstTest
{
    Q_OBJECT
private slots:
    void intTest();
    void stringTest();
    void enumTest();
    void copyTest();
    void castTest();
    void qdebugTest();
};

void ValueTest::intTest()
{
    QGlib::Value v(10);
    QVERIFY(v.isValid());
    QCOMPARE(v.type(), QGlib::GetType<int>());
    QCOMPARE(v.get<int>(), 10);
}

void ValueTest::stringTest()
{
    QGlib::Value v;
    QVERIFY(!v.isValid());
    v.init<QString>();
    v.set(QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(v.type(), QGlib::GetType<QString>());
    QByteArray b = v.get<QByteArray>();
    QCOMPARE(QString::fromUtf8(b), QString::fromUtf8("Γειά σου κόσμε"));
    QCOMPARE(v.get<QString>(), QString::fromUtf8("Γειά σου κόσμε"));
}

void ValueTest::enumTest()
{
    QGlib::Value v;
    v.init<QGst::PadDirection>();
    v.set(QGst::PadSink);
    QVERIFY(v.isValid());
    QCOMPARE(v.type(), QGlib::GetType<QGst::PadDirection>());
    QCOMPARE(v.get<QGst::PadDirection>(), QGst::PadSink);
}

void ValueTest::copyTest()
{
    QGlib::Value v(10);

    QGlib::SharedValue sv(v);
    QVERIFY(sv.isValid());
    QCOMPARE(sv.type(), QGlib::GetType<int>());
    QCOMPARE(sv.get<int>(), 10);

    sv.set(20);
    QCOMPARE(v.get<int>(), 20);

    {
        QGlib::Value v2(sv);
        QVERIFY(v2.isValid());
        QCOMPARE(v2.type(), QGlib::GetType<int>());
        QCOMPARE(v2.get<int>(), 20);

        v2.set(30);
        QCOMPARE(v.get<int>(), 20);
        QCOMPARE(sv.get<int>(), 20);
        QCOMPARE(v2.get<int>(), 30);

        v2 = v;
        QCOMPARE(v2.get<int>(), 20);
    }

    {
        QGlib::Value v2(v);
        QVERIFY(v2.isValid());
        QCOMPARE(v2.type(), QGlib::GetType<int>());
        QCOMPARE(v2.get<int>(), 20);

        v2.set(30);
        QCOMPARE(v.get<int>(), 20);
        QCOMPARE(sv.get<int>(), 20);
        QCOMPARE(v2.get<int>(), 30);

        v2 = sv;
        QCOMPARE(v2.get<int>(), 20);
    }
}

void ValueTest::castTest()
{
    QGlib::Value v(10);
    GValue *gv = v;
    QCOMPARE(G_VALUE_TYPE(gv), G_TYPE_INT);
    QCOMPARE(g_value_get_int(gv), 10);
}

void ValueTest::qdebugTest()
{
    qDebug() << QGlib::Value(10);
    qDebug() << QGlib::Value(QByteArray("Hello world"));
    qDebug() << QGlib::Value(QGlib::ObjectPtr());
}


QTEST_APPLESS_MAIN(ValueTest)

#include "moc_qgsttest.cpp"
#include "valuetest.moc"
