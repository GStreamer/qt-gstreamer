/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "caps.h"
#include "structure.h"
#include <QtCore/QDebug>
#include <gst/gstcaps.h>

namespace QGst {

//static
CapsPtr Caps::newSimple(const QGlib::String & mediaType)
{
    return CapsPtr::wrap(gst_caps_new_simple(mediaType, NULL), false);
}

//static
CapsPtr Caps::newAny()
{
    return CapsPtr::wrap(gst_caps_new_any(), false);
}

//static
CapsPtr Caps::newEmpty()
{
    return CapsPtr::wrap(gst_caps_new_empty(), false);
}

//static
CapsPtr Caps::fromString(const QGlib::String & string)
{
    return CapsPtr::wrap(gst_caps_from_string(string), false);
}

QGlib::String Caps::toString() const
{
    return QGlib::String::fromGCharPtr(gst_caps_to_string(GST_CAPS(object())));
}

//static
CapsPtr Caps::fromXml(xmlNodePtr node)
{
    return CapsPtr::wrap(gst_caps_load_thyself(node), false);
}

xmlNodePtr Caps::toXml(xmlNodePtr parent) const
{
    return gst_caps_save_thyself(GST_CAPS(object()), parent);
}

void Caps::append(const CapsPtr & caps2)
{
    gst_caps_append(GST_CAPS(object()), gst_caps_copy(caps2));
}

void Caps::merge(const CapsPtr & caps2)
{
    gst_caps_merge(GST_CAPS(object()), gst_caps_copy(caps2));
}

void Caps::setValue(const QGlib::String & field, const QGlib::Value & value)
{
    gst_caps_set_value(GST_CAPS(object()), field, value);
}

bool Caps::simplify()
{
    return gst_caps_do_simplify(GST_CAPS(object()));
}

void Caps::truncate()
{
    gst_caps_truncate(GST_CAPS(object()));
}

SharedStructure Caps::structure(uint index)
{
    return SharedStructure(gst_caps_get_structure(GST_CAPS(object()), index));
}

void Caps::appendStructure(const StructureBase & structure)
{
    gst_caps_append_structure(GST_CAPS(object()), gst_structure_copy(structure));
}

void Caps::mergeStructure(const StructureBase & structure)
{
    gst_caps_merge_structure(GST_CAPS(object()), gst_structure_copy(structure));
}

void Caps::removeStructure(uint index)
{
    gst_caps_remove_structure(GST_CAPS(object()), index);
}

uint Caps::size() const
{
   return gst_caps_get_size(GST_CAPS(object()));
}

bool Caps::isSimple() const
{
    return GST_CAPS_IS_SIMPLE(GST_CAPS(object()));
}

bool Caps::isAny() const
{
    return gst_caps_is_any(GST_CAPS(object()));
}

bool Caps::isEmpty() const
{
    return gst_caps_is_empty(GST_CAPS(object()));
}

bool Caps::isFixed() const
{
    return gst_caps_is_fixed(GST_CAPS(object()));
}

bool Caps::equals(const CapsPtr & caps2) const
{
    return gst_caps_is_equal(GST_CAPS(object()), caps2);
}

bool Caps::isAlwaysCompatibleWith(const CapsPtr & caps2) const
{
    return gst_caps_is_always_compatible(GST_CAPS(object()), caps2);
}

bool Caps::isSubsetOf(const CapsPtr & superset) const
{
    return gst_caps_is_subset(GST_CAPS(object()), superset);
}

bool Caps::canIntersect(const CapsPtr & caps2) const
{
    return gst_caps_can_intersect(GST_CAPS(object()), caps2);
}

CapsPtr Caps::getIntersection(const CapsPtr & caps2) const
{
    return CapsPtr::wrap(gst_caps_intersect(GST_CAPS(object()), caps2), false);
}

CapsPtr Caps::getUnion(const CapsPtr & caps2) const
{
    return CapsPtr::wrap(gst_caps_union(GST_CAPS(object()), caps2), false);
}

CapsPtr Caps::getNormal() const
{
    return CapsPtr::wrap(gst_caps_normalize(GST_CAPS(object())), false);
}

CapsPtr Caps::subtract(const CapsPtr & subtrahend) const
{
    return CapsPtr::wrap(gst_caps_subtract(GST_CAPS(object()), subtrahend), false);
}

CapsPtr Caps::copy() const
{
    return CapsPtr::wrap(gst_caps_copy(GST_CAPS(object())), false);
}

CapsPtr Caps::copyNth(uint index) const
{
    return CapsPtr::wrap(gst_caps_copy_nth(GST_CAPS(object()), index), false);
}

void Caps::ref()
{
    gst_caps_ref(GST_CAPS(m_object));
}

void Caps::unref()
{
    gst_caps_unref(GST_CAPS(m_object));
}

void Caps::makeWritable()
{
    m_object = gst_caps_make_writable(GST_CAPS(m_object));
}

} //namespace QGst

QDebug operator<<(QDebug debug, const QGst::CapsPtr & caps)
{
    debug.nospace() << "QGst::Caps(" << caps->toString() << ")";
    return debug.space();
}
