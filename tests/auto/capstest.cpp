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
#include <QGst/Caps>
#include <QGst/Structure>
#include <QGst/Bin>

class CapsTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void simpleTest();
    void anyTest();
    void fullTest();
    void writabilityTest();
    void setValueTest();
};

void CapsTest::simpleTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    QVERIFY(caps->isSimple());
    QVERIFY(!caps->isEmpty());
    QVERIFY(!caps->isAny());
    QVERIFY(caps->size() == 1);

    caps->setValue("width", 320);
    caps->setValue("height", 240);
    qDebug() << caps;

    QGst::StructurePtr ss = caps->internalStructure(0);
    QVERIFY(ss->isValid());
    qDebug() << ss;
    ss->setValue("width", 400);
    qDebug() << caps;

    //Check access with temporaries
    caps->internalStructure(0)->setValue("width", 800);
    QCOMPARE(caps->internalStructure(0)->value("width").get<int>(), 800);

    //Note: GStreamer will thrown an assertion on the line below, this is expected
    QVERIFY(!caps->internalStructure(1)->isValid());
}

void CapsTest::anyTest()
{
    QGst::CapsPtr caps = QGst::Caps::createAny();
    QVERIFY(!caps->isSimple());
    QVERIFY(!caps->isEmpty());
    QVERIFY(caps->isAny());
    QVERIFY(caps->size() == 0);
}

void CapsTest::fullTest()
{
    QGst::Structure s("video/x-raw");
    s.setValue("width", 320);
    s.setValue("height", 240);

    QGst::CapsPtr caps = QGst::Caps::createEmpty();
    QVERIFY(!caps->isSimple());
    QVERIFY(caps->isEmpty());
    QVERIFY(!caps->isAny());
    QVERIFY(caps->size() == 0);

    caps->appendStructure(s);
    QVERIFY(caps->isSimple());
    QVERIFY(!caps->isEmpty());
    QVERIFY(!caps->isAny());
    QVERIFY(caps->size() == 1);

    QGst::CapsPtr caps2 = QGst::Caps::createSimple("video/x-raw");
    caps2->setValue("width", 320);
    caps2->setValue("height", 240);

    QVERIFY(caps->equals(caps2));
}

void CapsTest::writabilityTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(static_cast<GstCaps *>(caps)) == 1);

    {
        QGst::CapsPtr caps2 = caps;
        QCOMPARE(GST_CAPS_REFCOUNT_VALUE(static_cast<GstCaps *>(caps)), 1);
        QVERIFY(static_cast<GstCaps*>(caps2) == static_cast<GstCaps*>(caps));
    }

    GstCaps *oldPtr = caps;
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(static_cast<GstCaps *>(caps)) == 1);
    //Increase external refcount to lock it
    gst_caps_ref(oldPtr);
    QVERIFY(oldPtr == static_cast<GstCaps*>(caps));
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(static_cast<GstCaps *>(caps)) == 2);
    QVERIFY(!caps->isWritable());

    caps = caps->makeWritable(); //creates a copy
    QVERIFY(caps->isWritable());
    QVERIFY(oldPtr != static_cast<GstCaps*>(caps)); //no longer same gstcaps object
}

void CapsTest::setValueTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");

    QGst::BinPtr bin = QGst::Bin::create();

    {
        caps->setValue("binptr", bin);
        QGlib::Value v = caps->internalStructure(0)->value("binptr");
        QCOMPARE(v.type(), QGlib::GetType<QGst::Bin>());
        QVERIFY(v.get<QGst::BinPtr>() == bin);
    }

    {
        QGlib::Value v = QGlib::Value::create(bin);
        caps->setValue("binptr2", v);
        v = caps->internalStructure(0)->value("binptr2");
        QCOMPARE(v.type(), QGlib::GetType<QGst::Bin>());
        QVERIFY(v.get<QGst::BinPtr>() == bin);
    }
}

QTEST_APPLESS_MAIN(CapsTest)

#include "moc_qgsttest.cpp"
#include "capstest.moc"
