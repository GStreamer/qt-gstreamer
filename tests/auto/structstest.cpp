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
#include "qgsttest.h"
#include <QGlib/Value>
#include <QGst/Fourcc>
#include <QGst/Fraction>
#include <QGst/IntRange>
#include <QGst/DoubleRange>
#include <QGst/FractionRange>

class StructsTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void fourccTest();
    void fractionTest();
    void doubleRangeTest();
    void fractionRangeTest();
};

void StructsTest::fourccTest()
{
    QGst::Fourcc f("MJPG");
    QCOMPARE(f.value.as_bytes.first, 'M');
    QCOMPARE(f.value.as_bytes.second, 'J');
    QCOMPARE(f.value.as_bytes.third, 'P');
    QCOMPARE(f.value.as_bytes.fourth, 'G');
    QCOMPARE(f.value.as_integer, GST_STR_FOURCC("MJPG"));

    f = QGst::Fourcc(GST_STR_FOURCC("MJPG")); //quint32 constructor
    QCOMPARE(f.value.as_bytes.first, 'M');
    QCOMPARE(f.value.as_bytes.second, 'J');
    QCOMPARE(f.value.as_bytes.third, 'P');
    QCOMPARE(f.value.as_bytes.fourth, 'G');
    QCOMPARE(f.value.as_integer, GST_STR_FOURCC("MJPG"));

    QGlib::Value v = QGlib::Value::create(f);
    f = v.get<QGst::Fourcc>();
    QCOMPARE(f.value.as_bytes.first, 'M');
    QCOMPARE(f.value.as_bytes.second, 'J');
    QCOMPARE(f.value.as_bytes.third, 'P');
    QCOMPARE(f.value.as_bytes.fourth, 'G');
    QCOMPARE(f.value.as_integer, GST_STR_FOURCC("MJPG"));
}

void StructsTest::fractionTest()
{
    QGst::Fraction f(25, 1);
    QCOMPARE(f.numerator, 25);
    QCOMPARE(f.denominator, 1);

    QGlib::Value v = QGlib::Value::create(f);
    f = v.get<QGst::Fraction>();
    QCOMPARE(f.numerator, 25);
    QCOMPARE(f.denominator, 1);
}

void StructsTest::doubleRangeTest()
{
    QGst::DoubleRange r(1.0, 2.0);
    QCOMPARE(r.start, 1.0);
    QCOMPARE(r.end, 2.0);

    QGlib::Value v = QGlib::Value::create(r);
    r = v.get<QGst::DoubleRange>();
    QCOMPARE(r.start, 1.0);
    QCOMPARE(r.end, 2.0);
}

void StructsTest::fractionRangeTest()
{
    QGst::FractionRange r(QGst::Fraction(1, 1), QGst::Fraction(25, 1));
    QCOMPARE(r.start.numerator, 1);
    QCOMPARE(r.end.numerator, 25);

    QGlib::Value v = QGlib::Value::create(r);
    r = v.get<QGst::FractionRange>();
    QCOMPARE(r.start.numerator, 1);
    QCOMPARE(r.end.numerator, 25);
}

QTEST_APPLESS_MAIN(StructsTest)

#include "moc_qgsttest.cpp"
#include "structstest.moc"
