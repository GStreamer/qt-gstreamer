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
#ifndef GENERICSURFACEPAINTER_H
#define GENERICSURFACEPAINTER_H

#include "abstractsurfacepainter.h"

/**
 * Generic painter that paints using the QPainter API.
 * No colorspace conversion is done and no colors adjustment either.
 */
class GenericSurfacePainter : public AbstractSurfacePainter
{
public:
    GenericSurfacePainter();

    static QSet<BufferFormat::PixelFormat> supportedPixelFormats();

    virtual bool supportsFormat(BufferFormat::PixelFormat format) const {
        return supportedPixelFormats().contains(format);
    }

    virtual void init(const BufferFormat &format);
    virtual void cleanup();

    virtual void paint(quint8 *data, const BufferFormat & frameFormat, const QRectF & clipRect,
                       QPainter *painter, const PaintAreas & areas);

    virtual void updateColors(int brightness, int contrast, int hue, int saturation,
                              BufferFormat::YCbCrColorSpace colorSpace);

private:
    QImage::Format m_imageFormat;
};

#endif // GENERICSURFACEPAINTER_H
