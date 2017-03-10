/*
    Copyright (C) 2017 Aleix Pol Gonzalez <aleixpol@kde.org>

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

#include <QTest>
#include <QSignalSpy>
#include <QGst/Init>
#include <QQmlApplicationEngine>
#include <QQmlParserStatus>
#include <QQmlContext>
#include <QGlib/Connect>
#include <QGst/Pipeline>
#include <QGst/Bus>
#include <QGst/Parse>
#include <QGst/ElementFactory>
#include <QGst/Message>
#include <QGst/Quick/VideoSurface>


class QtQuick2Test : public QObject
{
    Q_OBJECT
private:
    void onBusMessage(const QGst::MessagePtr &msg)
    {
        switch (msg->type()) {
        case QGst::MessageError: //Some error occurred.
            qCritical() << msg.staticCast<QGst::ErrorMessage>()->error();
            break;
        default:
//             qDebug() << msg->type();
//             qDebug() << msg->typeName();
//             qDebug() << msg->internalStructure()->name();
            break;
        }
    }

private Q_SLOTS:
    void testLaunch();
};

void QtQuick2Test::testLaunch()
{
    QGst::init();

    QQmlApplicationEngine* engine = new QQmlApplicationEngine(this);
    QGst::Quick::VideoSurface *surface = new QGst::Quick::VideoSurface(this);

    auto source = QGst::ElementFactory::make("videotestsrc", "");

    auto pipeline = QGst::Pipeline::create();
    auto bus = pipeline->bus();
    bus->addSignalWatch();
    QGlib::connect(bus, "message", this, &QtQuick2Test::onBusMessage);
    pipeline->add(source, surface->videoSink());

    source->link(surface->videoSink());

    pipeline->setState(QGst::StatePlaying);

    engine->rootContext()->setContextProperty("surface1", surface);
    engine->load(QUrl("videoitemtest.qml"));

    QSignalSpy spy(engine->rootObjects().first(), SIGNAL(frameSwapped()));
    spy.wait(100);

    pipeline->setState(QGst::StateNull);

    delete engine;
}

QTEST_MAIN(QtQuick2Test)

#include "qtquick2test.moc"
