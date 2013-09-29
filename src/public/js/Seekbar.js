function Progress(page, onUpdateCallback) {
    this.seconds = 0;
    this.path = null;
    this.metaData = null;
    this.page = page;
    this.onUpdateCallback = onUpdateCallback;
    this.onResetCallback = null;
    
    this.progressUpdateIntervalId = null;
    this.progressSyncIntervalId = null;
    this.refetchMetaDataTimeout = null;
}

Progress.prototype.fetchMetaData = function(callback) {
    var self = this;
    
    $.getJSON("api/player/metaData", function(data) {
        if (data.duration < 0) {
            self.refetchMetaDataTimeout = window.setTimeout(function() {
                self.fetchMetaData(callback);
            }, 500);
            return;
        }
        self.metaData = data;
        self.startUp();
        if (self.onResetCallback) {
            self.onResetCallback();
        }
        if (callback) {
            callback();
        }
    });
}

Progress.prototype.startUp = function() {
    var self = this;
    
    $.getJSON("api/player/progress", function(data) {
        self.path = data.path;
        self.update(data.seconds);
        self.clearIntervals();
        self.progressUpdateIntervalId = window.setInterval(function() {
            if (self.page.togglePauseButton.attr("data-status") == "unPaused") {
                self.update(self.seconds +0.05);
            }
        }, 50);
        // sync with the player's progress every 10s
        self.progressSyncIntervalId = window.setInterval(function() {
            self.resync();
        }, 10000);
    }).error(function(a,b,c,d,e) {
        console.log("error",a,b,c,d,e);
    });
}

Progress.prototype.update = function(second) {
    this.seconds = Math.min(second, this.metaData.duration);
    this.onUpdateCallback(second, this.metaData.duration)
    if (this.seconds >= this.metaData.duration) {
        this.resync();
    }
}

Progress.prototype.resync = function() {
    var self = this;
    $.getJSON("api/player/progress", function(d) {
        self.update(d.seconds);
        console.log(d.path);
        if (d.path != self.path) {
            self.clearIntervals();
            self.fetchMetaData();
        }
    });
}

Progress.prototype.clearIntervals = function() {
    window.clearInterval(this.progressUpdateIntervalId);
    window.clearInterval(this.progressSyncIntervalId);
    window.clearTimeout(this.refetchMetaDataTimeout);
}

function Seekbar(page) {
    this.page = page;
    this.progressUpdateIntervalId = null;
    this.progressSyncIntervalId = null;
    this.progress = null;
    this.tooltip = null;
    this.createNodes();
}

Seekbar.prototype.setTooltip = function(time, x, y, img, chapter) {
    this.tooltip.empty();
    if (chapter) {
        this.tooltip.append("<span>" + chapter + "</span>");
        this.tooltip.append("<br/>");
        y -= 15;
    }
    this.tooltip.append("<span>" +
        Utils.paddedNumber(Math.floor(time.minute), 2) + ":" +
        Utils.paddedNumber(Math.floor(time.second), 2) +
        "</span>"
    );
    if (img) {
        this.tooltip.append(img);
    }
    this.tooltip.css("left", x);
    this.tooltip.css("top", y);
}

Seekbar.prototype.reset = function() {
    this.thumbnailCache = new ThumbnailCache();
    if (G.preloadSeekBarPreviews) {
        this.thumbnailCache.loadAll(this.progress.metaData.duration);
    }
}

Seekbar.prototype.bindListeners = function() {
    var self = this;

    this.bar.mousemove(function(event) {
        var x = Math.max(event.clientX, 50);
        x = Math.min(x, window.innerWidth - 50);
        var y = self.bar.offset().top;
        
        var videoPos = self.progress.metaData.duration * 
                       (event.clientX / window.innerWidth);
        var minute = (videoPos / 60);
        var second = videoPos % 60;
        
        var chapter = null;
        for (var i=0; i < self.progress.metaData.chapters.length; ++i) {
            if (videoPos > self.progress.metaData.chapters[i].start &&
                videoPos < self.progress.metaData.chapters[i].end) {
                
                chapter = self.progress.metaData.chapters[i].title;
            }
        }
        function setTooltip(img) {
            self.setTooltip(
                {
                    total: videoPos,
                    minute: minute,
                    second: second
                },
                x,
                y,
                img,
                chapter
            );
        }
        setTooltip();
        self.thumbnailCache.get(videoPos, setTooltip);
    });
    this.bar.mouseenter(function(event) {
        self.tooltip.show();
    });
    this.bar.mouseleave(function(event) {
        self.tooltip.hide();
    });
        
    this.bar.click(function(event) {
        var videoPos = self.progress.metaData.duration * 
                       (event.clientX / window.innerWidth);
        $.getJSON("api/player/jumpTo?" + Math.floor(videoPos), function() {
            self.page.setPauseDisplay("unPaused");
            self.progress.update(videoPos);
        });
    });
}

Seekbar.prototype.createNodes = function() {
    this.bar = $(document.createElement("div"));
    this.bar.addClass("seekbar");
    
    this.tooltip = $(document.createElement("div"));
    this.tooltip.addClass("seekbartooltip");
    
    this.progressBar = document.createElement("div");
    this.progressBar.className = "progressBar";
    this.bar.append(this.progressBar);
    
    var self = this;
    this.progress = new Progress(this.page, function(second, maximum) {
        self.progressBar.style.width = ((second/maximum)*100) + "%";
    });
    this.progress.onResetCallback = function() {
        self.reset();
    }
    this.progress.fetchMetaData(function() {
        self.bindListeners();
    });
}

Seekbar.prototype.destroy = function() {
    this.progress.clearIntervals();
}
