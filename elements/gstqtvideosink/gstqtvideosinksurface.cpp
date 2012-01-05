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

#include "gstqtvideosinksurface.h"
#include "genericsurfacepainter.h"
#include "openglsurfacepainter.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStack>
#include <QtGui/QPainter>

#define QSIZE_FORMAT "(%d x %d)"
#define QSIZE_FORMAT_ARGS(size) \
    size.width(), size.height()
#define QRECTF_FORMAT "(x: %f, y: %f, w: %f, h: %f)"
#define QRECTF_FORMAT_ARGS(rect) \
    (float) rect.x(), (float) rect.y(), (float) rect.width(), (float) rect.height()


GstQtVideoSinkSurface::GstQtVideoSinkSurface(GstQtVideoSink *sink, QObject *parent)
    : QObject(parent)
    , m_painter(0)
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    , m_glContext(0)
    , m_supportedShaderTypes(NoShaders)
#endif
    , m_colorsDirty(true)
    , m_brightness(0)
    , m_contrast(0)
    , m_hue(0)
    , m_saturation(0)
    , m_forceAspectRatioDirty(true)
    , m_forceAspectRatio(false)
    , m_formatDirty(true)
    , m_isActive(false)
    , m_buffer(NULL)
    , m_sink(sink)
{
}

GstQtVideoSinkSurface::~GstQtVideoSinkSurface()
{
    Q_ASSERT(!isActive());
    destroyPainter();
}

//-------------------------------------

QSet<BufferFormat::PixelFormat> GstQtVideoSinkSurface::supportedPixelFormats() const
{
    QSet<BufferFormat::PixelFormat> result;
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    if (m_glContext) {
        result += GlslSurfacePainter::supportedPixelFormats();
        result += ArbFpSurfacePainter::supportedPixelFormats();
    }
#endif
    result += GenericSurfacePainter::supportedPixelFormats();
    return result;
}

bool GstQtVideoSinkSurface::isActive() const
{
    QReadLocker l(&m_isActiveLock);
    return m_isActive;
}

void GstQtVideoSinkSurface::setActive(bool active)
{
    GST_INFO_OBJECT(m_sink, active ? "Activating" : "Deactivating");

    QWriteLocker l(&m_isActiveLock);
    m_isActive = active;
    if (!active) {
        QCoreApplication::postEvent(this, new DeactivateEvent());
    }
}

//-------------------------------------

int GstQtVideoSinkSurface::brightness() const
{
    QReadLocker l(&m_colorsLock);
    return m_brightness;
}

void GstQtVideoSinkSurface::setBrightness(int brightness)
{
    QWriteLocker l(&m_colorsLock);
    m_brightness = brightness;
    m_colorsDirty = true;
}

int GstQtVideoSinkSurface::contrast() const
{
    QReadLocker l(&m_colorsLock);
    return m_contrast;
}

void GstQtVideoSinkSurface::setContrast(int contrast)
{
    QWriteLocker l(&m_colorsLock);
    m_contrast = contrast;
    m_colorsDirty = true;
}

int GstQtVideoSinkSurface::hue() const
{
    QReadLocker l(&m_colorsLock);
    return m_hue;
}

void GstQtVideoSinkSurface::setHue(int hue)
{
    QWriteLocker l(&m_colorsLock);
    m_hue = hue;
    m_colorsDirty = true;
}

int GstQtVideoSinkSurface::saturation() const
{
    QReadLocker l(&m_colorsLock);
    return m_saturation;
}

void GstQtVideoSinkSurface::setSaturation(int saturation)
{
    QWriteLocker l(&m_colorsLock);
    m_saturation = saturation;
    m_colorsDirty = true;
}

//-------------------------------------

bool GstQtVideoSinkSurface::forceAspectRatio() const
{
    QReadLocker l(&m_forceAspectRatioLock);
    return m_forceAspectRatio;
}

void GstQtVideoSinkSurface::setForceAspectRatio(bool force)
{
    QWriteLocker l(&m_forceAspectRatioLock);
    if (m_forceAspectRatio != force) {
        m_forceAspectRatio = force;
        m_forceAspectRatioDirty = true;
    }
}

