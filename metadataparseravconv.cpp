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
    QRegExp streamRegex("\\sStream #([0-9\\.]?):\\(.*?\\): (.*?):");
    QList<MetaDataTrack> l;
    int latestIndex = outputString.indexOf(streamRegex);
    while (-1 != latestIndex) {
        MetaDataTrack t;
        t.id = streamRegex.cap(1).toFloat();
        t.name = streamRegex.cap(2);
        // TODO get more info from the streams by using the type (cap3)
        t.type = 0;
        latestIndex = outputString.indexOf(streamRegex);
    }

    return l;
}
