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
#ifndef SIGNALSTEST_H
#define SIGNALSTEST_H

#include "qgsttest.h"
#include <QGst/Object>

class SignalsTest : public QGstTest
{
    Q_OBJECT
private:
    void closureTestClosure(QGst::ObjectPtr obj, QGst::ObjectPtr parentObj);
private slots:
    void closureTest();
};

#endif // SIGNALSTEST_H
