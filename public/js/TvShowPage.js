function TvShowPage() {
    this.tvShow = null;
}

TvShowPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    var playButton = $("<input class='playButton' type='button'/>");
    
    var seasonsEl = $(document.createElement("div"));
    seasonsEl.addClass("seasons");
    
    $.getJSON("api/page/showDetails", function(data) {
        console.log(data);
        self.tvShow = data;
        $.each(data.seasons, function() {
            self.createSeasonList(this, seasonsEl);
        })
    });
    
    playButton.click(function() {
        self.play();
    })
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
    $.getJSON("api/playEpisode?" + JSON.stringify(json), function() {
        console.log("TODO set page to mplayer remote controller");
    });

}
