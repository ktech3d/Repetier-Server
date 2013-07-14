GaugeDirectives = angular.module('Gauge',[]);

GaugeDirectives.directive('gauge', function(){
    return {
        restrict: 'E',
        scope: {
            minVal: '@min',
            maxVal: '@max',
            value: '@value',
            scale:'@scale',
            title:'@title'
        },
        template: '<canvas></canvas>',
        controller: function($scope,$element,$timeout) {
            $timeout(function() {
            console.log("scale "+$scope.scale+" max "+$scope.maxVal+" title "+$scope.title);
            console.log($scope);
            console.log($scope.maxVal);
            min = $scope.minVal || 0;
            max = $scope.maxVal || 100;
            title = $scope.title || false;
            units = $scope.units || false;
            if(!$scope.scale || $scope.scale == 0)
                $scope.gauge = new Gauge({ renderTo: $element[0],minValue:min,maxValue:max,title:title,units:units,glow:true });
            else if($scope.scale == 1) {
                console.log("Temp scale");
                $scope.gauge = new Gauge({ renderTo: $element[0],minValue:min,maxValue:max,title:title,units:units,glow:true,
                    highlights:[{ from: 0, to: 60, color: '#72F584' }, { from: 60, to: 260, color: '#f5d185' }, { from: 260, to: max, color: '#F53922' }]});
            } else if($scope.scale == 2)
                $scope.gauge = new Gauge({ renderTo: $element[0],minValue:min,maxValue:max,title:title,units:units,glow:true });
            $scope.$on('value',function(event) {
              $scope.gauge.setValue($scope.value);
            });
            $scope.gauge.draw();
            });
        },
        replace: true,
        link: function(scope, elem, attr) {
            scope.maxVal = attr.maxVal;
            scope.scale = attr.scale;
            console.log("link "+scope.maxVal+ " / "+attr.maxVal);
        }
    };
});

GaugeDirectives.directive("drawing", function(){
    return {
        restrict: "E",
        template:'<canvas></canvas>',
        replace:true,
        link: function(scope, element){
            var ctx = element[0].getContext('2d');
            // variable that decides if something should be drawn on mousemove
            var drawing = false;
            // the last coordinates before the current move
            var lastX;
            var lastY;
            element.bind('mousedown', function(event){
                lastX = event.offsetX;
                lastY = event.offsetY;
                // begins new line
                ctx.beginPath();
                drawing = true;
            });
            element.bind('mousemove', function(event){
                if(drawing){
                    // get current mouse position
                    currentX = event.offsetX;
                    currentY = event.offsetY;

                    draw(lastX, lastY, currentX, currentY);

                    // set current coordinates to last one
                    lastX = currentX;
                    lastY = currentY;
                }
            });
            element.bind('mouseup', function(event){
                // stop drawing
                drawing = false;
            });
            // canvas reset
            function reset(){
                element[0].width = element[0].width;
            }
            function draw(lX, lY, cX, cY){
                // line from
                ctx.moveTo(lX,lY);
                // to
                ctx.lineTo(cX,cY);
                // color
                ctx.strokeStyle = "#4bf";
                // draw it
                ctx.stroke();
            }
        }
    };
});