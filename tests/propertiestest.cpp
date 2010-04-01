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

class PropertiesTest : public QGstTest
{
    Q_OBJECT
private slots:
    void findPropertyTest();
    void listPropertiesTest();
    void getPropertyTest();
};

void PropertiesTest::findPropertyTest()
{
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);
    QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin, false);

    QGlib::ParamSpecPtr param = object->findProperty("name");
    QVERIFY(!param.isNull());
    QVERIFY(param->name() == "name");

    qDebug() << param->name() << param->nick() << param->description();
}

void PropertiesTest::listPropertiesTest()
{
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);
    QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin, false);

    QList<QGlib::ParamSpecPtr> paramlist = object->listProperties();
    QVERIFY(!paramlist.isEmpty());

    foreach(const QGlib::ParamSpecPtr & param, paramlist) {
        qDebug() << param->name() << param->nick() << param->description();
    }
}

void PropertiesTest::getPropertyTest()
{
    GstObject *bin = GST_OBJECT(gst_object_ref(GST_OBJECT(gst_bin_new(NULL))));
    gst_object_sink(bin);
    QGst::ObjectPtr object = QGst::ObjectPtr::wrap(bin, false);

    try {
        QString name = object->property<QString>("name");
        QVERIFY(!name.isEmpty());
        qDebug() << name;
    } catch(...) {
        QFAIL("Exception");
    }
}

QTEST_APPLESS_MAIN(PropertiesTest)

#include "moc_qgsttest.cpp"
#include "propertiestest.moc"
