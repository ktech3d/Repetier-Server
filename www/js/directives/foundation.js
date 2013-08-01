FoundationDirectives = angular.module('FoundationHelper', []);
var fidCounter = 0;
function newFid() {
    fidCounter++;
    return "fid" + fidCounter;
}
FoundationDirectives.directive('foundation', function () {
    return {
        restrict: 'A',
        controller: function ($scope, $element) {
        },
        replace: false,
        link: function (scope, elem, attr) {
            console.log("link foundation");
            console.log(elem);
            $(document).foundation();
        }
    };
});
FoundationDirectives.directive('switch', function () {
    return {
        restrict: 'A',
        controller: function ($scope, $element) {
        },
        scope: {value: '=value'},
        template: '<input id="{{idoff}}" name="{{idoff}}" type="radio" ng-model="value" value="0"><label for="{{idoff}}">{{off}}</label><input id="{{idon}}" name="{{idoff}}" ng-model="value" type="radio" value="1"><label for="{{idon}}">{{on}}</label><span></span>',
        replace: false,
        link: function (scope, elem, attr, $timeout) {
            scope.idon = newFid();
            scope.idoff = newFid();
            scope.on = attr.on || "On";
            scope.off = attr.off || "Off";
            $(elem[0]).addClass('switch');
        }
    };
});
FoundationDirectives.directive('boolswitch', function () {
    return {
        restrict: 'A',
        controller: function ($scope, $element) {
        },
        scope: {value: '=value'},
        template: '<input id="{{idoff}}" name="{{idoff}}" type="radio" ng-model="value2" value="0"><label for="{{idoff}}">{{off}}</label><input id="{{idon}}" name="{{idoff}}" ng-model="value2" type="radio" value="1"><label for="{{idon}}">{{on}}</label><span></span>',
        replace: false,
        link: function (scope, elem, attr, $timeout) {
            scope.idon = newFid();
            scope.idoff = newFid();
            scope.on = attr.on || "On";
            scope.off = attr.off || "Off";
            $(elem[0]).addClass('switch');
            scope.$watch('value',function(nv) {
                scope.value2 = (nv ? 1 : 0);
            })
            scope.$watch('value2',function(nv) {
                scope.value = nv == "1";
            })
        }
    };
});
