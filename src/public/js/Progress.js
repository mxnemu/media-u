function Progress(onUpdateCallback) {
    this.seconds = 0;
    this.metaData = null;
    this.onUpdateCallback = onUpdateCallback;
    this.isPaused = false;
    this.isReady = false;
    this.onReadyCallbacks = [];
    this.onResetCallbacks = [];

    this.progressUpdateIntervalId = null;
    this.bindEvents();
    this.startUp();

    if (G.app.serverEvents.readyState == EventSource.OPEN) {
        this.startUp();
    }
}

Progress.prototype.isPlaying = function() {
    return this.metaData && this.metaData.duration !== -1;
}

Progress.prototype.onReady = function(callback) {
    if (!this.isReady) {
        this.onReadyCallbacks.push(callback);
    } else {
        callback();
    }
}

Progress.prototype.onReset = function(callback) {
    this.onResetCallbacks.push(callback);
}


Progress.prototype.startUp = function() {
    var self = this;

    function logError(a,b,c,d,e) {
        console.log("error",a,b,c,d,e);
    }

    $.getJSON("api/player/metaData", function(metaData) {
        self.initMetaData(metaData);
        $.getJSON("api/player/pauseStatus", function(data) {
            self.isPaused = data.status === "paused";
            $.getJSON("api/player/progress", function(data) {
                self.initProgress(data.seconds);
            }).error(logError);
        }).error(logError);
    }).error(logError);
}

Progress.prototype.initMetaData = function(metaData) {
    this.metaData = metaData;
    this.isReady = true;
    for (var i in this.onReadyCallbacks) {
        this.onReadyCallbacks[i]();
    }
}

Progress.prototype.initProgress = function(second) {
    this.set(second);

    // progress bar update
    window.clearInterval(this.progressUpdateIntervalId);
    var self = this;
    var lastDate = new Date();
    this.progressUpdateIntervalId = window.setInterval(function() {
        if (!self.isPaused) {
            var date = new Date();
            var diff = (date.getTime() - lastDate.getTime()) / 1000;
            lastDate = date;
            self.set(self.seconds + diff);
        }
    }, 50);
}

Progress.prototype.set = function(second) {
    this.seconds = Math.min(second, this.metaData.duration);
    if (this.seconds < this.metaData.duration) {
        this.onUpdateCallback(second, this.metaData.duration);
    }
}

Progress.prototype.reset = function() {
    this.isReady = false;
    window.clearInterval(this.progressUpdateIntervalId);
    if (this.metaData) {
        this.set(0);
    }
    this.metaData = null;
    this.seconds = 0;
    for (var i in this.onResetCallbacks) {
        this.onResetCallbacks[i]();
    }
}

Progress.prototype.bindEvents = function() {
    var self = this;

    this.onPlaybackStartedListener = function(event) {
        var data = JSON.parse(event.data);
        self.initMetaData(data.metaData);
        self.initProgress(data.seconds);
        self.isPaused = false;
    }

    this.onPlaybackEndedListener = function(event) {
        self.reset();
    }

    this.onPausedListener = function(event) {
        self.isPaused = true;
    }

    this.onUnpausedListener = function(event) {
        self.isPaused = false;
    }

    this.onJumpedListener = function(event) {
        var data = JSON.parse(event.data);
        if (self.isReady) {
            self.set(data.seconds);
        }
    }

    G.app.serverEvents.addEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.addEventListener("playbackEnded", this.onPlaybackEndedListener);
    G.app.serverEvents.addEventListener("paused", this.onPausedListener);
    G.app.serverEvents.addEventListener("unpaused", this.onUnpausedListener);
    G.app.serverEvents.addEventListener("jumped", this.onJumpedListener);
}

Progress.prototype.unbindEvents = function() {
    window.clearInterval(this.progressUpdateIntervalId);

    G.app.serverEvents.removeEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.removeEventListener("playbackEnded", this.onPlaybackEndedListener);
    G.app.serverEvents.removeEventListener("paused", this.onPausedListener);
    G.app.serverEvents.removeEventListener("unpaused", this.onUnpausedListener);
    G.app.serverEvents.removeEventListener("jumped", this.onJumpedListener);
}
