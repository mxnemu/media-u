function Seekbar(page) {
    this.page = page;
    this.createNodes();
}

Seekbar.prototype.createNodes = function() {
    var bar = $(document.createElement("div"));
    bar.addClass("seekbar");
    
    var tooltip = $(document.createElement("div"));
    tooltip.addClass("seekbartooltip");
    bar.hover(function(event) {
        tooltip.show();
    });
    bar.mouseout(function() {
        tooltip.hide();
    });
    
    var metaData = null;
    var self = this;
    $.getJSON("api/player/metaData", function(data) {
        metaData = data;
        
        var thumbnailCache = new ThumbnailCache();
        if (G.preloadSeekBarPreviews) {
            thumbnailCache.loadAll(metaData.duration);
        }
        bar.mousemove(function(event) {
            var x = Math.max(event.clientX, 50);
            x = Math.min(x, window.innerWidth - 50);
            var tooltipY = bar.offset().top
            
            var videoPos = metaData.duration * (event.clientX / window.innerWidth);
            var minute = (videoPos / 60);
            var second = videoPos % 60;
            
            var chapter = null;
            for (var i=0; i < metaData.chapters.length; ++i) {
                if (videoPos > metaData.chapters[i].start &&
                    videoPos < metaData.chapters[i].end) {
                    
                    chapter = metaData.chapters[i].title;
                }
            }
            
            function setTooltip(img) {
                tooltip.empty();
                if (chapter) {
                    tooltip.append("<span>" + chapter + "</span>");
                    tooltip.append("<br/>");
                    tooltipY -= 15;
                }
                tooltip.append("<span>" +
                    Utils.paddedNumber(Math.floor(minute), 2) + ":" +
                    Utils.paddedNumber(Math.floor(second), 2) +
                    "</span>"
                );
                if (img) {
                    tooltip.append(img);
                }
                tooltip.css("left", x);
                tooltip.css("top", tooltipY);
            }
            setTooltip();
            thumbnailCache.get(videoPos, setTooltip);
        });
        
        // TODO cleanup this lazy mess
        var progress;
        var progressBar = document.createElement("div");
        progressBar.className = "progressBar";
        bar.append(progressBar);
        function setProgress(second) {
            progress = Math.min(second, metaData.duration);
            progressBar.style.width = ((second/metaData.duration)*100) + "%";
        };
        
        $.getJSON("api/player/progress", function(data) {
            setProgress(data.progress);
            window.setInterval(function() {
                if (self.page.togglePauseButton.attr("data-status") == "unPaused") {
                    setProgress(progress +1);
                }
            }, 1000);
            // sync with the player's progress every 10s
            window.setInterval(function() {
                $.getJSON("api/player/progress", function(d) {
                    setProgress(d.progress);
                });
            }, 10000);
        });
        
        bar.click(function(event) {
            var videoPos = metaData.duration * (event.clientX / window.innerWidth);
            $.getJSON("api/player/jumpTo?" + Math.floor(videoPos), function() {
                self.page.setPauseDisplay("unPaused");
                setProgress(videoPos);
            });
        });
    });
    
    this.bar = bar;
    this.tooltip = tooltip;
}
