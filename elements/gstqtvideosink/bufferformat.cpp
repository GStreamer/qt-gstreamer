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
#include "bufferformat.h"
#include <QtCore/QByteArray>

BufferFormat BufferFormat::fromCaps(GstCaps *caps)
{
    BufferFormat result;
    if (caps) {
        GstVideoFormat format;
        int width;
        int height;
        if (gst_video_format_parse_caps(caps, &format, &width, &height)) {
            result.d->videoFormat = format;
            result.d->frameSize = QSize(width, height);

            if (!gst_video_parse_caps_pixel_aspect_ratio(caps,
                    &result.d->pixelAspectRatio.numerator,
                    &result.d->pixelAspectRatio.denominator)) {
                result.d->pixelAspectRatio = Fraction(1,1);
            }

            const char *colorMatrix = gst_video_parse_caps_color_matrix(caps);
            if (!qstrcmp("hdtv", colorMatrix)) {
                result.d->colorMatrix = GST_VIDEO_COLOR_MATRIX_BT709;
            } else if (!qstrcmp("sdtv", colorMatrix)) {
                result.d->colorMatrix = GST_VIDEO_COLOR_MATRIX_BT601;
            } else {
                result.d->colorMatrix = GST_VIDEO_COLOR_MATRIX_RGB;
            }
        }
    }
    return result;
}

GstCaps* BufferFormat::newTemplateCaps(GstVideoFormat format)
{
    GstCaps *caps = gst_video_format_new_template_caps(format);

    // workaround for https://bugzilla.gnome.org/show_bug.cgi?id=667681
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    switch (format) {
    case GST_VIDEO_FORMAT_RGB16:
    case GST_VIDEO_FORMAT_BGR16:
    case GST_VIDEO_FORMAT_RGB15:
    case GST_VIDEO_FORMAT_BGR15:
        gst_caps_set_simple(caps, "endianness", G_TYPE_INT, G_LITTLE_ENDIAN, NULL);
        break;
    default:
        break;
    }
#endif

    return caps;
}

GstCaps* BufferFormat::newCaps(GstVideoFormat format, const QSize & size,
                               const Fraction & framerate, const Fraction & pixelAspectRatio)
{
    GstCaps *caps = gst_video_format_new_caps(format, size.width(), size.height(),
            framerate.numerator, framerate.denominator,
            pixelAspectRatio.numerator, pixelAspectRatio.denominator);

    // workaround for https://bugzilla.gnome.org/show_bug.cgi?id=667681
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    switch (format) {
    case GST_VIDEO_FORMAT_RGB16:
    case GST_VIDEO_FORMAT_BGR16:
    case GST_VIDEO_FORMAT_RGB15:
    case GST_VIDEO_FORMAT_BGR15:
        gst_caps_set_simple(caps, "endianness", G_TYPE_INT, G_LITTLE_ENDIAN, NULL);
        break;
    default:
        break;
    }
#endif

    return caps;
}

int BufferFormat::bytesPerLine(int component) const
{
    return gst_video_format_get_row_stride(videoFormat(), component, frameSize().width());
}
