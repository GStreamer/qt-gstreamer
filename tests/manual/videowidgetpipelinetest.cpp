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
#include "ui_videowidgetpipelinetest.h"
#include <QGst/Init>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QApplication>

class VideoWidgetPipelineTest : public QWidget
{
    Q_OBJECT
public:
    VideoWidgetPipelineTest(QWidget* parent = 0, Qt::WindowFlags f = 0);

private Q_SLOTS:
    void on_button_clicked();

private:
    Ui::VideoWidgetPipelineTest m_ui;
    QGst::PipelinePtr m_pipeline;
};

VideoWidgetPipelineTest::VideoWidgetPipelineTest(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    m_ui.setupUi(this);

    m_pipeline = QGst::ElementFactory::make("playbin2").dynamicCast<QGst::Pipeline>();
    if (!m_pipeline) {
        throw std::runtime_error("Unable to create a playbin2 pipeline");
    }

    m_ui.videoWidget->watchPipeline(m_pipeline);
}

void VideoWidgetPipelineTest::on_button_clicked()
{
    if (m_pipeline->currentState() == QGst::StateNull) {
        m_pipeline->setProperty("uri", m_ui.lineEdit->text());
        m_pipeline->setState(QGst::StatePlaying);
    } else {
        m_pipeline->setState(QGst::StateNull);
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    VideoWidgetPipelineTest test;
    test.show();

    return app.exec();
}

#include "videowidgetpipelinetest.moc"
