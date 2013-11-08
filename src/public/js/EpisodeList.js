function EpisodeList(episodes) {
    this.episodes = episodes;
    this.sort();
}

EpisodeList.prototype.sort = function() {
    this.episodes = this.episodes.sort(function(a,b) {
        if (a.numericEpisodeNumber < -1 && b.numericEpisodeNumber >= 0) {
            return 1
        }
        if (b.numericEpisodeNumber < -1 && a.numericEpisodeNumber >= 0) {
            return -1
        }
        if (a.numericEpisodeNumber != b.numericEpisodeNumber) {
            return a.numericEpisodeNumber < b.numericEpisodeNumber ? -1 : 1;
        }
        if (a.episodeNumber != b.episodeNumber) {
            return a.episodeNumber < b.episodeNumber ? -1 : 1;
        }
        if (a.episodeName == b.episodeName) {
            return 0;
        }
        return a.episodeName < b.episodeName ? -1 : 1;
    })
}

EpisodeList.prototype.unwatchedArray = function() {
    var files = [];
    for (var i in this.episodes) {
        var ep = this.episodes[i];
        if (!ep.watched) {
            files.push(ep.path);
        }
    }
    return files;
}

EpisodeList.prototype.watchedArray = function() {
    var files = [];
    for (var i in this.episodes) {
        var ep = this.episodes[i];
        if (ep.watched) {
            files.push(ep.path);
        }
    }
    return files;
}
