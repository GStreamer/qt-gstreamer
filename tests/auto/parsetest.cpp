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
#include <QGlib/Error>
#include <QGst/Parse>
#include <QGst/Pipeline>
#include <QGst/Pad>

class ParseTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void launchTest();
    void binFromDescriptionTest();
};

void ParseTest::launchTest()
{
    try {
        QGst::ElementPtr e = QGst::Parse::launch("audiotestsrc ! audioconvert ! fakesink");
        QVERIFY(!e.isNull());
        QVERIFY(!e.dynamicCast<QGst::Pipeline>().isNull());
    } catch (const QGlib::Error & error) {
        QFAIL(error.what());
    }

    try {
        QGst::ElementPtr e = QGst::Parse::launch("foobar");
        QFAIL("element \"foobar\" exists???");
    } catch (const QGlib::Error & error) {
        qDebug() << error;
    }
}

void ParseTest::binFromDescriptionTest()
{
    try {
        QGst::BinPtr bin = QGst::Bin::fromDescription("audiotestsrc ! audioconvert");
        QVERIFY(!bin.isNull());
        QVERIFY(!bin->getStaticPad("src").isNull());
        QVERIFY(bin->getStaticPad("sink").isNull());
    } catch (const QGlib::Error & error) {
        QFAIL(error.what());
    }

    try {
        QGst::BinPtr bin = QGst::Bin::fromDescription("audiotestsrc ! audioconvert", QGst::Bin::NoGhost);
        QVERIFY(!bin.isNull());
        QVERIFY(bin->getStaticPad("src").isNull());
        QVERIFY(bin->getStaticPad("sink").isNull());
    } catch (const QGlib::Error & error) {
        QFAIL(error.what());
    }
}

QTEST_APPLESS_MAIN(ParseTest)

#include "moc_qgsttest.cpp"
#include "parsetest.moc"
