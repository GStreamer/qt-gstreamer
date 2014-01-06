/*
    Copyright (C) 2012 Openismus GmbH
      @author Mathias Hasselmann <mathias@openismus.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "qgsttest.h"

#include <QGlib/Connect>
#include <QGlib/Error>

#include <QGst/Caps>
#include <QGst/ClockTime>
#include <QGst/Discoverer>

#include <gst/pbutils/pbutils.h>

// Declare a simple tag list that actually supports iteration. Needed for our test data.
typedef QPair<const char *, QGlib::Value> Tag;
typedef QList<Tag> TagList;

static Tag makeTag(const char *name, const QGlib::Value &value)
{
    return qMakePair(name, value);
}

namespace QGlib {

// Declare a simple compare operator for QGlib::Value.
// Comparing by string value isn't accurate at all, but good enough for our testing purposes.
static bool operator ==(const Value &a, const Value &b)
{
    bool okA = false, okB = false;
    return a.type() == b.type() && a.toString(&okA) == b.toString(&okB) && okA && okB;
}

static bool operator !=(const Value &a, const Value &b)
{
    return !(a == b);
}

} //namespace QGlib

// This classes describes what kind of streams we expect.
class StreamInfo : public QSharedData
{
    Q_DISABLE_COPY(StreamInfo)

public:
    enum DebugMode
    {
        NoDebug,
        Debug
    };

    enum CapsMode
    {
        AutoCaps,
        ManualCaps
    };

    virtual ~StreamInfo() {}

protected:
    StreamInfo(const QString &caps, CapsMode capsMode)
        : m_isNative(true)
        , m_caps(QGst::Caps::fromString(caps))
        , m_capsMode(capsMode)
    {
    }

    template<typename T>
    class Field
    {
    public:
        Field()
            : m_value(T())
            , m_assigned(false)
        {
        }

        Field & operator =(const T &value)
        {
            m_value = value;
            m_assigned = true;
            return *this;
        }

        bool operator ==(const T &other) const
        {
            return !m_assigned || m_value == other;
        }

        bool operator !=(const T &other) const
        {
            return !operator =(other);
        }

    private:
        T m_value;
        bool m_assigned;
    };

public:
    virtual bool acceptStreamType(QGlib::Type streamType) const
    {
        return streamType.isA(GST_TYPE_DISCOVERER_STREAM_INFO);
    }

    virtual bool acceptStream(QGst::DiscovererStreamInfoPtr info) const
    {
        return acceptStream(info, NoDebug);
    }

    virtual bool acceptStream(QGst::DiscovererStreamInfoPtr info, DebugMode debugMode) const
    {
        if (info.isNull()) {
            if (debugMode) {
                qDebug() << "stream info is null";
            }
            return false;
        }

        if (info->tags().isEmpty() != m_tags.isEmpty()) {
            if (debugMode) {
                qDebug() << (m_tags.isEmpty() ? "tags should be empty, but aren't"
                                              : "none of the expected tags found");
                qDebug() << m_tags;
            }

            return false;
        }

        Q_FOREACH(const Tag &tag, m_tags) {
            if (info->tags().tagValue(tag.first) != tag.second) {
                if (debugMode) {
                    qDebug() << tag.first << "tag has bad value\n"
                             << "  expected value:" << tag.second << "\n"
                             << "  actual value: " << info->tags().tagValue(tag.first);
                }
                return false;
            }
        }

        if (!info->caps()->isAlwaysCompatibleWith(m_caps)) {
            if (debugMode) {
                qDebug() << "capabilities are incompatible\n"
                         << "  expected caps:" << m_caps->toString() << "\n"
                         << "  actual caps: " << info->caps()->toString();
            }

            return false;
        }

        return true;
    }

    StreamInfo * setNative(bool value) { m_isNative = value; return this; }
    bool isNative() const { return m_isNative; }

    StreamInfo * addTag(const char *name, const QGlib::Value &value)
    {
        m_tags += makeTag(name, value);
        return this;
    }

    StreamInfo * addCap(const char *name, const QGlib::Value &value)
    {
        if (m_capsMode == AutoCaps) {
            m_caps->setValue(name, value);
        }

        return this;
    }

    bool isImage() const
    {
        return m_caps->toString().startsWith("image/");
    }

private:
    bool m_isNative;
    TagList m_tags;
    QGst::CapsPtr m_caps;
    CapsMode m_capsMode;
};

class AudioStreamInfo : public StreamInfo
{
public:
    AudioStreamInfo(const QString &caps, CapsMode capsMode = AutoCaps)
        : StreamInfo(caps, capsMode)
    {
    }

    bool acceptStreamType(QGlib::Type streamType) const
    {
        return streamType.isA(GST_TYPE_DISCOVERER_AUDIO_INFO);
    }

    bool acceptStream(QGst::DiscovererStreamInfoPtr info) const
    {
        QGst::DiscovererAudioInfoPtr audioInfo = info.dynamicCast<QGst::DiscovererAudioInfo>();

        return !audioInfo.isNull()
                && StreamInfo::acceptStream(info)
                && m_channels == audioInfo->channels()
                && m_sampleRate == audioInfo->sampleRate()
                && m_bitrate == audioInfo->bitrate()
                && m_depth == audioInfo->depth();
    }

    AudioStreamInfo * setChannels(uint value) { m_channels = value; addCap("channels", int(value)); return this; }
    AudioStreamInfo * setSampleRate(uint value) { m_sampleRate = value; addCap("rate", int(value)); return this; }
    AudioStreamInfo * setBitrate(uint value) { m_bitrate = value; addTag("bitrate", value); return this; }
    AudioStreamInfo * setDepth(uint value) { m_depth = value; return this; }

private:
    Field<uint> m_channels;
    Field<uint> m_sampleRate;
    Field<uint> m_bitrate;
    Field<uint> m_depth;
};

class VideoStreamInfo : public StreamInfo
{
public:
    VideoStreamInfo(const QString &caps, CapsMode capsMode = AutoCaps)
        : StreamInfo(caps, capsMode)
    {
        if (capsMode != AutoCaps) {
            qWarning("working around PNG decoder bug");
        }
    }

    bool acceptStreamType(QGlib::Type streamType) const
    {
        return streamType.isA(GST_TYPE_DISCOVERER_VIDEO_INFO);
    }

    bool acceptStream(QGst::DiscovererStreamInfoPtr info) const
    {
        QGst::DiscovererVideoInfoPtr videoInfo = info.dynamicCast<QGst::DiscovererVideoInfo>();

        return !videoInfo.isNull()
                && StreamInfo::acceptStream(info)
                && isImage() == videoInfo->isImage()
                && m_width == videoInfo->width()
                && m_height == videoInfo->height()
                && m_interlaced == videoInfo->isInterlaced()
                && m_bitrate == videoInfo->bitrate()
                && m_framerate == videoInfo->framerate()
                && m_pixelAspectRatio == videoInfo->pixelAspectRatio();
    }

    VideoStreamInfo * setWidth(uint value) { m_width = value; addCap("width", int(value)); return this; }
    VideoStreamInfo * setHeight(uint value) { m_height = value; addCap("height", int(value)); return this; }
    VideoStreamInfo * setInterlaced(bool value) { m_interlaced = value; return this; }
    VideoStreamInfo * setBitrate(uint value) { m_bitrate = value; addTag("bitrate", value); return this; }
    VideoStreamInfo * setFramerate(const QGst::Fraction &value) { m_framerate = value; addCap("framerate", QGlib::Value::create(value)); return this; }
    VideoStreamInfo * setPixelAspectRatio(const QGst::Fraction &value) { m_pixelAspectRatio = value; addCap("pixel-aspect-ratio", QGlib::Value::create(value)); return this; }

private:
    Field<uint> m_width;
    Field<uint> m_height;
    Field<bool> m_interlaced;
    Field<uint> m_bitrate;
    Field<QGst::Fraction> m_framerate;
    Field<QGst::Fraction> m_pixelAspectRatio;
};

typedef QSharedDataPointer<StreamInfo> StreamInfoPtr;
typedef QList<StreamInfoPtr> StreamInfoList;

// A few filtering algorithms
template<class Container, class UnaryPredicate>
static Container filter(const Container &input, UnaryPredicate predicate)
{
    Container result;

    const typename Container::ConstIterator end = input.constEnd();
    for(typename Container::ConstIterator it = input.constBegin(); it != end; ++it) {
        if (predicate(it->data())) {
            result += *it;
        }
    }

    return result;
}

static StreamInfoList filterByStreamType(const StreamInfoList &streams, QGlib::Type streamType)
{
    return filter(streams, std::bind2nd(std::mem_fun(&StreamInfo::acceptStreamType), streamType));
}

// Declare a few metatypes, so that we can use this types for test data.
Q_DECLARE_METATYPE(QGst::ClockTime)
Q_DECLARE_METATYPE(StreamInfoList)
Q_DECLARE_METATYPE(TagList)

// The actual test
class DiscovererTest : public QGstTest
{
    Q_OBJECT

public:
    enum DiscoveryState {
        DiscoveryPending,
        DiscoveryStarted,
        UriDiscovered,
        DiscoveryFinished
    };

private:
    void setupDiscoveryData();
    void verifyStreamInfo(QGst::DiscovererInfoPtr info);

private Q_SLOTS:
    void cleanup();

    void testGLibTypes_data();
    void testGLibTypes();

    void testSyncDiscovery_data();
    void testSyncDiscovery();

    void testAsyncDiscovery_data();
    void testAsyncDiscovery();

protected: // mark as protected to avoid that QTestLib invoking those methods as test
    void onStartingDiscovery();
    void onUriDiscovered(QGst::DiscovererInfoPtr info, const QGlib::Error &error);
    void onDiscoveryFinished();

private:
    QScopedPointer<QEventLoop> m_eventLoop;
    DiscoveryState m_discoveryState;
};

namespace QTest {
template<> char *toString(const DiscovererTest::DiscoveryState &state)
{
    switch(state) {
    case DiscovererTest::DiscoveryPending: return toString("discovery-pending");
    case DiscovererTest::DiscoveryStarted: return toString("discovery-started");
    case DiscovererTest::UriDiscovered: return toString("uri-discovered");
    case DiscovererTest::DiscoveryFinished: return toString("discovery-finished");
    }

    return toString(uint(state));
}
}

void DiscovererTest::setupDiscoveryData()
{
    QTest::addColumn<QUrl>("uri");
    QTest::addColumn<int>("errorCode");
    QTest::addColumn<QString>("errorDomain");
    QTest::addColumn<QGst::ClockTime>("duration");
    QTest::addColumn<bool>("seekable");
    QTest::addColumn<StreamInfoList>("streams");
    QTest::addColumn<TagList>("expectedTags");

    const bool Seekable = true;
    const bool NonSeekable = false;
    const QUrl baseUrl = QUrl::fromLocalFile(QString::fromLocal8Bit(SRCDIR) + "/");

    QTest::newRow("null")
            << QUrl("about:null")
            << int(GST_CORE_ERROR_MISSING_PLUGIN)
            << QString::fromUtf8(g_quark_to_string(GST_CORE_ERROR))
            << QGst::ClockTime(0) << Seekable
            << StreamInfoList()
            << TagList();
    QTest::newRow("numbers.ogv")
            << baseUrl.resolved(QUrl::fromEncoded("data/numbers.ogv")) << 0 << QString()
            << QGst::ClockTime(2017333333) << Seekable
            << (StreamInfoList()
                << StreamInfoPtr((new AudioStreamInfo("audio/x-flac, framed=true"))
                                 ->setSampleRate(48000)->setChannels(1)->setDepth(16)
                                 ->addTag("audio-codec", "FLAC")
                                 ->addTag("container-format", "Ogg"))
                << StreamInfoPtr((new AudioStreamInfo("audio/x-flac"))
                                 ->setSampleRate(48000)->setChannels(1)
                                 ->setNative(false))
                << StreamInfoPtr((new VideoStreamInfo("video/x-theora"))
                                 ->setWidth(160)->setHeight(120)->setInterlaced(false)
                                 ->setBitrate(200000)->setFramerate(QGst::Fraction(5, 1))
                                 ->setPixelAspectRatio(QGst::Fraction(1, 1))
                                 ->addTag("container-format", "Ogg")
                                 ->addTag("video-codec", "Theora")))
            << (TagList()
                << makeTag("container-format", "Ogg")
                << makeTag("audio-codec", "FLAC")
                << makeTag("video-codec", "Theora")
                << makeTag("bitrate", 200000U));
    QTest::newRow("numbers07.png")
            << baseUrl.resolved(QUrl::fromEncoded("data/numbers07.png")) << 0 << QString()
            << QGst::ClockTime(0) << Seekable
            << (StreamInfoList()
                << StreamInfoPtr((new VideoStreamInfo("image/png", VideoStreamInfo::ManualCaps))
                                 ->setWidth(160)->setHeight(120)->setInterlaced(false)))
            << (TagList());
    QTest::newRow("numbers07.jpg")
            << baseUrl.resolved(QUrl::fromEncoded("data/numbers07.jpg")) << 0 << QString()
            << QGst::ClockTime(0) << NonSeekable
            << (StreamInfoList()
                << StreamInfoPtr((new VideoStreamInfo("image/jpeg"))
                                 ->setWidth(120)->setHeight(160)->setInterlaced(false)))
            << (TagList());
    QTest::newRow("sine.ogg")
            << baseUrl.resolved(QUrl::fromEncoded("data/sine.ogg")) << 0 << QString()
            << QGst::ClockTime::fromSeconds(2) << Seekable
            << (StreamInfoList()
                << StreamInfoPtr((new AudioStreamInfo("audio/x-vorbis"))
                                 ->setSampleRate(48000)->setChannels(1)
                                 ->setBitrate(80000)
                                 ->addTag("container-format", "Ogg")
                                 ->addTag("audio-codec", "Vorbis")))
            << (TagList()
                << makeTag("container-format", "Ogg")
                << makeTag("audio-codec", "Vorbis")
                << makeTag("bitrate", 80000U));
}

void DiscovererTest::verifyStreamInfo(QGst::DiscovererInfoPtr info)
{
    // verify discovery result
    QVERIFY(!info.isNull());
    QTEST(info->uri(), "uri");
    QTEST(QString(), "errorDomain");
    QCOMPARE(info->result(), QGst::DiscovererOk);
    QTEST(info->duration(), "duration");
    QTEST(info->seekable(), "seekable");
    QVERIFY(!info->misc().isValid());

    QFETCH(TagList, expectedTags);
    QCOMPARE(info->tags().isEmpty(), expectedTags.isEmpty());

    Q_FOREACH(const Tag &tag, expectedTags) {
        QCOMPARE(info->tags().tagValue(tag.first), tag.second);
    }

    QFETCH(StreamInfoList, streams);
    QVERIFY(!info->streamInfo().isNull());

    if (streams.count() != 1 || !(*streams.constBegin())->isImage()) {
        const QGst::DiscovererContainerInfoPtr root = info->streamInfo().dynamicCast<QGst::DiscovererContainerInfo>();
        QVERIFY(!root.isNull());

        const StreamInfoList nativeStreams = filter(streams, std::mem_fun(&StreamInfo::isNative));
        QCOMPARE(root->streams().count(), nativeStreams.count());
    } else {
        const QGst::DiscovererVideoInfoPtr root = info->streamInfo().dynamicCast<QGst::DiscovererVideoInfo>();
        QVERIFY(!root.isNull());
        QVERIFY(root->isImage());
    }

    const StreamInfoList audioStreams = filterByStreamType(streams, GST_TYPE_DISCOVERER_AUDIO_INFO);
    QCOMPARE(info->streams(GST_TYPE_DISCOVERER_AUDIO_INFO).count(), audioStreams.count());
    QCOMPARE(info->audioStreams().count(), audioStreams.count());

    const StreamInfoList videoStreams = filterByStreamType(streams, GST_TYPE_DISCOVERER_VIDEO_INFO);
    QCOMPARE(info->streams(GST_TYPE_DISCOVERER_VIDEO_INFO).count(), videoStreams.count());
    QCOMPARE(info->videoStreams().count(), videoStreams.count());

    const StreamInfoList subtitleStreams = filterByStreamType(streams, GST_TYPE_DISCOVERER_SUBTITLE_INFO);
    QCOMPARE(info->streams(GST_TYPE_DISCOVERER_SUBTITLE_INFO).count(), subtitleStreams.count());
    QCOMPARE(info->subtitleStreams().count(), subtitleStreams.count());

    const StreamInfoList containerStreams = filterByStreamType(streams, GST_TYPE_DISCOVERER_CONTAINER_INFO);
    QCOMPARE(info->streams(GST_TYPE_DISCOVERER_CONTAINER_INFO).count(), containerStreams.count());
    QCOMPARE(info->containerStreams().count(), containerStreams.count());

    QCOMPARE(info->streams(GST_TYPE_DISCOVERER_STREAM_INFO).count(), streams.count());
    QCOMPARE(info->streams().count(), streams.count());

    Q_FOREACH(const QGst::DiscovererStreamInfoPtr &streamInfo, info->streams()) {
        const StreamInfoList::ConstIterator it =
                std::find_if(streams.constBegin(), streams.constEnd(),
                             std::bind2nd(std::mem_fun(&StreamInfo::acceptStream), streamInfo));

        if (it == streams.constEnd()) {
            Q_FOREACH(const StreamInfoPtr expectedInfo, streams) {
                expectedInfo->acceptStream(streamInfo, StreamInfo::Debug);
            }

            QFAIL(qPrintable("Unexpected stream: " + streamInfo->caps()->toString()));
        }

        streams.removeAt(it - streams.constBegin());
        QVERIFY(!streamInfo->misc().isValid());
    }

    QVERIFY(streams.isEmpty());
}

void DiscovererTest::cleanup()
{
    m_eventLoop.reset();
}

void DiscovererTest::testGLibTypes_data()
{
    QTest::addColumn<QString>("wrappedTypeName");
    QTest::addColumn<QString>("expectedTypeName");

    QTest::newRow("discoverer")
            << QGlib::GetType<QGst::Discoverer>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER));
    QTest::newRow("discoverer-info")
            << QGlib::GetType<QGst::DiscovererInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_INFO));
    QTest::newRow("discoverer-stream-info")
            << QGlib::GetType<QGst::DiscovererStreamInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_STREAM_INFO));
    QTest::newRow("discoverer-container-info")
            << QGlib::GetType<QGst::DiscovererContainerInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_CONTAINER_INFO));
    QTest::newRow("discoverer-audio-info")
            << QGlib::GetType<QGst::DiscovererAudioInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_AUDIO_INFO));
    QTest::newRow("discoverer-video-info")
            << QGlib::GetType<QGst::DiscovererVideoInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_VIDEO_INFO));
    QTest::newRow("discoverer-subtitle-info")
            << QGlib::GetType<QGst::DiscovererSubtitleInfo>().name()
            << QString::fromLatin1(g_type_name(GST_TYPE_DISCOVERER_SUBTITLE_INFO));
}

void DiscovererTest::testGLibTypes()
{
    QFETCH(QString, wrappedTypeName);
    QFETCH(QString, expectedTypeName);

    QCOMPARE(wrappedTypeName, expectedTypeName);
}

void DiscovererTest::testSyncDiscovery_data()
{
    setupDiscoveryData();
}

void DiscovererTest::testSyncDiscovery()
{
    // create a discoverer
    QGst::DiscovererPtr discoverer = QGst::Discoverer::create(QGst::ClockTime::fromSeconds(1));
    QVERIFY(!discoverer.isNull());

    // test discovery request
    QGst::DiscovererInfoPtr info;
    QFETCH(QUrl, uri);

    try {
        info = discoverer->discoverUri(uri);
    } catch(const QGlib::Error &error) {
        QTEST(error.domain().toString(), "errorDomain");
        QTEST(error.code(), "errorCode");
        return;
    }

    verifyStreamInfo(info);
}

void DiscovererTest::testAsyncDiscovery_data()
{
    setupDiscoveryData();
}

void DiscovererTest::testAsyncDiscovery()
{
// glib event loop required - see QCoreApplicationPrivate::createEventDispatcher() for the defines check
#if defined(Q_OS_WIN) || defined(Q_OS_BLACKBERRY) || defined(QT_NO_GLIB)
    QSKIP_PORT("Platform does not have a GLib event loop", SkipAll);
#endif

    // setup discovery timeout
    m_eventLoop.reset(new QEventLoop);
    QTimer::singleShot(3000, m_eventLoop.data(), SLOT(quit()));

    // create a discoverer
    m_discoveryState = DiscoveryPending;
    QGst::DiscovererPtr discoverer = QGst::Discoverer::create(QGst::ClockTime::fromSeconds(1));
    QVERIFY(!discoverer.isNull());

    QVERIFY(QGlib::connect(discoverer, "starting", this, &DiscovererTest::onStartingDiscovery, 0/*QGlib::PassSender*/));
    QVERIFY(QGlib::connect(discoverer, "discovered", this, &DiscovererTest::onUriDiscovered));
    QVERIFY(QGlib::connect(discoverer, "finished", this, &DiscovererTest::onDiscoveryFinished, QGlib::PassSender));

    // place URI to discover
    QFETCH(QUrl, uri);
    discoverer->start();
    QVERIFY(discoverer->discoverUriAsync(uri));

    // verify discovery succeeded
    QVERIFY2(m_eventLoop->exec(), "Discovery timeout out");
    QCOMPARE(m_discoveryState, DiscoveryFinished);
}

void DiscovererTest::onStartingDiscovery()
{
    QCOMPARE(m_discoveryState, DiscoveryPending);
    m_discoveryState = DiscoveryStarted;
}

void DiscovererTest::onUriDiscovered(QGst::DiscovererInfoPtr info, const QGlib::Error &error)
{
    QCOMPARE(m_discoveryState, DiscoveryStarted);
    m_discoveryState = UriDiscovered;

    if (error) {
        QTEST(error.domain().toString(), "errorDomain");
        QTEST(error.code(), "errorCode");
        return;
    }

    verifyStreamInfo(info);
}

void DiscovererTest::onDiscoveryFinished()
{
    m_eventLoop->exit(1);
    QCOMPARE(m_discoveryState, UriDiscovered);
    m_discoveryState = DiscoveryFinished;
}

QTEST_MAIN(DiscovererTest)

#include "moc_qgsttest.cpp"
#include "discoverertest.moc"