//-------------------------------------

/* Modified version of gst_video_sink_center_rect */
static QRectF centerRect(const QRectF & src, const QRectF & dst)
{
    QRectF result;
    qreal srcRatio = src.width() / src.height();
    qreal dstRatio = dst.width() / dst.height();

    if (srcRatio > dstRatio) {
        result.setWidth(dst.width());
        result.setHeight(dst.width() / srcRatio);
        result.moveTop((dst.height() - result.height()) / 2);
    } else if (srcRatio < dstRatio) {
        result.setWidth(dst.height() * srcRatio);
        result.setHeight(dst.height());
        result.moveLeft((dst.width() - result.width()) / 2);
    } else {
        result = dst;
    }

    return result;
}

void GstQtVideoSinkSurface::paint(QPainter *painter, qreal x, qreal y, qreal width, qreal height)
{
    QRectF targetArea(x, y, width, height);

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
            m_areas.targetArea = targetArea;
            m_forceAspectRatioDirty = false;

            if (m_forceAspectRatio) {
                qreal aspectRatio = (qreal) format.pixelAspectRatio().width() / format.pixelAspectRatio().height();
                qreal aspectRatioInv = (qreal) format.pixelAspectRatio().height() / format.pixelAspectRatio().width();

                QRectF srcRect(QPointF(0,0), QSizeF(format.frameSize().width() * aspectRatio,
                                                    format.frameSize().height() * aspectRatioInv));
                QRectF destRect(QPointF(0,0), m_areas.targetArea.size());

                m_areas.videoArea = centerRect(srcRect, destRect);

                m_areas.blackArea1 = QRectF(
                    m_areas.targetArea.left(),
                    m_areas.targetArea.top(),
                    m_areas.videoArea.left() == m_areas.targetArea.left() ?
                        m_areas.targetArea.width() : m_areas.videoArea.left() - m_areas.targetArea.left(),
                    m_areas.videoArea.top() == m_areas.targetArea.top() ?
                        m_areas.targetArea.height() : m_areas.videoArea.top() - m_areas.targetArea.top()
                );

                m_areas.blackArea2 = QRectF(
                    m_areas.videoArea.right() == m_areas.targetArea.right() ?
                        m_areas.targetArea.left() : m_areas.videoArea.right() + 1,
                    m_areas.videoArea.bottom() == m_areas.targetArea.bottom() ?
                        m_areas.targetArea.top() : m_areas.videoArea.bottom() + 1,
                    m_areas.videoArea.right() == m_areas.targetArea.right() ?
                        m_areas.targetArea.width() : m_areas.targetArea.right() - m_areas.videoArea.right(),
                    m_areas.videoArea.bottom() == m_areas.targetArea.bottom() ?
                        m_areas.targetArea.height() : m_areas.targetArea.bottom() - m_areas.videoArea.bottom()
                );
            } else {
                m_areas.videoArea = m_areas.targetArea;
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

        if (m_formatDirty || !m_painter) {
            //if either pixelFormat or frameSize have changed, we need to reset the painter
            //and/or change painter, in case the current one does not handle the requested format
            if (format.pixelFormat() != m_bufferFormat.pixelFormat()
                || format.frameSize() != m_bufferFormat.frameSize()
                || !m_painter)
            {
                changePainter(format);
            }

            //if we have a different colorspace, we need to update the colors
            if (format.yCbCrColorSpace() != m_bufferFormat.yCbCrColorSpace()) {
                QReadLocker l(&m_colorsLock);
                m_colorsDirty = true;
            }

            m_bufferFormat = format;
            m_formatDirty = false;
        }

        if (G_LIKELY(m_painter)) {
            QReadLocker colorsLocker(&m_colorsLock);
            if (m_colorsDirty) {
                m_painter->updateColors(m_brightness, m_contrast, m_hue, m_saturation,
                                        m_bufferFormat.yCbCrColorSpace());
                m_colorsDirty = false;
            }
            colorsLocker.unlock();

            m_painter->paint(m_buffer->data, m_bufferFormat, m_clipRect, painter, m_areas);
        }
    }
}

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL

