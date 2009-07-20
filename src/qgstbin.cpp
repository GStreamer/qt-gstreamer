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
#include "qgstbin.h"
#include <QtCore/QDebug>
#include <gst/gstbin.h>

namespace QtGstreamer {

QGstBin::QGstBin(const char *name)
    : QGstElement(gst_bin_new(name))
{
}

//static
QGstBinPtr QGstBin::newBin(const char *name)
{
    return QGstBinPtr(new QGstBin(name));
}

QGstBin::QGstBin(GstBin *gstBin)
    : QGstElement(GST_ELEMENT(gstBin))
{
}

//static
QGstBinPtr QGstBin::fromGstBin(GstBin *gstBin)
{
    return QGstBinPtr(new QGstBin(gstBin));
}

QGstBin::~QGstBin()
{
}

bool QGstBin::add(const QGstElementPtr & element)
{
    return gst_bin_add(GST_BIN(m_object), GST_ELEMENT(element->m_object));
}

QGstBin & QGstBin::operator<<(const QGstElementPtr & element)
{
    add(element);
    return *this;
}

bool QGstBin::remove(const QGstElementPtr & element)
{
    return gst_bin_remove(GST_BIN(m_object), GST_ELEMENT(element->m_object));
}

QGstElementPtr QGstBin::getByName(const char *name)
{
    GstElement *element;
    element = gst_bin_get_by_name(GST_BIN(m_object),name);
    if ( !element ) {
        qWarning() << "Unable to find " << name << "element";
        return QGstElementPtr();
    }
    return QGstElement::fromGstElement(element);
}

}

#include "qgstbin.moc"
