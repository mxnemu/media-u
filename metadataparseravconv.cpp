#include "metadataparseravconv.h"
#include <QProcess>
#include <QDebug>

MetaDataParserAvconv::MetaDataParserAvconv()
{
}

// todo getThumb: avconv -ss 00:0:57 -t 1 -i Higurashi\ no\ Naku\ Koro\ ni\ 1x01.mkv -s 50x30 -vframes 1 -f image2 /tmp/test%d.jpg

MetaData MetaDataParserAvconv::parse(QString filename) const {
    QProcess process;
    process.start("avconv", QStringList() << "-i" << filename);
    process.waitForFinished(); // TODO use signals

    // read error, because avconv expects an output file
    // and thinks just checking the input is an error
    QString output(process.readAllStandardError());

    MetaData m;

    // Example output:
    //$ avconv -i Higurashi\ no\ Naku\ Koro\ ni\ 1x01.mkv
    //    avconv version 0.8.6-6:0.8.6-1ubuntu2, Copyright (c) 2000-2013 the Libav developers
    //      built on Mar 30 2013 22:20:06 with gcc 4.7.2
    //    [matroska,webm @ 0x112fa20] Estimating duration from bitrate, this may be inaccurate
    //    Input #0, matroska,webm, from 'Higurashi no Naku Koro ni 1x01.mkv':
    //      Duration: 00:23:42.04, start: 0.000000, bitrate: N/A
    //        Chapter #0.0: start 0.105000, end 1422.047000
    //        Metadata:
    //          title           : 00:00:00.105
    //        Stream #0.0(eng): Video: h264 (High 10), yuv420p10le, 720x480 [PAR 853:720 DAR 853:480], PAR 186:157 DAR 279:157, 23.98 tbr, 1k tbn, 1073741824 tbc (default)
    //        Stream #0.1(jpn): Subtitle: [0][0][0][0] / 0x0000 (default)
    //        Stream #0.2: Audio: aac, 44100 Hz, stereo, s16 (default)
    //    At least one output file must be specified

    m.duration = parseDuration(output);
    m.tracks = parseTracks(output);

    return m;
}

int MetaDataParserAvconv::parseDuration(QString durationString) const {
    QRegExp durationRegex("\\sDuration:\\s([0-9]*):([0-9]*):([0-9]*).([0-9]*),");
    durationRegex.setMinimal(true);
    bool foundAnything = -1 != durationString.indexOf(durationRegex);
    if (foundAnything) {
        int hours = durationRegex.cap(1).toInt();
        int minutes = durationRegex.cap(2).toInt();
        int seconds = durationRegex.cap(3).toInt();
        //if (durationRegex.captureCount() == 4) {
        //    int ms = durationRegex.cap(4).toInt(); // ignore
        //}

        return (hours * 60 * 60) + (minutes * 60) + seconds;
    }
    return -1;
}

QList<MetaDataTrack> MetaDataParserAvconv::parseTracks(QString outputString) const {
    QRegExp streamRegex("\\sStream \\#([0-9\\.]*)(\\((.*)\\))?: (.*):");
    streamRegex.setMinimal(true);
    QList<MetaDataTrack> l;
    int latestIndex = outputString.indexOf(streamRegex);
    qDebug() << "found at" << latestIndex;
    while (-1 != latestIndex) {
        int nextIndex = outputString.indexOf(streamRegex, latestIndex+1);
        latestIndex = outputString.indexOf(streamRegex, latestIndex); // stupid repetition to gain back the captures

        MetaDataTrack t;
        t.id = streamRegex.cap(1).toFloat();
        t.name = streamRegex.cap(3); // skip 2 as it's the optional name with the brackets
        QString typeString = streamRegex.cap(4);
        QString detailsString = outputString.mid(latestIndex + streamRegex.cap(0).length(), nextIndex != -1 ? (outputString.length() - (nextIndex-latestIndex)) : 0);

        if (typeString == "Video") {
            t.type = video;
            qDebug() << detailsString;
            QRegExp videoDetailsRegex("([0-9]+)x([0-9]+).");

            if (-1 != detailsString.indexOf(videoDetailsRegex)) {
                t.track.video.resolutionX = videoDetailsRegex.cap(1).toInt();
                t.track.video.resolutionY = videoDetailsRegex.cap(2).toInt();
            } else {
                t.track.video.resolutionX = 0;
                t.track.video.resolutionY = 0;
            }

        } else if (typeString == "Audio") { t.type = audio; }
        else if (typeString == "Subtitle") { t.type = subtitle; }
        else if (typeString == "Attachment") { t.type = attachment; }
        // TODO parse type specific infos like video resolution

        l.append(t);
        latestIndex = nextIndex;
    }

    return l;
}

QList<MetaDataChapter> MetaDataParserAvconv::parseChapters(QString outputString) const {
    QList<MetaDataChapter> chapters;
    QRegExp chapterHeadRegex("\\sChapter \\#([0-9\\.]*)(\\((.*)\\))?: start ([0-9\\.]*), end ([0-9\\.]*)");
    int chapterHead = outputString.indexOf(chapterHeadRegex);
    while (-1 != chapterHead) {
        int nextIndex = outputString.indexOf(chapterHeadRegex, chapterHead+1);
        outputString.indexOf(chapterHeadRegex, chapterHead);

        MetaDataChapter c;
        c.start = chapterHeadRegex.cap(4).toFloat();
        c.end = chapterHeadRegex.cap(5).toFloat();

        /* Probably just a bad example I saved up above. gonna leave this since I don't have example files atm
        QRegExp chapterTitleRegex("Metadata:\n(\\s*)title(\\s*):\\s([0-9]*):([0-9]*):([0-9\\.]*)");
        int hour = chapterTitleRegex.cap(3).toInt();
        int minute = chapterTitleRegex.cap(4).toInt();
        float second = chapterTitleRegex.cap(5).toFloat();
        */

        QRegExp chapterTitleRegex("Metadata:\n(\\s*)title(\\s*):\\s([^\n])\n");
        int chapterTitle = outputString.indexOf(chapterTitleRegex, chapterHead);
        if (-1 != chapterTitle && chapterTitle < nextIndex) {
            c.title = chapterTitleRegex.cap(3);
        }
        chapters.append(c);
        chapterHead = nextIndex;
    }
}
