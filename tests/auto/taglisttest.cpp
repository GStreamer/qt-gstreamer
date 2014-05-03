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
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "qgsttest.h"
#include <QGst/TagList>
#include <QGst/Buffer>
#include <QGst/Caps>
#include <QGst/Sample>
#include <QGst/Structure>
#include <QGst/Segment>
#include <QtCore/QDate>

class TagListTest : public QGstTest
{
    Q_OBJECT
private Q_SLOTS:
    void simpleTest();
    void dateTest();
    void dateTimeTest();
    void copyTest();
    void stringsTest();
    void sampleTest();
    void numericTest();
};

void TagListTest::simpleTest()
{
    QGst::TagList tl;
    tl.setTitle("abc");
    QCOMPARE(tl.titleCount(), 1);
    QCOMPARE(tl.title(), QString("abc"));
    //append and verify multiple values
    tl.setTitle("bcd", QGst::TagMergeAppend);
    QCOMPARE(tl.titleCount(), 2);
    QCOMPARE(tl.title(), QString("abc"));
    QCOMPARE(tl.title(1), QString("bcd"));

    //try to access bad tag index
    QVERIFY(tl.title(2).isNull());

    //now use the generic form to set a tag by name
    QString s3("def");
    tl.setTagValue("title", s3, QGst::TagMergeReplaceAll);
    //check both the shortcut and the generic value accessor
    QCOMPARE(tl.title(), s3);
    QCOMPARE(tl.tagValue("title").get<QString>(), s3);

    //try to get a tag not set yet
    QVERIFY(tl.artist().isNull());
    QVERIFY(!tl.artistCount());
    QGlib::Value val = tl.tagValue("artist");
    QVERIFY(!val.isValid());

    //uint tag
    QVERIFY(!tl.trackNumber());
    tl.setTrackNumber(7);
    QCOMPARE(tl.trackNumber(), (quint32) 7);
}

void TagListTest::dateTest()
{
    QGst::TagList tl;
    QDate date(1969, 8, 4);
    tl.setDate(date);
    QCOMPARE(tl.date(), date);
    //replace and retest
    QDate date2(100, 12, 30);
    tl.setDate(date2);
    QCOMPARE(tl.date(), date2);
}

void TagListTest::dateTimeTest()
{
    QGst::TagList tl;
    QCOMPARE(tl.dateTime(), QDateTime());

    QDate date(1969, 8, 4);
    QTime time(1, 10, 15, 432);
    QDateTime dateTime(date, time);
    tl.setDateTime(dateTime);
    QCOMPARE(tl.dateTime(), dateTime);

    //replace and retest
    QDate date2(100, 12, 30);
    QTime time2(14, 50, 38, 0);
    QDateTime dateTime2(date2, time2);
    tl.setDateTime(dateTime2);
    QCOMPARE(tl.dateTime(), dateTime2);
}

void TagListTest::copyTest()
{
    QGst::TagList tl;
    tl.setTitle("abc");
    tl.setDuration(1234567);
    tl.setBitrate(320);
    tl.setAlbumGain(0.85);
    QGst::BufferPtr buffer = QGst::Buffer::create(10);
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    caps->setValue("width", 320);
    caps->setValue("height", 240);
    QGst::SamplePtr sample = QGst::Sample::create(buffer, caps, QGst::Segment(), QGst::Structure());
    tl.setImage(sample);

    QGst::TagList tl2(tl);
    QCOMPARE(tl.title(), tl2.title());
    QCOMPARE(tl.duration(), tl2.duration());
    QCOMPARE(tl.bitrate(), tl2.bitrate());
    QCOMPARE(tl.albumGain(), tl2.albumGain());

    QGst::SamplePtr sample2 = tl2.image();
    QVERIFY(!sample2.isNull());
    QGst::CapsPtr caps2 = sample2->caps();
    QGst::StructurePtr structure = caps2->internalStructure(0);
    QCOMPARE(structure->name(), QString("video/x-raw"));
    int width = structure->value("width").get<int>();
    QCOMPARE(width, 320);
}

