/*
    Copyright (C) 2013  Diane Trout <diane@ghic.org>.
      (This was copied from buffertest.cpp)

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
#include <QGst/Element>
#include <QGst/ElementFactory>
#include <QGst/Pad>
#include <QGst/Caps>
#include <QGst/Event>

class PadTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void capsTest();
};

void PadTest::capsTest()
{
    QGst::ElementPtr queue = QGst::ElementFactory::make("queue", NULL);
    QGst::PadPtr pad = queue->getStaticPad("sink");
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    caps->setValue("width", 320);
    caps->setValue("height", 240);

    queue->setState(QGst::StatePlaying);

    QGst::CapsEventPtr event = QGst::CapsEvent::create(caps);
    QVERIFY(pad->sendEvent(event));

    QGst::CapsPtr caps2 = pad->currentCaps();

    QVERIFY(caps->equals(caps2));
    queue->setState(QGst::StateNull);
}
QTEST_APPLESS_MAIN(PadTest)

#include "moc_qgsttest.cpp"
#include "padtest.moc"


