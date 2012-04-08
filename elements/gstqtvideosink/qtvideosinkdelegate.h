/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef QT_VIDEO_SINK_DELEGATE_H
#define QT_VIDEO_SINK_DELEGATE_H

#include "gstqtvideosinkbase.h"
#include "bufferformat.h"
#include "abstractsurfacepainter.h"

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QSet>
#include <QtCore/QReadWriteLock>

class QGLContext;


class QtVideoSinkDelegate : public QObject
{
    Q_OBJECT
public:
    enum PainterType {
        Generic = 0x00,
        ArbFp = 0x01,
        Glsl = 0x02
    };
    Q_DECLARE_FLAGS(PainterTypes, PainterType);

    enum EventType {
        BufferEventType = QEvent::User,
        DeactivateEventType
    };

    //-------------------------------------

    class BufferEvent : public QEvent
    {
    public:
        inline BufferEvent(GstBuffer *buf, bool formatDirty)
            : QEvent(static_cast<QEvent::Type>(BufferEventType)),
              buffer(gst_buffer_ref(buf)),
              formatDirty(formatDirty)
        {}

        virtual ~BufferEvent() {
            gst_buffer_unref(buffer);
        }

        GstBuffer *buffer;
        bool formatDirty;
    };

    class DeactivateEvent : public QEvent
    {
    public:
        inline DeactivateEvent()
            : QEvent(static_cast<QEvent::Type>(DeactivateEventType))
        {
        }
    };

    //-------------------------------------

    explicit QtVideoSinkDelegate(GstQtVideoSinkBase *sink, QObject *parent = 0);
    virtual ~QtVideoSinkDelegate();

    // API for GstQtVideoSinkBase

    bool isActive() const;
    void setActive(bool playing);

    PainterTypes supportedPainterTypes() const { return m_supportedPainters; }

    // GstColorBalance interface

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

    // pixel-aspect-ratio property
    Fraction pixelAspectRatio() const;
    void setPixelAspectRatio(const Fraction & f);

    // force-aspect-ratio property
    bool forceAspectRatio() const;
    void setForceAspectRatio(bool force);

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    // glcontext property
    QGLContext *glContext() const;
    void setGLContext(QGLContext *context);
#endif

    // paint action
    void paint(QPainter *painter, const QRectF & targetArea);

protected:
    // internal event handling
    virtual bool event(QEvent *event);

    // tells the surface to repaint itself
    virtual void update();

private:
    void changePainter(const BufferFormat & format);
    void destroyPainter();

    AbstractSurfacePainter *m_painter;
    PainterTypes m_supportedPainters;

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    QGLContext *m_glContext;
#endif

    // colorbalance interface properties
    mutable QReadWriteLock m_colorsLock;
    bool m_colorsDirty;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

    // pixel-aspect-ratio property
    mutable QReadWriteLock m_pixelAspectRatioLock;
    Fraction m_pixelAspectRatio;

    // force-aspect-ratio property
    mutable QReadWriteLock m_forceAspectRatioLock;
    bool m_forceAspectRatioDirty;
    bool m_forceAspectRatio;

    // format caching
    bool m_formatDirty;
    BufferFormat m_bufferFormat;
    PaintAreas m_areas;
    QRectF m_clipRect;

    // whether the sink is active (PAUSED or PLAYING)
    mutable QReadWriteLock m_isActiveLock;
    bool m_isActive;

    // the buffer to be drawn next
    GstBuffer *m_buffer;

protected:
    GstQtVideoSinkBase * const m_sink;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QtVideoSinkDelegate::PainterTypes)

#endif // QT_VIDEO_SINK_DELEGATE_H
