function Application() {
    this.page = null;
    this.setScreenFromLocationHash();
}

Application.prototype.setScreenFromLocationHash = function()
{
    var self = this;
    if (!this.page) {
        $(".page").empty();
    }
    
    var fields = Utils.parseHashUrl(window.location.hash);
    
    if (fields.length === 1) {
        if (self.pageList[fields[0]]) {
            self.setPage(new self.pageList[fields[0]]);
        }
    } else {
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
    }
}

Application.prototype.pageList = {
    "StartPage": StartPage,
    "TvShowPage": TvShowPage
};

Application.prototype.setPage = function(page) {
    if (this.page && this.page.removeNodes) {
        this.page.removeNodes();
    }
    $(".page").empty();
    
    this.page = page;
    if (page) {
        page.createNodes();
    }
}

Application.prototype.setBackground = function(img) {
    $(".page").css("background-image:" + img);
}

$(function() {
    window.G = {
        app: new Application()
    }
    
    $(window).bind("hashchange", function()
    {
        G.app.setScreenFromLocationHash();
    });
});
