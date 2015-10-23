/*
    Copyright (C) 2015  Pavel Bludov <pbludov@gmail.com>

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
#include "mainwindow.h"
#include <QApplication>

#include <QGst/Init>

int main(int argc, char *argv[])
{
    QGst::init(&argc, &argv);
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    int ret = a.exec();

    QGst::cleanup();
    return ret;
}
