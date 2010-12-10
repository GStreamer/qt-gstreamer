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

#include <QtGui>
#include <QtCore/QTime>
#include "mediaapp.h"

MediaApp::MediaApp(QWidget *parent) : QWidget(parent) {
    player = new Player(this);
    player->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    player->setAutoFillBackground(false);
    connect(player, SIGNAL(positionChanged(QTime)), this, SLOT(posChanged()));
    connect(player, SIGNAL(stateChanged(QGst::State)), this, SLOT(stateChanged()));

    baseDir = ".";
    fsTimer.setSingleShot(true);
    connect(&fsTimer, SIGNAL(timeout()), this, SLOT(hideControls()));

    QVBoxLayout *appLayout = new QVBoxLayout;
    appLayout->setContentsMargins(0,0,0,0);
    createUI(appLayout);
    setLayout(appLayout);

    stateChanged();
    setWindowTitle(tr("QtGstreamer example player"));
    resize(400, 400);
}

MediaApp::~MediaApp() {
    delete player;
}

void MediaApp::open() {
    player->stop();
    QString fileName = QFileDialog::getOpenFileName(
            this, tr("Open a Movie"), baseDir);

    if (!fileName.isEmpty())
        openFile(fileName);
}

void MediaApp::mouseMoveEvent(QMouseEvent* event) {
    if(isFullScreen()){
        if(!openButton->isVisible())
            showControls(true);

        fsTimer.start(3000);
    }
}


void MediaApp::showControls(bool show) {
    openButton->setVisible(show);
    fullScreenButton->setVisible(show);
    playButton->setVisible(show);
    pauseButton->setVisible(show);
    stopButton->setVisible(show);
    posSlider->setVisible(show);
    posLabel->setVisible(show);
}

void MediaApp::toggleFullScreen() {
    if(isFullScreen()){
        setMouseTracking(false);
        fsTimer.stop();
        showNormal();
    } else {
        showFullScreen();
        setMouseTracking(true);
        showControls(false);
    }
}

void MediaApp::openFile(const QString &fileName) {
    baseDir = QFileInfo(fileName).path();

    player->setUri(fileName);
    player->play();
}

void MediaApp::posChanged() {
    QTime length = player->length();
    QTime curpos = player->position();
    posLabel->setText(curpos.toString() + "/" + length.toString());

    if(length != QTime(0,0,0,0)){
        posSlider->setValue(curpos.msecsTo(QTime(0,0,0,0)) * 100 / 
                length.msecsTo(QTime(0,0,0,0))); 
    }

    if(curpos != QTime(0,0,0,0)){
        posLabel->setEnabled(true);
        posSlider->setEnabled(true);
    }
}

void MediaApp::stateChanged() {
    playButton->setEnabled(player->state() != QGst::StatePlaying);
    pauseButton->setEnabled(player->state() == QGst::StatePlaying);
    stopButton->setEnabled(player->state() != QGst::StateReady);
    if(player->state() == QGst::StateReady){
        posLabel->setText("00:00:00/00:00:00");
        posSlider->setValue(0);
    }
}

void MediaApp::setPos(int value) {
    quint64 length = -player->length().msecsTo(QTime(0,0,0,0));
    if(length != 0 && value > 0){
        QTime pos(0,0,0,0);
        pos = pos.addMSecs( length*value/100);
        player->setPosition(pos);
    }
}

inline QToolButton* MediaApp::initButton(
        QStyle::StandardPixmap icon, 
        const char *tip, 
        QObject *dstobj, const char* slot_method,
        QLayout *layout)
{
    QToolButton *button = new QToolButton;
    button->setIcon(style()->standardIcon(icon));
    button->setIconSize(QSize(36,36));
    button->setToolTip(tr(tip));
    connect(button, SIGNAL(clicked()), dstobj, slot_method);
    layout->addWidget(button);

    return button;
}

void MediaApp::createUI(QBoxLayout *appLayout) {
    appLayout->addWidget(player);

    posLabel = new QLabel();

    posSlider = new QSlider(Qt::Horizontal);
    posSlider->setTickPosition(QSlider::TicksBelow);
    posSlider->setTickInterval(10);
    posSlider->setMaximum(100);

    connect(posSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPos(int)));

    QGridLayout *posLayout = new QGridLayout;
    posLayout->addWidget(posLabel, 1, 0);
    posLayout->addWidget(posSlider, 1, 1, 1, 2);
    appLayout->addLayout(posLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();

    openButton = initButton(
            QStyle::SP_DialogOpenButton, "Open File", this, SLOT(open()), btnLayout);

    playButton = initButton(
            QStyle::SP_MediaPlay, "Play", player, SLOT(play()), btnLayout);

    pauseButton = initButton(
            QStyle::SP_MediaPause, "Pause", player, SLOT(pause()), btnLayout);

    stopButton = initButton(
            QStyle::SP_MediaStop, "Stop", player, SLOT(stop()), btnLayout);

    fullScreenButton = initButton(
            QStyle::SP_TitleBarMaxButton, "Fullscreen", this, SLOT(toggleFullScreen()), btnLayout);

    btnLayout->addStretch();
    appLayout->addLayout(btnLayout);
}
