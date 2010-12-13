/*
   Copyright (C) 2010  Marco Ballesio <gibrovacco@gmail.com>

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

#include "player.h"
#include <iostream>
#include <QtCore/QTime>
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QGlib/Signal>
#include <QGst/Global>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/XOverlay>
#include <QGst/Bus>
#include <QGst/Message>
#include <QGst/Query>
#include <QGst/Clock>
#include <QGst/Event>

/* This is a simple example of a command-line player. It accepts the URI of
 * a media file as the first command line argument and then constructs a pipeline
 * that uses playbin2 to decode the stream. */

Player::Player(QWidget *parent) 
    : QGst::Ui::VideoWidget(parent) {
        connect(&m_positionTimer, SIGNAL(timeout()), this, SLOT(updatePosition()));
        setMouseTracking(true);
    }

void Player::setUri(const QString & uri) {
    QString realUri = uri;

    if(uri.indexOf("://") < 0){
        if(realUri[0] != '/'){
            realUri = QDir::current().path() + "/" + realUri;
        }
        realUri = "file://" + realUri;
    }

    if(!m_pipeline){
        QGst::ElementPtr playbin = QGst::ElementFactory::make("playbin2");
        if(playbin) {
            m_pipeline = playbin.dynamicCast<QGst::Pipeline>();
            QGst::BusPtr bus = m_pipeline->bus();
            QGlib::Signal::connect(bus, "message", this, &Player::busMessage);
            bus->addSignalWatch();
        }
    }

    if(m_pipeline){
        m_pipeline->setProperty("uri", realUri);
    }
}

Player::~Player() {
    if(m_pipeline)
        m_pipeline->setState(QGst::StateNull);
}

void Player::updatePosition() {
    Q_EMIT positionChanged(position());
}

void Player::play() {
    if(m_pipeline){
        m_pipeline->setState(QGst::StatePlaying);
    }
}

void Player::pause( ){
    if(m_pipeline){
        m_pipeline->setState(QGst::StatePaused);
    }
}

void Player::stop() {
    if(m_pipeline){
        m_pipeline->setState(QGst::StateReady);
    }
}

void Player::setPosition(QTime pos) {
    QGst::SeekEventPtr evt = QGst::SeekEvent::create(
            1.0, QGst::FormatTime, QGst::SeekFlagFlush,
            QGst::SeekTypeSet, QGst::Clock::clockTimeFromTime(pos),
            QGst::SeekTypeNone, -1);

    m_pipeline->sendEvent(evt);
}

QTime Player::position() {
    if(m_pipeline){
        QGst::PositionQueryPtr query = QGst::PositionQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::Clock::timeFromClockTime(query->position());
    }
    return QTime(0, 0, 0, 0);
}

QTime Player::length() {
    if(m_pipeline){
        QGst::DurationQueryPtr query = QGst::DurationQuery::create(QGst::FormatTime);
        m_pipeline->query(query);
        return QGst::Clock::timeFromClockTime(query->duration());
    }
    return QTime(0, 0, 0, 0);
}

QGst::State Player::state() {
    QGst::State state;
    if(!m_pipeline || 
        m_pipeline->getState(&state, NULL, 1e9) != QGst::StateChangeSuccess)
    {
        state = QGst::StateNull;
    }
    return state;
}

void Player::handleStateChange(QGst::StateChangedMessagePtr scm) {
    switch(scm->newState()){
        case QGst::StatePlaying:
            Q_EMIT positionChanged(position());
            m_positionTimer.start(500);
            break;
        case QGst::StatePaused:
            if(scm->oldState() == QGst::StateReady){
                QGst::ElementPtr sink = 
                    m_pipeline->property("video-sink").get<QGst::ElementPtr>();
                if(sink){
                    setVideoSink(sink);
                    QGst::ChildProxyPtr proxy = sink.dynamicCast<QGst::ChildProxy>();
                    if (proxy)
                        proxy->childByIndex(0)->setProperty("force-aspect-ratio", true);
                }
            } else
                m_positionTimer.stop();
            break;
        case QGst::StateReady:
            if(scm->oldState() == QGst::StatePaused)
                /* Remove the sink now to avoid inter-thread issues with Qt
                   for the next time the pipeline goes to StatePlaying */
                setVideoSink(QGst::ElementPtr());
        break;
        default:
        break;
    }
}

void Player::busMessage(const QGst::MessagePtr & message) {
    switch(message->type()) {
        case QGst::MessageEos: //End of stream. We reached the end of the file.
            qDebug() << "got eos";
            m_pipeline->setState(QGst::StateReady);
            break;
        case QGst::MessageError: //Some error occurred.
            /*TODO: send a message to UI */
            break;
        case QGst::MessageStateChanged:
            if(QGlib::Type::fromInstance(message->source()).
                    isA(QGlib::GetType<QGst::Pipeline>())) 
            {
                QGst::StateChangedMessagePtr scm = 
                    message.dynamicCast<QGst::StateChangedMessage>();
                handleStateChange(scm);
                Q_EMIT stateChanged(scm->newState());
            }

            break;
        default:
            break;
    }
}

