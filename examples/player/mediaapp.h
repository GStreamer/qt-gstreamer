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
#ifndef MEDIAAPP_H
#define MEDIAAPP_H

#include <QTimer>
#include <QWidget>
#include <QStyle>

class Player;
class QBoxLayout;
class QLabel;
class QSlider;
class QToolButton;
class QTimer;

class MediaApp : public QWidget
{
    Q_OBJECT
public:
    MediaApp(QWidget *parent = 0);
    ~MediaApp();

    void openFile(const QString & fileName);

private Q_SLOTS:
    void open();
    void toggleFullScreen();

    void onStateChanged();
    void onPositionChanged();

    void setPosition(int position);

    void showControls(bool show = true);
    void hideControls() { showControls(false); }

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    QToolButton *initButton(QStyle::StandardPixmap icon, const QString & tip,
                            QObject *dstobj, const char *slot_method, QLayout *layout);
    void createUI(QBoxLayout *appLayout);

    QString m_baseDir;
    Player *m_player;
    QToolButton *m_openButton;
    QToolButton *m_fullScreenButton;
    QToolButton *m_playButton;
    QToolButton *m_pauseButton;
    QToolButton *m_stopButton;
    QSlider *m_positionSlider;
    QSlider *m_volumeSlider;
    QLabel *m_positionLabel;
    QLabel *m_volumeLabel;
    QTimer m_fullScreenTimer;
};

#endif
