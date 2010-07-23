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
#include "qgsttest.h"
#include <QGst/Caps>
#include <QGst/Structure>

class CapsTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void simpleTest();
    void anyTest();
    void fullTest();
    void writabilityTest();
};

void CapsTest::simpleTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw-yuv");
    QVERIFY(caps->isSimple());
    QVERIFY(!caps->isEmpty());
    QVERIFY(!caps->isAny());
    QVERIFY(caps->size() == 1);

    caps->setValue("width", 320);
    caps->setValue("height", 240);
    qDebug() << caps;

    QGst::SharedStructure ss = caps->structure(0);
    QVERIFY(ss.isValid());
    qDebug() << ss;
    ss.setValue("width", 400);
    qDebug() << caps;

    QVERIFY(!caps->structure(1).isValid());
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
    QGst::Structure s("video/x-raw-yuv");
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

    QGst::CapsPtr caps2 = QGst::Caps::createSimple("video/x-raw-yuv");
    caps2->setValue("width", 320);
    caps2->setValue("height", 240);

    QVERIFY(caps->equals(caps2));
}

void CapsTest::writabilityTest()
{
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw-yuv");
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps) == 1);

    {
        QGst::CapsPtr caps2 = caps;
        QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps) == 2);
        QVERIFY(static_cast<GstCaps*>(caps2) == static_cast<GstCaps*>(caps));

        (void)caps2->structure(0); //non-const method, so makeWritable() is called internally
        QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps) == 1);
        QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps2) == 1);
        QVERIFY(static_cast<GstCaps*>(caps2) != static_cast<GstCaps*>(caps));
    }

    GstCaps *oldPtr = caps;
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps) == 1);
    (void)caps->structure(0); //non-const method, so makeWritable() is called internally
    QVERIFY(oldPtr == static_cast<GstCaps*>(caps));
    QVERIFY(GST_CAPS_REFCOUNT_VALUE(caps) == 1);
}

QTEST_APPLESS_MAIN(CapsTest)

#include "moc_qgsttest.cpp"
#include "capstest.moc"
