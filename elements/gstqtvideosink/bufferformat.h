/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011-2012 Collabora Ltd. <info@collabora.com>

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
#ifndef BUFFERFORMAT_H
#define BUFFERFORMAT_H

#include "utils.h"
#include <QtCore/QSharedData>
#include <gst/video/video.h>

//0.11 stuff
typedef enum {
  GST_VIDEO_COLOR_MATRIX_UNKNOWN = 0,
  GST_VIDEO_COLOR_MATRIX_RGB,
  GST_VIDEO_COLOR_MATRIX_FCC,
  GST_VIDEO_COLOR_MATRIX_BT709,
  GST_VIDEO_COLOR_MATRIX_BT601,
  GST_VIDEO_COLOR_MATRIX_SMPTE240M
} GstVideoColorMatrix;


/**
 * This class is a cheap way to represent Caps.
 * Based on QVideoSurfaceFormat.
 */
class BufferFormat
{
public:
    static BufferFormat fromCaps(GstCaps *caps);
    static GstCaps *newTemplateCaps(GstVideoFormat format);
    static GstCaps *newCaps(GstVideoFormat format, const QSize & size,
            const Fraction & framerate, const Fraction & pixelAspectRatio);

    inline BufferFormat() : d(new Data) {}

    inline GstVideoFormat videoFormat() const       { return d->videoFormat; }
    inline GstVideoColorMatrix colorMatrix() const  { return d->colorMatrix; }
    inline QSize frameSize() const                  { return d->frameSize; }
    inline Fraction pixelAspectRatio() const        { return d->pixelAspectRatio; }
    int bytesPerLine(int component = 0) const;

private:
    struct Data : public QSharedData
    {
        Data() :
            videoFormat(GST_VIDEO_FORMAT_UNKNOWN),
            colorMatrix(GST_VIDEO_COLOR_MATRIX_UNKNOWN)
        {}

        GstVideoFormat videoFormat;
        GstVideoColorMatrix colorMatrix;
        QSize frameSize;
        Fraction pixelAspectRatio;
    };
    QSharedDataPointer<Data> d;
};


Q_DECLARE_METATYPE(GstVideoFormat)
Q_DECLARE_METATYPE(GstVideoColorMatrix)
Q_DECLARE_METATYPE(BufferFormat)

#endif // BUFFERFORMAT_H
