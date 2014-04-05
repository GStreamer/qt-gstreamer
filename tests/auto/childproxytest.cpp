/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include <QGst/Bin>
#include <QGst/ElementFactory>

class ChildProxyTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void inspectionTest();
    void removeTest();
    void propertiesTest();
};

void ChildProxyTest::inspectionTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QVERIFY(!bin.isNull());
    QCOMPARE(bin->childrenCount(), static_cast<uint>(0));

    QGst::ElementPtr tee = QGst::ElementFactory::make("tee", "mytee");
    QVERIFY(!tee.isNull());
    QCOMPARE(bin->add(tee), true);
    QCOMPARE(bin->childrenCount(), static_cast<uint>(1));

    {
        QGst::ElementPtr tee2 = bin->childByIndex(0).dynamicCast<QGst::Element>();
        QVERIFY(!tee2.isNull());
        QCOMPARE(tee2->name(), QString("mytee"));
    }

    {
        QGst::ElementPtr tee2 = bin->childByName("mytee").dynamicCast<QGst::Element>();
        QVERIFY(!tee2.isNull());
        QCOMPARE(tee2->name(), QString("mytee"));
    }
}

void ChildProxyTest::removeTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QVERIFY(!bin.isNull());
    QCOMPARE(bin->childrenCount(), static_cast<uint>(0));

    QGst::ElementPtr tee = QGst::ElementFactory::make("tee", "mytee");
    QVERIFY(!tee.isNull());
    QCOMPARE(bin->add(tee), true);
    QCOMPARE(bin->childrenCount(), static_cast<uint>(1));

    QCOMPARE(bin->remove(tee), true);
    QCOMPARE(bin->childrenCount(), static_cast<uint>(0));
}

void ChildProxyTest::propertiesTest()
{
    QGst::BinPtr bin = QGst::Bin::create();
    QVERIFY(!bin.isNull());

    QGst::ElementPtr tee = QGst::ElementFactory::make("tee", "mytee");
    QVERIFY(!tee.isNull());
    QCOMPARE(bin->add(tee), true);

    {
        QGlib::ObjectPtr obj;
        QGlib::ParamSpecPtr param;
        QCOMPARE(bin->findChildProperty("mytee::has-chain", &obj, &param), true);
        QVERIFY(!obj.isNull());
        QVERIFY(!param.isNull());
        QCOMPARE(obj->property("name").get<QString>(), QString("mytee"));
        QCOMPARE(param->name(), QString("has-chain"));
    }

    {
        QGlib::Value v = bin->childProperty("mytee::has-chain");
        QVERIFY(v.isValid());
        QCOMPARE(v.type(), QGlib::Type(QGlib::Type::Boolean));
    }
}


QTEST_APPLESS_MAIN(ChildProxyTest)

#include "moc_qgsttest.cpp"
#include "childproxytest.moc"
