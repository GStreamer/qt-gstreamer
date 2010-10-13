/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#ifndef QGST_QUERY_H
#define QGST_QUERY_H

#include "miniobject.h"
#include "structure.h"

class QUrl;

namespace QGst {

    /*! \headerfile query.h <QGst/Query>
     * \brief Wrapper class for GstQuery
     *
     * Queries are lightweight objects that are sent from the application directly to elements
     * or pads using the query() methods. A typical usage is to find out the duration or current
     * position of media, or to display the status of a buffering operation.
     *
     * Queries are implemented as a subclass of MiniObject with a generic GstStructure as the
     * content. This allows for writing custom queries without requiring an API change while
     * allowing a wide range of different types of queries.
     *
     * In these bindings, for convenience, each query type has its own Query subclass. This
     * does not reflect 1-1 the native C API, where there is only one Query class with tens of
     * 'new_foo' and 'parse_foo' methods. You can use RefPointer::dynamicCast() to cast a QueryPtr
     * to a RefPointer of one of the Query subclasses and it will behave as expected (i.e. it will
     * only succeed if the queries type matches the queries type that the subclass handles). Note
     * however that the Query subclasses \em cannot be used with ValueBase::get(), since a GValue
     * will actually contain a GstQuery (the subclasses do not exist in C) and ValueBase::get()
     * is not able to do dynamic casts. As a result of that, Query subclasses also \em cannot be
     * used as arguments in slots connected to GObject signals, even though you may know that your
     * slot will only be called with that type of queries.
     */
class Query : public MiniObject
{
    QGST_WRAPPER(Query)
public:
    static QueryPtr create(QueryType type, const StructureBase & structure = SharedStructure(NULL));

    QString name() const;
    QueryType type() const;

    SharedStructure structure();
    const SharedStructure structure() const;

protected:
    //Workaround while waiting for global refCount hash, REMOVE in the near future
    virtual void ref();
    virtual void unref();

};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::PositionQuery
 */
class PositionQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(PositionQuery, Query)
public:
    static PositionQueryPtr create(Format format);

    Format format() const;
    qint64 position() const;
    void setPosition(Format format, qint64 position);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::DurationQuery
 */
class DurationQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(DurationQuery, Query)
public:
    static DurationQueryPtr create(Format format);

    Format format() const;
    qint64 duration() const;
    void setDuration(Format format, qint64 duration);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::LatencyQuery
 */
class LatencyQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(LatencyQuery, Query)
public:
    static LatencyQueryPtr create();

    bool hasLive() const;
    ClockTime minimumLatency() const;
    ClockTime maximumLatency() const;

    void setLatency(bool live, ClockTime minimumLatency, ClockTime maximumLatency);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::SeekingQuery
 */
class SeekingQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(SeekingQuery, Query)
public:
    static SeekingQueryPtr create(Format format);

    Format format() const;
    bool seekable() const;
    qint64 segmentStart() const;
    qint64 segmentEnd() const;

    void setSeeking(Format format, bool seekable, qint64 segmentStart, qint64 segmentEnd);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::SegmentQuery
 */
class SegmentQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(SegmentQuery, Query)
public:
    static SegmentQueryPtr create(Format format);

    Format format() const;
    double rate() const;
    qint64 startValue() const;
    qint64 stopValue() const;

    void setSegment(Format format, double rate, qint64 startValue, qint64 stopValue);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::ConvertQuery
 */
class ConvertQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(ConvertQuery, Query)
public:
    static ConvertQueryPtr create(Format sourceFormat, qint64 value, Format destinationFormat);

    Format sourceFormat() const;
    qint64 sourceValue() const;
    Format destinationFormat() const;
    qint64 destinationValue() const;

    void setConvert(Format sourceFormat, qint64 sourcevalue, Format destinationFormat,
                    qint64 destinationValue);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::FormatsQuery
 */
class FormatsQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(FormatsQuery, Query)
public:
    static FormatsQueryPtr create();

    QList<Format> formats();

