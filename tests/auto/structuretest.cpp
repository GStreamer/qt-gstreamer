/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2011 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include <QGst/Structure>
#include <QGst/Element>
#include <QGst/ElementFactory>
#include <QGst/Caps>
#include <QGst/Pad>
#include <QGst/Event>

class StructureTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void bindingsTest();
    void copyTest();
    void valueTest();
    void sharedStructureTest();
};

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

    QGst::Structure ss(s);
    QVERIFY(ss.isValid());
    QCOMPARE(ss.value("intfield").get<int>(), 10);

    ss.setValue("intfield", 20);
    QCOMPARE(s.value("intfield").get<int>(), 10);

    {
        QGst::Structure s2(ss);
        QVERIFY(s2.isValid());
        QCOMPARE(s2.value("intfield").get<int>(), 20);

        s2.setValue("intfield", 30);
        QCOMPARE(s.value("intfield").get<int>(), 10);
        QCOMPARE(ss.value("intfield").get<int>(), 20);
        QCOMPARE(s2.value("intfield").get<int>(), 30);

        s2 = s;
        QCOMPARE(s2.value("intfield").get<int>(), 10);
    }

}

void StructureTest::valueTest()
{
    QGlib::Value v;

    {
        QGst::Structure s("mystructure");
        s.setValue("foo", 1);
        v = QGlib::Value::create(s);
        QVERIFY(v.isValid());
    }

    {
        QGst::Structure s = v.get<QGst::Structure>();
        QVERIFY(s.isValid());
        QCOMPARE(s.name(), QString("mystructure"));
        QCOMPARE(s.value("foo").toInt(), 1);
    }
}

void StructureTest::sharedStructureTest()
{
    QGst::ElementPtr queue = QGst::ElementFactory::make("queue", NULL);
    QGst::PadPtr pad = queue->getStaticPad("sink");
    queue->setState(QGst::StatePlaying);

    {
        QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
        caps->setValue("width", 320);
        caps->setValue("height", 240);

        // verify the Caps was created correctly
        QGst::StructurePtr structure = caps->internalStructure(0);
        QCOMPARE(caps->size(), static_cast<unsigned int>(1));
        QCOMPARE(structure->name(), QString("video/x-raw"));

        QGst::CapsEventPtr event = QGst::CapsEvent::create(caps);
        QVERIFY(pad->sendEvent(event));
    }

    QCOMPARE(pad->currentCaps()->size(), static_cast<unsigned int>(1));
    QGst::StructurePtr structure = pad->currentCaps()->internalStructure(0);
    QCOMPARE(structure->name(), QString("video/x-raw"));
    QCOMPARE(structure->value("width").toInt(), 320);
    QCOMPARE(structure->value("height").toInt(), 240);

    QGst::Structure s = structure->copy();
    QCOMPARE(s.name(), QString("video/x-raw"));
    QCOMPARE(s.value("width").toInt(), 320);
    QCOMPARE(s.value("height").toInt(), 240);

    queue->setState(QGst::StateNull);
}


QTEST_APPLESS_MAIN(StructureTest)

#include "moc_qgsttest.cpp"
#include "structuretest.moc"
