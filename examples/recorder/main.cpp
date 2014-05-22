/*
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
#include "ui_recorder.h"

#include <QDir>
#include <QApplication>
#include <QDialog>
#include <QMessageBox>

#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/ElementFactory>
#include <QGst/ChildProxy>
#include <QGst/Pipeline>
#include <QGst/Pad>
#include <QGst/Event>
#include <QGst/Message>
#include <QGst/Bus>

#ifdef Q_WS_X11
# include <QX11Info>
#endif


class Recorder : public QDialog
{
    Q_OBJECT
public:
    Recorder(QWidget *parent = 0);

private:
    enum Device { AudioSrc, VideoSrc };

    QGst::BinPtr createAudioSrcBin();
    QGst::BinPtr createVideoSrcBin();

    void start();
    void stop();

    void onBusMessage(const QGst::MessagePtr & message);

private Q_SLOTS:
    void on_startStopButton_clicked();

private:
    Ui::Recorder m_ui;
    QGst::PipelinePtr m_pipeline;
};

Recorder::Recorder(QWidget *parent)
    : QDialog(parent)
{
    m_ui.setupUi(this);

    QGst::ElementFactoryPtr ximagesrc = QGst::ElementFactory::find("ximagesrc");
    if (!ximagesrc) {
        //if we don't have ximagesrc, disable the choice to use it
        m_ui.videoSourceComboBox->removeItem(1);
    } else {
#ifdef Q_WS_X11
        //setup the default screen to be the screen that this dialog is shown
        m_ui.displayNumSpinBox->setValue(QX11Info::appScreen());
#endif
    }

    //set default output file
    m_ui.outputFileEdit->setText(QDir::currentPath() + QDir::separator() + "out.ogv");
}

QGst::BinPtr Recorder::createAudioSrcBin()
{
    QGst::BinPtr audioBin;

    try {
        audioBin = QGst::Bin::fromDescription("autoaudiosrc name=\"audiosrc\" ! audioconvert ! "
                                              "audioresample ! audiorate ! speexenc ! queue");
    } catch (const QGlib::Error & error) {
        qCritical() << "Failed to create audio source bin:" << error;
        return QGst::BinPtr();
    }

    QGst::ElementPtr src = audioBin->getElementByName("audiosrc");
    //autoaudiosrc creates the actual source in the READY state

    src->setState(QGst::StateReady);
    return audioBin;
}

QGst::BinPtr Recorder::createVideoSrcBin()
{
    QGst::BinPtr videoBin;

    try {
        if (m_ui.videoSourceComboBox->currentIndex() == 0) { //camera
            videoBin = QGst::Bin::fromDescription("autovideosrc name=\"videosrc\" !"
                                                  "videoconvert ! theoraenc ! queue");
            return videoBin;
        } else { //screencast
            videoBin = QGst::Bin::fromDescription("ximagesrc name=\"videosrc\" ! "
                                                  "videoconvert ! theoraenc ! queue");
        }
    } catch (const QGlib::Error & error) {
        qCritical() << "Failed to create video source bin:" << error;
        return QGst::BinPtr();
    }

    //set the source's properties
    if (m_ui.videoSourceComboBox->currentIndex() == 0) { //camera
    } else { //screencast
        videoBin->getElementByName("videosrc")->setProperty("screen-num", m_ui.displayNumSpinBox->value());
    }

    return videoBin;
}

void Recorder::start()
{
    QGst::BinPtr audioSrcBin = createAudioSrcBin();
    QGst::BinPtr videoSrcBin = createVideoSrcBin();
    QGst::ElementPtr mux = QGst::ElementFactory::make("oggmux");
    QGst::ElementPtr sink = QGst::ElementFactory::make("filesink");

    if (!audioSrcBin || !videoSrcBin || !mux || !sink) {
        QMessageBox::critical(this, tr("Error"), tr("One or more elements could not be created. "
                              "Verify that you have all the necessary element plugins installed."));
        return;
    }

    sink->setProperty("location", m_ui.outputFileEdit->text());

    m_pipeline = QGst::Pipeline::create();
    m_pipeline->add(audioSrcBin, videoSrcBin, mux, sink);

    //link elements
    QGst::PadPtr audioPad = mux->getRequestPad("audio_%u");

    audioSrcBin->getStaticPad("src")->link(audioPad);

    QGst::PadPtr videoPad = mux->getRequestPad("video_%u");
    videoSrcBin->getStaticPad("src")->link(videoPad);

    mux->link(sink);

    //connect the bus
    m_pipeline->bus()->addSignalWatch();
    QGlib::connect(m_pipeline->bus(), "message", this, &Recorder::onBusMessage);

    //go!
    m_pipeline->setState(QGst::StatePlaying);
    m_ui.startStopButton->setText(tr("Stop recording"));
}

void Recorder::stop()
{
    //stop recording
    m_pipeline->setState(QGst::StateNull);

    //clear the pointer, destroying the pipeline as its reference count drops to zero.
    m_pipeline.clear();

    //restore the button's text
    m_ui.startStopButton->setText(tr("Start recording"));
}

void Recorder::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos:
        //got end-of-stream - stop the pipeline
        stop();
        break;
    case QGst::MessageError:
        //check if the pipeline exists before destroying it,
        //as we might get multiple error messages
        if (m_pipeline) {
            stop();
        }
        QMessageBox::critical(this, tr("Pipeline Error"),
                              message.staticCast<QGst::ErrorMessage>()->error().message());
        break;
    default:
        break;
    }
}

void Recorder::on_startStopButton_clicked()
{
    if (m_pipeline) { //pipeline exists - destroy it
        //send an end-of-stream event to flush metadata and cause an EosMessage to be delivered
        m_pipeline->sendEvent(QGst::EosEvent::create());
    } else { //pipeline doesn't exist - start a new one
        start();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    Recorder r;
    r.show();

    return app.exec();
}

#include "main.moc"
