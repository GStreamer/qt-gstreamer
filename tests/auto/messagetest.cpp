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
#include <QGst/Message>
#include <QGlib/Error>
#include <QGlib/Quark>
#include <QGst/Bus>
#include <QGst/Pipeline>

class MessageTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void eosMessageTest();
    void errorMessageTest();
    void warningMessageTest();
    void infoMessageTest();
    void bufferingMessageTest();
    void stateChangedMessageTest();
    void stepDoneMessageTest();
    void streamStatusMessageTest();
    void applicationMessageTest();
    void elementMessageTest();
    void segmentDoneMessageTest();
    void durationChangedMessageTest();
    void latencyMessageTest();
    void asyncDoneMessageTest();
    void requestStateMessageTest();
    void stepStartMessageTest();
    void qosMessageTest();
};

void MessageTest::eosMessageTest()
{
    QGst::EosMessagePtr msg = QGst::EosMessage::create(QGst::ObjectPtr());

    QVERIFY(msg->type()==QGst::MessageEos);
    QCOMPARE(msg->typeName(), QString("eos"));

    msg->setSequenceNumber(1456);
    QCOMPARE(msg->sequenceNumber(), 1456U);
}

void MessageTest::errorMessageTest()
{
    QGlib::Quark q = QGlib::Quark::fromString("test");
    QGlib::Error err(q, 10, "test error");
    QString debugString("Test suite error");
    QGst::ErrorMessagePtr msg = QGst::ErrorMessage::create(QGst::ObjectPtr(), err,
                                                           debugString.toUtf8());

    QVERIFY(msg->type()==QGst::MessageError);
    QCOMPARE(msg->typeName(), QString("error"));

    QGlib::Error msgerr(msg->error());
    QCOMPARE(err.code(), msgerr.code());
    QCOMPARE(err.domain(), msgerr.domain());
    QCOMPARE(err.message(), msgerr.message());
    QCOMPARE(msg->debugMessage(), debugString);
}

void MessageTest::warningMessageTest()
{
    QGlib::Quark q = QGlib::Quark::fromString("test");
    QGlib::Error err(q, 10, "test warning");
    QString debugString("Test suite warning");
    QGst::WarningMessagePtr msg = QGst::WarningMessage::create(QGst::ObjectPtr(), err,
                                                           debugString.toUtf8());

    QVERIFY(msg->type()==QGst::MessageWarning);
    QCOMPARE(msg->typeName(), QString("warning"));

    QGlib::Error msgerr(msg->error());
    QCOMPARE(err.code(), msgerr.code());
    QCOMPARE(err.domain(), msgerr.domain());
    QCOMPARE(err.message(), msgerr.message());
    QCOMPARE(msg->debugMessage(), debugString);
}

void MessageTest::infoMessageTest()
{
    QGlib::Quark q = QGlib::Quark::fromString("test");
    QGlib::Error err(q, 10, "test info");
    QString debugString("Test suite info");
    QGst::InfoMessagePtr msg = QGst::InfoMessage::create(QGst::ObjectPtr(), err,
                                                           debugString.toUtf8());

    QVERIFY(msg->type()==QGst::MessageInfo);
    QCOMPARE(msg->typeName(), QString("info"));

    QGlib::Error msgerr(msg->error());
    QCOMPARE(err.code(), msgerr.code());
    QCOMPARE(err.domain(), msgerr.domain());
    QCOMPARE(err.message(), msgerr.message());
    QCOMPARE(msg->debugMessage(), debugString);
}

//TODO TagMessage

void MessageTest::bufferingMessageTest()
{
    QGst::BusPtr bus(QGst::Bus::create());
    QGst::BufferingMessagePtr msg = QGst::BufferingMessage::create(bus, 75);

    QVERIFY(msg->type()==QGst::MessageBuffering);
    QCOMPARE(msg->typeName(), QString("buffering"));

    QCOMPARE(msg->percent(), 75);

    msg->setStats(QGst::BufferingDownload, 123, 456, 12345678);

    QVERIFY(msg->mode()==QGst::BufferingDownload);
    QCOMPARE(msg->averageInputRate(), 123);
    QCOMPARE(msg->averageOutputRate(), 456);
    QCOMPARE(msg->bufferingTimeLeft(), static_cast<qint64>(12345678));
}

