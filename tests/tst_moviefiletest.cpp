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
    /*
    QTest::newRow("SeasonXEpisode") <<
        "/mnt/fields2/torrents/When They Cry - Higurashi [Hi10]/Season1/Higurashi no Naku Koro ni 1x04.mkv" <<
        "1x04" <<
        4;
    */
}


void MovieFileTest::testEpisodeNumberParsing()
{
    QFETCH(QString, path);
    QFETCH(QString, episodeNumber);
    QFETCH(int, numericEpisodeNumber);

    MovieFile m(path);
    QCOMPARE(m.episodeNumber(), episodeNumber);
    QCOMPARE(m.numericEpisodeNumber(), numericEpisodeNumber);
}

QTEST_APPLESS_MAIN(MovieFileTest)

#include "tst_moviefiletest.moc"
