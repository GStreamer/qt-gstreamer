/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/Init>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <signal.h>

static void sighandler(int code)
{
    Q_UNUSED(code);
    qDebug("SIGINT");
    QCoreApplication::quit();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QGst::PipelinePtr pipeline = QGst::Pipeline::create();

    QGst::ElementPtr src = QGst::ElementFactory::make("audiotestsrc");
    QGst::ElementPtr sink = QGst::ElementFactory::make("autoaudiosink");

    pipeline->add(src);
    pipeline->add(sink);
    src->link(sink);

    pipeline->setState(QGst::StatePlaying);

    signal(SIGINT, sighandler);
    int result = app.exec();

    pipeline->setState(QGst::StateNull);
    QGst::cleanup();

    return result;
}
