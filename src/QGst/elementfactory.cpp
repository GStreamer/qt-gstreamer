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
ElementFactoryPtr ElementFactory::find(const QGlib::String & factoryName)
{
    return ElementFactoryPtr::wrap(gst_element_factory_find(factoryName), false);
}

//static
ElementPtr ElementFactory::make(const QGlib::String & factoryName, const QGlib::String & elementName)
{
    GstElement *e = gst_element_factory_make(factoryName, elementName);
    gst_object_ref_sink(e);
    return ElementPtr::wrap(e, false);
}

QGlib::Type ElementFactory::elementType() const
{
    return gst_element_factory_get_element_type(GST_ELEMENT_FACTORY(object()));
}

QGlib::String ElementFactory::longName() const
{
    return QGlib::String(gst_element_factory_get_longname(GST_ELEMENT_FACTORY(object())));
}

QGlib::String ElementFactory::klass() const
{
    return QGlib::String(gst_element_factory_get_klass(GST_ELEMENT_FACTORY(object())));
}

QGlib::String ElementFactory::description() const
{
    return QGlib::String(gst_element_factory_get_description(GST_ELEMENT_FACTORY(object())));
}

QGlib::String ElementFactory::author() const
{
    return QGlib::String(gst_element_factory_get_author(GST_ELEMENT_FACTORY(object())));
}

uint ElementFactory::padTemplatesCount() const
{
    return gst_element_factory_get_num_pad_templates(GST_ELEMENT_FACTORY(object()));
}

int ElementFactory::uriType() const
{
    return gst_element_factory_get_uri_type(GST_ELEMENT_FACTORY(object()));
}

bool ElementFactory::hasInterface(const QGlib::String & interfaceName) const
{
    return gst_element_factory_has_interface(GST_ELEMENT_FACTORY(object()), interfaceName);
}

bool ElementFactory::canSinkCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_sink_caps(GST_ELEMENT_FACTORY(object()), caps);
}

bool ElementFactory::canSrcCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_src_caps(GST_ELEMENT_FACTORY(object()), caps);
}

ElementPtr ElementFactory::create(const QGlib::String & elementName) const
{
    GstElement *e = gst_element_factory_create(GST_ELEMENT_FACTORY(object()), elementName);
    gst_object_ref_sink(e);
    return ElementPtr::wrap(e, false);
}

}
