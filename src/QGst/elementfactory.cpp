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
#include "helpers_p.h"
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>
#include <gst/gstutils.h>

namespace QGst {

//static
ElementFactoryPtr ElementFactory::find(const QString & factoryName)
{
    return ElementFactoryPtr::wrap(gst_element_factory_find(qstringToGcharPtr(factoryName)), false);
}

//static
ElementPtr ElementFactory::make(const QString & factoryName, const QString & elementName)
{
    GstElement *e = gst_element_factory_make(qstringToGcharPtr(factoryName),
                                             qstringToGcharPtr(elementName));
    gst_object_ref_sink(e);
    return ElementPtr::wrap(e, false);
}

QGlib::Type ElementFactory::elementType() const
{
    return gst_element_factory_get_element_type(GST_ELEMENT_FACTORY(m_object));
}

QString ElementFactory::longName() const
{
    return QString::fromUtf8(gst_element_factory_get_longname(GST_ELEMENT_FACTORY(m_object)));
}

QString ElementFactory::klass() const
{
    return QString::fromUtf8(gst_element_factory_get_klass(GST_ELEMENT_FACTORY(m_object)));
}

QString ElementFactory::description() const
{
    return QString::fromUtf8(gst_element_factory_get_description(GST_ELEMENT_FACTORY(m_object)));
}

QString ElementFactory::author() const
{
    return QString::fromUtf8(gst_element_factory_get_author(GST_ELEMENT_FACTORY(m_object)));
}

uint ElementFactory::padTemplatesCount() const
{
    return gst_element_factory_get_num_pad_templates(GST_ELEMENT_FACTORY(m_object));
}

int ElementFactory::uriType() const
{
    return gst_element_factory_get_uri_type(GST_ELEMENT_FACTORY(m_object));
}

bool ElementFactory::hasInterface(const QString & interfaceName) const
{
    return gst_element_factory_has_interface(GST_ELEMENT_FACTORY(m_object),
                                             qstringToGcharPtr(interfaceName));
}

bool ElementFactory::canSinkCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_sink_caps(GST_ELEMENT_FACTORY(m_object), caps);
}

bool ElementFactory::canSrcCaps(const CapsPtr & caps) const
{
    return gst_element_factory_can_src_caps(GST_ELEMENT_FACTORY(m_object), caps);
}

ElementPtr ElementFactory::create(const QString & elementName) const
{
    GstElement *e = gst_element_factory_create(GST_ELEMENT_FACTORY(m_object),
                                               qstringToGcharPtr(elementName));
    gst_object_ref_sink(e);
    return ElementPtr::wrap(e, false);
}

}
