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
#include "qgstbus.h"
#include <QtCore/QDebug>
#include <gst/gstelement.h>
#include <gst/gstutils.h>
#include <gst/gstpad.h>

namespace QtGstreamer {

QGST_WRAPPER_GTYPE(QGstElement, GST_TYPE_ELEMENT)

class QGstElementPrivate
{
public:
    static void no_more_pads(GstElement *element, QGstElement *self);
    static void pad_added(GstElement *element, GstPad *pad, QGstElement *self);
    static void pad_removed(GstElement *element, GstPad *pad, QGstElement *self);
};

//static
void QGstElementPrivate::no_more_pads(GstElement *element, QGstElement *self)
{
    emit self->noMorePads();
}

//static
void QGstElementPrivate::pad_added(GstElement *element, GstPad *pad, QGstElement *self)
{
    emit self->padAdded(QGstPad::fromGstPad(pad));
}

//static
void QGstElementPrivate::pad_removed(GstElement *element, GstPad *pad, QGstElement *self)
{
    emit self->padRemoved(QGstPad::fromGstPad(pad));
}


QGstElement::QGstElement(GstElement *gstElement)
    : QGstObject(GST_OBJECT(gstElement))
{
    g_signal_connect(m_object, "no-more-pads", G_CALLBACK(&QGstElementPrivate::no_more_pads), this);
    g_signal_connect(m_object, "pad-added", G_CALLBACK(&QGstElementPrivate::pad_added), this);
    g_signal_connect(m_object, "pad-removed", G_CALLBACK(&QGstElementPrivate::pad_removed), this);
}

//static
QGstElementPtr QGstElement::fromGstElement(GstElement *gstElement)
{
    return QGstElementPtr(new QGstElement(gstElement));
}

QGstElement::~QGstElement()
{
    g_signal_handlers_disconnect_by_func(m_object, (void*) &QGstElementPrivate::no_more_pads, this);
    g_signal_handlers_disconnect_by_func(m_object, (void*) &QGstElementPrivate::pad_added, this);
    g_signal_handlers_disconnect_by_func(m_object, (void*) &QGstElementPrivate::pad_removed, this);

    //if this is the last object holding a reference to the element, the element
    //is now going to be destroyed, so let it cleanup fully by setting its state to NULL
    if ( GST_OBJECT_REFCOUNT_VALUE(m_object) == 1 ) {
        setState(Null);
    }
}

QGstElement::StateChangeReturn QGstElement::getState(State *state, State *pending, quint64 timeout)
{
    GstState curState, pendingState;
    GstStateChangeReturn result = gst_element_get_state(GST_ELEMENT(m_object),
                                                        &curState, &pendingState, timeout);
    if ( state ) {
        *state = static_cast<State>(curState);
    }
    if ( pending ) {
        *pending = static_cast<State>(pendingState);
    }
    return static_cast<StateChangeReturn>(result);
}

QGstElement::StateChangeReturn QGstElement::setState(State state)
{
    return static_cast<StateChangeReturn>(gst_element_set_state(GST_ELEMENT(m_object),
                                                                static_cast<GstState>(state)));
}

bool QGstElement::syncStateWithParent()
{
   return gst_element_sync_state_with_parent(GST_ELEMENT(m_object));
}

bool QGstElement::addPad(const QGstPadPtr & pad)
{
    return gst_element_add_pad(GST_ELEMENT(m_object), GST_PAD(pad->peekNativeObject()));
}

QGstPadPtr QGstElement::getStaticPad(const char *name)
{
    GstPad *pad = gst_element_get_static_pad(GST_ELEMENT(m_object), name);
    if (!pad) {
        qWarning() << "Could not get static pad" << name;
        return QGstPadPtr();
    }
    return QGstPad::fromGstPad(pad);
}

QGstPadPtr QGstElement::getRequestPad(const char *name)
{
    GstPad *pad = gst_element_get_request_pad(GST_ELEMENT(m_object), name);
    if (!pad) {
        qWarning() << "Could not get request pad" << name;
        return QGstPadPtr();
    }
    return QGstPad::fromGstPad(pad);
}

void QGstElement::releaseRequestPad(const QGstPadPtr & pad)
{
    gst_element_release_request_pad(GST_ELEMENT(m_object), GST_PAD(pad->peekNativeObject()));
}

QGstBusPtr QGstElement::getBus()
{
    return QGstBus::fromGstBus(gst_element_get_bus(GST_ELEMENT(m_object)));
}

//static
bool QGstElement::link(const QGstElementPtr & element1, const QGstElementPtr & element2,
                       const QGstElementPtr & element3, const QGstElementPtr & element4,
                       const QGstElementPtr & element5 , const QGstElementPtr & element6,
                       const QGstElementPtr & element7, const QGstElementPtr & element8,
                       const QGstElementPtr & element9, const QGstElementPtr & element10)
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
void QGstElement::unlink(const QGstElementPtr & element1, const QGstElementPtr & element2,
                         const QGstElementPtr & element3, const QGstElementPtr & element4,
                         const QGstElementPtr & element5 , const QGstElementPtr & element6,
                         const QGstElementPtr & element7, const QGstElementPtr & element8,
                         const QGstElementPtr & element9, const QGstElementPtr & element10)
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
