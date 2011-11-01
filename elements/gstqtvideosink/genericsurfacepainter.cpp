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
#include "genericsurfacepainter.h"
#include <gst/gst.h>
#include <QtGui/QPainter>

GenericSurfacePainter::GenericSurfacePainter()
    : m_imageFormat(QImage::Format_Invalid)
{
}

//static
QSet<BufferFormat::PixelFormat> GenericSurfacePainter::supportedPixelFormats()
{
    return QSet<BufferFormat::PixelFormat>()
        << BufferFormat::RGB32
#ifndef QT_OPENGL_ES // The raster formats should be a subset of the GL formats.
        << BufferFormat::RGB24
#endif
        << BufferFormat::ARGB32
        << BufferFormat::RGB565;
}

void GenericSurfacePainter::init(const BufferFormat &format)
{
    m_imageFormat = BufferFormat::pixelFormatToImageFormat(format.pixelFormat());
}

void GenericSurfacePainter::cleanup()
{
    m_imageFormat = QImage::Format_Invalid;
}

void GenericSurfacePainter::paint(quint8 *data,
        const BufferFormat & frameFormat,
        QPainter *painter,
        const QRect & videoArea,
        const QRect & clipRect)
{
    Q_ASSERT(m_imageFormat != QImage::Format_Invalid);

    QImage image(
        data,
        frameFormat.frameSize().width(),
        frameFormat.frameSize().height(),
        frameFormat.bytesPerLine(),
        m_imageFormat);

    painter->drawImage(videoArea, image, clipRect);
}

void GenericSurfacePainter::updateColors(int, int, int, int, BufferFormat::YCbCrColorSpace)
{
}
