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
#include "ui_qwidgetvideosinktest.h"
#include <QGst/Init>
#include <QGst/Pipeline>
#include <QGst/ElementFactory>
#include <QApplication>
#include <QWidget>


class QWidgetVideoSinkTest : public QWidget
{
    Q_OBJECT
public:
    QWidgetVideoSinkTest(QWidget *parent = 0, Qt::WindowFlags f = 0);

private Q_SLOTS:
    void on_leftRadioButton_toggled(bool on);
    void onButtonClicked(int id);

private:
    Ui::QWidgetVideoSinkTest m_ui;
    QGst::PipelinePtr m_pipeline;
    QGst::ElementPtr m_sink;
};

QWidgetVideoSinkTest::QWidgetVideoSinkTest(QWidget *parent, Qt::WindowFlags f)
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
    QGst::ElementPtr src = QGst::ElementFactory::make("videotestsrc");
    m_sink = QGst::ElementFactory::make(QWIDGETVIDEOSINK_NAME);

    if (!src || !m_sink) {
        throw std::runtime_error("Unable to initialize the required elements");
    }

    m_pipeline->add(src, m_sink);
    src->link(m_sink);

    //GValue of G_TYPE_POINTER can only be set as void* in the bindings
    m_sink->setProperty<void*>("widget", m_ui.leftWidget);
}

void QWidgetVideoSinkTest::on_leftRadioButton_toggled(bool on)
{
    m_sink->setProperty<void*>("widget", on ? m_ui.leftWidget : m_ui.rightWidget);
}

void QWidgetVideoSinkTest::onButtonClicked(int id)
{
    QGst::State state = static_cast<QGst::State>(id);
    m_pipeline->setState(state);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    QGst::init(&argc, &argv);

    QWidgetVideoSinkTest test;
    test.show();

    return app.exec();
}

#include "qwidgetvideosinktest.moc"