void MessageTest::stateChangedMessageTest()
{
    QGst::BusPtr bus(QGst::Bus::create());
    QGst::StateChangedMessagePtr msg = QGst::StateChangedMessage::create(QGst::ObjectPtr(),
                                                                         QGst::StatePaused,
                                                                         QGst::StateReady,
                                                                         QGst::StatePlaying);

    QVERIFY(msg->type()==QGst::MessageStateChanged);
    QCOMPARE(msg->typeName(), QString("state-changed"));

    QCOMPARE(msg->oldState(), QGst::StatePaused);
    QCOMPARE(msg->newState(), QGst::StateReady);
    QCOMPARE(msg->pendingState(), QGst::StatePlaying);
}

void MessageTest::stepDoneMessageTest()
{

    QGst::StepDoneMessagePtr msg = QGst::StepDoneMessage::create(QGst::ObjectPtr(),
                                                                 QGst::FormatBytes, 4567, 1.0, true,
                                                                 false, 1234567, false);

    QVERIFY(msg->type()==QGst::MessageStepDone);
    QCOMPARE(msg->typeName(), QString("step-done"));

    QVERIFY(msg->format()==QGst::FormatBytes);
    QCOMPARE(msg->amount(), static_cast<quint64>(4567));
    QCOMPARE(msg->rate(), 1.0);
    QCOMPARE(msg->isFlushingStep(), true);
    QCOMPARE(msg->isIntermediateStep(), false);
    QCOMPARE(msg->duration(), static_cast<quint64>(1234567));
    QCOMPARE(msg->causedEos(), false);
}

void MessageTest::streamStatusMessageTest()
{
    QGst::PipelinePtr pipe(QGst::Pipeline::create("test"));
    QGst::StreamStatusMessagePtr msg = QGst::StreamStatusMessage::create(QGst::ObjectPtr(),
                                                                         QGst::StreamStatusTypeStop,
                                                                         pipe);

    QVERIFY(msg->type()==QGst::MessageStreamStatus);
    QCOMPARE(msg->typeName(), QString("stream-status"));

    QVERIFY(msg->statusType()==QGst::StreamStatusTypeStop);
    QVERIFY(msg->owner()==pipe);

    QGlib::Value v(78);
    msg->setStreamStatusObject(v);
    QCOMPARE(msg->streamStatusObject().get<int>(), 78);
}

void MessageTest::applicationMessageTest()
{
    QGst::Structure s("mystruct");
    s.setValue("width", 365);
    QGst::ApplicationMessagePtr msg = QGst::ApplicationMessage::create(QGst::ObjectPtr(), s);

    QVERIFY(msg->type()==QGst::MessageApplication);
    QCOMPARE(msg->typeName(), QString("application"));

    QVERIFY(msg->internalStructure()->isValid());
    QCOMPARE(msg->internalStructure()->value("width").get<int>(), 365);
}

void MessageTest::elementMessageTest()
{
    QGst::Structure s("mystruct");
    s.setValue("myfield", 56.56);
    QGst::ElementMessagePtr msg = QGst::ElementMessage::create(QGst::ObjectPtr(), s);

    QVERIFY(msg->type()==QGst::MessageElement);
    QCOMPARE(msg->typeName(), QString("element"));

    QVERIFY(msg->internalStructure()->isValid());
    QCOMPARE(msg->internalStructure()->value("myfield").get<double>(), 56.56);
}

