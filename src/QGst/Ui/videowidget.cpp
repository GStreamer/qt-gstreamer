/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "../childproxy.h"
#include "../../QGlib/signal.h"
#include <QtCore/QDebug>
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
    XOverlayRenderer(const XOverlayPtr & sink, QWidget *parent)
        : QObject(parent), m_sink(sink)
    {
        WId windowId = widget()->winId(); //create a new X window (if we are on X11 with alien widgets)
        QApplication::syncX(); //inform other applications about the new window (on X11)
        m_sink->setWindowId(static_cast<ulong>(windowId)); //### cast needs checking on non-X11 platforms

        widget()->installEventFilter(this);
        widget()->setAttribute(Qt::WA_NoSystemBackground, true);
        widget()->setAttribute(Qt::WA_PaintOnScreen, true);
        widget()->update();
    }

    virtual ~XOverlayRenderer()
    {
        m_sink->setWindowId(0);
        widget()->removeEventFilter(this);
        widget()->setAttribute(Qt::WA_NoSystemBackground, false);
        widget()->setAttribute(Qt::WA_PaintOnScreen, false);
        widget()->update();
    }

    virtual ElementPtr videoSink() const { return m_sink; }

    virtual bool eventFilter(QObject *filteredObject, QEvent *event)
    {
        if (filteredObject == parent() && event->type() == QEvent::Paint) {
            State currentState;
            m_sink->getState(&currentState, NULL, 0);
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


class ProxyRenderer : public AbstractRenderer
{
public:
    ProxyRenderer(const ChildProxyPtr & sink, QWidget *parent)
        : m_renderer(NULL), m_parent(parent), m_sink(sink)
    {
        for (uint i=0; m_renderer == NULL && i<sink->childrenCount(); i++) {
            childAdded(sink->childByIndex(i));
        }

        m_childAddedHandler = QGlib::Signal::connect(sink, "child-added",
                                                     this, &ProxyRenderer::childAdded);
        m_childRemovedHandler = QGlib::Signal::connect(sink, "child-removed",
                                                       this, &ProxyRenderer::childRemoved);
    }

    virtual ~ProxyRenderer()
    {
        m_childAddedHandler.disconnect();
        m_childRemovedHandler.disconnect();
    }

    virtual ElementPtr videoSink() const { return m_sink.dynamicCast<Element>(); }

    void childAdded(const QGlib::ObjectPtr & child)
    {
        if (!m_renderer) {
            ElementPtr element = child.dynamicCast<Element>();
            if (element) {
                m_renderer = AbstractRenderer::create(element, m_parent);
            }
        }
    }

    void childRemoved(const QGlib::ObjectPtr & child)
    {
        if (m_renderer && m_renderer->videoSink() == child) {
            delete m_renderer;
            m_renderer = NULL;
        }
    }

private:
    AbstractRenderer *m_renderer;
    QWidget *m_parent;
    ChildProxyPtr m_sink;
    QGlib::SignalHandler m_childAddedHandler;
    QGlib::SignalHandler m_childRemovedHandler;
};


AbstractRenderer *AbstractRenderer::create(const ElementPtr & sink, QWidget *videoWidget)
{
#if !defined(Q_WS_QWS)
    XOverlayPtr overlay = sink.dynamicCast<XOverlay>();
    if (overlay) {
        return new XOverlayRenderer(overlay, videoWidget);
    }
#endif

    if (QGlib::Type::fromInstance(sink).name() == QLatin1String("GstQWidgetVideoSink")) {
        return new QWidgetVideoSinkRenderer(sink, videoWidget);
    }

    ChildProxyPtr childProxy = sink.dynamicCast<ChildProxy>();
    if (childProxy) {
        return new ProxyRenderer(childProxy, videoWidget);
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
    delete d;
    d = NULL;

    if (sink) {
        d = AbstractRenderer::create(sink, this);

        if (!d) {
            qCritical() << "QGst::Ui::VideoWidget: Could not construct a renderer for the specified element";
        }
    }
}

} //namespace Ui
} //namespace QGst
