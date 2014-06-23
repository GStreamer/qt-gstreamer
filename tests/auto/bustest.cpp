/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include <QGlib/Connect>
#include <QGst/Bus>
#include <QGst/Structure>
#include <QGst/Message>

class BusTest : public QGstTest
{
    Q_OBJECT
private:
    void messageClosure(const QGst::MessagePtr &);

private Q_SLOTS:
    void watchTest();
    void watchTestWithWatchRemoval();

private:
    QEventLoop m_eventLoop;
    int m_messagesReceived;
};

class MessagePushThread : public QThread
{
public:
    QGst::BusPtr bus;

private:
    virtual void run();
};

void MessagePushThread::run()
{
    for (int i=0; i<10; ++i) {
        QGst::Structure s("test");
        s.setValue("sequence", i);
        QGst::MessagePtr msg = QGst::ApplicationMessage::create(bus, s);
        QVERIFY(!msg.isNull());
        bus->post(msg);

        if (i > 5) {
            msleep(10); //add some delay for some messages
        }
    }
}

void BusTest::messageClosure(const QGst::MessagePtr & msg)
{
    //we should receive this signal from the main thread
    QCOMPARE(QThread::currentThread(), QCoreApplication::instance()->thread());

    //check if the message is the same we sent from the other thread
    QVERIFY(!msg.isNull());
    QCOMPARE(msg->type(), QGst::MessageApplication);
    QGst::StructureConstPtr s = msg->internalStructure();
    QCOMPARE(s->name(), QString("test"));
    QCOMPARE(s->value("sequence").get<int>(), m_messagesReceived);

    if (++m_messagesReceived == 10) {
        m_eventLoop.exit(1);
    }
}

void BusTest::watchTest()
{
    MessagePushThread thread;
    thread.bus = QGst::Bus::create();

    m_messagesReceived = 0;
    thread.bus->addSignalWatch();
    QGlib::connect(thread.bus, "message", this, &BusTest::messageClosure);

    thread.start();

    //kill the event loop after 5 seconds
    QTimer::singleShot(5000, &m_eventLoop, SLOT(quit()));
    int code = m_eventLoop.exec();
    QCOMPARE(code, 1); //we get 1 if we quit from messageClosure and 0 if we quit from the timer
    QCOMPARE(m_messagesReceived, 10);

    thread.wait(); //allow the thread to cleanup properly
}

//This is the same as watchTest(), but we remove the signal watch in the end.
//This tests the case where the watch is removed before the bus is destroyed.
void BusTest::watchTestWithWatchRemoval()
{
    MessagePushThread thread;
    thread.bus = QGst::Bus::create();

    m_messagesReceived = 0;
    thread.bus->addSignalWatch();
    QGlib::connect(thread.bus, "message", this, &BusTest::messageClosure);

    thread.start();

    //kill the event loop after 5 seconds
    QTimer::singleShot(5000, &m_eventLoop, SLOT(quit()));
    int code = m_eventLoop.exec();
    QCOMPARE(code, 1); //we get 1 if we quit from messageClosure and 0 if we quit from the timer
    QCOMPARE(m_messagesReceived, 10);

    thread.wait(); //allow the thread to cleanup properly

    thread.bus->removeSignalWatch();
}

QTEST_MAIN(BusTest)

#include "moc_qgsttest.cpp"
#include "bustest.moc"
