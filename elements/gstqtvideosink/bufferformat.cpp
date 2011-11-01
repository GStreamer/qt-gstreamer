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
#include "bufferformat.h"
#include <gst/gst.h>

//BEGIN YuvFormat

struct YuvFormat
{
    BufferFormat::PixelFormat pixelFormat;
    guint32 fourcc;
    int bitsPerPixel;
};

static const YuvFormat qt_yuvColorLookup[] =
{
    { BufferFormat::YUV420P, GST_MAKE_FOURCC('I','4','2','0'), 8 },
    { BufferFormat::YV12,    GST_MAKE_FOURCC('Y','V','1','2'), 8 },
    { BufferFormat::UYVY,    GST_MAKE_FOURCC('U','Y','V','Y'), 16 },
    { BufferFormat::YUYV,    GST_MAKE_FOURCC('Y','U','Y','2'), 16 },
    { BufferFormat::NV12,    GST_MAKE_FOURCC('N','V','1','2'), 8 },
    { BufferFormat::NV21,    GST_MAKE_FOURCC('N','V','2','1'), 8 },
    { BufferFormat::AYUV444, GST_MAKE_FOURCC('A','Y','U','V'), 32 }
};

static int indexOfYuvColor(BufferFormat::PixelFormat format)
{
    const int count = sizeof(qt_yuvColorLookup) / sizeof(YuvFormat);

    for (int i = 0; i < count; ++i)
        if (qt_yuvColorLookup[i].pixelFormat == format)
            return i;

    return -1;
}

static int indexOfYuvColor(guint32 fourcc)
{
    const int count = sizeof(qt_yuvColorLookup) / sizeof(YuvFormat);

    for (int i = 0; i < count; ++i)
        if (qt_yuvColorLookup[i].fourcc == fourcc)
            return i;

    return -1;
}

//END YuvFormat
//BEGIN RgbFormat

struct RgbFormat
{
    BufferFormat::PixelFormat pixelFormat;
    int bitsPerPixel;
    int depth;
    int endianness;
    int red;
    int green;
    int blue;
    int alpha;
};

