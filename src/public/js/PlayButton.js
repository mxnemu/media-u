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
        function nonEmptyArray(arrays) {
            if (!arrays.length) { return []; }
            var head = arrays[0];
            var tail = arrays.length > 1 ? arrays.slice(1, arrays.length-1) : [];
            return head.length != 0 ? head : nonEmptyArray(tail);
        }

        return function() {
            var files = nonEmptyArray([episodeList.unwatchedArray(),
                                       episodeList.rewatchPlaylist(),
                                       episodeList.unwatchedArray(true),
                                       episodeList.watchedArray()]);
            if (files.length) {
                PlayButton.play(files);
            }
        }
    },

    ajaxClickCallback: function(tvShow) {
        return function() {
            var url = "api/library/tvshow/" + encodeURIComponent(tvShow) +
                "/details";
            $.getJSON(url, function(data) {
                var episodeList = new EpisodeList(data.episodes, data);
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
