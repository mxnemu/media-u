function StartPage() {
    
}

StartPage.prototype.fetchInfos = function(callback) {
    var self = this;
    var unfetched = 2;
    var onFetched = function() {
        --unfetched;
        if (unfetched <= 0) {
            callback();
        }
    };
    $.getJSON("api/page/lists", function(data) {
        this.lists = data;
        onFetched();
    });
    $.getJSON("api/page/background", function(data) {
        console.log("got a bg pic:" + data.image);
        self.background = data.image;
        onFetched();
    });
}

StartPage.prototype.createNodes = function() {
    this.fetchInfos(function() {
        var p = $(".page");
        G.app.setBackground(this.background);
        p.append("<h1>StartPage</h1>");
        console.log("start page loaded")
    });
}
