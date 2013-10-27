#include <QString>
#include <QtTest>

#include "../src/moviefile.h"
#include "../src/systemutils.h"
#include "../src/utils.h"

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
    void testShowName_data();
    void testShowName();
    void qstringEcodingSize();

    // TODO figure out how I can use multiple test files in qt
    void testCommandExists();
    void testCommonSliceInStrings_data();
    void testCommonSliceInStrings();
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

    /*
    QTest::newRow("ed is the end of the title (Elfen lied 03)") <<
        "/tmp/Elfen lied 03.ogg" <<
        "03" <<
        3;
    */
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

    QTest::newRow("at the end") <<
        "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_Ep02_Instruments!_[1080p,BluRay,x264]_-_THORA.mkv" <<
        "THORA";

    QTest::newRow("at the end with version behind") <<
        "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_ED_[1080p,BluRay,x264]_-_THORA v2.mkv" <<
        "THORA";


    QTest::newRow("no goup") <<
        "/home/nehmulos/Downloads/Spice and Wolf Complete Series/Season 01/Spice and Wolf Episode 02.mkv" <<
        "";

    QTest::newRow("no group2") <<
        "/mnt/fields2/torrents/When They Cry - Higurashi [Hi10]/Season1/Higurashi no Naku Koro ni 1x01.mkv" <<
        "";

}

void MovieFileTest::testReleaseGroup() {
    QFETCH(QString, path);
    QFETCH(QString, releaseGroup);

    MovieFile m(path);
    QCOMPARE(m.releaseGroup(), releaseGroup);
}

void MovieFileTest::testShowName_data() {
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("showName");

    QTest::newRow("[group] name - num [tech][tags].vid") <<
        "/home/nehmulos/Downloads/[DeadFish] Bakemonogatari [BD][1080p][MP4][AAC]/[DeadFish] Bakemonogatari - 01 [BD][1080p][AAC].mp4" <<
        "Bakemonogatari";

    QTest::newRow("name Ep.num[tech][tags].vid") <<
        "/home/nehmulos/Downloads/[SSP-Corp] Noir [Dual-Audio]/Noir_Ep.01[h.264-AAC][SSP-Corp][E5A84450].mkv" <<
        "Noir";

    QTest::newRow("mind the stupid space before v2") <<
        "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_ED_[1080p,BluRay,x264]_-_THORA v2.mkv" <<
        "K-ON!";

    QTest::newRow("ed is the end of the title (Elfen lied 03)") <<
        "/mnt/fields2/torrents/[Coalgirls]_Elfen_Lied_(1280x720_Blu-ray_FLAC)/[Coalgirls]_Elfen_Lied_01_(1280x720_Blu-ray_FLAC)_[5BC74BC6].mkv" <<
        "Elfen Lied";
}

void MovieFileTest::testShowName() {
    QFETCH(QString, path);
    QFETCH(QString, showName);

    MovieFile m(path);
    QCOMPARE(m.showName(), showName);
}

void MovieFileTest::testCommandExists() {
    QVERIFY(true  == SystemUtils::commandExists("command"));
    QVERIFY(false == SystemUtils::commandExists("alsdhadhaasghoaishknasflkhasdfkljkashf")); // let's hope nobody installed this
}

void MovieFileTest::testCommonSliceInStrings_data() {
    QTest::addColumn<QStringList>("strings");
    QTest::addColumn<QString>("commonSlice");

    QTest::newRow("caseinsensitive, prefix and postfix") <<
                          (QStringList() << "Bakemonogatari" << "Nisemonogatari" << "Nekomonogatari black" << "Monogatari Series: Second Season") <<
        "monogatari";

}

void MovieFileTest::testCommonSliceInStrings() {
    QFETCH(QStringList, strings);
    QFETCH(QString, commonSlice);

    QCOMPARE(Utils::commonSliceInStrings(strings).toLower(), commonSlice.toLower());
}

void MovieFileTest::qstringEcodingSize() {
    QVERIFY(QString("☆☆☆").toUtf8().size() != QString("☆☆☆").toLatin1().size());
    QVERIFY(QString("☆☆☆").toUtf8().size() > 3);
    QVERIFY(QString("☆☆☆").size() == 3);
    QVERIFY(QString("☆☆☆").toLatin1().size() == 3);
}

QTEST_APPLESS_MAIN(MovieFileTest)

#include "tst_moviefiletest.moc"
