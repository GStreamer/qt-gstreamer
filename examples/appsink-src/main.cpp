/*
    Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include <iostream>
#include <QCoreApplication>
#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/Pipeline>
#include <QGst/Parse>
#include <QGst/Message>
#include <QGst/Utils/ApplicationSink>
#include <QGst/Utils/ApplicationSource>


class MySink : public QGst::Utils::ApplicationSink
{
public:
    MySink(QGst::Utils::ApplicationSource *src)
        : QGst::Utils::ApplicationSink(), m_src(src) {}

protected:
    virtual void eos()
    {
        m_src->endOfStream();
    }

    virtual QGst::FlowReturn newSample()
    {
        QGst::SamplePtr sample = pullSample();
        m_src->pushBuffer(sample->buffer());
        return QGst::FlowOk;
    }

private:
    QGst::Utils::ApplicationSource *m_src;
};


class Player : public QCoreApplication
{
public:
    Player(int argc, char **argv);
    ~Player();

private:
    void onBusMessage(const QGst::MessagePtr & message);

private:
    QGst::Utils::ApplicationSource m_src;
    MySink m_sink;
    QGst::PipelinePtr pipeline1;
    QGst::PipelinePtr pipeline2;
};

Player::Player(int argc, char **argv)
    : QCoreApplication(argc, argv), m_sink(&m_src)
{
    QGst::init(&argc, &argv);

    if (argc <= 1) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        std::exit(1);
    }

    const char *caps = "audio/x-raw, format=(string)S16LE, channels=(int)1,"
                       " rate=(int)44100, layout=(string)interleaved";

    /* source pipeline */
    QString pipe1Descr = QString("filesrc location=\"%1\" ! "
                                 "decodebin ! "
                                 "audioconvert ! "
                                 "audioresample ! "
                                 "appsink name=\"mysink\" caps=\"%2\"").arg(argv[1], caps);
    pipeline1 = QGst::Parse::launch(pipe1Descr).dynamicCast<QGst::Pipeline>();
    m_sink.setElement(pipeline1->getElementByName("mysink"));
    QGlib::connect(pipeline1->bus(), "message::error", this, &Player::onBusMessage);
    pipeline1->bus()->addSignalWatch();

    /* sink pipeline */
    QString pipe2Descr = QString("appsrc name=\"mysrc\" caps=\"%1\" is-live=true format=3 ! "
                                 "autoaudiosink").arg(caps);
    pipeline2 = QGst::Parse::launch(pipe2Descr).dynamicCast<QGst::Pipeline>();
    m_src.setElement(pipeline2->getElementByName("mysrc"));
    QGlib::connect(pipeline2->bus(), "message", this, &Player::onBusMessage);
    pipeline2->bus()->addSignalWatch();

    /* start playing */
    pipeline1->setState(QGst::StatePlaying);
    pipeline2->setState(QGst::StatePlaying);
}

Player::~Player()
{
    pipeline1->setState(QGst::StateNull);
    pipeline2->setState(QGst::StateNull);
}

void Player::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos:
        quit();
        break;
    case QGst::MessageError:
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        break;
    default:
        break;
    }
}


int main(int argc, char **argv)
{
    Player p(argc, argv);
    return p.exec();
}
