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
#include <QGst/Object>
#include <QGst/Bin>

class RefPointerTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void refTest1();
    void refTest2();
    void dynamicCastTest();
    void compilationTest();
};

void RefPointerTest::refTest1()
{
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);
    QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin, false);
    QCOMPARE(GST_OBJECT_REFCOUNT_VALUE(bin), 1);
}

void RefPointerTest::refTest2()
{
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);
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
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);

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

static void testMethod(const QGlib::ObjectPtr & obj)
{
    QVERIFY(!obj.dynamicCast<QGst::Bin>().isNull());
};

void RefPointerTest::compilationTest()
{
    //This is mostly a compilation test. If it compiles, it's fine, if it doesn't, there is a problem.
    QGst::BinPtr bin = QGst::Bin::create();

    {
        //operator=()
        QGst::ObjectPtr obj = bin;
        QVERIFY(!obj.dynamicCast<QGst::Bin>().isNull());
    }

    {
        //copy constructor
        QGst::ObjectPtr obj(bin);
        QVERIFY(!obj.dynamicCast<QGst::Bin>().isNull());
    }

    {
        //implicit cast
        testMethod(bin);
    }

#if 0
    {
        QGst::ObjectPtr obj = bin;
        QGst::BinPtr bin2 = obj; //should fail to compile
    }
#endif

    {
        const QGst::ObjectPtr obj = bin;
        (void)obj->name(); //should work
#if 0
        obj->setName("foo"); //should fail to compile
#endif
    }
}

QTEST_APPLESS_MAIN(RefPointerTest)

#include "moc_qgsttest.cpp"
#include "refpointertest.moc"