void TagListTest::stringsTest()
{
    QGst::TagList tl;
    QString s1;
    QString s2;

    s1 = "title";
    tl.setTitle(s1);
    QCOMPARE(tl.title(), s1);
    s2 = "title2";
    tl.setTitle(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.title(1), s2);
    QCOMPARE(tl.titleCount(), 2);

    s1 = "titlesortname";
    tl.setTitleSortName(s1);
    QCOMPARE(tl.titleSortName(), s1);

    s1 = "artist";
    tl.setArtist(s1);
    QCOMPARE(tl.artist(), s1);
    s2 = "artist2";
    tl.setArtist(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.artist(1), s2);
    QCOMPARE(tl.artistCount(), 2);

    s1 = "artistsortname";
    tl.setArtistSortName(s1);
    QCOMPARE(tl.artistSortName(), s1);

    s1 = "composer";
    tl.setComposer(s1);
    QCOMPARE(tl.composer(), s1);
    s2 = "composer2";
    tl.setComposer(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.composer(1), s2);
    QCOMPARE(tl.composerCount(), 2);

    s1 = "genre";
    tl.setGenre(s1);
    QCOMPARE(tl.genre(), s1);
    s2 = "genre2";
    tl.setGenre(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.genre(1), s2);
    QCOMPARE(tl.genreCount(), 2);

    s1 = "comment";
    tl.setComment(s1);
    QCOMPARE(tl.comment(), s1);
    s2 = "comment2";
    tl.setComment(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.comment(1), s2);
    QCOMPARE(tl.commentCount(), 2);

    s1 = "extendedcomment";
    tl.setExtendedComment(s1);
    QCOMPARE(tl.extendedComment(), s1);
    s2 = "extendedcomment2";
    tl.setExtendedComment(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.extendedComment(1), s2);
    QCOMPARE(tl.extendedCommentCount(), 2);

    s1 = "location";
    tl.setLocation(s1);
    QCOMPARE(tl.location(), s1);
    s2 = "location2";
    tl.setLocation(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.location(1), s2);
    QCOMPARE(tl.locationCount(), 2);

    s1 = "homepage";
    tl.setHomepage(s1);
    QCOMPARE(tl.homepage(), s1);
    s2 = "homepage2";
    tl.setHomepage(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.homepage(1), s2);
    QCOMPARE(tl.homepageCount(), 2);

    s1 = "description";
    tl.setDescription(s1);
    QCOMPARE(tl.description(), s1);
    s2 = "description2";
    tl.setDescription(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.description(1), s2);
    QCOMPARE(tl.descriptionCount(), 2);

    s1 = "version";
    tl.setVersion(s1);
    QCOMPARE(tl.version(), s1);

    s1 = "isrc";
    tl.setIsrc(s1);
    QCOMPARE(tl.isrc(), s1);

    s1 = "organization";
    tl.setOrganization(s1);
    QCOMPARE(tl.organization(), s1);
    s2 = "organization2";
    tl.setOrganization(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.organization(1), s2);
    QCOMPARE(tl.organizationCount(), 2);

    s1 = "copyright";
    tl.setCopyright(s1);
    QCOMPARE(tl.copyright(), s1);

    s1 = "copyrightUri";
    tl.setCopyrightUri(s1);
    QCOMPARE(tl.copyrightUri(), s1);

    s1 = "contact";
    tl.setContact(s1);
    QCOMPARE(tl.contact(), s1);

    s1 = "license";
    tl.setLicense(s1);
    QCOMPARE(tl.license(), s1);

    s1 = "licenseUri";
    tl.setLicenseUri(s1);
    QCOMPARE(tl.licenseUri(), s1);

    s1 = "performer";
    tl.setPerformer(s1);
    QCOMPARE(tl.performer(), s1);
    s2 = "performer2";
    tl.setPerformer(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.performer(1), s2);
    QCOMPARE(tl.performerCount(), 2);

    s1 = "codec";
    tl.setCodec(s1);
    QCOMPARE(tl.codec(), s1);

    s1 = "videoCodec";
    tl.setVideoCodec(s1);
    QCOMPARE(tl.videoCodec(), s1);

    s1 = "audioCodec";
    tl.setAudioCodec(s1);
    QCOMPARE(tl.audioCodec(), s1);

    s1 = "subtitleCodec";
    tl.setSubtitleCodec(s1);
    QCOMPARE(tl.subtitleCodec(), s1);

    s1 = "containerFormat";
    tl.setContainerFormat(s1);
    QCOMPARE(tl.containerFormat(), s1);

    s1 = "encoder";
    tl.setEncoder(s1);
    QCOMPARE(tl.encoder(), s1);

    s1 = "languageCode";
    tl.setLanguageCode(s1);
    QCOMPARE(tl.languageCode(), s1);

    s1 = "keywords";
    tl.setKeywords(s1);
    QCOMPARE(tl.keywords(), s1);
    s2 = "keywords2";
    tl.setKeywords(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.keywords(1), s2);
    QCOMPARE(tl.keywordsCount(), 2);

    s1 = "geoLocationName";
    tl.seGeoLocationName(s1);
    QCOMPARE(tl.geoLocationName(), s1);

    s1 = "geoLocationCountry";
    tl.setGeoLocationCountry(s1);
    QCOMPARE(tl.geoLocationCountry(), s1);

    s1 = "geoLocationCity";
    tl.setGeoLocationCity(s1);
    QCOMPARE(tl.geoLocationCity(), s1);

    s1 = "geoLocationSublocation";
    tl.setGeoLocationSublocation(s1);
    QCOMPARE(tl.geoLocationSublocation(), s1);

    s1 = "showName";
    tl.setShowName(s1);
    QCOMPARE(tl.showName(), s1);
    s2 = "showName2";
    tl.setShowName(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.showName(1), s2);
    QCOMPARE(tl.showNameCount(), 2);

    s1 = "showSortName";
    tl.setShowSortName(s1);
    QCOMPARE(tl.showSortName(), s1);

    s1 = "lyrics";
    tl.setLyrics(s1);
    QCOMPARE(tl.lyrics(), s1);
    s2 = "lyrics2";
    tl.setLyrics(s2, QGst::TagMergeAppend);
    QCOMPARE(tl.lyrics(1), s2);
    QCOMPARE(tl.lyricsCount(), 2);

    s1 = "composerSortName";
    tl.setComposerSortName(s1);
    QCOMPARE(tl.composerSortName(), s1);

    s1 = "grouping";
    tl.setGrouping(s1);
    QCOMPARE(tl.grouping(), s1);

    s1 = "deviceManufacturer";
    tl.setDeviceManufacturer(s1);
    QCOMPARE(tl.deviceManufacturer(), s1);

    s1 = "deviceModel";
    tl.setDeviceModel(s1);
    QCOMPARE(tl.deviceModel(), s1);

    s1 = "imageOrientation";
    tl.setImageOrientation(s1);
    QCOMPARE(tl.imageOrientation(), s1);

    qDebug() << tl;
}

