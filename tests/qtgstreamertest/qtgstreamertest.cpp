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
#include "../../src/qgstpipeline.h"
#include "../../src/qgstelementfactory.h"
#include "../../src/qgstvalue.h"
#include "../../src/qgstcaps.h"
#include "../../src/qgststructure.h"
#include "qtgstreamertest.moc"
#include <QtCore/QDebug>
#include <gst/gst.h>

using namespace QtGstreamer;

void QtGstreamerTest::initTestCase()
{
    gst_init(NULL, NULL);
}

void QtGstreamerTest::propertyTest()
{
    GstElement *foo = gst_element_factory_make("fakesrc", "test");
    gst_object_ref(foo);
    gst_object_sink(foo);

    QGstElementPtr qgelement = QGstElement::fromGstElement(foo);
    gst_object_unref(foo);

    QCOMPARE(qgelement->property<QByteArray>("name"), QByteArray("test"));

    qgelement->setProperty("name", QByteArray("test2"));
    QCOMPARE(qgelement->property<QByteArray>("name"), QByteArray("test2"));
}

void QtGstreamerTest::player()
{
    QGstElementPtr src = QGstElementFactory::make("audiotestsrc");
    src->setProperty("freq", 880);
    QCOMPARE(src->property<double>("freq"), 880.0);

    QGstElementPtr sink = QGstElementFactory::make("alsasink");

    QGstPipelinePtr bin = QGstPipeline::newPipeline();
    *bin << src << sink;
    QCOMPARE(QGstElement::link(src, sink), true);

    QGstElement::State state;
    bin->setState(QGstElement::Playing);
    bin->getState(&state, NULL, (quint64) -1);
    QCOMPARE(state, QGstElement::Playing);

    QTest::qWait(1000);

    bin->setState(QGstElement::Null);
    bin->getState(&state, NULL, (quint64) -1);
    QCOMPARE(state, QGstElement::Null);
}

void QtGstreamerTest::gValueTest()
{
    QGstValue v(800);
    qDebug() << v;
    QCOMPARE(v.value<int>(), 800);

    v = QGstValue::fromValue(123.123);
    qDebug() << v;
    QCOMPARE(v.value<double>(), 123.123);

    v = QGstValue::fromValue<QByteArray>("hello world");
    qDebug() << v;
    QCOMPARE(v.value<QByteArray>(), QByteArray("hello world"));
}

void QtGstreamerTest::capsTest()
{
    QGstCapsPtr caps = QGstCaps::newEmpty();
    qDebug() << caps->toString();

    QGstStructure s("audio/x-raw-int");
    s.setValue("rate", 22000);
    s.setValue("channels", 1);
    s.setValue("width", 16);

    caps->makeWritable();
    caps->appendStructure(s);
    qDebug() << caps->toString();
    QCOMPARE(caps->toString(), QByteArray("audio/x-raw-int, rate=(int)22000, channels=(int)1, width=(int)16"));
}

void QtGstreamerTest::cleanupTestCase()
{
    gst_deinit();
}

QTEST_MAIN(QtGstreamerTest)
