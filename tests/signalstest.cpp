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
#include "signalstest.h"
#include <QGlib/Closure>

static bool closureCalled = false;

void SignalsTest::closureTestClosure(QGst::ObjectPtr obj, QGst::ObjectPtr parentObj)
{
    qDebug() << "Closure called";
    QCOMPARE(obj->property<QString>("name"), QString("mybin"));
    QCOMPARE(parentObj->property<QString>("name"), QString("mypipeline"));
    closureCalled = true;
}

void SignalsTest::closureTest()
{
    GstObject *pipeline = GST_OBJECT(gst_pipeline_new("mypipeline"));
    GstObject *bin = GST_OBJECT(gst_bin_new("mybin"));

    closureCalled = false;
    QGlib::ClosurePtr closure = QGlib::CppClosure::newCppClosure(&SignalsTest::closureTestClosure, this);
    g_signal_connect_closure(bin, "parent-set", closure, FALSE);
    gst_object_set_parent(bin, pipeline);
    QCOMPARE(closureCalled, true);

    gst_object_sink(bin);
    gst_object_sink(pipeline);
}

QTEST_APPLESS_MAIN(SignalsTest)

#include "signalstest.moc"
#include "qgsttest.moc"
