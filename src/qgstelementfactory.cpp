/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "qgstelementfactory.h"
#include "qgstelement.h"
#include <QtCore/QtDebug>
#include <gst/gstelement.h>
#include <gst/gstelementfactory.h>

namespace QtGstreamer {

QGST_WRAPPER_GTYPE(QGstElementFactory, GST_TYPE_ELEMENT_FACTORY)

QGstElementFactory::QGstElementFactory(GstElementFactory *factory)
    : QGstObject(GST_OBJECT(factory))
{
}

//static
QGstElementFactoryPtr QGstElementFactory::fromGstElementFactory(GstElementFactory *factory)
{
    return QGstElementFactoryPtr(new QGstElementFactory(factory));
}

QGstElementFactory::~QGstElementFactory()
{
}

//static
QGstElementFactoryPtr QGstElementFactory::find(const char *factoryName)
{
    GstElementFactory *factory = gst_element_factory_find(factoryName);
    if ( !factory ) {
        return QGstElementFactoryPtr();
    }
    return fromGstElementFactory(factory);
}

//static
QGstElementPtr QGstElementFactory::make(const char *factoryName, const char *elementName)
{
    GstElement *element = gst_element_factory_make(factoryName, elementName);
    if ( !element ) {
        qWarning() << "Unable to construct" << factoryName << "element";
        return QGstElementPtr();
    }
    return QGstElement::fromGstElement(element);
}

}

#include "qgstelementfactory.moc"