static const RgbFormat qt_rgbColorLookup[] =
{
    { BufferFormat::RGB32 , 32, 24, 4321, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x00000000 },
    { BufferFormat::RGB32 , 32, 24, 1234, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { BufferFormat::BGR32 , 32, 24, 4321, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x00000000 },
    { BufferFormat::BGR32 , 32, 24, 1234, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { BufferFormat::ARGB32, 32, 24, 4321, 0x0000FF00, 0x00FF0000, 0xFF000000, 0x000000FF },
    { BufferFormat::ARGB32, 32, 24, 1234, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000 },
    { BufferFormat::RGB24 , 24, 24, 4321, 0x00FF0000, 0x0000FF00, 0x000000FF, 0x00000000 },
    { BufferFormat::BGR24 , 24, 24, 4321, 0x000000FF, 0x0000FF00, 0x00FF0000, 0x00000000 },
    { BufferFormat::RGB565, 16, 16, 1234, 0x0000F800, 0x000007E0, 0x0000001F, 0x00000000 }
};

static int indexOfRgbColor(
        int bits, int depth, int endianness, int red, int green, int blue, int alpha)
{
    const int count = sizeof(qt_rgbColorLookup) / sizeof(RgbFormat);

    for (int i = 0; i < count; ++i) {
        if (qt_rgbColorLookup[i].bitsPerPixel == bits
            && qt_rgbColorLookup[i].depth == depth
            && qt_rgbColorLookup[i].endianness == endianness
            && qt_rgbColorLookup[i].red == red
            && qt_rgbColorLookup[i].green == green
            && qt_rgbColorLookup[i].blue == blue
            && qt_rgbColorLookup[i].alpha == alpha) {
            return i;
        }
    }
    return -1;
}

//END RgbFormat
//BEGIN BufferFormat

QImage::Format BufferFormat::pixelFormatToImageFormat(PixelFormat format)
{
    switch (format) {
    case ARGB32:
        return QImage::Format_ARGB32;
    case RGB32:
        return QImage::Format_RGB32;
    case RGB24:
        return QImage::Format_RGB888;
    case RGB565:
        return QImage::Format_RGB16;
    case RGB555:
        return QImage::Format_RGB555;
    case BGRA32:
    case BGR32:
    case BGR24:
    case BGR565:
    case BGR555:
    case AYUV444:
    case YUV444:
    case YUV420P:
    case YV12:
    case UYVY:
    case YUYV:
    case NV12:
    case NV21:
    case Invalid:
        return QImage::Format_Invalid;
    }
    return QImage::Format_Invalid;
}

GstCaps *BufferFormat::pixelFormatToCaps(PixelFormat format)
{
    GstCaps *caps = gst_caps_new_empty();

    if (int index = indexOfYuvColor(format) != -1)
    {
        GstStructure *structure = gst_structure_new(
            "video/x-raw-yuv",
            "framerate", GST_TYPE_FRACTION_RANGE, 0, 1, INT_MAX, 1,
            "width"    , GST_TYPE_INT_RANGE, 1, INT_MAX,
            "height"   , GST_TYPE_INT_RANGE, 1, INT_MAX,
            "format"   , GST_TYPE_FOURCC, qt_yuvColorLookup[index].fourcc,
            NULL);
        gst_caps_append_structure(caps, structure);
    }
    else
    {
        const int count = sizeof(qt_rgbColorLookup) / sizeof(RgbFormat);

        for (int i = 0; i < count; ++i) {
            if (qt_rgbColorLookup[i].pixelFormat == format) {
                GstStructure *structure = gst_structure_new(
                    "video/x-raw-rgb",
                    "framerate" , GST_TYPE_FRACTION_RANGE, 0, 1, INT_MAX, 1,
                    "width"     , GST_TYPE_INT_RANGE, 1, INT_MAX,
                    "height"    , GST_TYPE_INT_RANGE, 1, INT_MAX,
                    "bpp"       , G_TYPE_INT, qt_rgbColorLookup[i].bitsPerPixel,
                    "depth"     , G_TYPE_INT, qt_rgbColorLookup[i].depth,
                    "endianness", G_TYPE_INT, qt_rgbColorLookup[i].endianness,
                    "red_mask"  , G_TYPE_INT, qt_rgbColorLookup[i].red,
                    "green_mask", G_TYPE_INT, qt_rgbColorLookup[i].green,
                    "blue_mask" , G_TYPE_INT, qt_rgbColorLookup[i].blue,
                    NULL);

                if (qt_rgbColorLookup[i].alpha != 0) {
                    gst_structure_set(
                            structure, "alpha_mask", G_TYPE_INT, qt_rgbColorLookup[i].alpha, NULL);
                }
                gst_caps_append_structure(caps, structure);
            }
        }
    }

    return caps;
}

BufferFormat BufferFormat::fromCaps(const GstCaps *caps)
{
    BufferFormat result;
    if (caps) {
        const GstStructure *structure = gst_caps_get_structure(caps, 0);
        int bitsPerPixel = 0;

        if (qstrcmp(gst_structure_get_name(structure), "video/x-raw-yuv") == 0) {
            guint32 fourcc = 0;
            gst_structure_get_fourcc(structure, "format", &fourcc);

            int index = indexOfYuvColor(fourcc);
            if (index != -1) {
                result.d->pixelFormat = qt_yuvColorLookup[index].pixelFormat;
                bitsPerPixel = qt_yuvColorLookup[index].bitsPerPixel;
            }
        } else if (qstrcmp(gst_structure_get_name(structure), "video/x-raw-rgb") == 0) {
            int depth = 0;
            int endianness = 0;
            int red = 0;
            int green = 0;
            int blue = 0;
            int alpha = 0;

            gst_structure_get_int(structure, "bpp", &bitsPerPixel);
            gst_structure_get_int(structure, "depth", &depth);
            gst_structure_get_int(structure, "endianness", &endianness);
            gst_structure_get_int(structure, "red_mask", &red);
            gst_structure_get_int(structure, "green_mask", &green);
            gst_structure_get_int(structure, "blue_mask", &blue);
            gst_structure_get_int(structure, "alpha_mask", &alpha);

            int index = indexOfRgbColor(bitsPerPixel, depth, endianness, red, green, blue, alpha);

            if (index != -1) {
                result.d->pixelFormat = qt_rgbColorLookup[index].pixelFormat;
            }
        }

        if (result.d->pixelFormat != BufferFormat::Invalid) {
            gst_structure_get_int(structure, "width", &result.d->frameSize.rwidth());
            gst_structure_get_int(structure, "height", &result.d->frameSize.rheight());
            gst_structure_get_fraction(structure, "pixel-aspect-ratio",
                    &result.d->pixelAspectRatio.rwidth(),
                    &result.d->pixelAspectRatio.rheight());

            result.d->bytesPerLine = ((result.d->frameSize.width() * bitsPerPixel / 8) + 3) & ~3;

            //FIXME not sure if this should be read from caps somehow
            //The original QtMM code always uses this colorspace
            result.d->colorSpace = BufferFormat::YCbCr_BT601;
        }
    }
    return result;
}

//END BufferFormat
