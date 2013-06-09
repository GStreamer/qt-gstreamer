/*
    Copyright (C) 2010 Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#include "ui_videoorientationtest.h"
#include <QGst/Init>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QGst/VideoOrientation>
#include <QApplication>
#include <QMessageBox>
#include <QCheckBox>

class VideoOrientationTest : public QWidget
{
    Q_OBJECT
public:
    VideoOrientationTest(QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~VideoOrientationTest();

private Q_SLOTS:
    void on_hFlipCheckBox_clicked(bool on);
    void on_vFlipCheckBox_clicked(bool on);

private:
    Ui::VideoOrientationTest m_ui;
    QGst::PipelinePtr m_pipeline;
    QGst::ElementPtr m_src;
    QGst::ElementPtr m_converter;
    QGst::ElementPtr m_sink;
};

VideoOrientationTest::VideoOrientationTest(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    m_ui.setupUi(this);

    m_pipeline = QGst::Pipeline::create();
    m_src = QGst::ElementFactory::make("v4l2src");

    if (!m_src) {
        throw std::runtime_error("Unable to create a v4l2src stream");
    }

    m_pipeline->add(m_src);

    //Insert a ffmpegcolorspace converter element, in case the camera output is not compatible with our sink
    m_converter = QGst::ElementFactory::make("ffmpegcolorspace");
    if (!m_converter) {
        throw std::runtime_error("Unable to create the ffmpegcolorspace converter");
    }

    m_pipeline->add(m_converter);
    m_src->link(m_converter);

    m_sink = QGst::ElementFactory::make("xvimagesink");

    if (!m_sink) {
        throw std::runtime_error("Unable to create the sink element");
    }

    m_pipeline->add(m_sink);
    m_converter->link(m_sink);

    m_ui.videoWidget->setVideoSink(m_sink);

    //Start the pipeline
    m_pipeline->setState(QGst::StatePlaying);
}

VideoOrientationTest::~VideoOrientationTest()
{
    m_pipeline->setState(QGst::StateNull);

    /* When m_pipeline is destructed, the last reference to our pipeline will be gone,
     * and with it all the elements, buses, etc will be destroyed too. As a result,
     * there is no need to cleanup here. */
}

void VideoOrientationTest::on_hFlipCheckBox_clicked(bool checked)
{
    if (m_src) {
        QGst::VideoOrientationPtr orientationinterface = m_src.dynamicCast<QGst::VideoOrientation>();
        if (orientationinterface) {
            bool canFlip = orientationinterface->enableHorizontalFlip(checked);
            if (!canFlip) {
                QMessageBox::warning(this, "Warning", "Source can not flip horizontally");
            }
        }
        m_ui.videoWidget->update();
    }
}

void VideoOrientationTest::on_vFlipCheckBox_clicked(bool checked)
{
    if (m_src) {
        QGst::VideoOrientationPtr orientationinterface = m_src.dynamicCast<QGst::VideoOrientation>();
        if (orientationinterface) {
            bool canFlip = orientationinterface->enableVerticalFlip(checked);
            if (!canFlip) {
                QMessageBox::warning(this, "Warning", "Source can not flip vertically");
            }
        }
        m_ui.videoWidget->update();
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    VideoOrientationTest test;
    test.show();

    return app.exec();
}

#include "videoorientationtest.moc"

