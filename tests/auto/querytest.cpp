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
    GNU General Public License for more details.

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
    QGst::Structure s("mystructure");
    QGst::QueryPtr query = QGst::Query::create(QGst::QueryCustom, s);
    QVERIFY(query->type()==QGst::QueryCustom);

    QGst::SharedStructure ss = query->structure();
    QVERIFY(ss.isValid());
    ss.setValue("myfield", 365);
    QCOMPARE(ss.value("myfield").get<int>(), 365);

}

void QueryTest::positionTest()
{
    QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QueryPosition);
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setPosition(QGst::FormatTime, 1234567);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);
    QVERIFY(query->position()==1234567);
}

void QueryTest::durationTest()
{
    QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QueryDuration);
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setDuration(QGst::FormatTime, 1234567);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);
    QVERIFY(query->duration()==1234567);
}

void QueryTest::latencyTest()
{
    QGst::LatencyQueryPtr query = QGst::LatencyQuery::create();
    QVERIFY(query->type()==QGst::QueryLatency);

    query->setLatency(true, 10000, 100000);
    QVERIFY(query->hasLive());
    QVERIFY(query->minimumLatency()==10000);
    QVERIFY(query->maximumLatency()==100000);
}

void QueryTest::seekingTest()
{
    QGst::SeekingQueryPtr query = QGst::SeekingQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QuerySeeking);
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setSeeking(QGst::FormatTime, true, 1234567, 23456789);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);

    QVERIFY(query->seekable());
    QVERIFY(query->segmentStart()==1234567);
    QVERIFY(query->segmentEnd()==23456789);
}

void QueryTest::segmentTest()
{
    QGst::SegmentQueryPtr query = QGst::SegmentQuery::create(QGst::FormatBytes);
    QVERIFY(query->type()==QGst::QuerySegment);
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setSegment(QGst::FormatTime, 2.00 , 1234567, 23456789);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);

    QCOMPARE(query->rate(), 2.00);
    QVERIFY(query->startValue()==1234567);
    QVERIFY(query->stopValue()==23456789);
}

void QueryTest::convertTest()
{
    QGst::ConvertQueryPtr query = QGst::ConvertQuery::create(QGst::FormatBytes, 100000,
                                                             QGst::FormatTime);
    QVERIFY(query->type()==QGst::QueryConvert);

    query->setConvert(QGst::FormatBytes, 100000 , QGst::FormatTime, 2222222);
    QVERIFY(query->sourceFormat()==QGst::FormatBytes);
    QVERIFY(query->destinationFormat()==QGst::FormatTime);
    QVERIFY(query->sourceValue()==100000);
    QVERIFY(query->destinationValue()==2222222);
}

void QueryTest::formatsTest()
{
    QGst::FormatsQueryPtr query = QGst::FormatsQuery::create();
    QVERIFY(query->type()==QGst::QueryFormats);

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
    QVERIFY(query->format()==QGst::FormatBytes);

    query->setPercent(true, 85);
    QVERIFY(query->isBusy());
    QCOMPARE(query->percent(), 85);

    query->setStats(QGst::BufferingStream, 12345, 23456, 345678);
    QVERIFY(query->mode()!=QGst::BufferingDownload);
    QVERIFY(query->mode()==QGst::BufferingStream);
    QCOMPARE(query->averageIn(), 12345);
    QCOMPARE(query->averageOut(), 23456);
    QVERIFY(query->bufferingLeft()==345678);

    query->setRange(QGst::FormatTime, 23456, 34567, 100000);
    QVERIFY(query->format()!=QGst::FormatBytes);
    QVERIFY(query->format()==QGst::FormatTime);
    QVERIFY(query->rangeStart()==23456);
    QVERIFY(query->rangeStop()==34567);
    QVERIFY(query->estimatedTotal()==100000);
}

void QueryTest::uriTest()
{
    QGst::UriQueryPtr query = QGst::UriQuery::create();
    QVERIFY(query->type()==QGst::QueryUri);

    query->setUri(QUrl::fromLocalFile("/bin/sh"));
    QCOMPARE(query->uri(), QUrl::fromLocalFile("/bin/sh"));
}

QTEST_APPLESS_MAIN(QueryTest)

#include "moc_qgsttest.cpp"
#include "querytest.moc"

