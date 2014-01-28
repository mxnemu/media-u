#include "videoplayer.h"
#include "gifcreator.h"
#include <server.h>

VideoPlayer::VideoPlayer(Library& library, const SnapshotConfig& snapshotConfig, QObject* parent) :
    QObject(parent),
    library(library),
    snapshotConfig(snapshotConfig)
{
    pauseStatus = false;
    connect(this, SIGNAL(playbackEndedNormally()), this, SLOT(onPlaybackEndedNormally()));
    connect(this, SIGNAL(playbackCanceled()), this, SLOT(onPlaybackCanceled()));
}

VideoPlayer::~VideoPlayer() {
    this->process.kill();
}

bool VideoPlayer::playFile(QString filepath) {
    if (!QFile::exists(filepath)) {
        qDebug() << "can not play: file does not exists. Is the drive connected?" << filepath;
    }

    this->nowPlaying.seconds = 0;
    this->nowPlaying.path = filepath;
    this->nowPlaying.metaData = this->metaDataParser->parse(filepath);
    this->nowPlaying.tvShow = library.filter().getTvShowForPath(filepath);
    Episode* episode = library.filter().getEpisodeForPath(filepath);
    bool succeeded = false;
    if (episode) {
        succeeded = this->playFileImpl(filepath, library.existingTvShow(episode->getShowName())->playerSettings);
    }
    if (!succeeded) {
        resetPlayingStatus();
    } else {
        emit playbackStarted();
    }
    return succeeded;
}

void VideoPlayer::togglePause() {
    if (pauseStatus) {
        unPause();
    } else {
        pause();
    }
}

void VideoPlayer::pause() {
    pauseImpl();
    this->pauseStatus = true;
    emit paused();
}

void VideoPlayer::unPause() {
    unPauseImpl();
    this->pauseStatus = false;
    emit unpaused();
}

void VideoPlayer::jumpTo(int second) {
    int difference = second - this->nowPlaying.seconds;
    if (difference > 0) {
        forwards(difference);
    } else if (difference < 0) {
        backwards(-difference);
    }
}

void VideoPlayer::receivedPlaylist(QHttpResponse *resp, const QByteArray& body) {
    std::stringstream ss;
    ss << body.data();
    nw::JsonReader jr(ss);
    jr.describeValueArray("episodes", -1);
    QStringList episodes;
    for (int i=0; jr.enterNextElement(i); ++i) {
        std::string ep;
        jr.describeValue(ep);
        episodes.append(ep.c_str());
    }
    jr.close();

    this->setPlaylist(episodes);

    if (!episodes.isEmpty()) {
        QString firstEpisode = this->playlist.takeFirst();
        const bool success = this->playFile(firstEpisode);
        if (success) {
            Server::simpleWrite(resp, 200, QString("{\"status\":\"playback started\"}"), mime::json);
        } else {
            Server::simpleWrite(resp, 500, QString("{\"error\":\"could not start playback\"}"), mime::json);
        }
    } else {
        Server::simpleWrite(resp, 500, QString("{\"error\":\"playlist is empty\"}"), mime::json);
    }
}

