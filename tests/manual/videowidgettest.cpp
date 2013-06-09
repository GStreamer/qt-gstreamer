/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "ui_videowidgettest.h"
#include <QGst/Init>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QApplication>
#include <QMessageBox>
#include <QButtonGroup>

class VideoWidgetTest : public QWidget
{
    Q_OBJECT
public:
    VideoWidgetTest(QWidget* parent = 0, Qt::WindowFlags f = 0);

private Q_SLOTS:
    void onButtonClicked(int id);
    void on_forceAspectRatioCheckBox_toggled(bool on);

private:
    Ui::VideoWidgetTest m_ui;
    QGst::PipelinePtr m_pipeline;
    QGst::ElementPtr m_src;
    QGst::ElementPtr m_sink;
};

VideoWidgetTest::VideoWidgetTest(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
{
    m_ui.setupUi(this);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(m_ui.playingButton, QGst::StatePlaying);
    group->addButton(m_ui.pausedButton, QGst::StatePaused);
    group->addButton(m_ui.readyButton, QGst::StateReady);
    group->addButton(m_ui.nullButton, QGst::StateNull);
    connect(group, SIGNAL(buttonClicked(int)), SLOT(onButtonClicked(int)));

    m_pipeline = QGst::Pipeline::create();
    m_src = QGst::ElementFactory::make("videotestsrc");

    if (!m_src) {
        throw std::runtime_error("Unable to create a videotestsrc");
    }

    m_pipeline->add(m_src);
}

void VideoWidgetTest::onButtonClicked(int id)
{
    QGst::State currentState;
    m_pipeline->getState(&currentState, NULL, 0);

    if (currentState == QGst::StateNull) {
        if (m_sink) {
            m_ui.videoWidget->releaseVideoSink();;
            m_pipeline->remove(m_sink);
        }

        m_sink = QGst::ElementFactory::make(m_ui.elementNameEdit->text());

        if (!m_sink) {
            QMessageBox::critical(this, "Error", "Could not create the specified sink element");
        } else {
            m_pipeline->add(m_sink);
            m_src->link(m_sink);
            m_ui.videoWidget->setVideoSink(m_sink);
        }
    }

    if (m_sink) {
        QGst::State state = static_cast<QGst::State>(id);
        m_pipeline->setState(state);
    }
}

void VideoWidgetTest::on_forceAspectRatioCheckBox_toggled(bool on)
{
    if (m_sink) {
        m_sink->setProperty("force-aspect-ratio", on);
        m_ui.videoWidget->update();
    }
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    VideoWidgetTest test;
    test.show();

    return app.exec();
}

#include "videowidgettest.moc"
