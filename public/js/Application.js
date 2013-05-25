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
        // TODO set page by fields and field content
    } else {
        $.getJSON("api/activePage", function(data) {
            self.setPage(new self.pageList[data.page]);
        });
    }
}

Application.prototype.pageList = {
    "StartPage": StartPage
};

Application.prototype.setPage = function(page) {
    if (this.page) {
        this.page.removeNodes();
    }
    this.page = page;
    if (page) {
        page.createNodes();
    }
}

$(function() {
    window.G = {
        app: new Application()
    }
});