bool VideoPlayer::handleApiRequest(QHttpRequest *req, QHttpResponse *resp) {
    if (req->path().startsWith("/api/player/play") && !req->url().query().isEmpty()) {
        std::stringstream ss;
        ss << req->url().query(QUrl::FullyDecoded).toStdString();
        QString episode;
        nw::JsonReader jr(ss);
        NwUtils::describe(jr, "filename", episode);
        jr.close();

        int error = this->playFile(episode);
        if (this->process.state() == QProcess::Running) {
            Server::simpleWrite(resp, 200, QString("{\"status\":\"playback started\"}"));
        } else {
            Server::simpleWrite(resp, 500, QString("{\"status\":\"could not start playback\", \"error\":%1}").arg(error));
        }
    } else if (req->path() == "/api/player/setPlaylist") {
        RequestBodyListener* bodyListener = new RequestBodyListener(resp, this);
        connect(req, SIGNAL(data(QByteArray)), bodyListener, SLOT(onDataReceived(QByteArray)));
        connect(bodyListener, SIGNAL(bodyReceived(QHttpResponse*,const QByteArray&)), this, SLOT(receivedPlaylist(QHttpResponse*,const QByteArray&)));
    } else if (req->path() == "/api/player/stop") {
        this->stop();
        Server::simpleWrite(resp, 200, "{\"status\":\"stopped\"}", mime::json);
    } else if (req->path() == "/api/player/togglePause") {
        this->togglePause();
        QString status = pauseStatus ? "paused" : "unpaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status), mime::json);
    } else if (req->path() == "/api/player/unPause") {
        this->unPause();
        Server::simpleWrite(resp, 200, "{\"status\":\"unpaused\"}", mime::json);
    } else if (req->path() == "/api/player/pause") {
        this->pause();
        Server::simpleWrite(resp, 200, "{\"status\":\"paused\"}", mime::json);
    } else if (req->path().startsWith("/api/player/jumpTo")) {
        bool ok;
        int second = req->url().query().toInt(&ok);
        if (ok) {
            this->jumpTo(second);
            Server::simpleWrite(resp, 200, "{\"status\":\"done\"}", mime::json);
        } else {
            QByteArray errorData;
            Server::simpleWriteBytes(resp, 404, errorData);
        }
    } else if (req->path() == "/api/player/backwards") {
        this->backwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else if (req->path() == "/api/player/forwards") {
        this->forwards();
        Server::simpleWrite(resp, 200, "{\"status\":\"ok\"}", mime::json);
    } else if (req->path() == "/api/player/incrementVolume") {
        float volume = this->incrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume), mime::json);
    } else if (req->path() == "/api/player/decrementVolume") {
        float volume = this->decrementVolume();
        Server::simpleWrite(resp, 200, QString("{\"status\":\"ok\",\"volume\":%1}").arg(volume), mime::json);
    } else if (req->path() == "/api/player/pauseStatus") {
        QString status = pauseStatus ? "paused" : "unpaused";
        Server::simpleWrite(resp, 200, QString("{\"status\":\"%1\"}").arg(status), mime::json);
    } else if (req->path() == "/api/player/metaData") {
        Server::simpleWrite(resp, 200, nowPlaying.metaData.toJson(), mime::json);
    } else if (req->path().startsWith("/api/player/thumbnail")) {
        bool ok;
        int second = req->url().query().toInt(&ok);
        if (ok) {
            ThumbCreationCallback* tcc = thumbnailCreator->generateJpeg(nowPlaying.path, second, 100, 70, resp);
            connect(tcc, SIGNAL(jpegGenerated(QByteArray)), this, SLOT(onThumbnailCreated(QByteArray)));
            tcc->start();
        } else {
            QByteArray errorData;
            Server::simpleWriteBytes(resp, 404, errorData);
        }
    } else  if (req->path().startsWith("/api/player/createGif")) {
        std::stringstream ss;
        ss << req->url().query(QUrl::FullyDecoded).toStdString();

        float start;
        float end;
        nw::JsonReader jr(ss);
        NwUtils::describe(jr, "start", start);
        NwUtils::describe(jr, "end", end);
        jr.close();

        createGif(start, end);
        Server::simpleWrite(resp, 200, "{\"status\": \"done\"}", mime::json);
    } else if (req->path() == "/api/player/progress") {
        Server::simpleWrite(resp, 200, nowPlaying.toSecondsAndPathJson(), mime::json);
    } else {
        return customHandleApiRequest();
    }
    return true;
}

void VideoPlayer::createGif(float startSecond, float endSecond) {
    QString outputPath = gifOutputPath(startSecond, endSecond);
    GifCreator gifc;
    std::pair<int,int> resolution = gifc.suggestedResolution(nowPlaying.metaData.resolution());
    gifc.create(nowPlaying.path, outputPath, startSecond, endSecond, resolution);
}

const MetaDataParser *VideoPlayer::getMetaDataParser() const
{
    return metaDataParser;
}

void VideoPlayer::setMetaDataParser(const MetaDataParser *value)
{
    metaDataParser = value;
}

const ThumbnailCreator *VideoPlayer::getThumbnailCreator() const
{
    return thumbnailCreator;
}

void VideoPlayer::setThumbnailCreator(const ThumbnailCreator *value)
{
    thumbnailCreator = value;
}

void VideoPlayer::onThumbnailCreated(const QByteArray img) {
    ThumbCreationCallback* tcc = dynamic_cast<ThumbCreationCallback*>(this->sender());
    QHttpResponse* resp = static_cast<QHttpResponse*>(tcc->data);
    Server::simpleWriteBytes(resp, 200, img, "image/jpeg");
    tcc->deleteLater();
}

void VideoPlayer::onPlaybackEndedNormally() {

    Episode* episode = this->library.filter().getEpisodeForPath(nowPlaying.path);
    if (episode) {
        episode->setWatched(true);
    }

    this->resetPlayingStatus();
    emit playbackEnded();
    if (this->playlist.length() > 0) {
        this->playFile(this->playlist.first());
        this->playlist.removeFirst();
    }
}

