/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GST_QT_VIDEO_SINK_SURFACE_H
#define GST_QT_VIDEO_SINK_SURFACE_H

#include "gstqtvideosink.h"
#include "bufferformat.h"
#include "abstractsurfacepainter.h"

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QSet>
#include <QtCore/QReadWriteLock>

class QGLContext;


class GstQtVideoSinkSurface : public QObject
{
    Q_OBJECT
public:
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
        {
        }

        GstBuffer *buffer;
        bool formatDirty;
    };

    //-------------------------------------

    class DeactivateEvent : public QEvent
    {
    public:
        inline DeactivateEvent()
            : QEvent(static_cast<QEvent::Type>(DeactivateEventType))
        {
        }
    };

    //-------------------------------------

    explicit GstQtVideoSinkSurface(GstQtVideoSink *sink, QObject *parent = 0);
    ~GstQtVideoSinkSurface();

    // API for GstQtVideoSink

    QSet<BufferFormat::PixelFormat> supportedPixelFormats() const;

    bool isActive() const;
    void setActive(bool playing);

    // GstColorBalance interface

    int brightness() const;
    void setBrightness(int brightness);

    int contrast() const;
    void setContrast(int contrast);

    int hue() const;
    void setHue(int hue);

    int saturation() const;
    void setSaturation(int saturation);

    // force-aspect-ratio property

    bool forceAspectRatio() const;
    void setForceAspectRatio(bool force);

    // glcontext property

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    QGLContext *glContext() const;
    void setGLContext(QGLContext *context);

    enum ShaderType
    {
        NoShaders = 0x00,
        FragmentProgramShader = 0x01,
        GlslShader = 0x02
    };

    Q_DECLARE_FLAGS(ShaderTypes, ShaderType)
#endif

    // paint action signal

    void paint(QPainter *painter, int x, int y, int width, int height);

protected:
    bool event(QEvent *event);

private:
    void changePainter(const BufferFormat & format);
    void destroyPainter();


    AbstractSurfacePainter *m_painter;

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    QGLContext *m_glContext;
    ShaderTypes m_supportedShaderTypes;
    ShaderType m_shaderType;
#endif

    // colorbalance interface properties
    mutable QReadWriteLock m_colorsLock;
    bool m_colorsDirty;
    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

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

    GstQtVideoSink *m_sink;
};

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
Q_DECLARE_OPERATORS_FOR_FLAGS(GstQtVideoSinkSurface::ShaderTypes)
#endif

#endif // GST_QT_VIDEO_SINK_SURFACE_H
