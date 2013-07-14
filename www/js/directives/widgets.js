WidgetsDirectives = angular.module('Widgets', []);

WidgetsDirectives.directive('autoscroll', function(){
    return {
        scope: {scroll:'=autoscroll'},
        restrict: 'A',
        link: function(scope, elem, attrs, ctrls){
            scope.$watch('scroll',function() {
                console.log("auto to "+scope.scroll);
                if(scope.scroll){
                    elem[0].scrollTop = elem[0].scrollHeight;
                }
            });

            elem.bind('scroll', function() {
                console.log("scoll detected");
                scope.$apply(function() {
                    if(scope.scroll)
                        elem[0].scrollTop = elem[0].scrollHeight;
                });
            });
        }
    };
});

WidgetsDirectives.directive('enter', function() {
    return function(scope, element, attrs) {
        element.bind("keydown keypress", function(event) {
            if(event.which === 13) {
                scope.$apply(function(){
                    scope.$eval(attrs.enter);
                });
                event.preventDefault();
            }
        });
    };
});
