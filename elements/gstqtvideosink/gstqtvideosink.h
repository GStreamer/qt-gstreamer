/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011-2012 Collabora Ltd. <info@collabora.com>

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

#ifndef GST_QT_VIDEO_SINK_H
#define GST_QT_VIDEO_SINK_H

#include "gstqtvideosinkbase.h"
#include <QtCore/QtGlobal>

#define GST_TYPE_QT_VIDEO_SINK \
  (GstQtVideoSink::get_type())
#define GST_QT_VIDEO_SINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_QT_VIDEO_SINK, GstQtVideoSink))
#define GST_QT_VIDEO_SINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_QT_VIDEO_SINK, GstQtVideoSinkClass))


struct GstQtVideoSink
{
public:
    GstQtVideoSinkBase parent;

    static GType get_type();
    static void emit_update(GstQtVideoSink *sink);

private:
    enum {
        PROP_0,
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
        PROP_GLCONTEXT
#endif
    };

    enum {
        PAINT_SIGNAL,
        UPDATE_SIGNAL,
        LAST_SIGNAL
    };

    static void base_init(gpointer g_class);
    static void class_init(gpointer g_class, gpointer class_data);
    static void init(GTypeInstance *instance, gpointer g_class);

    static void set_property(GObject *object, guint prop_id,
                             const GValue *value, GParamSpec *pspec);
    static void get_property(GObject *object, guint prop_id,
                             GValue *value, GParamSpec *pspec);

    static void paint(GstQtVideoSink *sink, gpointer painter,
                      qreal x, qreal y, qreal width, qreal height);


    static guint s_signals[LAST_SIGNAL];
    static GstQtVideoSinkBaseClass *s_parent_class;
};


struct GstQtVideoSinkClass
{
    GstQtVideoSinkBaseClass parent_class;

    /* paint action signal */
    void (*paint) (GstQtVideoSink *sink, gpointer painter,
                   qreal x, qreal y, qreal width, qreal height);
};

#endif
