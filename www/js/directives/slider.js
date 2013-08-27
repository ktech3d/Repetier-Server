WidgetsDirectives = angular.module('Slider', []);

/* WidgetsDirectives.directive('slider', function(){
    return {
        scope: {
            model:'=ngModel',
            orientation:'@orientation',
            min:'@min',
            max:'@max',
            step:'@step',
            precision:'@precision'
        },
        restrict: 'A',
        replace:false,
        link: function(scope, elem, attrs, ctrls){
            var w = elem.width();
            var h = elem.height();
            var hor;
            if(scope.orientation == 'horizontal') {
                h = 40;
                elem.height(40);
                hor = true;
            } else {
                w = 40;
                elem.width(40);
                hor = false;
            }
            var stage = new Kinetic.Stage({
                container: elem.prop('id'),
                width: w,
                height: h
            });
            var layer = new Kinetic.Layer();
            stage.add(layer);
            var backline;
            var slider;
            var textMin;
            var textMax;
            var textSlider;
            if(hor) {
                console.log("add line");
                backline = new Kinetic.Line({
                    points:[10,h/2,w,h/2],
                    lineCap:'round',
                    lineJoin: 'round',
                    //width:20,
                    stroke:'black',
                    strokeWidth:20,
                    //   fillLinearGradientStartPoint:[0,5],
                    // fillLinearGradientEndPoint:[0,-5],
                    fillLinearGradientColorStops: [0, 'red', 1, 'yellow']
                });
                slider = new Kinetic.Circle({
                    x: stage.getWidth() / 2,
                    y: h / 2,
                    radius: 14,
                    fill: 'red',
                    stroke: 'black',
                    strokeWidth: 2,
                    draggable:true,
                    dragBoundFunc: function(pos) {
                        cury = this.getAbsolutePosition().y;
                        if(cury<0) cury = 0;
                        if(cury>=w) cury = w-1;
                        return {
                            x: pos.x,
                            y: cury
                        }
                    }
                });
                var textMin = new Kinetic.Text({
                    x: stage.getWidth() / 2,
                    y: 15,
                    text: 'Simple Text',
                    fontSize: 30,
                    fontFamily: 'Calibri',
                    fill: 'green'
                });
            }
            layer.add(backline);
            layer.add(slider);
            stage.draw();
            console.log("min = "+scope.min+" max = "+scope.max+" step="+scope.step+" precision="+scope.precision+" val = "+scope.model);
            console.log(elem.prop('id'));
            var reposition = function() {
                console.log("repo w"+w);
                if(stage == null) return; // already deleted
                stage.draw();
            }
            scope.$watch('model',function() {
                reposition();
            });
            var resizer = function () {
                console.log("new width "+elem.width());
                if(hor) {
                    w = elem.width();
                    stage.setWidth(w);
                } else {
                    h = elem.height();
                    stage.setHeight(h);
                }
                reposition();
            };
            $(window).resize(resizer);
            scope.$on('$destroy',function() {
                console.log("Destroy slider");
                $(window).off("resize",resizer);
                stage.destroy();
                stage = null;
            });
            console.log("w = "+w+" h = "+h);
        }
    };
});*/

WidgetsDirectives.directive('slider', function($timeout){
    return {
        scope: {
            model:'=ngModel',
            min:'@min',
            max:'@max',
            step:'@step',
            precision:'@precision',
            size:'@',
            enabled:'@',
            moved:'&'
        },
        restrict: 'A',
        replace:false,
        link: function(scope, elem, attrs, ctrls){
            flip = false;
            var letter = '';
            if(attrs.flip)
              flip = Boolean(attrs.flip);
            if(attrs.letter)
              letter = attrs.letter;
            slider = $(elem).slider({
                orientation:attrs.orientation,
                step:attrs.step,
                min:0,
                max:100,
                flip:flip,
                letter:letter,
                formater:function(x) {
                    return scope.$eval(x+" | number:precision");
                }
            });
            slider.on('slideStop',function(ev) {
                scope.$apply(function() {
                    scope.model = parseFloat(ev.value);
                    if(scope.moved)
                        scope.moved({value:parseFloat(ev.value)});
                })
            });
            scope.$watch('model',function() {
                //console.log("model "+letter+" "+typeof(attrs.enabled)+" / "+scope.enabled+ " / v= "+scope.model+" min "+scope.min+" max "+scope.max);
                if(typeof(attrs.enabled) === "undefined" || Boolean(scope.enabled)) {
                    //console.log("set "+scope.model+ " / "+scope.enabled);
                    $(elem).slider('setValue',scope.model);
                }
            });
            scope.$watch('enabled',function() {
                $(elem).slider('setEnabled',Boolean(scope.enabled));
                if(scope.enabled)
                    $(elem).slider('setValue',scope.model);
            });
            scope.$watch('min',function() {
                $(elem).slider("setMin",parseFloat(scope.min));
                $(elem).slider('setValue',scope.model);
            });
            scope.$watch('max',function() {
                $(elem).slider("setMax",parseFloat(scope.max));
                $(elem).slider('setValue',scope.model);
            });
            scope.$watch('step',function() {
                $(elem).slider("setStep",parseFloat(scope.step));
                $(elem).slider('setValue',scope.model);
            });
            scope.$watch('size',function() {
                $(elem).slider("setSize",parseFloat(scope.size));
                $(elem).slider('setValue',scope.model);
            });
        }
    };
});