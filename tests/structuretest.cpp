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
#include "structuretest.h"
#include <QGst/Structure>

void StructureTest::bindingsTest()
{
    QGst::Structure s("mystructure");

    QVERIFY(s.isValid());
    QCOMPARE(s.name(), QString("mystructure"));
    QCOMPARE(s.numberOfFields(), static_cast<unsigned int>(0));

    s.setValue("intfield", 20);
    QCOMPARE(s.numberOfFields(), static_cast<unsigned int>(1));
    QVERIFY(s.hasField("intfield"));
    QCOMPARE(s.fieldName(0), QString("intfield"));
    QCOMPARE(s.value("intfield").get<int>(), 20);
    QCOMPARE(s.fieldType("intfield"), QGlib::Type(QGlib::Type::Int));

    s.setValue("strfield", QString("hello world"));
    QCOMPARE(s.numberOfFields(), static_cast<unsigned int>(2));

    qDebug() << s;

    s.removeField("intfield");
    QCOMPARE(s.numberOfFields(), static_cast<unsigned int>(1));

    s.removeAllFields();
    QCOMPARE(s.numberOfFields(), static_cast<unsigned int>(0));
}

void StructureTest::copyTest()
{
    QGst::Structure s("mystructure");
    s.setValue("intfield", 10);

    QGst::SharedStructure ss(s.peekGstStructure());
    QVERIFY(ss.isValid());
    QCOMPARE(ss.value("intfield").get<int>(), 10);

    ss.setValue("intfield", 20);
    QCOMPARE(s.value("intfield").get<int>(), 20);

    {
        QGst::Structure s2(ss);
        QVERIFY(s2.isValid());
        QCOMPARE(s2.value("intfield").get<int>(), 20);

        s2.setValue("intfield", 30);
        QCOMPARE(s.value("intfield").get<int>(), 20);
        QCOMPARE(ss.value("intfield").get<int>(), 20);
        QCOMPARE(s2.value("intfield").get<int>(), 30);

        s2 = s;
        QCOMPARE(s2.value("intfield").get<int>(), 20);
    }

    {
        QGst::Structure s2(s);
        QVERIFY(s2.isValid());
        QCOMPARE(s2.value("intfield").get<int>(), 20);

        s2.setValue("intfield", 30);
        QCOMPARE(s.value("intfield").get<int>(), 20);
        QCOMPARE(ss.value("intfield").get<int>(), 20);
        QCOMPARE(s2.value("intfield").get<int>(), 30);

        s2 = ss;
        QCOMPARE(s2.value("intfield").get<int>(), 20);
    }
}

QTEST_APPLESS_MAIN(StructureTest)

#include "structuretest.moc"
#include "qgsttest.moc"
