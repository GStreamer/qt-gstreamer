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
#include "elementfactory.h"
#include "element.h"
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>
#include <gst/gstutils.h>

namespace QGst {

//static
ElementFactoryPtr ElementFactory::find(const char *factoryName)
{
    return ElementFactoryPtr::wrap(gst_element_factory_find(factoryName), false);
}

//static
ElementPtr ElementFactory::make(const char *factoryName, const char *elementName)
{
    GstElement *e = gst_element_factory_make(factoryName, elementName);
    if (e) {
        gst_object_ref_sink(e);
    }
    return ElementPtr::wrap(e, false);
}

QGlib::Type ElementFactory::elementType() const
{
    return gst_element_factory_get_element_type(object<GstElementFactory>());
}

QString ElementFactory::longName() const
{
    return QString::fromUtf8(gst_element_factory_get_longname(object<GstElementFactory>()));
}

QString ElementFactory::klass() const
{
    return QString::fromUtf8(gst_element_factory_get_klass(object<GstElementFactory>()));
}

QString ElementFactory::description() const
{
    return QString::fromUtf8(gst_element_factory_get_description(object<GstElementFactory>()));
}

QString ElementFactory::author() const
{
    return QString::fromUtf8(gst_element_factory_get_author(object<GstElementFactory>()));
}

QString ElementFactory::documentationUri() const
{
    return QString::fromUtf8(gst_element_factory_get_documentation_uri(object<GstElementFactory>()));
}

QString ElementFactory::iconName() const
{
    return QString::fromUtf8(gst_element_factory_get_icon_name(object<GstElementFactory>()));
}

uint ElementFactory::padTemplatesCount() const
{
    return gst_element_factory_get_num_pad_templates(object<GstElementFactory>());
}

int ElementFactory::uriType() const
{
    return gst_element_factory_get_uri_type(object<GstElementFactory>());
}

bool ElementFactory::hasInterface(const char *interfaceName) const
{
    return gst_element_factory_has_interface(object<GstElementFactory>(), interfaceName);
}

bool ElementFactory::canSinkCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_sink_caps(object<GstElementFactory>(), caps);
}

bool ElementFactory::canSrcCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_src_caps(object<GstElementFactory>(), caps);
}

ElementPtr ElementFactory::create(const char *elementName) const
{
    GstElement *e = gst_element_factory_create(object<GstElementFactory>(), elementName);
    if (e) {
        gst_object_ref_sink(e);
    }
    return ElementPtr::wrap(e, false);
}

}
