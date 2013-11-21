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
    void testPathParsing_data();
    void testPathParsing();

    // TODO figure out how I can use multiple test files in qt
    void testCommandExists();
    void testCommonSliceInStrings_data();
    void testCommonSliceInStrings();
    void qstringEcodingSize();
};

MovieFileTest::MovieFileTest()
{
}

void MovieFileTest::testPathParsing_data()
{
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("releaseGroup");
    QTest::addColumn<QString>("showName");
    QTest::addColumn<QString>("episodeNumber");
    QTest::addColumn<float>("numericEpisodeNumber");
    QTest::addColumn<QString>("episodeName");

    QTest::newRow("Number: simple number") <<
                                              "/mnt/fields1/torrents/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_02_[356D9C93].mkv" <<
                                              "[Mazui]" <<
                                              "Hyouka" <<
                                              "02" <<
                                              2.f <<
                                              "";

    QTest::newRow("Number: SeasonXEpisode") <<
                                               "/mnt/fields2/torrents/When They Cry - Higurashi [Hi10]/Season1/Higurashi no Naku Koro ni 1x04.mkv" <<
                                               "" <<
                                               "Higurashi no Naku Koro ni" <<
                                               "1x04" <<
                                               4.f <<
                                               "";

    QTest::newRow("Number: versionedEpisode") <<
                                                 "/t/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_13v2_[BE011245].mkv" <<
                                                 "[Mazui]" <<
                                                 "Hyouka" <<
                                                 "13v2" <<
                                                 13.f <<
                                                 "";

    QTest::newRow("Number: decimalEpisode") <<
                                               "/mnt/fields1/torrents/[Mazui]_Hyouka_[720p]/[Mazui]_Hyouka_-_11.5_[3C10E2F9].mkv" <<
                                               "[Mazui]" <<
                                               "Hyouka" <<
                                               "11.5" <<
                                               11.5f <<
                                               "";

    QTest::newRow("Number: 'Episode' prefix") <<
                                                 "/home/nehmulos/Downloads/Spice and Wolf Complete Series/Season 01/Spice and Wolf Episode 01.mkv" <<
                                                 "" <<
                                                 "Spice and Wolf" <<
                                                 "Episode 01" <<
                                                 1.f <<
                                                 "";

    QTest::newRow("Number: Ep prefix") <<
                                          "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_Ep02_Instruments!_[1080p,BluRay,x264]_-_THORA.mkv" <<
                                          "THORA" <<
                                          "K-ON!" <<
                                          "Ep02" <<
                                          2.f <<
                                          "Instruments!";

    QTest::newRow("Number: ed is the end of the title (Elfen lied 03)") <<
                                                                           "/tmp/Elfen lied 03.ogg" <<
                                                                           "" <<
                                                                           "Elfen lied" <<
                                                                           "03" <<
                                                                           3.f <<
                                                                           "";

    QTest::newRow("Name: after obvious Ep number") <<
                                                      "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_Ep02_Instruments!_[1080p,BluRay,x264]_-_THORA.mkv" <<
                                                      "THORA" <<
                                                      "K-ON!" <<
                                                      "Ep02" <<
                                                      2.f <<
                                                      "Instruments!";

    QTest::newRow("Name: Without Ep name") <<
                                              "/t/[Coalgirls]_The_Melancholy_of_Haruhi_Suzumiya_(1280x720_Blu-Ray_FLAC)/Season I/[Coalgirls]_The_Melancholy_of_Haruhi_Suzumiya_-_Remote_Island_Syndrome_I_(1280x720_Blu-Ray_FLAC)_[10DBFB45].mkv" <<
                                              "[Coalgirls]" <<
                                              "The Melancholy of Haruhi Suzumiya" <<
                                              "" <<
                                              -1.f <<
                                              "Remote Island Syndrome I";

    QTest::newRow("Group: in front with [techtags] behind") <<
                                                               "/mnt/fields1/torrents/[DeadFish] Nisemonogatari - Batch [BD][720p][MP4][AAC]/[DeadFish] Nisemonogatari - 01 [BD][720p][AAC].mp4" <<
                                                               "[DeadFish]" <<
                                                               "Nisemonogatari" <<
                                                               "01" <<
                                                               1.f <<
                                                               "";


    QTest::newRow("Group: at the end") <<
                                          "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_Ep02_Instruments!_[1080p,BluRay,x264]_-_THORA.mkv" <<
                                          "THORA" <<
                                          "K-ON!" <<
                                          "Ep02" <<
                                          2.f <<
                                          "Instruments!";

    QTest::newRow("Group: at the end with version behind") <<
                                                              "/home/nehmulos/Downloads/K-ON!_(2009)_[1080p,BluRay,x264]_-_THORA/K-ON!_ED_[1080p,BluRay,x264]_-_THORA v2.mkv" <<
                                                              "THORA" <<
                                                              "K-ON!" <<
                                                              "ED" <<
                                                              -2.f <<
                                                              "";


    QTest::newRow("Group: no goup") <<
                                       "/home/nehmulos/Downloads/Spice and Wolf Complete Series/Season 01/Spice and Wolf Episode 02.mkv" <<
                                       ""<<
                                       "Spice and Wolf" <<
                                       "Episode 02" <<
                                       2.f <<
                                       "";

    QTest::newRow("Group: no group2") <<
                                         "/mnt/fields2/torrents/When They Cry - Higurashi [Hi10]/Season1/Higurashi no Naku Koro ni 1x01.mkv" <<
                                         "" <<
                                         "Higurashi no Naku Koro ni" <<
                                         "1x01" <<
                                         1.f <<
                                         "";

    QTest::newRow("ShowName: [group] name - num [tech][tags].vid") <<
                                                                      "/home/nehmulos/Downloads/[DeadFish] Bakemonogatari [BD][1080p][MP4][AAC]/[DeadFish] Bakemonogatari - 01 [BD][1080p][AAC].mp4" <<
                                                                      "[DeadFish]" <<
                                                                      "Bakemonogatari" <<
                                                                      "01" <<
                                                                      1.f <<
                                                                      "";

    QTest::newRow("ShowName: name Ep.num[tech][tags].vid") <<
                                                              "/home/nehmulos/Downloads/[SSP-Corp] Noir [Dual-Audio]/Noir_Ep.01[h.264-AAC][SSP-Corp][E5A84450].mkv" <<
                                                              "" <<
                                                              "Noir" <<
                                                              "Ep 01" <<
                                                              1.f <<
                                                              "";

    QTest::newRow("ShowName: ed is the end of the title (Elfen lied 03)") <<
                                                                             "/mnt/fields2/torrents/[Coalgirls]_Elfen_Lied_(1280x720_Blu-ray_FLAC)/[Coalgirls]_Elfen_Lied_01_(1280x720_Blu-ray_FLAC)_[5BC74BC6].mkv" <<
                                                                             "[Coalgirls]" <<
                                                                             "Elfen Lied" <<
                                                                             "01" <<
                                                                             1.f <<
                                                                             "";
}


void MovieFileTest::testPathParsing() {
    QFETCH(QString, path);
    QFETCH(QString, releaseGroup);
    QFETCH(QString, showName);
    QFETCH(QString, episodeNumber);
    QFETCH(float, numericEpisodeNumber);
    QFETCH(QString, episodeName);

    const MovieFile m(path);
    QCOMPARE(m.releaseGroup, releaseGroup);
    QCOMPARE(m.showName, showName);
    QCOMPARE(m.episodeNumber, episodeNumber);
    QCOMPARE(m.numericEpisodeNumber(), numericEpisodeNumber);
    QCOMPARE(m.episodeName, episodeName);
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
