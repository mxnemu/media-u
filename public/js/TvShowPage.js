function TvShowPage() {
    this.tvShow = null;
}

TvShowPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/";
    });
    
    var playButton = $("<input class='playButton' type='button'/>");
    playButton.attr("value", "play");
    playButton.attr("disabled", "disabled");
    
    playButton.click(function() {
        var file = $(".season:first li:first").attr("data-fileName");
        self.play(file);
        console.log("TODO check for first unplayed file and play it");
    })
    
    var seasonsEl = $(document.createElement("div"));
    seasonsEl.addClass("seasons");
    
    $.getJSON("api/page/showDetails", function(data) {
        console.log(data);
        self.tvShow = data;
        playButton.removeAttr("disabled");
        $.each(data.seasons, function() {
            self.createSeasonList(this, seasonsEl);
        })
    });

    page.append(backButton);
    page.append(playButton);
    page.append(seasonsEl);
}

TvShowPage.prototype.createSeasonList = function(season, seasonsEl) {
    var self = this;
    var seasonEl = $(document.createElement("ul"));
    seasonEl.addClass("season");
    $.each(season.episodes, function() {
        var episodeEl = $(document.createElement("li"));
        episodeEl.text(this); // TODO get another json with all the details
        episodeEl.attr("data-fileName", this);
        
        episodeEl.click(function() {
            self.play($(this).attr("data-fileName"));
        });
        
        seasonEl.append(episodeEl);
    });
    seasonsEl.append(seasonEl);
}

TvShowPage.prototype.play = function(episode) {
    var json = {
        tvShow: this.tvShow.name,
        filename: episode
    }
    if (G.playerType == "stream") {
        G.video = "/video/" + episode; // TODO DON'T SAVE IT HERE
        window.location.hash = "#!/StreamPlayerPage/";
    } else {
        $.getJSON("api/player/play?" + JSON.stringify(json), function(data) {
            console.log("TODO set page to mplayer remote controller");
            if (!data.error) {
                window.location.hash = "#!/PlayerPage";
            }
        });
    }
}
