PrinterConfigController = function($scope,$routeParams,WS,$rootScope,$timeout) {
    slug = $routeParams.slug;
    $rootScope.selectPrinter(slug);
    window.ed = $scope;
    $scope.editor = angular.copy($rootScope.printerConfig[slug]) || {};

    $scope.saveConfig = function() {
        WS.send("setPrinterConfig",$scope.editor);
    }
    $scope.resetConfig = function() {
        $scope.editor = $rootScope.printerConfig[slug];
    }
    $rootScope.$watch('printerConfig.'+slug,function(newVal) {
        console.log("new edit");
        $scope.editor = angular.copy(newVal);
    });
    //$(document).foundation();
}
PrinterController = function($scope,$routeParams,WS,$rootScope,$timeout) {
    slug = $routeParams.slug;
    $rootScope.selectPrinter(slug);
    $scope.logCommands = 0;
    $scope.logACK = 0;
    $scope.logPause = 0;
    $scope.logAutoscroll = 1;
    $scope.cmd = '';
    $scope.models = [];
    $scope.queue = [];
    $scope.activeGCode = false;
    $scope.activeQueue = false;

    lastLogStart = 0;
    var getLoglevel = function() {
        return 12+($scope.logCommands>0 ? 1 : 0) + ($scope.logACK>0 ? 2 : 0);
    }
    var addLogLine = function(line) {
        newline = line.time+': '+line.text;
        $rootScope.active.log.push({c:'log'+line.type,t:newline});
        $rootScope.active.log.splice(0,$rootScope.active.log.length-500);
        lastLogStart = line.id;
    }
    var responsePoller = function() {
        filter = 12+($scope.logCommands>0 ? 1 : 0) + ($scope.logACK>0 ? 2 : 0);
        WS.send("response",{filter:filter,start:lastLogStart}).then(function(r) {
            /*lastLogStart = r['lastid'];
            angular.forEach(r.lines,function(line) {
                addLogLine(line);
            });
*/
            $rootScope.active.state = r.state;
            $timeout(responsePoller, 3000);
        });
    };
    var fetchPrintqueue = function() {
        WS.send("listJobs",{}).then(function(r) {
           $scope.queue = r.data;
        });
    }
    var fetchModels = function() {
        WS.send("listModels",{}).then(function(r) {
           $scope.models = r.data;
        });
    }
    $scope.$watch('logACK',function() {
        WS.send("setLoglevel",{level:getLoglevel()});
    });
    $scope.$watch('logCommands',function() {
        WS.send("setLoglevel",{level:getLoglevel()});
    });
    $scope.$on("connected",function(event) {
        fetchPrintqueue();
        fetchModels();
        WS.send("setLoglevel",{level:getLoglevel()});
    });
    $scope.$on("printqueueChanged",function(event) {
        fetchPrintqueue();
    });
    $scope.$on("jobsChanged",function(event) {
       fetchModels();
    });
    $scope.$on("log",function(event,data) {
        console.log(data);
        addLogLine(data.data);
        $scope.$apply()
    });
    $scope.test = function(v) {
        console.log("test");
        console.log(v);
    }
    $scope.selectQueue = function(gc) {
        console.log("select queue");
        $scope.activeQueue = gc;
    }
    $scope.selectGCode = function(gc) {
        $scope.activeGCode = gc;
    }
    $scope.dequeActive = function() {
        console.log("dequeu");
        WS.send("removeJob",{id:$scope.activeQueue.id}).then(function(r) {
            //$scope.queue = r.data;
            $scope.activeQueue = false;
        });
    }
    $scope.printActiveQueue = function() {
        WS.send("startJob",{id:$scope.activeQueue.id});
    }
    $scope.deleteActiveGCode = function() {
        WS.send("removeModel",{id:$scope.activeGCode.id}).then(function(r) {
            //$scope.models = r.data;
            $scope.activeGCode = false;
            $('#deleteGCodeQuestion').foundation('reveal', 'close');
        });
    }
    $scope.uploadGCode = function() {
        $('#formuploadgcode').ajaxSubmit(function(r) {
            r = jQuery.parseJSON(r);
            $('#uploadGCode').foundation('reveal', 'close');
            //$scope.models = r.data;
        });
    }
    $scope.printGCode = function() {
        console.log("printGCode");
        WS.send("copyModel",{id:$scope.activeGCode.id}).then(function(r) {
            $scope.queue = r.data;
            if($scope.queue.length == 1) {
                WS.send("startJob",{id:$scope.queue[0].id}).then(function(r) {
                   // $scope.queue = r.data;
                });
            }
        });
    }
    $scope.isJobActive = function() {
        return $rootScope.active.status.job!='none';
    }
    $scope.sendCmd = function() {
        WS.send("send",{cmd:$scope.cmd});
        $scope.cmd = "";
    }
    responsePoller();
    fetchModels();
    $(document).foundation();
}