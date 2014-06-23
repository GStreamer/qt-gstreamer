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
#include <QGst/Query>

class QueryTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void baseTest();
    void positionTest();
    void durationTest();
    void latencyTest();
    void seekingTest();
    void segmentTest();
    void convertTest();
    void formatsTest();
    void bufferingTest();
    void uriTest();
};

void QueryTest::baseTest()
{
    QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatBytes);
    QGst::StructureConstPtr ss = query->internalStructure();
    QVERIFY(ss->isValid());
}

void QueryTest::positionTest()
{
    QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QueryPosition);
    QCOMPARE(query->typeName(), QString("position"));
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setValues(QGst::FormatBytes, 1234567);
    QCOMPARE(query->position(), static_cast<qint64>(1234567));
}

void QueryTest::durationTest()
{
    QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
    QVERIFY(query->type()==QGst::QueryDuration);
    QCOMPARE(query->typeName(), QString("duration"));
    QVERIFY(query->format()==QGst::FormatTime);

    query->setValues(QGst::FormatTime, 1234567);
    QVERIFY(query->format()==QGst::FormatTime);
    QCOMPARE(query->duration(), static_cast<qint64>(1234567));
}

void QueryTest::latencyTest()
{
    QGst::LatencyQueryPtr query = QGst::LatencyQuery::create();
    QVERIFY(query->type()==QGst::QueryLatency);
    QCOMPARE(query->typeName(), QString("latency"));

    query->setValues(true, 10000, 100000);
    QVERIFY(query->hasLive());
    QCOMPARE(query->minimumLatency(), QGst::ClockTime(10000));
    QCOMPARE(query->maximumLatency(), QGst::ClockTime(100000));
}

void QueryTest::seekingTest()
{
    QGst::SeekingQueryPtr query = QGst::SeekingQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QuerySeeking);
    QCOMPARE(query->typeName(), QString("seeking"));
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setValues(QGst::FormatTime, true, 1234567, 23456789);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);

    QVERIFY(query->seekable());
    QCOMPARE(query->segmentStart(), static_cast<qint64>(1234567));
    QCOMPARE(query->segmentEnd(), static_cast<qint64>(23456789));
}

void QueryTest::segmentTest()
{
    QGst::SegmentQueryPtr query = QGst::SegmentQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QuerySegment);
    QCOMPARE(query->typeName(), QString("segment"));
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setValues(QGst::FormatTime, 2.00 , 1234567, 23456789);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);

    QCOMPARE(query->rate(), 2.00);
    QCOMPARE(query->startValue(), static_cast<qint64>(1234567));
    QCOMPARE(query->stopValue(), static_cast<qint64>(23456789));
}

void QueryTest::convertTest()
{
    QGst::ConvertQueryPtr query = QGst::ConvertQuery::create(QGst::FormatBytes, 100000,
                                                             QGst::FormatTime);
    QVERIFY(query->type()==QGst::QueryConvert);
    QCOMPARE(query->typeName(), QString("convert"));

    query->setValues(QGst::FormatBytes, 100000 , QGst::FormatTime, 2222222);
    QVERIFY(query->sourceFormat()==QGst::FormatBytes);
    QVERIFY(query->destinationFormat()==QGst::FormatTime);
    QCOMPARE(query->sourceValue(), static_cast<qint64>(100000));
    QCOMPARE(query->destinationValue(), static_cast<qint64>(2222222));
}

void QueryTest::formatsTest()
{
    QGst::FormatsQueryPtr query = QGst::FormatsQuery::create();
    QVERIFY(query->type()==QGst::QueryFormats);
    QCOMPARE(query->typeName(), QString("formats"));

    QList<QGst::Format> formats;
    formats.append(QGst::FormatPercent);
    formats.append(QGst::FormatTime);
    formats.append(QGst::FormatBytes);

    query->setFormats(formats);
    QList<QGst::Format> answer = query->formats();
    QVERIFY(answer.count()==3);
    QVERIFY(answer.contains(QGst::FormatPercent));
    QVERIFY(answer.contains(QGst::FormatTime));
    QVERIFY(answer.contains(QGst::FormatBytes));
    QVERIFY(!answer.contains(QGst::FormatBuffers));
    QVERIFY(!answer.contains(QGst::FormatUndefined));
}

void QueryTest::bufferingTest()
{
    QGst::BufferingQueryPtr query = QGst::BufferingQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QueryBuffering);
    QCOMPARE(query->typeName(), QString("buffering"));
    QVERIFY(query->rangeFormat()==QGst::FormatBytes);

    query->setBufferingPercent(true, 85);
    QVERIFY(query->isBusy());
    QCOMPARE(query->percent(), 85);

    query->setBufferingStats(QGst::BufferingStream, 12345, 23456, 345678);
    QVERIFY(query->mode()!=QGst::BufferingDownload);
    QVERIFY(query->mode()==QGst::BufferingStream);
    QCOMPARE(query->averageIn(), 12345);
    QCOMPARE(query->averageOut(), 23456);
    QCOMPARE(query->bufferingLeft(), static_cast<qint64>(345678));

    query->setBufferingRange(QGst::FormatTime, 23456, 34567, 100000);
    QVERIFY(query->rangeFormat()!=QGst::FormatBytes);
    QVERIFY(query->rangeFormat()==QGst::FormatTime);
    QCOMPARE(query->rangeStart(), static_cast<qint64>(23456));
    QCOMPARE(query->rangeStop(), static_cast<qint64>(34567));
    QCOMPARE(query->estimatedTotal(), static_cast<qint64>(100000));
}

void QueryTest::uriTest()
{
    QGst::UriQueryPtr query = QGst::UriQuery::create();
    QVERIFY(query->type()==QGst::QueryUri);
    QCOMPARE(query->typeName(), QString("uri"));

    query->setUri(QUrl::fromLocalFile("/bin/sh"));
    QCOMPARE(query->uri(), QUrl::fromLocalFile("/bin/sh"));
}

QTEST_APPLESS_MAIN(QueryTest)

#include "moc_qgsttest.cpp"
#include "querytest.moc"

