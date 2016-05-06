/*
    Copyright (C) 2014  Diane Trout <diane@ghic.org>

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
#include <QGst/QGstMemory>
#include <QGst/Allocator>

class MemoryTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void testMap();
};

void MemoryTest::testMap()
{
    QGst::AllocatorPtr allocator = QGst::Allocator::getDefault();
    QGst::MemoryPtr mem = allocator->alloc(100);

    QCOMPARE(mem->size(), static_cast<size_t>(100));
    QCOMPARE(mem->offset(), static_cast<size_t>(0));
    QVERIFY(mem->maxSize() >= 100);

    QGst::MapInfo info;
    QVERIFY(mem->map(info, QGst::MapRead));
    QVERIFY(info.data() != NULL);
    QCOMPARE(info.size(), static_cast<size_t>(100));
    QCOMPARE(info.maxSize(), mem->maxSize());

    mem->unmap(info);
    allocator->free(mem);
}

QTEST_APPLESS_MAIN(MemoryTest)

#include "moc_qgsttest.cpp"
#include "memorytest.moc"
