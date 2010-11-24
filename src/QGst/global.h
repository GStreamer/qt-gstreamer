/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef QGST_GLOBAL_H
#define QGST_GLOBAL_H

#include "enums.h"
#include <QtCore/QtGlobal>

#define QGST_WRAPPER_GSTCLASS_DECLARATION(Class) \
    typedef struct _Gst##Class Gst##Class;

#define QGST_WRAPPER_REFPOINTER_DECLARATION(Class) \
    namespace QGst { \
        class Class; \
        typedef QGlib::RefPointer<Class> Class##Ptr; \
    }

#define QGST_WRAPPER_DECLARATION(Class) \
    QGST_WRAPPER_GSTCLASS_DECLARATION(Class) \
    QGST_WRAPPER_REFPOINTER_DECLARATION(Class)

#define QGST_SUBCLASS_REGISTER_CONVERTERS(BASECLASS, TYPE) \
    namespace QGlib { \
    namespace Private { \
        template <> \
        struct CanConvertTo<QGst::TYPE##BASECLASS> \
        { \
            static inline bool from(void *instance) \
            { \
                return (Type::fromInstance(instance).isA(GetType<QGst::BASECLASS>()) && \
                        QGst::BASECLASS##Ptr::wrap(static_cast<Gst##BASECLASS *>(instance))->type() \
                        == QGst::BASECLASS##TYPE); \
            } \
            /* NO 'static inline bool from(Type t)' to disallow usage from Value::get */ \
            /* ValueBase::get is not supposed to provide dynamic_cast capabilities */ \
        }; \
        \
        template <> \
        struct CanConvertFrom<QGst::TYPE##BASECLASS##Ptr> \
        { \
            static inline bool to(Type t) \
            { \
                return GetType<QGst::BASECLASS>().isA(t); \
            } \
        }; \
    } /* namespace Private */ \
    } /* namespace QGlib */

#define QGST_SUBCLASS_REGISTER_VALUEIMPL(BASECLASS, TYPE) \
    namespace QGlib { \
        template<> \
        struct ValueImpl<QGst::TYPE##BASECLASS##Ptr> \
        { \
            static void set(ValueBase & value, const QGst::TYPE##BASECLASS##Ptr & data) { \
                ValueImpl<QGst::BASECLASS##Ptr>::set(value, data); \
            } \
        }; \
    } /* namespace QGlib */

#define QGST_REGISTER_SUBCLASS(BASECLASS, TYPE) \
    QGST_SUBCLASS_REGISTER_CONVERTERS(BASECLASS, TYPE) \
        QGST_SUBCLASS_REGISTER_VALUEIMPL(BASECLASS, TYPE)

QGST_WRAPPER_DECLARATION(Bin)
QGST_WRAPPER_DECLARATION(Bus)
QGST_WRAPPER_DECLARATION(Caps)
QGST_WRAPPER_DECLARATION(ChildProxy)
QGST_WRAPPER_DECLARATION(Clock)
QGST_WRAPPER_DECLARATION(ColorBalanceChannel)
QGST_WRAPPER_DECLARATION(ColorBalance)
QGST_WRAPPER_DECLARATION(Element)
QGST_WRAPPER_DECLARATION(ElementFactory)
QGST_WRAPPER_DECLARATION(GhostPad)
QGST_WRAPPER_DECLARATION(Message)
QGST_WRAPPER_REFPOINTER_DECLARATION(EosMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(ErrorMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(WarningMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(InfoMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(BufferingMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(StateChangedMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(StepDoneMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(StreamStatusMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(ApplicationMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(ElementMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(SegmentDoneMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(DurationMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(LatencyMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(AsyncDoneMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(RequestStateMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(StepStartMessage)
QGST_WRAPPER_REFPOINTER_DECLARATION(QosMessage)
QGST_WRAPPER_DECLARATION(MiniObject)
QGST_WRAPPER_DECLARATION(Object)
QGST_WRAPPER_DECLARATION(Pad)
QGST_WRAPPER_DECLARATION(Pipeline)
QGST_WRAPPER_DECLARATION(PluginFeature)
QGST_WRAPPER_DECLARATION(PropertyProbe)
QGST_WRAPPER_DECLARATION(Query)
QGST_WRAPPER_REFPOINTER_DECLARATION(PositionQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(DurationQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(LatencyQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(SeekingQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(SegmentQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(ConvertQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(FormatsQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(BufferingQuery)
QGST_WRAPPER_REFPOINTER_DECLARATION(UriQuery)
QGST_WRAPPER_DECLARATION(Buffer)
QGST_WRAPPER_DECLARATION(Event)
QGST_WRAPPER_REFPOINTER_DECLARATION(FlushStartEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(FlushStopEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(EosEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(NewSegmentEvent)
//TODO QGST_REGISTER_EVENT_SUBCLASS(TagEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(BufferSizeEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(SinkMessageEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(QosEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(SeekEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(NavigationEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(LatencyEvent)
QGST_WRAPPER_REFPOINTER_DECLARATION(StepEvent)
QGST_WRAPPER_DECLARATION(StreamVolume)
QGST_WRAPPER_GSTCLASS_DECLARATION(Structure)
namespace QGst {
    class StructureBase;
    class Structure;
    class SharedStructure;
}
QGST_WRAPPER_GSTCLASS_DECLARATION(URIHandler)
QGST_WRAPPER_REFPOINTER_DECLARATION(UriHandler)
QGST_WRAPPER_DECLARATION(VideoOrientation)
QGST_WRAPPER_DECLARATION(XOverlay)

#undef QGST_WRAPPER_DECLARATION
#undef QGST_WRAPPER_REFPOINTER_DECLARATION
#undef QGST_WRAPPER_GSTCLASS_DECLARATION


#define QGST_WRAPPER_DIFFERENT_C_CLASS(Class, CClass) \
    public: \
        typedef Gst##CClass CType; \
    protected: \
        Class() {} \
        Class(const Class &); \
        Class & operator=(const Class &); \
        ~Class() {} \
        template <class T> friend class QGlib::RefPointer;

#define QGST_WRAPPER(Class) \
        QGST_WRAPPER_DIFFERENT_C_CLASS(Class, Class)

namespace QGst {
    /*! A datatype to hold a time, measured in nanoseconds */
    typedef quint64 ClockTime;

    /*! A datatype to hold a time difference, measured in nanoseconds */
    typedef qint64 ClockTimeDiff;
}

namespace QGst {
    /*! \overload */
    void init();

    /*! Initializes the GStreamer library, setting up internal path lists,
     * registering built-in elements, and loading standard plugins.
     * \param argc pointer to the application's argc
     * \param argv pointer to the application's argv
     * \throws QGlib::Error when initialization fails
     */
    void init(int *argc, char **argv[]);

    /*! Clean up any resources created by GStreamer in init().
     *
     * It is normally not needed to call this function in a normal application as the resources
     * will automatically be freed when the program terminates. This function is therefore mostly
     * used by testsuites and other memory profiling tools.
     *
     * After this call GStreamer (including this method) should not be used anymore.
     */
    void cleanup();
}

#endif
