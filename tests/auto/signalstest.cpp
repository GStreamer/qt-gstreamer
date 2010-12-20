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
#include <QGlib/Signal>
#include <QGst/Pipeline>

class SignalsTest : public QGstTest
{
    Q_OBJECT
private:
   void closureTestClosure(const QGst::ObjectPtr & obj, QGst::ObjectPtr parentObj);
   void emitTestClosure(const QGlib::ObjectPtr & instance, const QGlib::ParamSpecPtr & param);
private Q_SLOTS:
   void closureTest();
   void queryTest();
   void emitTest();
};

static bool closureCalled = false;

void SignalsTest::closureTestClosure(const QGst::ObjectPtr & obj, QGst::ObjectPtr parentObj)
{
    qDebug() << "closureTestClosure called";
    QCOMPARE(obj->property("name").get<QString>(), QString("mybin"));
    QCOMPARE(parentObj->property("name").get<QString>(), QString("mypipeline"));
    closureCalled = true;
}

void SignalsTest::closureTest()
{
    QGst::PipelinePtr pipeline = QGst::Pipeline::create("mypipeline");
    QGst::BinPtr bin = QGst::Bin::create("mybin");

    closureCalled = false;
    QGlib::connect(bin, "parent-set", this, &SignalsTest::closureTestClosure, QGlib::PassSender);
    bin->setParent(pipeline);
    QCOMPARE(closureCalled, true);
}

void SignalsTest::queryTest()
{
    //void user_function(GObject *gobject, GParamSpec *pspec, gpointer user_data)
    //Run First / No Recursion / Has Details / Action / No Hooks
    QGlib::Signal s = QGlib::Signal::lookup("notify", QGlib::GetType<QGlib::Object>());
    QVERIFY(s.isValid());

    QCOMPARE(s.name(), QString("notify"));
    QCOMPARE(s.instanceType(), QGlib::GetType<QGlib::Object>());

    QCOMPARE(s.flags(), QGlib::Signal::RunFirst | QGlib::Signal::NoRecurse |
                        QGlib::Signal::Detailed | QGlib::Signal::Action | QGlib::Signal::NoHooks);
    QCOMPARE(s.returnType(), QGlib::Type(QGlib::Type::None));

    QList<QGlib::Type> paramTypes = s.paramTypes();
    QCOMPARE(paramTypes.size(), 1);
    QCOMPARE(paramTypes[0], QGlib::GetType<QGlib::ParamSpec>());
}

void SignalsTest::emitTestClosure(const QGlib::ObjectPtr & instance, const QGlib::ParamSpecPtr & param)
{
    qDebug() << "emitTestClosure called";
    QCOMPARE(instance->property("name").get<QString>(), QString("mybin"));
    QCOMPARE(param->name(), QString("name"));
    closureCalled = true;
}

void SignalsTest::emitTest()
{
    QGst::BinPtr bin = QGst::Bin::create("mybin");
    QGlib::SignalHandler handler = QGlib::connect(bin, "notify::name",
                                                  this, &SignalsTest::emitTestClosure,
                                                  QGlib::PassSender);

    QVERIFY(handler.isConnected());

    closureCalled = false;
    QGlib::emit<void>(bin, "notify::name", bin->findProperty("name"));
    QCOMPARE(closureCalled, true);

    //calling with wrong return value. should show error message but *call* the signal
    //and return default constructed value for int
    closureCalled = false;
    int r = QGlib::emit<int>(bin, "notify::name", bin->findProperty("name"));
    QCOMPARE(r, int());
    QCOMPARE(closureCalled, true);

    //calling with wrong number of arguments. should show error message and *not call* the signal
    closureCalled = false;
    QGlib::emit<void>(bin, "notify::name");
    QCOMPARE(closureCalled, false);

    //calling wrong signal. will return default constructed value for int
    closureCalled = false;
    r = QGlib::emit<int>(bin, "foobar");
    QCOMPARE(r, int());
    QCOMPARE(closureCalled, false);

    handler.disconnect();
    QVERIFY(!handler.isConnected());

    closureCalled = false;
    QGlib::emit<void>(bin, "notify::name", bin->findProperty("name"));
    QCOMPARE(closureCalled, false);
}

QTEST_APPLESS_MAIN(SignalsTest)

#include "moc_qgsttest.cpp"
#include "signalstest.moc"
