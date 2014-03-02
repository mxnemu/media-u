function Application() {
    G.app = this;
    this.page = null;
    this.addServerEventListener();
    this.setScreenFromLocationHash();
    this.playerUi = new PlayerUi();
    
    var playerNode = this.playerUi.createNodes();
    $(function() {
        $(".playerFooter").append(playerNode);
    });
}

Application.prototype.setScreenFromLocationHash = function()
{
    var self = this;
    if (!this.page) {
        $(".page").empty();
    }
    
    var fields = Utils.parseHashUrl(window.location.hash);
    
    if (fields.length > 0) {
        if (self.pageList[fields[0]]) {
            var nextPage = new self.pageList[fields[0]];
            self.pageList[fields[0]].prototype.constructor.apply(
                nextPage,
                fields.slice(1)
            );
            self.setPage(nextPage);
        }
    } else {
        self.setPage(new StartPage());
        /*
        $.getJSON("api/activePage", function(data) {
            if (self.pageList[data.page]) {
                //self.setPage(new self.pageList[data.page]);
                self.setPage(new StartPage());
            } else if (data.page == "TvShowPage") {
                self.setPage(new StartPage());
            } else {
                self.setPage(new StartPage());
            }
        }).error(function() {
            console.error("did not receive active page id");
        });
        */
    }
}

Application.prototype.addServerEventListener = function() {
    this.serverEvents = new EventSource("api/events/all");
    this.serverEvents.onopen = function(event) {
        console.log("sse open", event);
    }
    this.serverEvents.onmessage = function(event) {
        console.log("sse message", event);
    }
    this.serverEvents.onerror = function(event) {
        console.log("sse error", event);
    }
}

Application.prototype.pageList = {
    "StartPage": StartPage,
    "TvShowPage": TvShowPage,
    "StreamPlayerPage": StreamPlayerPage
};

Application.prototype.setPage = function(page) {
    if (this.page && this.page.removeNodes) {
        this.page.removeNodes();
    }
    
    this.page = page;
    var node = null;
    if (page) {
        node = page.createNodes();
    }
    
    $(function() {
        $(".page").empty();
        if (node) {
            $(".page").append(node);
        }
    });
};

(function() {
    G.app = new Application();
    G.playerType = "remote";

    $(window).bind("hashchange", function()
    {
        G.app.setScreenFromLocationHash();
    });
})();