void MessageTest::segmentDoneMessageTest()
{
    QGst::SegmentDoneMessagePtr msg = QGst::SegmentDoneMessage::create(QGst::ObjectPtr(),
                                                                       QGst::FormatTime, 4567898);

    QVERIFY(msg->type()==QGst::MessageSegmentDone);
    QCOMPARE(msg->typeName(), QString("segment-done"));

    QVERIFY(msg->format()==QGst::FormatTime);
    QCOMPARE(msg->position(), static_cast<qint64>(4567898));
}

void MessageTest::durationChangedMessageTest()
{
    QGst::DurationChangedMessagePtr msg = QGst::DurationChangedMessage::create(QGst::ObjectPtr());
    QVERIFY(msg->type()==QGst::MessageDurationChanged);
    QCOMPARE(msg->typeName(), QString("duration-changed"));
}

void MessageTest::latencyMessageTest()
{
    QGst::LatencyMessagePtr msg = QGst::LatencyMessage::create(QGst::ObjectPtr());

    QVERIFY(msg->type()==QGst::MessageLatency);
    QCOMPARE(msg->typeName(), QString("latency"));
}

void MessageTest::asyncDoneMessageTest()
{
    QGst::ClockTime time = QGst::ClockTime::fromTime(QTime(17,0));
    QGst::AsyncDoneMessagePtr msg = QGst::AsyncDoneMessage::create(QGst::ObjectPtr(), time);

    QVERIFY(msg->type()==QGst::MessageAsyncDone);
    QCOMPARE(msg->typeName(), QString("async-done"));
    QCOMPARE(msg->runningTime().toTime(), QTime(17,0));
}

void MessageTest::requestStateMessageTest()
{
    QGst::RequestStateMessagePtr msg = QGst::RequestStateMessage::create(QGst::ObjectPtr(),
                                                                         QGst::StatePlaying);

    QVERIFY(msg->type()==QGst::MessageRequestState);
    QCOMPARE(msg->typeName(), QString("request-state"));

    QCOMPARE(msg->state(), QGst::StatePlaying);
}

void MessageTest::stepStartMessageTest()
{

    QGst::StepStartMessagePtr msg = QGst::StepStartMessage::create(QGst::ObjectPtr(), true,
                                                                 QGst::FormatBytes, 4567, 1.0, true,
                                                                 false);

    QVERIFY(msg->type()==QGst::MessageStepStart);
    QCOMPARE(msg->typeName(), QString("step-start"));

    QVERIFY(msg->format()==QGst::FormatBytes);
    QCOMPARE(msg->isActive(), true);
    QCOMPARE(msg->amount(), static_cast<quint64>(4567));
    QCOMPARE(msg->rate(), 1.0);
    QCOMPARE(msg->isFlushingStep(), true);
    QCOMPARE(msg->isIntermediateStep(), false);
}

void MessageTest::qosMessageTest()
{

    QGst::QosMessagePtr msg = QGst::QosMessage::create(QGst::ObjectPtr(), true, 345678, 6789098,
                                                                 234543, 56787654);

    QVERIFY(msg->type()==QGst::MessageQos);
    QCOMPARE(msg->typeName(), QString("qos"));

    QVERIFY(msg->live());
    QCOMPARE(msg->runningTime(), static_cast<quint64>(345678));
    QCOMPARE(msg->streamTime(), static_cast<quint64>(6789098));
    QCOMPARE(msg->timestamp(), static_cast<quint64>(234543));
    QCOMPARE(msg->duration(), static_cast<quint64>(56787654));

    msg->setValues(453, 0.1, 9);
    QCOMPARE(msg->jitter(), static_cast<qint64>(453));
    QCOMPARE(msg->proportion(), 0.1);
    QCOMPARE(msg->quality(), 9);

    msg->setStats(QGst::FormatBytes, 12345, 32);
    QVERIFY(msg->format()==QGst::FormatBytes);
    QCOMPARE(msg->processed(), static_cast<quint64>(12345));
    QCOMPARE(msg->dropped(), static_cast<quint64>(32));
}

QTEST_APPLESS_MAIN(MessageTest)

#include "moc_qgsttest.cpp"
#include "messagetest.moc"
