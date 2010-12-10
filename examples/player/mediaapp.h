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

#ifndef MEDIAAPP_H
#define MEDIAAPP_H

#include <QtGui/QWidget>
#include <QStyle>
#include "player.h"

QT_BEGIN_NAMESPACE
class QBoxLayout;
class QLabel;
class QSlider;
class QToolButton;
class QTimer;
QT_END_NAMESPACE

class MediaApp : public QWidget {
    Q_OBJECT

    public:
        MediaApp(QWidget *parent = 0);
        ~MediaApp();
        void openFile(const QString &fileName);

        private Q_SLOTS:
        void open();
        void toggleFullScreen();
        void stateChanged();
        void posChanged();
        void setPos(int);
        void hideControls(){ showControls(false); }

    private:
        QToolButton *initButton(
                QStyle::StandardPixmap, const char* , QObject* , const char*, QLayout*);
        void createUI(QBoxLayout *);
        void showControls(bool);
        void mouseMoveEvent(QMouseEvent*);

        QString baseDir;
        Player *player;
        QToolButton *openButton;
        QToolButton *fullScreenButton;
        QToolButton *playButton;
        QToolButton *pauseButton;
        QToolButton *stopButton;
        QSlider *posSlider;
        QLabel *posLabel;
        QTimer fsTimer;
};

#endif