    void setFormats(QList<Format> formats);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::BufferingQuery
 */
class BufferingQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(BufferingQuery, Query)
public:
    static BufferingQueryPtr create(Format format);

    bool isBusy() const;
    int percent() const;

    void setPercent(bool busy, int percent);

    BufferingMode mode() const;
    int averageIn() const;
    int averageOut() const;
    qint64 bufferingLeft() const;

    void setStats(BufferingMode mode, int averageIn, int averageOut, qint64 bufferingLeft);

    Format format() const;
    qint64 rangeStart() const;
    qint64 rangeStop() const;
    qint64 estimatedTotal() const;

    void setRange(Format rangeFormat, qint64 rangeStart, qint64 rangeStop, qint64 estimatedTotal);
};

/*! \headerfile query.h <QGst/Query>
 * \brief Wrapper class for queries of type QGst::UriQuery
 */
class UriQuery : public Query
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(UriQuery, Query)
public:
    static UriQueryPtr create();

    QUrl uri() const;

    void setUri(const QUrl & uri);
};

} //namespace QGst


#define QGST_QUERY_SUBCLASS_REGISTER_CONVERTERS(CLASS, QUERYTYPE) \
    namespace QGlib { \
    namespace Private { \
        template <> \
        struct CanConvertTo<CLASS> \
        { \
            static inline bool from(void *instance) \
            { \
                return (Type::fromInstance(instance).isA(GetType<QGst::Query>()) && \
                        QGst::QueryPtr::wrap(static_cast<GstQuery*>(instance))->type() \
                        == QUERYTYPE); \
            } \
            /* NO 'static inline bool from(Type t)' to disallow usage from Value::get */ \
            /* ValueBase::get is not supposed to provide dynamic_cast capabilities */ \
        }; \
        \
        template <> \
        struct CanConvertFrom<CLASS##Ptr> \
        { \
            static inline bool to(Type t) \
            { \
                return GetType<QGst::Query>().isA(t); \
            } \
        }; \
    } /* namespace Private */ \
    } /* namespace QGlib */

#define QGST_QUERY_SUBCLASS_REGISTER_VALUEIMPL(CLASSPTR, QUERYTYPE) \
    namespace QGlib { \
        template<> \
        struct ValueImpl<CLASSPTR> \
        { \
            static void set(ValueBase & value, const CLASSPTR & data) { \
                ValueImpl<QGst::QueryPtr>::set(value, data); \
            } \
        }; \
    } /* namespace QGlib */

#define QGST_REGISTER_QUERY_SUBCLASS(TYPE) \
    QGST_QUERY_SUBCLASS_REGISTER_CONVERTERS(QGst::TYPE##Query, QGst::Query##TYPE) \
    QGST_QUERY_SUBCLASS_REGISTER_VALUEIMPL(QGst::TYPE##QueryPtr, QGst::Query##TYPE)

QGLIB_REGISTER_TYPE(QGst::Query)
QGLIB_REGISTER_VALUEIMPL(QGst::QueryPtr)
QGST_REGISTER_QUERY_SUBCLASS(Position)
QGST_REGISTER_QUERY_SUBCLASS(Duration)
QGST_REGISTER_QUERY_SUBCLASS(Latency)
QGST_REGISTER_QUERY_SUBCLASS(Seeking)
QGST_REGISTER_QUERY_SUBCLASS(Segment)
QGST_REGISTER_QUERY_SUBCLASS(Convert)
QGST_REGISTER_QUERY_SUBCLASS(Formats)
QGST_REGISTER_QUERY_SUBCLASS(Buffering)
QGST_REGISTER_QUERY_SUBCLASS(Uri)

#undef QGST_REGISTER_QUERY_SUBCLASS
#undef QGST_QUERY_SUBCLASS_REGISTER_VALUEIMPL
#undef QGST_QUERY_SUBCLASS_REGISTER_CONVERTERS

/*! \relates QGst::Query */
QDebug operator<<(QDebug debug, QGst::QueryType type);

/*! \relates QGst::Query */
QDebug operator<<(QDebug debug, const QGst::QueryPtr & query);

#endif
