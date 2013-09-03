FilterModule = angular.module('Filter', []);

// Return a online status badge
FilterModule.filter('online', function () {
    return function (input) {
        if(!input.active)
            return '<span class="label">Deactivated</span>';
        if (input.online)
            return '<span class="label label-success">Online</span>';
        return '<span class="label label-danger">Offline</span>';
    }
});

// Add temperature unit or return off
FilterModule.filter('temp', function () {
    return function (input) {
        if(input == 0) return "Off";
        return input+"°C";
    }
});

FilterModule.filter('printing', function () {
    return function (input) {
        if(input.job == 'none')
            return 'No print job running';
        return input.job;
    }
});

FilterModule.filter('byte', function () {
    return function (input) {
        if(input < 1024) return input+" byte";
        input /= 1024.0;
        if(input < 1024) return input.toFixed(1)+" kB";
        input /= 1024.0;
        return input.toFixed(1)+" MB";
    }
});

function two_digits(a) {
    a = a.toFixed(0);
    if(a.length==1) return "0"+a;
    return a;
}
FilterModule.filter('hms', function () {
    return function (input) {
        if(input<0) return "Computing...";
        hours = Math.floor(input/3600);
        input-=3600*hours;
        min = Math.floor(input/60);
        input -= min*60;
        t = "";
        if(hours>0)
            t += hours+"h ";
        if(min>0 || hours>0)
            t += two_digits(min)+"m ";
        return t+two_digits(input)+"s";
    }
});
