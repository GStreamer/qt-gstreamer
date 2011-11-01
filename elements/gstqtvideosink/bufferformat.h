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
#ifndef BUFFERFORMAT_H
#define BUFFERFORMAT_H

#include <QtCore/QSharedData>
#include <QtCore/QSize>
#include <QtGui/QImage>

typedef struct _GstCaps GstCaps;


/**
 * This class is a cheap way to represent Caps.
 * Based on QVideoSurfaceFormat.
 */
class BufferFormat
{
public:
    enum PixelFormat {
        Invalid,
        ARGB32,
        RGB32,
        RGB24,
        RGB565,
        RGB555, /* unused */
        BGRA32, /* unused */
        BGR32,
        BGR24,
        BGR565, /* unused */
        BGR555, /* unused */
        AYUV444,
        YUV444, /* unused */
        YUV420P,
        YV12,
        UYVY,
        YUYV,
        NV12,
        NV21
    };

    enum YCbCrColorSpace {
        YCbCr_Undefined,
        YCbCr_BT601,
        YCbCr_BT709,
        YCbCr_xvYCC601,
        YCbCr_xvYCC709,
        YCbCr_JPEG
    };

    static QImage::Format pixelFormatToImageFormat(PixelFormat format);
    static GstCaps *pixelFormatToCaps(PixelFormat format);

    static BufferFormat fromCaps(const GstCaps *caps);
    inline BufferFormat() : d(new Data) {}

    inline PixelFormat pixelFormat() const          { return d->pixelFormat; }
    inline YCbCrColorSpace yCbCrColorSpace() const  { return d->colorSpace; }
    inline QSize frameSize() const                  { return d->frameSize; }
    inline QSize pixelAspectRatio() const           { return d->pixelAspectRatio; }
    inline int bytesPerLine() const                 { return d->bytesPerLine; }

private:
    struct Data : public QSharedData
    {
        Data() :
            pixelFormat(Invalid),
            colorSpace(BufferFormat::YCbCr_Undefined),
            bytesPerLine(0)
        {}

        PixelFormat pixelFormat;
        YCbCrColorSpace colorSpace;
        QSize frameSize;
        QSize pixelAspectRatio;
        int bytesPerLine;
    };
    QSharedDataPointer<Data> d;
};

#endif // BUFFERFORMAT_H
