BootstrapDirectives = angular.module('FoundationHelper', []);
var fidCounter = 0;
function newFid() {
    fidCounter++;
    return "fid" + fidCounter;
}
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
BootstrapDirectives.directive('inputerror', function () {
    return {
        restrict: 'E',
        scope: {error: '=error'},
        template: '<div class="error" ng-show="error">{{error}}</div>',
        replace: true,
        link: function (scope, elem, attr) {
        }
    };
});

BootstrapDirectives.directive('fileUpload',function() {
    return {
        restrict:'E',
        scope: {
            done:'&',
            url:'@',
            title:'@',
            response:'&',
            formData:'@'
        },
        replace:true,
        //transclude:true,
        template:'<div><form enctype="multipart/form-data"><span class="btn  fileinput-button"><i class="icon-upload"></i> {{infoText}}<input type="file" name="file" ng-disabled="disabled">'+
            '</span><div ng-show="uploading"><p class="size">File size:{{fileSize | formatFileSize}}</p>'+
            '<div class="progress progress-success progress-striped active"><div class="bar" style="width:{{percent}}%}"><span style="position:absolute;text-align:center;left:0;right:0;color:#000000">{{percent}}%</span></div></div></div></form>' +
            '<div ng-show="fail" class="alert alert-error"><h4>Upload failed</h4>{{fail}}</div>' +
            '</div>',
        link:function(scope,element,attr) {
            console.log("to");
            console.log("link "+scope.url+" title "+scope.title);
            if(scope.title)
                scope.infoText = scope.title;
            else
                scope.infoText = "Upload";
            var optionsObj = {
                dataType:'json',
                add: function (e, data) {
                    scope.$apply(function() {
                        scope.uploading = true;
                        scope.percent = 0;
                        scope.fail = false;
                    });
                    data.submit();
                },
                fail: function (e, data) {
                    scope.$apply(function() {
                        scope.uploading = false;
                        scope.percent = 0;
                        scope.fail = "Upload failed";
                        reason = angular.fromJson(data.response().jqXHR.responseText);
                        if(reason.error.message)
                            scope.fail = reason.error.message;
                    });
                },
                url:scope.url,
                formData:scope.formData
            };
            scope.percent = 0;
            scope.uploading = false;
            scope.file = $('input[type=file]',$(element));
            optionsObj.done = function(e,data) {
                scope.$apply(function() {
                    scope.uploading = false;
                    console.log("done");console.log(scope.response);
                    if(scope.response)
                        scope.response({data:angular.fromJson(data.response().jqXHR.responseText)});
                    if(scope.done)
                        scope.done({e:e,data:data});
                })
            }
            optionsObj.progress = function(e,data) {
                scope.$apply(function() {
                    scope.fileSize = data.total;
                    scope.percent = parseInt(data.loaded / data.total * 100, 10);
                })
            }
            scope.file.fileupload(optionsObj);
            attr.$observe('url', function(value) {
                scope.file.fileupload('option','url', value);
            });
            attr.$observe('title', function(value) {
                scope.infoText = value;
            });
        }
    };
})