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
#include <gst/gstbin.h>

namespace QtGstreamer {

QGstBin::QGstBin(QObject *parent)
    : QGstElement(gst_bin_new(NULL), parent)
{
}

QGstBin::QGstBin(const char *name, QObject *parent)
    : QGstElement(gst_bin_new(name), parent)
{
}

QGstBin::QGstBin(GstBin *gstBin, QObject *parent)
    : QGstElement(GST_ELEMENT(gstBin), parent)
{
}

QGstBin::~QGstBin()
{
}

bool QGstBin::add(QGstElement *element)
{
    if ( gst_bin_add(GST_BIN(m_object), GST_ELEMENT(element->m_object)) ) {
        element->setParent(this);
        return true;
    }
    return false;
}

QGstBin & QGstBin::operator<<(QGstElement *element)
{
    add(element);
    return *this;
}

bool QGstBin::remove(QGstElement *element)
{
    if ( gst_bin_remove(GST_BIN(m_object), GST_ELEMENT(element->m_object)) ) {
        element->setParent(NULL);
        return true;
    }
    return false;
}

}

#include "qgstbin.moc"
