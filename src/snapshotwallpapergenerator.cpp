#include "snapshotwallpapergenerator.h"
namespace SnapshotWallpaperGenerator {

Client::Client(MetaDataParser& mp, ThumbnailCreator& tc) :
    WallpaperDownload::Client(QString()),
    metaDataParser(mp),
    thumbnailCreator(tc)
{
    outstandingWallpapers = 0;
}

SearchResult Client::fetchPostsBlocking(const TvShow* show, int page) {
    SearchResult result;
    if (page != 1) {
        return result;
    }

    const QList<Episode*>& eps = show->episodeList().episodes;
    foreach (const Episode* ep, eps) {
        const VideoFile* mf = ep->bestFile(show->getReleaseGroupPreference());
        if (!mf) {
            continue;
        }
        MetaData data = metaDataParser.parse(mf->path);
        if (-1 == data.duration) {
            continue;
        }
        for (float i=1; i < 3.f; ++i) {
            // paranoid check for stupid compilers
            int second = std::min(data.duration-1, (int)(data.duration * (0.3 * i)));
            Entry e;
            e.id = mf->path;
            e.fileUrl = QString::number(second);
            e.score = second;
            result.entries.push_back(e);
        }
    }

    return result;
}

void Client::downloadResults(QDir directory, const QList<Entry>& entries, bool onlyTheBest) {
    int generated = 0;
    directory.mkpath(".");

    foreach (const Entry& entry, entries) {
        // TODO check if this is a good wallpaper source
        // always consider snapshot wallpapers as worse than online-fetched once
        // TODO have a setting for that ^^^
        if (onlyTheBest) {
            return;
        }
        if (generated >= limit) {
            break;
        }
        int s = QString(entry.fileUrl).toInt();

        QString fileName = QString("snapshot_%1 %2.png").arg(QFileInfo(entry.id).fileName(), QString::number(s));
        QString filePath = directory.absoluteFilePath(fileName);

        if (QFile::exists(filePath)) {
            continue;
        }

        ++generated;
        ++outstandingWallpapers;
        ThumbnailCreationData* data = new ThumbnailCreationData();
        data->filePath = filePath;
        data->client = this;
        ThumbCreationCallback* tcc = thumbnailCreator.generatePng(entry.id, s, -1, -1, data);
        connect(tcc, SIGNAL(jpegGenerated(QByteArray)), this, SLOT(wallpaperReady(QByteArray)));
        tcc->start();
    }

    //while (outstandingWallpapers) {
    //    QThread::msleep(10);
    //}
}

void Client::generated(QString file) {
    emit wallpaperDownloaded(file);
}

void Client::wallpaperReady(QByteArray data) {
    --outstandingWallpapers;
    ThumbCreationCallback* self = dynamic_cast<ThumbCreationCallback*>(sender());
    if (!self) {
        return;
    }
    ThumbnailCreationData* userData = static_cast<ThumbnailCreationData*>(self->data);
    QString path = userData->filePath;
    QFile file(path);
    file.open(QFile::WriteOnly);
    file.write(data);
    file.close();
    userData->client->generated(path);
    delete userData;
}

} // namespace
