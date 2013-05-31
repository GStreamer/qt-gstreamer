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
#include <QGst/UriHandler>
#include <QGst/Element>
#include <QGst/ElementFactory>

class UriHandlerTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void interfaceTest();
    void makeTest();
};

void UriHandlerTest::interfaceTest()
{
    QGst::ElementPtr e = QGst::ElementFactory::make("filesrc");
    QGst::UriHandlerPtr u = e.dynamicCast<QGst::UriHandler>();
    QVERIFY(!u.isNull());

    QCOMPARE(u->uriType(), QGst::UriSrc);
    QCOMPARE(u->supportedProtocols(), QStringList() << "file");

    QVERIFY(u->setUri(QUrl::fromLocalFile("/bin/sh")));
    QCOMPARE(u->uri(), QUrl::fromLocalFile("/bin/sh"));
    QCOMPARE(u->property("location").get<QString>(),
             QDir::toNativeSeparators(QLatin1String("/bin/sh")));
}

void UriHandlerTest::makeTest()
{
    QVERIFY(QGst::UriHandler::protocolIsSupported(QGst::UriSrc, "file"));

    QGst::ElementPtr e = QGst::UriHandler::makeFromUri(QGst::UriSrc, QUrl::fromLocalFile("/bin/sh"));
    QVERIFY(!e.isNull());

    QGst::UriHandlerPtr u = e.dynamicCast<QGst::UriHandler>();
    QVERIFY(!u.isNull());
    QCOMPARE(u->uri(), QUrl::fromLocalFile("/bin/sh"));
}


QTEST_APPLESS_MAIN(UriHandlerTest)

#include "moc_qgsttest.cpp"
#include "urihandlertest.moc"
