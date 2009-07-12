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
#include "qgstelement.h"
#include "qgstpad.h"
#include <QtCore/QDebug>
#include <gst/gstelement.h>
#include <gst/gstutils.h>
#include <gst/gstpad.h>

namespace QtGstreamer {

QGstElement::QGstElement(GstElement *gstElement, QObject *parent)
    : QGstObject(GST_OBJECT(gstElement), parent)
{
}

QGstElement::~QGstElement()
{
}

QGstElement::State QGstElement::currentState() const
{
    GstState s;
    gst_element_get_state(GST_ELEMENT(m_object), &s, NULL, 0);
    return static_cast<State>(s);
}

void QGstElement::setState(State state)
{
    gst_element_set_state(GST_ELEMENT(m_object), static_cast<GstState>(state));
}

QGstPad *QGstElement::getStaticPad(const char *name)
{
    GstPad *pad = gst_element_get_static_pad(GST_ELEMENT(m_object), name);
    if (!pad) {
        qWarning() << "Could not get static pad" << name;
        return NULL;
    }
    return new QGstPad(pad, this);
}

QGstPad *QGstElement::getRequestPad(const char *name)
{
    GstPad *pad = gst_element_get_request_pad(GST_ELEMENT(m_object), name);
    if (!pad) {
        qWarning() << "Could not get request pad" << name;
        return NULL;
    }
    return new QGstPad(pad, this);
}


//static
bool QGstElement::link(QGstElement *element1, QGstElement *element2, QGstElement *element3,
                       QGstElement *element4, QGstElement *element5 , QGstElement *element6,
                       QGstElement *element7, QGstElement *element8, QGstElement *element9,
                       QGstElement *element10)
{
    return gst_element_link_many(GST_ELEMENT(element1->m_object), GST_ELEMENT(element2->m_object),
                                 element3 ? GST_ELEMENT(element3->m_object) : NULL,
                                 element4 ? GST_ELEMENT(element4->m_object) : NULL,
                                 element5 ? GST_ELEMENT(element5->m_object) : NULL,
                                 element6 ? GST_ELEMENT(element6->m_object) : NULL,
                                 element7 ? GST_ELEMENT(element7->m_object) : NULL,
                                 element8 ? GST_ELEMENT(element8->m_object) : NULL,
                                 element9 ? GST_ELEMENT(element9->m_object) : NULL,
                                 element10 ? GST_ELEMENT(element10->m_object) : NULL, NULL);
}

//static
void QGstElement::unlink(QGstElement *element1, QGstElement *element2, QGstElement *element3,
                         QGstElement *element4, QGstElement *element5 , QGstElement *element6,
                         QGstElement *element7, QGstElement *element8, QGstElement *element9,
                         QGstElement *element10)
{
    gst_element_unlink_many(GST_ELEMENT(element1->m_object), GST_ELEMENT(element2->m_object),
                            element3 ? GST_ELEMENT(element3->m_object) : NULL,
                            element4 ? GST_ELEMENT(element4->m_object) : NULL,
                            element5 ? GST_ELEMENT(element5->m_object) : NULL,
                            element6 ? GST_ELEMENT(element6->m_object) : NULL,
                            element7 ? GST_ELEMENT(element7->m_object) : NULL,
                            element8 ? GST_ELEMENT(element8->m_object) : NULL,
                            element9 ? GST_ELEMENT(element9->m_object) : NULL,
                            element10 ? GST_ELEMENT(element10->m_object) : NULL, NULL);
}

}

#include "qgstelement.moc"
