/*
    Copyright (C) 2010 Marco Ballesio <gibrovacco@gmail.com>
    Copyright (C) 2011 Collabora Ltd.
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
#ifndef PLAYER_H
#define PLAYER_H

#include <QTimer>
#include <QTime>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class Player : public QGst::Ui::VideoWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

    void setUri(const QString & uri);

    QTime position() const;
    void setPosition(const QTime & pos);
    int volume() const;

    QTime length() const;
    QGst::State state() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();
    void setVolume(int volume);

Q_SIGNALS:
    void positionChanged();
    void stateChanged();

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);

    QGst::PipelinePtr m_pipeline;
    QTimer m_positionTimer;
};

#endif
