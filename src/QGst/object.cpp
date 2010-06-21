/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "object.h"
#include <gst/gstobject.h>

namespace QGst {

QGlib::String Object::name() const
{
    return QGlib::String::fromGCharPtr(gst_object_get_name(GST_OBJECT(object())));
}

bool Object::setName(const QGlib::String & name)
{
    return gst_object_set_name(GST_OBJECT(object()), name);
}

ObjectPtr Object::parent() const
{
    return ObjectPtr::wrap(gst_object_get_parent(GST_OBJECT(object())), false);
}

bool Object::setParent(const ObjectPtr & parent)
{
    return gst_object_set_parent(GST_OBJECT(object()), parent);
}

void Object::unparent()
{
    gst_object_unparent(GST_OBJECT(object()));
}

QGlib::String Object::namePrefix() const
{
    return QGlib::String::fromGCharPtr(gst_object_get_name_prefix(GST_OBJECT(object())));
}

void Object::setNamePrefix(const QGlib::String & prefix)
{
    gst_object_set_name_prefix(GST_OBJECT(object()), prefix);
}

bool Object::isAncestorOf(const ObjectPtr & obj) const
{
    return gst_object_has_ancestor(obj, GST_OBJECT(object()));
}

void Object::ref()
{
    gst_object_ref(m_object);
}

void Object::unref()
{
    gst_object_unref(m_object);
}

}
