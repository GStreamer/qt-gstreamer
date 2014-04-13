/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
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
#include <QGst/Object>
#include <QGst/Message>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/UriHandler>
#include <QGst/StreamVolume>

class RefPointerTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void refTest1();
    void refTest2();
    void dynamicCastTest();
    void dynamicCastDownObjectTest();
    void dynamicCastUpObjectTest();
    void dynamicCastObjectToIfaceTest();
    void dynamicCastIfaceToObjectTest();
    void cppWrappersTest();
    void messageDynamicCastTest();
    void equalityTest();
};

void RefPointerTest::refTest1()
{
    GstElement *element = gst_bin_new(NULL);
    GstObject *bin1 = GST_OBJECT(element);
    QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(element), 1);

    GstObject *bin2 = GST_OBJECT(gst_object_ref_sink(bin1));
    QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin2), 1);
    QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin2, false);
    QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin2), 1);
}

void RefPointerTest::refTest2()
{
    GstObject *bin = GST_OBJECT(gst_object_ref_sink(GST_OBJECT(gst_bin_new(NULL))));

    {
        QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin);
        QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin), 2);
        {
            QGst::ObjectPtr object2 = object;
            QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin), 3);
        }
    }
    QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin), 1);
    gst_object_unref(bin);
}

void RefPointerTest::dynamicCastTest()
{
    GstObject *bin = GST_OBJECT(gst_object_ref_sink(GST_OBJECT(gst_bin_new(NULL))));

    {
        QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin);
        QVERIFY(!object.dynamicCast<QGlib::Object>().isNull());
    }

    {
        QGlib::ObjectPtr object = QGlib::ObjectPtr::wrap(G_OBJECT(bin));
        QVERIFY(!object.dynamicCast<QGst::Object>().isNull());
    }

    gst_object_unref(bin);
}

void RefPointerTest::dynamicCastDownObjectTest()
{
    GstObject *bin = GST_OBJECT(gst_object_ref_sink(gst_bin_new(NULL)));

    {
        QGlib::ObjectPtr object = QGlib::ObjectPtr::wrap(G_OBJECT(bin));
        QVERIFY(!object.dynamicCast<QGst::Object>().isNull());
        QVERIFY(!object.dynamicCast<QGst::Bin>().isNull());
        QVERIFY(object.dynamicCast<QGst::Pipeline>().isNull());
    }

    gst_object_unref(bin);
}

void RefPointerTest::dynamicCastUpObjectTest()
{
    GstBin *bin = GST_BIN(gst_object_ref_sink(gst_bin_new(NULL)));

    {
        QGst::BinPtr object = QGst::BinPtr::wrap(bin);
        QVERIFY(!object.dynamicCast<QGst::Element>().isNull());
        QVERIFY(!object.dynamicCast<QGlib::Object>().isNull());
        QVERIFY(!object.dynamicCast<QGst::ChildProxy>().isNull());
    }

    gst_object_unref(bin);
}

void RefPointerTest::dynamicCastObjectToIfaceTest()
{
    QGst::ElementPtr e = QGst::ElementFactory::make("fakesrc");
    QGst::UriHandlerPtr u = e.dynamicCast<QGst::UriHandler>();
    QVERIFY(u.isNull());

    e = QGst::ElementFactory::make("filesrc");
    u = e.dynamicCast<QGst::UriHandler>();
    QVERIFY(!u.isNull());
}

void RefPointerTest::dynamicCastIfaceToObjectTest()
{
    GstElement *e = gst_element_factory_make("filesrc", NULL);
    gst_object_ref_sink(e);

    QGst::UriHandlerPtr u = QGst::UriHandlerPtr::wrap(GST_URI_HANDLER(e), false);
    QVERIFY(!u.isNull());
    QVERIFY(!u.dynamicCast<QGst::Element>().isNull());
}

void RefPointerTest::cppWrappersTest()
{
    QGst::ElementPtr e = QGst::ElementFactory::make("playbin");
    QVERIFY(!e.isNull());

    {
        QGst::PipelinePtr pipeline = e.dynamicCast<QGst::Pipeline>();
        QVERIFY(!pipeline.isNull());
        //the C++ wrappers must be the same
        QCOMPARE(static_cast<QGlib::RefCountedObject*>(pipeline.operator->()),
                 static_cast<QGlib::RefCountedObject*>(e.operator->()));
    }

    {
        QGst::ChildProxyPtr proxy = e.dynamicCast<QGst::ChildProxy>();
        QVERIFY(!proxy.isNull());
        //the C++ wrappers must be the same
        QCOMPARE(static_cast<QGlib::RefCountedObject*>(proxy.operator->()),
                 static_cast<QGlib::RefCountedObject*>(e.operator->()));
    }

    { //new wrap() should give the same C++ instance
        GstElement *gobj = e;
        QGst::ElementPtr e2 = QGst::ElementPtr::wrap(gobj);
        QCOMPARE(static_cast<QGlib::RefCountedObject*>(e2.operator->()),
                 static_cast<QGlib::RefCountedObject*>(e.operator->()));
    }

    {
        QGst::StreamVolumePtr sv = e.dynamicCast<QGst::StreamVolume>();
        QVERIFY(!sv.isNull());
        //now the C++ wrapper must not be the same, since Pipeline does not inherit StreamVolume
        QVERIFY(static_cast<QGlib::RefCountedObject*>(sv.operator->())
                != static_cast<QGlib::RefCountedObject*>(e.operator->()));
    }

    {
        QGst::MessagePtr msg = QGst::ApplicationMessage::create(e);
        QGst::MessagePtr msg2 = msg;
        QCOMPARE(static_cast<QGlib::RefCountedObject*>(msg.operator->()),
                 static_cast<QGlib::RefCountedObject*>(msg2.operator->()));
        QVERIFY(msg2 == msg);

        QGst::MessagePtr msg3 = QGst::MessagePtr::wrap(msg2);
        QVERIFY(static_cast<QGlib::RefCountedObject*>(msg3.operator->())
                != static_cast<QGlib::RefCountedObject*>(msg2.operator->()));
        QVERIFY(msg3 == msg2);
    }
}

void RefPointerTest::messageDynamicCastTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGst::MessagePtr msg = QGst::ApplicationMessage::create(bin);
    QVERIFY(!msg.isNull());
    QVERIFY(!msg.dynamicCast<QGst::ApplicationMessage>().isNull());
    QVERIFY(msg.dynamicCast<QGst::EosMessage>().isNull());
}

void RefPointerTest::equalityTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QGst::ElementPtr element = bin;
    QVERIFY(element == bin);
    QVERIFY(bin == element);
    QVERIFY(bin == bin);

    GstElement *e = element;
    QVERIFY(e == element);
    QVERIFY(element == e);
    QVERIFY(bin == e);
    QVERIFY(e == bin);

    e++;
    QVERIFY(e != element);
    QVERIFY(element != e);
    QVERIFY(bin != e);
    QVERIFY(e != bin);

    e = NULL;
    QVERIFY(e != element);
    QVERIFY(element != e);
    QVERIFY(bin != e);
    QVERIFY(e != bin);

    element.clear();
    QVERIFY(element != bin);
    QVERIFY(bin != element);
    QVERIFY(e == element);
    QVERIFY(element == e);

    bin.clear();
    QVERIFY(element == bin);
    QVERIFY(bin == element);
    QVERIFY(bin == bin);
    QVERIFY(bin == e);
    QVERIFY(e == bin);
}

QTEST_APPLESS_MAIN(RefPointerTest)

#include "moc_qgsttest.cpp"
#include "refpointertest.moc"
