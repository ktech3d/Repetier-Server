FilterModule = angular.module('Filter', []);

// Return a online status badge
FilterModule.filter('online', function () {
    return function (input) {
        if(!input.active)
            return '<span class="label round">Deactivated</span>';
        if (input.online)
            return '<span class="success label round">Online</span>';
        return '<span class="alert label round">Offline</span>';
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