void TagListTest::sampleTest()
{
    QGst::TagList tl;
    QGst::BufferPtr buffer = QGst::Buffer::create(10);
    QGst::CapsPtr caps = QGst::Caps::createSimple("video/x-raw");
    caps->setValue("width", 320);
    caps->setValue("height", 240);
    QGst::SamplePtr sample = QGst::Sample::create(buffer, caps, QGst::Segment(), QGst::Structure());
    tl.setImage(sample);

    QGst::BufferPtr bufferb = QGst::Buffer::create(10);
    QGst::CapsPtr capsb = QGst::Caps::createSimple("video/x-raw");
    capsb->setValue("width", 160);
    QGst::SamplePtr sampleb = QGst::Sample::create(bufferb, capsb, QGst::Segment(), QGst::Structure());
    tl.setPreviewImage(sampleb);

    QGst::BufferPtr bufferc = QGst::Buffer::create(10);
    QGst::CapsPtr capsc = QGst::Caps::createSimple("files");
    capsc->setValue("attachment", QString("avalue"));
    QGst::SamplePtr samplec = QGst::Sample::create(bufferc, capsc, QGst::Segment(), QGst::Structure());
    tl.setAttachment(samplec);

    QGst::SamplePtr sample2 = tl.image();
    QGst::CapsPtr caps2 = sample2->caps();
    QGst::StructurePtr structure2 = caps2->internalStructure(0);
    QCOMPARE(structure2->name(), QString("video/x-raw"));
    QCOMPARE(structure2->value("width").get<int>(), 320);

    QGst::SamplePtr sample3 = tl.previewImage();
    QGst::CapsPtr caps3 = sample3->caps();
    QGst::StructurePtr structure3 = caps3->internalStructure(0);
    QCOMPARE(structure3->name(), QString("video/x-raw"));
    QCOMPARE(structure3->value("width").get<int>(), 160);

    QGst::SamplePtr sample4 = tl.attachment();
    QGst::CapsPtr caps4 = sample4->caps();
    QGst::StructurePtr structure4 = caps4->internalStructure(0);
    QCOMPARE(structure4->name(), QString("files"));
    QCOMPARE(structure4->value("attachment").get<QString>(), QString("avalue"));

    //now set multiple samples and verify the count
    tl.setImage(sample3, QGst::TagMergeAppend);
    QCOMPARE(tl.imageCount(), 2);

    tl.setAttachment(sample2, QGst::TagMergePrepend);
    QCOMPARE(tl.attachmentCount(), 2);

}

