function EpisodeList(episodes, tvshow) {
    this.episodes = episodes;
    this.tvshow = tvshow;
    this.sort();
}

EpisodeList.prototype.sort = function() {
    this.episodes = this.episodes.sort(function(a,b) {
        if (a.numericEpisodeNumber < 0 && b.numericEpisodeNumber >= 0) {
            return 1
        }
        if (b.numericEpisodeNumber < 0 && a.numericEpisodeNumber >= 0) {
            return -1
        }
        if (a.numericEpisodeNumber != b.numericEpisodeNumber) {
            if (a.numericEpisodeNumber < -1 && b.numericEpisodeNumber >= -1) {
                return 1;
            }
            if (b.numericEpisodeNumber < -1 && a.numericEpisodeNumber >= -1) {
                return -1;
            }
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

EpisodeList.prototype.playlist = function(predicate) {
    var files = [];
    for (var i in this.episodes) {
        var ep = this.episodes[i];
        if (predicate.call(this, ep)) {
            files.push(ep.path);
        }
    }
    return files;
}

EpisodeList.prototype.rewatchPlaylist = function() {
    return this.playlist(function(ep) {
        return ep.numericEpisodeNumber >= this.tvshow.rewatchMarker+1;
    });
}

EpisodeList.prototype.unwatchedArray = function(includeSpecials) {
    return this.playlist(function(ep) {
        return !ep.watched && (includeSpecials || ep.numericEpisodeNumber > -2)
    });
}

EpisodeList.prototype.unwatchedSpecials = function() {
    return this.playlist(function(ep) {
        return !ep.watched && ep.numericEpisodeNumber > -2
    });
}

EpisodeList.prototype.watchedArray = function() {
    return this.playlist(function(ep) {
        return ep.watched
    });
}
