/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#include <QGst/Buffer>
#include <QGst/QGstMemory>
#include <QGst/Caps>

class BufferTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void simpleTest();
    void flagsTest();
    void copyTest();
    void memoryPeekTest();
};

void BufferTest::simpleTest()
{
    QGst::BufferPtr buffer = QGst::Buffer::create(10);

    QCOMPARE(buffer->size(), (quint32) 10);
    QVERIFY(buffer->getMemory(0));
}

void BufferTest::flagsTest()
{
    QGst::BufferPtr buffer = QGst::Buffer::create(10);
    QGst::BufferFlags flags(QGst::BufferFlagLive & QGst::BufferFlagDiscont);
    buffer->setFlags(flags);

    QGst::BufferFlags flags2 = buffer->flags();
    QCOMPARE(flags, flags2);

    QGst::BufferFlags flags3(QGst::BufferFlagLive);
    QVERIFY(flags2!=flags3);
}

void BufferTest::copyTest()
{
    QGst::BufferPtr buffer = QGst::Buffer::create(10);
    QGst::BufferFlags flags(QGst::BufferFlagLive & QGst::BufferFlagDiscont);
    buffer->setFlags(flags);

    QGst::BufferPtr buffer2 = buffer->copy();

    QCOMPARE(buffer->size(), buffer2->size());
    QCOMPARE(buffer->decodingTimeStamp(), buffer2->decodingTimeStamp());
    QCOMPARE(buffer->presentationTimeStamp(), buffer2->presentationTimeStamp());
    QCOMPARE(buffer->duration(), buffer2->duration());

    QGst::BufferFlags flags2(QGst::BufferFlagDiscont);
    buffer2->setFlags(flags2);

    QVERIFY(buffer->flags() != buffer2->flags());
}

void BufferTest::memoryPeekTest()
{
    QGst::BufferPtr buffer = QGst::Buffer::create(10);
    guint8 bytes[100];
    size_t returned_bytes;

    returned_bytes = buffer->extract(0, &bytes, 10);
    QCOMPARE(returned_bytes, static_cast<size_t>(10));

    returned_bytes = buffer->extract(0, &bytes, 20);
    QCOMPARE(returned_bytes, static_cast<size_t>(10));

    QGst::MemoryPtr m = buffer->getMemory(0);

    QVERIFY(m);
    QVERIFY(m->isWritable());

}
QTEST_APPLESS_MAIN(BufferTest)

#include "moc_qgsttest.cpp"
#include "buffertest.moc"


