function Seekbar() {
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

Seekbar.prototype.start = function() {
    this.thumbnailCache = new ThumbnailCache();
    if (G.preloadSeekBarPreviews) {
        this.thumbnailCache.loadAll(this.progress.metaData.duration);
    }
    this.bindEvents();
}

Seekbar.prototype.reset = function() {
    this.unbindEvents();
}

Seekbar.prototype.unbindEvents = function() {
    this.tooltip.hide();
    this.bar.unbind("mousemove");
    this.bar.unbind("mouseenter");
    this.bar.unbind("mouseleave");
    this.bar.unbind("click");
    
}

Seekbar.prototype.bindEvents = function() {
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
        $.getJSON("api/player/jumpTo?" + Math.floor(videoPos));
    });
}

Seekbar.prototype.createNodes = function() {
    this.bar = $(document.createElement("div"));
    this.bar.addClass("seekbar");
    
    this.tooltip = $(document.createElement("div"));
    this.tooltip.addClass("seekbartooltip");
    this.tooltip.hide();
    
    this.progressBar = document.createElement("div");
    this.progressBar.className = "progressBar";
    this.bar.append(this.progressBar);
    
    var self = this;
    this.progress = new Progress(function(second, duration) {
        self.progressBar.style.width = ((second/duration)*100) + "%";
    });
    this.progress.onReady(function() {
        self.start();
    });
    
    this.progress.onReset(function() {
        self.reset();
    });
}

Seekbar.prototype.destroy = function() {
    this.unbindEvents();
    this.progress.unbindEvents();
}
