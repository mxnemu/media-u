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
    
    if (G.app.serverEvents.readyState == EventSource.OPEN) {
        this.startUp();
    }
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
    
    $.getJSON("api/player/metaData", function(metaData) {
        self.initMetaData(metaData);
        $.getJSON("api/player/progress", function(data) {
            self.initProgress(data.seconds);
        }).error(function(a,b,c,d,e) {
            console.log("error",a,b,c,d,e);
        });
    }).error(function(a,b,c,d,e) {
        console.log("error",a,b,c,d,e);
    });
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
    this.progressUpdateIntervalId = window.setInterval(function() {
        if (!self.isPaused) {
            self.set(self.seconds + 0.05);
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
    
    this.onConnectListener = function(event) {
        self.startUp();
    }
    
    this.onPlaybackStartedListener = function(event) {
        var data = JSON.parse(event.data);
        self.initMetaData(data.metaData);
        self.initProgress(data.seconds);
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
    
    G.app.serverEvents.addEventListener("open", this.onConnectListener);
    G.app.serverEvents.addEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.addEventListener("playbackEnded", this.onPlaybackEndedListener);
    G.app.serverEvents.addEventListener("paused", this.onPausedListener);
    G.app.serverEvents.addEventListener("unpaused", this.onUnpausedListener);
    G.app.serverEvents.addEventListener("jumped", this.onJumpedListener);
}

Progress.prototype.unbindEvents = function() {
    window.clearInterval(this.progressUpdateIntervalId);

    G.app.serverEvents.removeEventListener("open", this.onConnectListener);
    G.app.serverEvents.removeEventListener("playbackStarted", this.onPlaybackStartedListener);
    G.app.serverEvents.removeEventListener("playbackEnded", this.onPlaybackEndedListener);
    G.app.serverEvents.removeEventListener("paused", this.onPausedListener);
    G.app.serverEvents.removeEventListener("unpaused", this.onUnpausedListener);
    G.app.serverEvents.removeEventListener("jumped", this.onJumpedListener);
}
