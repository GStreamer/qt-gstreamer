/****************************************************************************
**
** Copyright (C) 2011 Collabora Ltd
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file was originally part of the Qt Mobility Components.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#ifndef VIDEOSURFACEGSTSINK_H
#define VIDEOSURFACEGSTSINK_H

#include <gst/video/gstvideosink.h>

#include <QtCore/qlist.h>
#include <QtCore/qmutex.h>
#include <QtCore/qqueue.h>
#include <QtCore/qpointer.h>
#include <QtCore/qwaitcondition.h>
#include <qvideosurfaceformat.h>
#include <qvideoframe.h>
#include <qabstractvideobuffer.h>

QT_BEGIN_NAMESPACE
class QAbstractVideoSurface;
QT_END_NAMESPACE

#if defined(Q_WS_X11) && !defined(QT_NO_XVIDEO)
class QmlGstXvImageBuffer;
class QmlGstXvImageBufferPool;
#endif

class QmlVideoSurfaceGstDelegate : public QObject
{
    Q_OBJECT
public:
    QmlVideoSurfaceGstDelegate(QAbstractVideoSurface *surface);

    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const;

    QVideoSurfaceFormat surfaceFormat() const;

    bool start(const QVideoSurfaceFormat &format, int bytesPerLine);
    void stop();

    bool isActive();

    void setPlaying(bool playing);

    GstFlowReturn render(GstBuffer *buffer);

private slots:
    void queuedStart();
    void queuedStop();
    void queuedRender();

    void supportedFormatsChanged();

private:
    QPointer<QAbstractVideoSurface> m_surface;
    QList<QVideoFrame::PixelFormat> m_supportedPixelFormats;
    QList<QVideoFrame::PixelFormat> m_supportedXVideoPixelFormats;
    QMutex m_mutex;
    QWaitCondition m_setupCondition;
    QWaitCondition m_renderCondition;
    QVideoSurfaceFormat m_format;
    QVideoFrame m_frame;
    GstFlowReturn m_renderReturn;
    int m_bytesPerLine;
    bool m_started;
};

class QmlVideoSurfaceGstSink
{
public:
    GstVideoSink parent;

    static QmlVideoSurfaceGstSink *createSink(QAbstractVideoSurface *surface);
    static QVideoSurfaceFormat formatForCaps(GstCaps *caps, int *bytesPerLine = 0);

private:
    static GType get_type();
    static void class_init(gpointer g_class, gpointer class_data);
    static void base_init(gpointer g_class);
    static void instance_init(GTypeInstance *instance, gpointer g_class);

    static void finalize(GObject *object);

    static GstStateChangeReturn change_state(GstElement *element, GstStateChange transition);

    static GstCaps *get_caps(GstBaseSink *sink);
    static gboolean set_caps(GstBaseSink *sink, GstCaps *caps);

    static GstFlowReturn buffer_alloc(
            GstBaseSink *sink, guint64 offset, guint size, GstCaps *caps, GstBuffer **buffer);

    static gboolean start(GstBaseSink *sink);
    static gboolean stop(GstBaseSink *sink);

    static gboolean unlock(GstBaseSink *sink);

    static gboolean event(GstBaseSink *sink, GstEvent *event);
    static GstFlowReturn preroll(GstBaseSink *sink, GstBuffer *buffer);
    static GstFlowReturn render(GstBaseSink *sink, GstBuffer *buffer);

private:
    QmlVideoSurfaceGstDelegate *delegate;

#if defined(Q_WS_X11) && !defined(QT_NO_XVIDEO)
    QmlGstXvImageBufferPool *pool;
#endif

    GstCaps *lastRequestedCaps;
    GstCaps *lastBufferCaps;
    QVideoSurfaceFormat *lastSurfaceFormat;
};


class QmlVideoSurfaceGstSinkClass
{
public:
    GstVideoSinkClass parent_class;
};

#endif
