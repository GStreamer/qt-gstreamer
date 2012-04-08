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

#include "gstqtvideosinkplugin.h"
#include "gstqtvideosink.h"
#include "gstqtglvideosink.h"
#include "gstqwidgetvideosink.h"

GST_DEBUG_CATEGORY(gst_qt_video_sink_debug);

/* entry point to initialize the plug-in */
static gboolean plugin_init(GstPlugin *plugin)
{
    GST_DEBUG_CATEGORY_INIT(gst_qt_video_sink_debug, "qtvideosink", 0,
                            "Debug category for GstQtVideoSink");

    gst_element_register(plugin, "qtvideosink",
            GST_RANK_NONE, GST_TYPE_QT_VIDEO_SINK);
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    gst_element_register(plugin, "qtglvideosink",
            GST_RANK_NONE, GST_TYPE_QT_GL_VIDEO_SINK);
#endif
    gst_element_register(plugin, "qwidgetvideosink",
            GST_RANK_NONE, GST_TYPE_QWIDGET_VIDEO_SINK);

    return TRUE;
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "qtvideosink",
    "A video sink that can draw on any Qt surface",
    plugin_init,
    PACKAGE_VERSION,
    "LGPL",
    PACKAGE_NAME,
    PACKAGE_ORIGIN
)
