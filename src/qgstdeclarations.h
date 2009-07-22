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
#ifndef _QTGSTREAMER_QGSTDECLARATIONS_H
#define _QTGSTREAMER_QGSTDECLARATIONS_H

#include <QtCore/QSharedPointer>

typedef struct _GstObject GstObject;
typedef struct _GstElement GstElement;
typedef struct _GstPad GstPad;
typedef struct _GstBin GstBin;
typedef struct _GstPipeline GstPipeline;
typedef struct _GstBus GstBus;
typedef struct _GstMessage GstMessage;
typedef struct _GValue GValue;
typedef struct _GstElementFactory GstElementFactory;
typedef struct _GstCaps GstCaps;
typedef struct _GstStructure GstStructure;

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
class QGValue;

}

#endif
