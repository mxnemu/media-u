function Seekbar(container) {
    this.progress = null;
    this.tooltip = null;
    this.container = container;
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
    
    function jump(clientX) {
        var videoPos = self.progress.metaData.duration * 
                       (clientX / window.innerWidth);
        $.getJSON("api/player/jumpTo?" + Math.floor(videoPos));
    }

    var getThumbTimeout = null;
    function previewMove(clientX, loadThumb) {
        window.clearTimeout(getThumbTimeout);
    
        var x = Math.max(clientX, 50);
        x = Math.min(x, window.innerWidth - 50);
        var y = self.bar.offset().top;
        
        var videoPos = self.progress.metaData.duration * 
                       (clientX / window.innerWidth);
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
        
        var delay = self.thumbnailCache.isLoading() ? 60 : 0;
        getThumbTimeout = window.setTimeout(function() {
            self.thumbnailCache.get(videoPos, setTooltip);
        }, delay);
    }

    //touch seek
    var doTouchSeek = false;
    this.container.on("touchstart", function(event) {
        if ($(event.originalEvent.target).hasClass("button")) {
            doTouchSeek = false;
        } else {
            doTouchSeek = true;
        }
        self.tooltip.show();
    });
    this.container.on("touchcanel", function() {
        self.tooltip.hide();
        touchMoved = false;
        doTouchSeek = false;
    });
    this.container.on("touchleave", function() {
        self.tooltip.hide();
        touchMoved = false;
        doTouchSeek = false;
        console.log("leave", touchMoved);
    });
    var touchMoved = false;    
    this.container.on("touchend", function(event) {
        if (touchMoved) {
            jump(event.originalEvent.changedTouches[0].clientX);
            touchMoved = false;
            doTouchSeek = false;
        }
        self.tooltip.hide();
    });
    var touchMoveHappenedDelay = null;
    this.container.on("touchmove", function(event) {
        if (doTouchSeek) {
            previewMove(event.originalEvent.touches[0].clientX);
            touchMoved = true;
            console.log("move", touchMoved);
        }
    });
    
    // seek
    this.bar.mousemove(function(event) {
        previewMove(event.clientX);
    });
    this.bar.mouseenter(function(event) {
        self.tooltip.show();
    });
    this.bar.mouseleave(function(event) {
        self.tooltip.hide();
        touchMoved = false;
        doTouchSeek = false;
    });
    
        
    this.bar.click(function(event) {
        jump(event.clientX);
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
