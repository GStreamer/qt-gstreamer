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
#ifndef QTGSTREAMER_QGSTDECLARATIONS_H
#define QTGSTREAMER_QGSTDECLARATIONS_H

#include <QtCore/QSharedPointer>

typedef struct _GstObject GstObject;
typedef struct _GstElement GstElement;
typedef struct _GstPad GstPad;
typedef struct _GstBin GstBin;
typedef struct _GstPipeline GstPipeline;
typedef struct _GstBus GstBus;
typedef struct _GValue GValue;
typedef struct _GstElementFactory GstElementFactory;
typedef struct _GstCaps GstCaps;
typedef struct _GstStructure GstStructure;
typedef struct _GstMiniObject GstMiniObject;
typedef struct _GstMessage GstMessage;

#define QGST_WRAPPER(_gstClass) \
    typedef _gstClass WrappedType; \
    static unsigned long wrappedGType(); \
    template <class X, class Y> friend QSharedPointer<X> qtgstreamer_cast(const QSharedPointer<Y> & ptr);

#define QGST_WRAPPER_GTYPE(_qgstClass, _gtype) \
    unsigned long _qgstClass::wrappedGType() { return (_gtype); }

namespace QtGstreamer {

class QGstObject;
typedef QSharedPointer<QGstObject> QGstObjectPtr;
class QGstElement;
typedef QSharedPointer<QGstElement> QGstElementPtr;
class QGstPad;
typedef QSharedPointer<QGstPad> QGstPadPtr;
class QGstBus;
typedef QSharedPointer<QGstBus> QGstBusPtr;
class QGstBin;
typedef QSharedPointer<QGstBin> QGstBinPtr;
class QGstPipeline;
typedef QSharedPointer<QGstPipeline> QGstPipelinePtr;
class QGstGhostPad;
typedef QSharedPointer<QGstGhostPad> QGstGhostPadPtr;
class QGstElementFactory;
typedef QSharedPointer<QGstElementFactory> QGstElementFactoryPtr;
class QGstCaps;
typedef QSharedPointer<QGstCaps> QGstCapsPtr;
class QGstStructure;
class QGstValue;
class QGstXOverlay;
class QGstMiniObject;
typedef QSharedPointer<QGstMiniObject> QGstMiniObjectPtr;
class QGstMessage;
typedef QSharedPointer<QGstMessage> QGstMessagePtr;

template <class X, class Y>
QSharedPointer<X> qtgstreamer_cast(const QSharedPointer<Y> & ptr)
{
    //internal helper function used for casting
    extern bool qgst_gtype_check_instance_type(void *instance, unsigned long gtype);

    bool isCorrectType = qgst_gtype_check_instance_type(ptr->m_object, X::wrappedGType());
    if ( isCorrectType ) {
        typename X::WrappedType *gstPtr = reinterpret_cast<typename X::WrappedType*>(ptr->m_object);
        return QSharedPointer<X>(new X(gstPtr));
    } else {
        return QSharedPointer<X>();
    }
}

}

#endif
