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
#ifndef GST_QT_VIDEO_SINK_PLUGIN_H
#define GST_QT_VIDEO_SINK_PLUGIN_H

#include <gst/gst.h>
#include <QtCore/QtGlobal>

GST_DEBUG_CATEGORY_EXTERN(gst_qt_video_sink_debug);
#define GST_CAT_DEFAULT gst_qt_video_sink_debug


#define DEFINE_TYPE_FULL(cpp_type, parent_type, additional_initializations) \
    GType cpp_type::get_type() \
    { \
        static volatile gsize gonce_data = 0; \
        if (g_once_init_enter(&gonce_data)) { \
            GType type; \
            type = gst_type_register_static_full( \
                parent_type, \
                g_intern_static_string(#cpp_type), \
                sizeof(cpp_type##Class), \
                &cpp_type::base_init, \
                NULL,   /* base_finalize */ \
                &cpp_type::class_init, \
                NULL,   /* class_finalize */ \
                NULL,   /* class_data */ \
                sizeof(cpp_type), \
                0,      /* n_preallocs */ \
                &cpp_type::init, \
                NULL, \
                (GTypeFlags) 0); \
            additional_initializations(type); \
            g_once_init_leave(&gonce_data, (gsize) type); \
        } \
        return (GType) gonce_data; \
    }

#define DEFINE_TYPE(cpp_type, parent_type) \
    DEFINE_TYPE_FULL(cpp_type, parent_type, Q_UNUSED)


inline bool qRealIsDouble() { return sizeof(qreal) == sizeof(double); }
#define G_TYPE_QREAL qRealIsDouble() ? G_TYPE_DOUBLE : G_TYPE_FLOAT


#endif
