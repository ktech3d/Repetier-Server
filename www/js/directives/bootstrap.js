BootstrapDirectives = angular.module('FoundationHelper', []);
var fidCounter = 0;
function newFid() {
    fidCounter++;
    return "fid" + fidCounter;
}
BootstrapDirectives.directive('foundation', function () {
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
BootstrapDirectives.directive('switch', function () {
    return {
        restrict: 'E',
        controller: function ($scope, $element) {
            $scope.toggle = function() {
                $scope.value = $scope.value ? 0 : 1;
            }
            $scope.name = function() {
                if($scope.value) return $scope.on;
                return $scope.off;
            }
        },
        scope: {value: '=value'},
        template: '<button ng-click="toggle()" class="btn btn-block" ng-class="{\'btn-success\':value,\'btn-danger\':!value}">{{name()}}</button>',
        replace: true,
        link: function (scope, elem, attr, $timeout) {
            scope.on = attr.on || "On";
            scope.off = attr.off || "Off";
        }
    };
});
BootstrapDirectives.directive('boolswitch', function () {
    return {
        restrict: 'E',
        controller: function ($scope, $element) {
            $scope.toggle = function() {
                $scope.value = !$scope.value;
                if($scope.changed)
                  $scope.changed();
            }
            $scope.name = function() {
                if($scope.value) return $scope.on;
                return $scope.off;
            }
        },
        scope: {value: '=value',changed:'&'},
        template: '<button ng-click="toggle()" class="btn btn-block" ng-class="{\'btn-success\':value,\'btn-danger\':!value}">{{name()}}</button>',
        replace: true,
        link: function (scope, elem, attr, $timeout) {
            scope.on = attr.on || "On";
            scope.off = attr.off || "Off";
        }
    };
});
BootstrapDirectives.directive('colorpicker', function () {
    return {
        restrict: 'A',
        controller: function ($scope, $element) {
        },
        scope: {value:'=ngModel'},
        replace: false,
        link: function (scope, elem, attr) {
            inside = false;
            scope.picker = elem.colorpicker();
            scope.picker.on('changeColor', function(ev){
                console.log("changed to "+ev.color.toHex());
                if(inside) return;
                inside = true;
                scope.value = ev.color.toHex();
                $(elem).val(scope.value);
                $(elem).css('border-color',ev.color.toHex());
                inside = false;
                scope.$apply();
            });
            scope.$watch('value',function(newCol) {
                $(elem).val(scope.value);
                $(elem).css('border-color',scope.value);

            });
            $(elem).val(scope.value);
        }
    };
});