/*
   Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
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

#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <QtCore/QEvent>
#include <QtCore/QTimer>
#include <QtCore/QString>
#include <QtCore/QTime>
#include <QGst/Global>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

/* This is a simple example of a command-line player. It accepts the URI of
 * an media file as the first command line argument and then constructs a pipeline
 * that uses playbin2 to decode the stream.
 * In the future this example will perhaps gain a simple GUI. */

class Player : public QGst::Ui::VideoWidget
{
    Q_OBJECT;
public:
    Player(QWidget *parent = 0);
    ~Player();

    void setUri(const QString &uri);
    QTime position();
    QTime length();
    QGst::State state();

    /* Everything is compiled with QT_NO_KEYWORDS because otherwise Signals::emit wouldn't compile
     * so use Q_SLOTS instead of "slots" */
public Q_SLOTS:
    void stop();
    void play();
    void pause();
    void setPosition(QTime pos);
    void updatePosition();

Q_SIGNALS:
    void positionChanged(QTime);
    void stateChanged(QGst::State state);

private:
    void busMessage(const QGst::MessagePtr & message);
    void elementAdded(const QGst::ElementPtr & element);
    void handleStateChange(QGst::StateChangedMessagePtr);
    QGst::XOverlayPtr binVideoSink(QGst::BinPtr & bin);

    /* This smart pointer is needed to keep a reference to the underlying GstPipeline object.
     * Even if we are not going to use it, we must keep a reference to the pipeline,
     * so that it remains in memory, together with all its child elements. If this
     * reference is gone, the pipeline and all the elements will be destroyed.
     * Note that we don't need to keep references to the individual elements, because
     * when they are added in the pipeline, the pipeline keeps a reference on them. */
    QGst::PipelinePtr m_pipeline;
    QTimer m_positionTimer;
};

#endif
