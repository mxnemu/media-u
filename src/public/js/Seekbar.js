function Progress(page, onUpdateCallback) {
    this.seconds = 0;
    this.path = null;
    this.metaData = null;
    this.page = page;
    this.onUpdateCallback = onUpdateCallback;
}

Progress.prototype.fetchMetaData = function(callback) {
    var self = this;
    $.getJSON("api/player/metaData", function(data) {
        self.metaData = data;
        self.startUp();
        callback();
    });
}

Progress.prototype.startUp = function() {
    var self = this;
    
    $.getJSON("api/player/progress", function(data) {
        self.path = data.path;
        self.update(data.seconds);
        self.progressUpdateIntervalId = window.setInterval(function() {
            if (self.page.togglePauseButton.attr("data-status") == "unPaused") {
                self.update(self.seconds +0.05);
            }
        }, 50);
        // sync with the player's progress every 10s
        self.progressSyncIntervalId = window.setInterval(function() {
            self.resync();
        }, 10000);
    });
}

Progress.prototype.update = function(second) {
    this.seconds = Math.min(second, this.metaData.duration);
    this.onUpdateCallback(second, this.metaData.duration)
    if (this.seconds >= this.metaData.duration) {
        window.clearInterval(this.progressUpdateIntervalId);
        window.clearInterval(this.progressSyncIntervalId);
    }
}

Progress.prototype.resync = function() {
    var self = this;
    $.getJSON("api/player/progress", function(d) {
        self.update(d.seconds);
    });
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

Seekbar.prototype.createNodes = function() {
    var bar = $(document.createElement("div"));
    bar.addClass("seekbar");
    
    this.tooltip = $(document.createElement("div"));
    this.tooltip.addClass("seekbartooltip");
    
    this.progressBar = document.createElement("div");
    this.progressBar.className = "progressBar";
    bar.append(this.progressBar);
    
    var self = this;
    this.progress = new Progress(this.page, function(second, maximum) {
        self.progressBar.style.width = ((second/maximum)*100) + "%";
    });
    
    this.progress.fetchMetaData(function() {

        var thumbnailCache = new ThumbnailCache();
        if (G.preloadSeekBarPreviews) {
            thumbnailCache.loadAll(self.progress.metaData.duration);
        }    
    
        bar.mousemove(function(event) {
            var x = Math.max(event.clientX, 50);
            x = Math.min(x, window.innerWidth - 50);
            var y = bar.offset().top;
            
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
            thumbnailCache.get(videoPos, setTooltip);
        });
        bar.mouseenter(function(event) {
            self.tooltip.show();
        });
        bar.mouseleave(function(event) {
            self.tooltip.hide();
        });
            
        bar.click(function(event) {
            var videoPos = self.progress.metaData.duration * 
                           (event.clientX / window.innerWidth);
            $.getJSON("api/player/jumpTo?" + Math.floor(videoPos), function() {
                self.page.setPauseDisplay("unPaused");
                self.progress.update(videoPos);
            });
        });
    });
        
    this.bar = bar;
}
