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
            if (self.pageList[data.page]) {
                self.setPage(new self.pageList[data.page]);
            } else {
                self.setPage(new StartPage());
            }
        });
    }
}

Application.prototype.pageList = {
    "StartPage": StartPage
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
});