QGLContext *GstQtVideoSinkSurface::glContext() const
{
    return m_glContext;
}

void GstQtVideoSinkSurface::setGLContext(QGLContext *context)
{
    GST_LOG_OBJECT(m_sink, "Setting GL context. context=%p m_glContext=%p m_painter=%p",
                   context, m_glContext, m_painter);

    if (m_glContext == context)
        return;

    m_glContext = context;

    m_supportedShaderTypes = NoShaders;

    if (m_glContext) {
        m_glContext->makeCurrent();

        const QByteArray extensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        GST_LOG_OBJECT(m_sink, "Available GL extensions: %s", extensions.constData());

#ifndef QT_OPENGL_ES
        if (extensions.contains("ARB_fragment_program"))
            m_supportedShaderTypes |= FragmentProgramShader;
#endif

#ifndef QT_OPENGL_ES_2
        if (QGLShaderProgram::hasOpenGLShaderPrograms(m_glContext)
                && extensions.contains("ARB_shader_objects"))
#endif
            m_supportedShaderTypes |= GlslShader;

    }

    GST_LOG_OBJECT(m_sink, "Done setting GL context. m_shaderTypes=%x", (int) m_supportedShaderTypes);
}

#endif

enum PainterType { Glsl, ArbFp, Generic };

void GstQtVideoSinkSurface::changePainter(const BufferFormat & format)
{
    if (m_painter) {
        m_painter->cleanup();
        if (!m_painter->supportsFormat(format.pixelFormat())) {
            delete m_painter;
            m_painter = 0;
        }
    }

    QStack<PainterType> possiblePainters;
    if (GenericSurfacePainter::supportedPixelFormats().contains(format.pixelFormat())) {
        possiblePainters.push(Generic);
    }

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    if (m_supportedShaderTypes & GstQtVideoSinkSurface::FragmentProgramShader
        && ArbFpSurfacePainter::supportedPixelFormats().contains(format.pixelFormat())) {
        possiblePainters.push(ArbFp);
    }

    if (m_supportedShaderTypes & GstQtVideoSinkSurface::GlslShader
        && GlslSurfacePainter::supportedPixelFormats().contains(format.pixelFormat())) {
        possiblePainters.push(Glsl);
    }
#endif

    while (!possiblePainters.isEmpty()) {
        if (!m_painter) {
            PainterType type = possiblePainters.pop();
            switch(type) {
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
            case Glsl:
                GST_LOG_OBJECT(m_sink, "Creating GLSL painter");
                m_painter = new GlslSurfacePainter(m_glContext);
                break;
            case ArbFp:
                GST_LOG_OBJECT(m_sink, "Creating ARB Fragment Shader painter");
                m_painter = new ArbFpSurfacePainter(m_glContext);
                break;
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
            GST_WARNING_OBJECT(m_sink, "Failed to start painter: %s", error.toUtf8().constData());
            delete m_painter;
            m_painter = 0;
        }
    }

    GST_ERROR_OBJECT(m_sink, "Failed to create a painter for the given format");
}

void GstQtVideoSinkSurface::destroyPainter()
{
    GST_LOG_OBJECT(m_sink, "Destroying painter");

    delete m_painter;
    m_painter = 0;
}

bool GstQtVideoSinkSurface::event(QEvent *event)
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
            m_buffer = bufEvent->buffer;
            m_formatDirty = bufEvent->formatDirty;
            g_signal_emit(m_sink, GstQtVideoSink::s_signals[GstQtVideoSink::UPDATE_SIGNAL], 0);
        } else {
            //not active, drop the frame
            gst_buffer_unref(bufEvent->buffer);
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

        g_signal_emit(m_sink, GstQtVideoSink::s_signals[GstQtVideoSink::UPDATE_SIGNAL], 0);

        return true;
    }
    default:
        return QObject::event(event);
    }
}

#include "gstqtvideosinksurface.moc"
