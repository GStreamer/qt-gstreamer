/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include <iostream>
#include <signal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QTime>
#include <QGlib/Signal>
#include <QGst/Global>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/GhostPad>
#include <QGst/Structure>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Clock>
#include <QGst/Event>

/* This is a simple example of a command-line audio player. It accepts the filename of
 * an audio file as the first command line argument and then constructs a pipeline
 * that uses decodebin2 to decode the audio stream and autoaudiosink to output it to
 * the sound card. In the future this example will be expanded to handle video as well
 * and perhaps it will gain a simple GUI too. */

class Player
{
public:
    Player(const QString & fileName);
    ~Player();

private:
    void onBusSyncMessage(const QGst::MessagePtr & message);
    void onNewDecodedPad(QGst::PadPtr newPad);
    static QGst::BinPtr createAudioSinkBin();

    /* This smart pointer is needed to keep a reference to the underlying GstPipeline object.
     * Even if we are not going to use it, we must keep a reference to the pipeline,
     * so that it remains in memory, together with all its child elements. If this
     * reference is gone, the pipeline and all the elements will be destroyed.
     * Note that we don't need to keep references to the individual elements, because
     * when they are added in the pipeline, the pipeline keeps a reference on them. */
    QGst::PipelinePtr m_pipeline;
};

Player::Player(const QString & fileName)
{
    m_pipeline = QGst::Pipeline::create();

    QGst::ElementPtr filesrc = QGst::ElementFactory::make("filesrc");
    QGst::ElementPtr decodebin = QGst::ElementFactory::make("decodebin2");

    filesrc->setProperty("location", fileName);
    QGlib::Signal::connect(decodebin, "new-decoded-pad", this, &Player::onNewDecodedPad);

    m_pipeline->add(filesrc);
    m_pipeline->add(decodebin);
    filesrc->link(decodebin);

    QGst::BusPtr bus = m_pipeline->bus();
    bus->enableSyncMessageEmission();
    QGlib::Signal::connect(bus, "sync-message", this, &Player::onBusSyncMessage);

    m_pipeline->setState(QGst::StatePlaying);
}

Player::~Player()
{
    m_pipeline->setState(QGst::StateNull);

    /* When m_pipeline is destructed, the last reference to our pipeline will be gone,
     * and with it all the elements, buses, etc will be destroyed too. As a result,
     * there is no need to cleanup here. */
}

void Player::onBusSyncMessage(const QGst::MessagePtr & message)
{
    /* WARNING this slot gets called in a different thread */
    switch(message->type()) {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
    case QGst::MessageError: //Some error occurred.
        /* QCoreApplication::quit is safe to be called in another thread.
         * It will schedule the main event loop to exit, when execution
         * in the main thread has reached the event loop. */
        QCoreApplication::quit();
        break;
    case QGst::MessageAsyncDone:
        {
        //File prerolled, queries the pipeline to get the file duration
        QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
        //This will create a temporary (cast to query).
        m_pipeline->query(query);

        //qDebug() << QGst::Clock::timeFromClockTime(query->duration());

        /*Set the pipeline to seek to 6 seconds in the stream and play until it reaches 15 secs
         * Notice that using element->seek() is probably better in most cases
         *
        QGst::SeekEventPtr evt = QGst::SeekEvent::create(1.0, QGst::FormatTime,
                                                     QGst::SeekFlagNone,
                                                     QGst::SeekTypeSet,
                                                     QGst::Clock::clockTimeFromTime(QTime(0,0,6)),
                                                     QGst::SeekTypeSet,
                                                     QGst::Clock::clockTimeFromTime(QTime(0,0,15))
                                                     );
        m_pipeline->sendEvent(evt);*/

        /* this is the simple seek version
        m_pipeline->seek(QGst::FormatTime, QGst::SeekFlagNone,
                         QGst::Clock::clockTimeFromTime(QTime(0,0,15)) );*/
        }
        break;
    default:
        break;
    }
}

/* This method will be called every time a new "src" pad is available on the decodebin2 element.
 * Here we have to check what kind of data this pad transfers (usually it is either "audio/x-raw-*"
 * or "video/x-raw-*") and connect an appropriate sink that can handle this type of data. */
void Player::onNewDecodedPad(QGst::PadPtr newPad)
{

    QGst::CapsPtr caps = newPad->caps();
    QGst::SharedStructure structure = caps->internalStructure(0);

    /* The caps' first structure's name tells us what kind of data the pad transfers.
     * Here we want to handle either audio/x-raw-int or audio/x-raw-float. Both types
     * can be handled by the audioconvert element that is contained in the audioSinkBin,
     * so there is no need to handle them separately */
    if (structure.name().contains("audio/x-raw")) {
        QGst::BinPtr audioSinkBin = createAudioSinkBin();
        m_pipeline->add(audioSinkBin);

        /* The newly created bin must go to the playing state in order to function.
         * Here we tell it to synchronise its state with its parent, the pipeline,
         * which is scheduled to go to the playing state. */
        audioSinkBin->syncStateWithParent();

        newPad->link(audioSinkBin->getStaticPad("sink"));
    }
}

QGst::BinPtr Player::createAudioSinkBin()
{
    QGst::BinPtr bin = QGst::Bin::create();

    QGst::ElementPtr audioconvert = QGst::ElementFactory::make("audioconvert");
    QGst::ElementPtr audiosink = QGst::ElementFactory::make("autoaudiosink");

    bin->add(audioconvert);
    bin->add(audiosink);
    audioconvert->link(audiosink);

    /* Add a sink pad to the bin that proxies the sink pad of the audioconvert element */
    bin->addPad(QGst::GhostPad::create(audioconvert->getStaticPad("sink"), "sink"));

    return bin;
}

static void sighandler(int code)
{
    Q_UNUSED(code);
    QCoreApplication::quit();
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QString fileName;
    if (argc > 1) {
        fileName = QFile::decodeName(argv[1]);
    }

    if (!QFile::exists(fileName)) {
        std::cerr << "Usage: " << argv[0] << " fileToPlay" << std::endl;
        return 1;
    }

    Player *p = new Player(fileName);

    signal(SIGINT, sighandler);
    int result = app.exec();

    delete p; // we must delete all gstreamer objects before calling QGst::cleanup()
    QGst::cleanup();

    return result;
}
