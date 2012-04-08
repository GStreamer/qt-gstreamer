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

#include "qtvideosinkdelegate.h"
#include "genericsurfacepainter.h"
#include "openglsurfacepainter.h"
#include "gstqtvideosink.h"
#include "gstqtglvideosink.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStack>
#include <QtGui/QPainter>

#define QSIZE_FORMAT "(%d x %d)"
#define QSIZE_FORMAT_ARGS(size) \
    size.width(), size.height()
#define QRECTF_FORMAT "(x: %f, y: %f, w: %f, h: %f)"
#define QRECTF_FORMAT_ARGS(rect) \
    (float) rect.x(), (float) rect.y(), (float) rect.width(), (float) rect.height()


QtVideoSinkDelegate::QtVideoSinkDelegate(GstQtVideoSinkBase *sink, QObject *parent)
    : QObject(parent)
    , m_painter(0)
    , m_supportedPainters(Generic)
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    , m_glContext(0)
#endif
    , m_colorsDirty(true)
    , m_brightness(0)
    , m_contrast(0)
    , m_hue(0)
    , m_saturation(0)
    , m_pixelAspectRatio(1, 1)
    , m_forceAspectRatioDirty(true)
    , m_forceAspectRatio(false)
    , m_formatDirty(true)
    , m_isActive(false)
    , m_buffer(NULL)
    , m_sink(sink)
{
}

QtVideoSinkDelegate::~QtVideoSinkDelegate()
{
    Q_ASSERT(!isActive());
    destroyPainter();
}

//-------------------------------------

bool QtVideoSinkDelegate::isActive() const
{
    QReadLocker l(&m_isActiveLock);
    return m_isActive;
}

void QtVideoSinkDelegate::setActive(bool active)
{
    GST_INFO_OBJECT(m_sink, active ? "Activating" : "Deactivating");

    QWriteLocker l(&m_isActiveLock);
    m_isActive = active;
    if (!active) {
        QCoreApplication::postEvent(this, new DeactivateEvent());
    }
}

//-------------------------------------

int QtVideoSinkDelegate::brightness() const
{
    QReadLocker l(&m_colorsLock);
    return m_brightness;
}

void QtVideoSinkDelegate::setBrightness(int brightness)
{
    QWriteLocker l(&m_colorsLock);
    m_brightness = qBound(-100, brightness, 100);
    m_colorsDirty = true;
}

int QtVideoSinkDelegate::contrast() const
{
    QReadLocker l(&m_colorsLock);
    return m_contrast;
}

void QtVideoSinkDelegate::setContrast(int contrast)
{
    QWriteLocker l(&m_colorsLock);
    m_contrast = qBound(-100, contrast, 100);
    m_colorsDirty = true;
}

int QtVideoSinkDelegate::hue() const
{
    QReadLocker l(&m_colorsLock);
    return m_hue;
}

void QtVideoSinkDelegate::setHue(int hue)
{
    QWriteLocker l(&m_colorsLock);
    m_hue = qBound(-100, hue, 100);
    m_colorsDirty = true;
}

int QtVideoSinkDelegate::saturation() const
{
    QReadLocker l(&m_colorsLock);
    return m_saturation;
}

void QtVideoSinkDelegate::setSaturation(int saturation)
{
    QWriteLocker l(&m_colorsLock);
    m_saturation = qBound(-100, saturation, 100);
    m_colorsDirty = true;
}

//-------------------------------------

Fraction QtVideoSinkDelegate::pixelAspectRatio() const
{
    QReadLocker l(&m_pixelAspectRatioLock);
    return m_pixelAspectRatio;
}

void QtVideoSinkDelegate::setPixelAspectRatio(const Fraction & f)
{
    QWriteLocker l(&m_pixelAspectRatioLock);
    m_pixelAspectRatio = f;
}

//-------------------------------------

bool QtVideoSinkDelegate::forceAspectRatio() const
{
    QReadLocker l(&m_forceAspectRatioLock);
    return m_forceAspectRatio;
}

void QtVideoSinkDelegate::setForceAspectRatio(bool force)
{
    QWriteLocker l(&m_forceAspectRatioLock);
    if (m_forceAspectRatio != force) {
        m_forceAspectRatio = force;
        m_forceAspectRatioDirty = true;
    }
}