void VideoPlayer::onPlaybackCanceled() {
    this->playlist.clear();
    this->resetPlayingStatus();
    emit playbackEnded();
}

void VideoPlayer::resetPlayingStatus() {
    this->nowPlaying.seconds = -1;
    this->nowPlaying.path = QString();
    this->nowPlaying.metaData = MetaData();
    this->nowPlaying.tvShow = NULL;
    this->pauseStatus = true;
}

QStringList VideoPlayer::getPlaylist() const {
    return playlist;
}

void VideoPlayer::setPlaylist(const QStringList &value) {
    playlist = value;
}


QString VideoProgress::toSecondsAndPathJson() {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    NwUtils::describe(jw, "seconds", seconds);
    NwUtils::describe(jw, "path", path);
    jw.close();
    return ss.str().c_str();
}

QString VideoProgress::toJson() {
    std::stringstream ss;
    nw::JsonWriter jw(ss);
    NwUtils::describe(jw, "seconds", seconds);
    NwUtils::describe(jw, "path", path);
    jw.push("metaData");
    metaData.describe(&jw);
    jw.pop();
    jw.close();
    return ss.str().c_str();
}

QString VideoPlayer::imageName(QString name, QString extension) {
    //$(tvShow)/$(file) - at: $(progressM):$(progressS).$(ext)
    static const QString tvShowReplaceText("$(tvShow)");
    static const QString fileReplaceText("$(file)");
    static const QString progressMReplaceText("$(progressM)");
    static const QString progressSReplaceText("$(progressS)");
    static const QString nowDateReplaceText("$(nowDate)");
    static const QString extReplaceText("$(ext)");

    QString tvShowName = nowPlaying.tvShow ? nowPlaying.tvShow->name() : QString();
    QString minuteString = QString::number((int)nowPlaying.seconds / 60);
    QString secondString = QString::number((int)nowPlaying.seconds % 60);
    QString nowDateString = QString::number(QDateTime::currentMSecsSinceEpoch());

    name = name.replace(tvShowReplaceText, tvShowName);
    name = name.replace(fileReplaceText, QFileInfo(nowPlaying.path).fileName());
    name = name.replace(progressMReplaceText, minuteString);
    name = name.replace(progressSReplaceText, secondString);
    name = name.replace(nowDateReplaceText, nowDateString);
    name = name.replace(extReplaceText, extension);

    return name;
}

QString VideoPlayer::snapshotOutputPath() {
    QString name = this->imageName(snapshotConfig.snapshotName, snapshotConfig.snapshotFormat);
    QDir baseDir = QDir(snapshotConfig.snapshotDir);
    return baseDir.absoluteFilePath(name);
}

QString VideoPlayer::gifOutputPath(float start, float end) {
    QString name = snapshotConfig.gifName;

    QString startMinuteString = QString::number((int)start / 60);
    QString startSecondString = QString::number((int)start % 60);
    QString endMinuteString = QString::number((int)end / 60);
    QString endSecondString = QString::number((int)end % 60);

    static const QString startMReplaceText("$(startM)");
    static const QString startSReplaceText("$(startS)");
    static const QString endMReplaceText("$(endM)");
    static const QString endSReplaceText("$(endS)");

    name = name.replace(startMReplaceText, startMinuteString);
    name = name.replace(startSReplaceText, startSecondString);
    name = name.replace(endMReplaceText, endMinuteString);
    name = name.replace(endSReplaceText, endSecondString);

    name = this->imageName(name, "gif");

    QDir baseDir = QDir(snapshotConfig.gifDir);
    return baseDir.absoluteFilePath(name);
}


void VideoPlayer::convertSnapshots() {
    QStringList keys = unhandledSnapshots.keys();
    foreach (QString key, keys) {
        convertSnapshot(key, unhandledSnapshots.take(key));
    }
}

bool VideoPlayer::convertSnapshot(QString snapshotPath, QString outputPath) {
    QDir dir = QFileInfo(outputPath).dir();
    dir.mkpath(".");

    if (snapshotConfig.snapshotFormat == QFileInfo(snapshotPath).suffix()) {
        return QFile::rename(snapshotPath, outputPath);
    }

    QPixmap p;
    if (!p.load(snapshotPath)) {
        return false;
    }
    if (p.save(outputPath, NULL, snapshotConfig.snapshotQuality)) {
        if (!QFile(snapshotPath).remove()) {
            qDebug() << "failed to delete original snapshot" << snapshotPath;
        }
    } else {
        qDebug() << "failed to convert snapshot" << snapshotPath << "to" << outputPath;
    }
    return true;
}

VideoProgress VideoPlayer::getNowPlaying() const {
    return nowPlaying;
}

