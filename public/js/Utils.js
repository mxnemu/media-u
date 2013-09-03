var Utils = function() {};
Utils.parseHashUrl = function(url) {
    if(url) {
        if(url.length > 1) {
            if(url.charAt(0) === '#') {
                var skipCharsAtStart = 1;
                if(url.charAt(skipCharsAtStart) === '!') {
                    skipCharsAtStart++;
                }
                if(url.charAt(skipCharsAtStart) === '/') {
                    skipCharsAtStart++;
                }
                url = url.substr(skipCharsAtStart);
            }

            if(url.charAt(url.length-1) === '/') {
                url = url.slice(0,url.length-1);
            }

            if(url.length != 0) {
                return url.split('/');
            }
        }
    }
    return [];
}

// Thanks to Chris Nielsen
// Url: http://stackoverflow.com/a/1359808
Utils.sortObject = function(o) {
    var sorted = {},
    key, a = [];

    for (key in o) {
        if (o.hasOwnProperty(key)) {
            a.push(key);
        }
    }

    a.sort();

    for (key = 0; key < a.length; key++) {
        sorted[a[key]] = o[a[key]];
    }
    return sorted;
}

// Thanks to Pointy
// Url: http://stackoverflow.com/a/10073788
Utils.paddedNumber = function(n, width, z) {
  z = z || '0';
  n = n + '';
  return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
}
