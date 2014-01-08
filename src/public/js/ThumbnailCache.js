function ThumbnailCache(duration) {
    this.thumbnails = {};
    this.openRequests = {};
    this.spacing = 5;
    if (duration > 0) {
        this.loadAll(duration);
    }
}

ThumbnailCache.prototype.get = function(second, callback) {
    second = this.secondToSpacing(second);
    if (this.thumbnails[second]) {
        if (callback) {
            callback(this.thumbnails[second]);
        }
        return;
    }
    if (this.openRequests[second]) {
        //this.openRequests.callbacks.push(callback);
        return;
    }
    var self = this;
    this.openRequests[second] = true;
    var img = new Image();
    img.onload = function(data) {
        delete self.openRequests[second];
        self.thumbnails[second] = img;
        if (callback) {
            callback(img);
        }
    };
    img.src ="api/player/thumbnail?" + second;
}

ThumbnailCache.prototype.loadAll = function(duration) {
    for (var i = 0; i < duration; i += this.spacing) {
        this.get(i);
    }
}

ThumbnailCache.prototype.isLoading = function() {
    for (var key in this.openRequests) {
        return true;
    }
    return false;
}

ThumbnailCache.prototype.secondToSpacing = function(second) {
    return second - (second % this.spacing);
}