//-------------------------------------

void QtVideoSinkDelegate::paint(QPainter *painter, const QRectF & targetArea)
{
    GST_TRACE_OBJECT(m_sink, "paint called");

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    if (m_glContext) {
        Q_ASSERT_X(m_glContext == QGLContext::currentContext(),
            "qtvideosink - paint",
            "Please use a QPainter that is initialized to paint on the "
            "GL surface that has the same context as the one given on the glcontext property"
        );
    }
#endif

    if (!m_buffer) {
        painter->fillRect(targetArea, Qt::black);
    } else {
        BufferFormat format = m_formatDirty ?
                BufferFormat::fromCaps(GST_BUFFER_CAPS(m_buffer)) : m_bufferFormat;

        //recalculate the video area if needed
        QReadLocker forceAspectRatioLocker(&m_forceAspectRatioLock);
        if (targetArea != m_areas.targetArea
             || (m_formatDirty && (format.frameSize() != m_bufferFormat.frameSize()
                               || format.pixelAspectRatio() != m_bufferFormat.pixelAspectRatio()))
             || m_forceAspectRatioDirty)
        {
            m_forceAspectRatioDirty = false;

            if (m_forceAspectRatio) {
                QReadLocker pixelAspectRatioLocker(&m_pixelAspectRatioLock);
                m_areas.calculate(targetArea, format.frameSize(),
                                  format.pixelAspectRatio(), m_pixelAspectRatio);
            } else {
                m_areas.targetArea = targetArea;
                m_areas.videoArea = targetArea;
                m_areas.blackArea1 = m_areas.blackArea2 = QRectF();
            }

            GST_LOG_OBJECT(m_sink,
                "Recalculated paint areas: "
                "Frame size: " QSIZE_FORMAT ", "
                "target area: " QRECTF_FORMAT ", "
                "video area: " QRECTF_FORMAT ", "
                "black1: " QRECTF_FORMAT ", "
                "black2: " QRECTF_FORMAT,
                QSIZE_FORMAT_ARGS(format.frameSize()),
                QRECTF_FORMAT_ARGS(m_areas.targetArea),
                QRECTF_FORMAT_ARGS(m_areas.videoArea),
                QRECTF_FORMAT_ARGS(m_areas.blackArea1),
                QRECTF_FORMAT_ARGS(m_areas.blackArea2)
            );
        }
        forceAspectRatioLocker.unlock();

        if (m_formatDirty /* || m_clipRectDirty */) {
            //TODO add properties for modifying clipRect
            m_clipRect = QRectF(QPointF(0,0), format.frameSize());
        }

        //if either pixelFormat or frameSize have changed, we need to reset the painter
        //and/or change painter, in case the current one does not handle the requested format
        if ((m_formatDirty && (format.videoFormat() != m_bufferFormat.videoFormat()
                || format.colorMatrix() != m_bufferFormat.colorMatrix()
                || format.frameSize() != m_bufferFormat.frameSize()))
            || !m_painter)
        {
            changePainter(format);

            m_bufferFormat = format;
            m_formatDirty = false;

            //make sure to update the colors after changing painter
            m_colorsDirty = true;
        }

        if (G_LIKELY(m_painter)) {
            QReadLocker colorsLocker(&m_colorsLock);
            if (m_colorsDirty) {
                m_painter->updateColors(m_brightness, m_contrast, m_hue, m_saturation);
                m_colorsDirty = false;
            }
            colorsLocker.unlock();

            m_painter->paint(m_buffer->data, m_bufferFormat, m_clipRect, painter, m_areas);
        }
    }
}

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL

QGLContext *QtVideoSinkDelegate::glContext() const
{
    return m_glContext;
}

void QtVideoSinkDelegate::setGLContext(QGLContext *context)
{
    if (m_glContext == context)
        return;

    m_glContext = context;
    m_supportedPainters = Generic;

    if (m_glContext) {
        m_glContext->makeCurrent();

        const QByteArray extensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        GST_LOG_OBJECT(m_sink, "Available GL extensions: %s", extensions.constData());

#ifndef QT_OPENGL_ES
        if (extensions.contains("ARB_fragment_program"))
            m_supportedPainters |= ArbFp;
#endif

#ifndef QT_OPENGL_ES_2
        if (QGLShaderProgram::hasOpenGLShaderPrograms(m_glContext)
                && extensions.contains("ARB_shader_objects"))
#endif
            m_supportedPainters |= Glsl;
    }

    GST_LOG_OBJECT(m_sink, "Done setting GL context. m_supportedPainters=%x", (int) m_supportedPainters);
}

