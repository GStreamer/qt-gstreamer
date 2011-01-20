/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2011 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include "videowidget.h"
#include "../xoverlay.h"
#include "../pipeline.h"
#include "../bus.h"
#include "../message.h"
#include "../../QGlib/connect.h"
#include <QtCore/QDebug>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtGui/QPainter>
#include <QtGui/QPaintEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QApplication>

namespace QGst {
namespace Ui {

class AbstractRenderer
{
public:
    static AbstractRenderer *create(const ElementPtr & sink, QWidget *videoWidget);

    virtual ~AbstractRenderer() {}
    virtual ElementPtr videoSink() const = 0;
};


class XOverlayRenderer : public QObject, public AbstractRenderer
{
public:
    XOverlayRenderer(QWidget *parent)
        : QObject(parent)
    {
        m_windowId = widget()->winId(); //create a new X window (if we are on X11 with alien widgets)
        QApplication::syncX(); //inform other applications about the new window (on X11)

        widget()->installEventFilter(this);
        widget()->setAttribute(Qt::WA_NoSystemBackground, true);
        widget()->setAttribute(Qt::WA_PaintOnScreen, true);
        widget()->update();
    }

    virtual ~XOverlayRenderer()
    {
        if (m_sink) {
            m_sink->setWindowHandle(0);
        }
        widget()->removeEventFilter(this);
        widget()->setAttribute(Qt::WA_NoSystemBackground, false);
        widget()->setAttribute(Qt::WA_PaintOnScreen, false);
        widget()->update();
    }

    void setVideoSink(const XOverlayPtr & sink)
    {
        QMutexLocker l(&m_sinkMutex);
        if (m_sink) {
            m_sink->setWindowHandle(0);
        }
        m_sink = sink;
        if (m_sink) {
            m_sink->setWindowHandle(m_windowId);
        }
    }

    virtual ElementPtr videoSink() const
    {
        QMutexLocker l(&m_sinkMutex);
        return m_sink.dynamicCast<Element>();
    }

protected:
    virtual bool eventFilter(QObject *filteredObject, QEvent *event)
    {
        if (filteredObject == parent() && event->type() == QEvent::Paint) {
            QMutexLocker l(&m_sinkMutex);
            State currentState = m_sink ? m_sink.dynamicCast<Element>()->currentState() : StateNull;

            if (currentState == StatePlaying || currentState == StatePaused) {
                m_sink->expose();
            } else {
                QPainter p(widget());
                p.fillRect(widget()->rect(), Qt::black);
            }
            return true;
        } else {
            return QObject::eventFilter(filteredObject, event);
        }
    }

private:
    inline QWidget *widget() { return static_cast<QWidget*>(parent()); }
    WId m_windowId;
    mutable QMutex m_sinkMutex;
    XOverlayPtr m_sink;
};


class QWidgetVideoSinkRenderer : public AbstractRenderer
{
public:
    QWidgetVideoSinkRenderer(const ElementPtr & sink, QWidget *parent)
        : m_sink(sink)
    {
        //GValue of G_TYPE_POINTER can only be set as void* in the bindings
        m_sink->setProperty<void*>("widget", parent);
    }

    virtual ~QWidgetVideoSinkRenderer()
    {
        m_sink->setProperty<void*>("widget", NULL);
    }

    virtual ElementPtr videoSink() const { return m_sink; }

private:
    ElementPtr m_sink;
};


class PipelineWatch : public QObject, public AbstractRenderer
{
public:
    PipelineWatch(const PipelinePtr & pipeline, QWidget *parent)
        : QObject(parent), m_renderer(new XOverlayRenderer(parent)), m_pipeline(pipeline)
    {
        pipeline->bus()->enableSyncMessageEmission();
        QGlib::connect(pipeline->bus(), "sync-message",
                       this, &PipelineWatch::onBusSyncMessage);
    }

    virtual ~PipelineWatch()
    {
        m_pipeline->bus()->disableSyncMessageEmission();
        delete m_renderer;
    }

    virtual ElementPtr videoSink() const { return m_renderer->videoSink(); }

    void releaseSink() { m_renderer->setVideoSink(XOverlayPtr()); }

private:
    void onBusSyncMessage(const MessagePtr & msg)
    {
        switch (msg->type()) {
        case MessageElement:
            if (msg->internalStructure()->name() == QLatin1String("prepare-xwindow-id")) {
                XOverlayPtr overlay = msg->source().dynamicCast<XOverlay>();
                m_renderer->setVideoSink(overlay);
            }
            break;
        case MessageStateChanged:
            //release the sink when it goes back to null state
            if (msg.staticCast<StateChangedMessage>()->newState() == StateNull &&
                msg->source() == m_renderer->videoSink())
            {
                releaseSink();
            }
        default:
            break;
        }
    }

private:
    XOverlayRenderer *m_renderer;
    PipelinePtr m_pipeline;
};


AbstractRenderer *AbstractRenderer::create(const ElementPtr & sink, QWidget *videoWidget)
{
    XOverlayPtr overlay = sink.dynamicCast<XOverlay>();
    if (overlay) {
        XOverlayRenderer *r = new XOverlayRenderer(videoWidget);
        r->setVideoSink(overlay);
        return r;
    }

    if (QGlib::Type::fromInstance(sink).name() == QLatin1String("GstQWidgetVideoSink")) {
        return new QWidgetVideoSinkRenderer(sink, videoWidget);
    }

    return NULL;
}


VideoWidget::VideoWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f), d(NULL)
{
}

VideoWidget::~VideoWidget()
{
    delete d;
}

ElementPtr VideoWidget::videoSink() const
{
    return d ? d->videoSink() : ElementPtr();
}

void VideoWidget::setVideoSink(const ElementPtr & sink)
{
    if (!sink) {
        releaseVideoSink();
        return;
    }

    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
    Q_ASSERT(d == NULL);

    d = AbstractRenderer::create(sink, this);

    if (!d) {
        qCritical() << "QGst::Ui::VideoWidget: Could not construct a renderer for the specified element";
    }
}

void VideoWidget::releaseVideoSink()
{
    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());

    if (d) {
        PipelineWatch *pw = dynamic_cast<PipelineWatch*>(d);
        if (pw) {
            pw->releaseSink();
        } else {
            delete d;
            d = NULL;
        }
    }
}

void VideoWidget::watchPipeline(const PipelinePtr & pipeline)
{
    if (!pipeline) {
        stopPipelineWatch();
        return;
    }

    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());
    Q_ASSERT(d == NULL);

    d = new PipelineWatch(pipeline, this);
}

void VideoWidget::stopPipelineWatch()
{
    Q_ASSERT(QThread::currentThread() == QApplication::instance()->thread());

    if (dynamic_cast<PipelineWatch*>(d)) {
        delete d;
        d = NULL;
    }
}

void VideoWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(event->rect(), Qt::black);
}

} //namespace Ui
} //namespace QGst
