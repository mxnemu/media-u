function StreamPlayerPage() {
    this.tvShow = null;
}

StreamPlayerPage.prototype.createNodes = function() {
    var self = this;
    var page = $(".page");
    
    var backButton = $("<input class='backButton' type='button'/>");
    backButton.attr("value", "back");
    backButton.click(function() {
        window.location.hash = "#!/TvShowPage"; // TODO get active page via api
    });
    
    
    var fields = Utils.parseHashUrl(window.location.hash);
    
    var video = document.createElement("video")
    video.src = G.video;
    video.innerHTML = "<source src='" + G.video + "'/>";
    
    page.append(backButton);
    page.append(video);
}