#endif

void QtVideoSinkDelegate::changePainter(const BufferFormat & format)
{
    if (m_painter) {
        m_painter->cleanup();
        if (G_UNLIKELY(!m_painter->supportsFormat(format.videoFormat()))) {
            destroyPainter();
        }
    }

    QStack<PainterType> possiblePainters;
    if (GenericSurfacePainter::supportedPixelFormats().contains(format.videoFormat())) {
        possiblePainters.push(Generic);
    }

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    if (OpenGLSurfacePainter::supportedPixelFormats().contains(format.videoFormat())) {
        if (m_supportedPainters & ArbFp) {
            possiblePainters.push(ArbFp);
        }

        if (m_supportedPainters & Glsl) {
            possiblePainters.push(Glsl);
        }
    }
#endif

    while (!possiblePainters.isEmpty()) {
        if (!m_painter) {
            PainterType type = possiblePainters.pop();
            switch(type) {
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
            case Glsl:
                GST_LOG_OBJECT(m_sink, "Creating GLSL painter");
                m_painter = new GlslSurfacePainter;
                break;
# ifndef QT_OPENGL_ES
            case ArbFp:
                GST_LOG_OBJECT(m_sink, "Creating ARB Fragment Shader painter");
                m_painter = new ArbFpSurfacePainter;
                break;
# endif
#endif
            case Generic:
                GST_LOG_OBJECT(m_sink, "Creating Generic painter");
                m_painter = new GenericSurfacePainter;
                break;
            default:
                Q_ASSERT(false);
            }
        }

        try {
            m_painter->init(format);
            return;
        } catch (const QString & error) {
            GST_ELEMENT_WARNING(m_sink, RESOURCE, FAILED,
                    ("Failed to start painter"), ("%s", error.toUtf8().constData()));
            delete m_painter;
            m_painter = 0;
        }
    }

    GST_ELEMENT_ERROR(m_sink, RESOURCE, FAILED,
            ("Failed to create a painter for the given format"), (NULL));
}

void QtVideoSinkDelegate::destroyPainter()
{
    GST_LOG_OBJECT(m_sink, "Destroying painter");

    delete m_painter;
    m_painter = 0;
}

bool QtVideoSinkDelegate::event(QEvent *event)
{
    switch((int) event->type()) {
    case BufferEventType:
    {
        BufferEvent *bufEvent = dynamic_cast<BufferEvent*>(event);
        Q_ASSERT(bufEvent);

        GST_TRACE_OBJECT(m_sink, "Received buffer %"GST_PTR_FORMAT, bufEvent->buffer);

        if (m_buffer) {
            //free the previous buffer
            gst_buffer_unref(m_buffer);
            m_buffer = NULL;
        }

        if (isActive()) {
            //schedule this frame for rendering
            m_buffer = gst_buffer_ref(bufEvent->buffer);
            if (bufEvent->formatDirty) {
                m_formatDirty = true;
            }
            update();
        }

        return true;
    }
    case DeactivateEventType:
    {
        GST_LOG_OBJECT(m_sink, "Received deactivate event");

        if (m_buffer) {
            gst_buffer_unref(m_buffer);
            m_buffer = NULL;
        }

        if (m_painter) {
            m_painter->cleanup();
            destroyPainter();
        }

        update();

        return true;
    }
    default:
        return QObject::event(event);
    }
}

void QtVideoSinkDelegate::update()
{
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    if (G_TYPE_CHECK_INSTANCE_TYPE(m_sink, GST_TYPE_QT_GL_VIDEO_SINK)) {
        GstQtGLVideoSink::emit_update(m_sink);
    } else
#endif
    if (G_TYPE_CHECK_INSTANCE_TYPE(m_sink, GST_TYPE_QT_VIDEO_SINK)) {
        GstQtVideoSink::emit_update(m_sink);
    } else {
        Q_ASSERT(false);
    }
}