void TagListTest::numericTest()
{
    QGst::TagList tl;
    quint32 i;
    double d;
    quint64 l;

    l = 123456789;
    tl.setDuration(l);
    QCOMPARE(tl.duration(), (quint64)l );

    i = 1;
    tl.setTrackNumber(i);
    QCOMPARE(tl.trackNumber(), (quint32)i );

    i = 2;
    tl.setTrackCount(i);
    QCOMPARE(tl.trackCount(), (quint32)i );

    i = 3;
    tl.setAlbumVolumeNumber(i);
    QCOMPARE(tl.albumVolumeNumber(), (quint32)i );

    i = 4;
    tl.setAlbumVolumeCount(i);
    QCOMPARE(tl.albumVolumeCount(), (quint32)i );

    i = 5;
    tl.setBitrate(i);
    QCOMPARE(tl.bitrate(), (quint32)i );

    i = 6;
    tl.setNominalBitrate(i);
    QCOMPARE(tl.nominalBitrate(), (quint32)i );

    i = 7;
    tl.setMinimumBitrate(i);
    QCOMPARE(tl.minimumBitrate(), (quint32)i );

    i = 8;
    tl.setMaximumBitrate(i);
    QCOMPARE(tl.maximumBitrate(), (quint32)i );

    i = 9;
    tl.setSerial(i);
    QCOMPARE(tl.serial(), (quint32)i );

    i = 10;
    tl.setEncoderVersion(i);
    QCOMPARE(tl.encoderVersion(), (quint32)i );

    i = 11;
    tl.setShowEpisodeNumber(i);
    QCOMPARE(tl.showEpisodeNumber(), (quint32)i );

    i = 12;
    tl.setShowSeasonNumber(i);
    QCOMPARE(tl.showSeasonNumber(), (quint32)i );

    i = 13;
    tl.setUserRating(i);
    QCOMPARE(tl.userRating(), (quint32)i );

    d = 14.1;
    tl.setTrackGain(d);
    QCOMPARE(tl.trackGain(), d );

    d = 15.1;
    tl.setTrackPeak(d);
    QCOMPARE(tl.trackPeak(), d );

    d = 16.1;
    tl.setAlbumGain(d);
    QCOMPARE(tl.albumGain(), d );

    d = 17.1;
    tl.setAlbumPeak(d);
    QCOMPARE(tl.albumPeak(), d );

    d = 18.1;
    tl.setReferenceLevel(d);
    QCOMPARE(tl.referenceLevel(), d );

    d = 19.1;
    tl.setBeatsPerMinute(d);
    QCOMPARE(tl.beatsPerMinute(), d );

    d = 20.1;
    tl.setGeoLocationLatitude(d);
    QCOMPARE(tl.geoLocationLatitude(), d );

    d = 21.1;
    tl.setGeoLocationLongitude(d);
    QCOMPARE(tl.geoLocationLongitude(), d );

    d = 22.1;
    tl.setGeoLocationElevation(d);
    QCOMPARE(tl.geoLocationElevation(), d );

    d = 23.1;
    tl.setGeoLocationMovementSpeed(d);
    QCOMPARE(tl.geoLocationMovementSpeed(), d );

    d = 24.1;
    tl.setGeoLocationMovementDirection(d);
    QCOMPARE(tl.geoLocationMovementDirection(), d );

    d = 25.1;
    tl.setGeoLocationCaptureDirector(d);
    QCOMPARE(tl.geoLocationCaptureDirection(), d );
}

QTEST_APPLESS_MAIN(TagListTest)

#include "moc_qgsttest.cpp"
#include "taglisttest.moc"
