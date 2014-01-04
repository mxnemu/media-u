PlayButton = {
    create: function(episodeList) {
        var button = $("<input class='playButton' type='button'/>");
        button.attr("value", "play");
        
        if (episodeList) {
            this.initOnClick(button, episodeList);
        }
        
        return button;
    },
    
    initOnClick: function(button, episodeList) {
        if (episodeList instanceof EpisodeList) {
            button.click(PlayButton.episodeListClickCallback(episodeList));
        } else if (typeof episodeList == "string") {
            button.click(PlayButton.ajaxClickCallback(episodeList));
        } else {
            var err = "invalid initOnClick with arg" + episodeList;
            throw err;
        }
    },
    
    initOnClickAjax: function() {
        
    },
    
    episodeListClickCallback: function(episodeList) {
        return function() {
            var files = episodeList.unwatchedArray();
            if (files.length) {
                PlayButton.play(files);
            } else {
                PlayButton.play(episodeList.watchedArray());
            }
        }
    },
    
    ajaxClickCallback: function(tvShow) {
        return function() {
            $.getJSON("/api/library/tvShowDetails?" + tvShow, function(data) {
                var episodeList = new EpisodeList(data.episodes);
                PlayButton.episodeListClickCallback(episodeList)();
            });
        }
    },
    
    play: function(files) {
        var json = {
            episodes: files
        }
        
        $.ajax({
            url: "api/player/setPlaylist",
            type: "POST",
            data: JSON.stringify(json)
        }).done(function(data) {
            if (data.error) {
                alert(data.error)
            }
        });
    }
};
