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
#include <QGlib/Closure>
#include <QGst/Object>

class SignalsTest : public QGstTest
{
    Q_OBJECT
private:
    void closureTestClosure(QGst::ObjectPtr obj, QGst::ObjectPtr parentObj);
private slots:
    void closureTest();
};

static bool closureCalled = false;

void SignalsTest::closureTestClosure(QGst::ObjectPtr obj, QGst::ObjectPtr parentObj)
{
    qDebug() << "Closure called";
    QCOMPARE(obj->property("name").get<QString>(), QString("mybin"));
    QCOMPARE(parentObj->property("name").get<QString>(), QString("mypipeline"));
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

#include "moc_qgsttest.cpp"
#include "signalstest.moc"
