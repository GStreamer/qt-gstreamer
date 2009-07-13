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

QGstElementFactory::QGstElementFactory(GstElementFactory *factory, QObject *parent)
    : QGstObject(GST_OBJECT(factory), parent)
{
}

QGstElementFactory::~QGstElementFactory()
{
}

//static
bool QGstElementFactory::exists(const char *factoryName)
{
    QGstElementFactory *factory = find(factoryName);
    if ( factory ) {
        delete factory;
        return true;
    } else {
        return false;
    }
}

//static
QGstElementFactory *QGstElementFactory::find(const char *factoryName)
{
    GstElementFactory *factory = gst_element_factory_find(factoryName);
    if ( !factory ) {
        return NULL;
    }
    return new QGstElementFactory(factory);
}

//static
QGstElement *QGstElementFactory::make(const char *factoryName, const char *elementName)
{
    GstElement *element = gst_element_factory_make(factoryName, elementName);
    if ( !element ) {
        qWarning() << "Unable to construct" << factoryName << "element";
        return NULL;
    }
    return new QGstElement(element);
}

}
