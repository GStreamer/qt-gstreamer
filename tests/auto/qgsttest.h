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
#ifndef QGSTTEST_H
#define QGSTTEST_H

#include <QtTest/QtTest>
#include <QGst/Init>
#include <gst/gst.h>

class QGstTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase() { QGst::init(); }
    void cleanupTestCase() { QGst::cleanup(); }
};

#endif
