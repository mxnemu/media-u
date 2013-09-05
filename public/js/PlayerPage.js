function PlayerPage() {
    this.tvShow = null;
}

PlayerPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/TvShowPage"; // TODO get active page via api
    });
    
    var togglePauseButton = $(document.createElement("span"));
    togglePauseButton.text("II");
    
    togglePauseButton.click(function() {
        if (togglePauseButton.attr("data-status") == "unPaused") {
            $.getJSON("api/player/togglePause", function(data) {
                togglePauseButton.attr("data-status", data.status);
            });
            togglePauseButton.text("▶");
        } else {
            $.getJSON("api/player/togglePause", function(data) {
                togglePauseButton.attr("data-status", data.status);
            });
            togglePauseButton.text("II");
        }
    });
    
    var stopButton = $(document.createElement("span"));
    stopButton.text("■");
    stopButton.addClass("stopButton");
    
    stopButton.click(function() {
        $.getJSON("api/player/stop");
    });
    
    var backwardsButton = $(document.createElement("span"));
    backwardsButton.text("<<");
    backwardsButton.click(function() {
        $.getJSON("api/player/backwards");
    });
    
    var forwardsButton = $(document.createElement("span"));
    forwardsButton.text(">>");
    forwardsButton.click(function() {
        $.getJSON("api/player/forwards");
    });
    
    $.getJSON("api/player/pauseStatus", function(data) {
        if (data.status != "unPaused") {
            togglePauseButton.text("▶");
        }
        togglePauseButton.attr("data-status", data.status);
    });
    
    /* TODO impl in server
    $.getJSON("api/player/tvShow", function(data) {
        
    });
    */
    
    var seekBar = $(document.createElement("div"));
    seekBar.addClass("seekBar");
    
    var seekBarTooltip = $(document.createElement("div"));
    seekBarTooltip.addClass("seekBarTooltip");
    seekBar.hover(function(event) {
        seekBarTooltip.show();
    });
    seekBar.mouseout(function() {
        seekBarTooltip.hide();
    });
    
    var metaData = null;
    $.getJSON("api/player/metaData", function(data) {
        metaData = data;
        
        var thumbnailCache = new ThumbnailCache();
        if (G.preloadSeekBarPreviews) {
            thumbnailCache.loadAll(metaData.duration);
        }
        seekBar.mousemove(function(event) {
            var x = Math.max(event.clientX, 50);
            x = Math.min(x, window.innerWidth - 50);
            seekBarTooltip.css("left", x);
            seekBarTooltip.css("top", seekBar.position().top);
            
            var videoPos = metaData.duration * (event.clientX / window.innerWidth);
            var minute = (videoPos / 60);
            var second = videoPos % 60;
            seekBarTooltip.text(
                Utils.paddedNumber(Math.floor(minute), 2) + ":" +
                Utils.paddedNumber(Math.floor(second), 2)
            );
            thumbnailCache.get(videoPos, function(img) {
                seekBarTooltip.text(
                    Utils.paddedNumber(Math.floor(minute), 2) + ":" +
                    Utils.paddedNumber(Math.floor(second), 2)
                );
                seekBarTooltip.append(img);
            });
        });
    });
    
    page.append(seekBarTooltip);
    page.append(seekBar);
    
    var playerControls = $(document.createElement("div"));
    playerControls.addClass("playerControls");

    playerControls.append(backwardsButton);
    playerControls.append(togglePauseButton);
    playerControls.append(forwardsButton);
    playerControls.append(stopButton);
    page.append(backButton);
    page.append(playerControls);
}
                 
