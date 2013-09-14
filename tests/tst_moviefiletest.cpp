#include <QString>
#include <QtTest>

#include "../src/moviefile.h"

class MovieFileTest : public QObject
{
    Q_OBJECT
    
public:
    MovieFileTest();
    
private Q_SLOTS:
    void testEpisodeNumberParsing_data();
    void testEpisodeNumberParsing();
    void testReleaseGroup_data();
    void testReleaseGroup();
};

MovieFileTest::MovieFileTest()
{
}

void MovieFileTest::testEpisodeNumberParsing_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("episodeNumber");
    QTest::addColumn<int>("numericEpisodeNumber");

    QTest::newRow("simple number") <<
        "/mnt/fields1/torrents/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_02_[356D9C93].mkv" <<
        "02" <<
        2;

    QTest::newRow("SeasonXEpisode") <<
        "/mnt/fields2/torrents/When They Cry - Higurashi [Hi10]/Season1/Higurashi no Naku Koro ni 1x04.mkv" <<
        "1x04" <<
        4;

    QTest::newRow("versionedEpisode") <<
        "/t/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_13v2_[BE011245].mkv" <<
        "13v2" <<
        13;

    QTest::newRow("decimalEpisode") <<
        "/mnt/fields1/torrents/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_11.5_[3C10E2F9].mkv" <<
        "11.5" <<
        11;

    QTest::newRow("'Episode' prefix") <<
        "/home/nehmulos/Downloads/Spice and Wolf Complete Series/Season 01/Spice and Wolf Episode 01.mkv" <<
        "Episode 01" <<
        1;

    QTest::newRow("Ep prefix") <<
        "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_Ep02_Instruments!_[1080p,BluRay,x264]_-_THORA.mkv" <<
        "Ep02" <<
        2;
}


void MovieFileTest::testEpisodeNumberParsing() {
    QFETCH(QString, path);
    QFETCH(QString, episodeNumber);
    QFETCH(int, numericEpisodeNumber);

    MovieFile m(path);
    QCOMPARE(m.episodeNumber(), episodeNumber);
    QCOMPARE(m.numericEpisodeNumber(), numericEpisodeNumber);
}

void MovieFileTest::testReleaseGroup_data() {
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("releaseGroup");

    QTest::newRow("in front with [techtags] behind") <<
        "/mnt/fields1/torrents/[DeadFish] Nisemonogatari - Batch [BD][720p][MP4][AAC]/[DeadFish] Nisemonogatari - 01 [BD][720p][AAC].mp4" <<
        "[DeadFish]";
}

void MovieFileTest::testReleaseGroup() {
    QFETCH(QString, path);
    QFETCH(QString, releaseGroup);

    MovieFile m(path);
    QCOMPARE(m.releaseGroup(), releaseGroup);
}

QTEST_APPLESS_MAIN(MovieFileTest)

#include "tst_moviefiletest.moc"
