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
    var responsePoller = function() {
        filter = 12+($scope.logCommands>0 ? 1 : 0) + ($scope.logACK>0 ? 2 : 0);
        WS.send("response",{filter:filter,start:lastLogStart}).then(function(r) {
            lastLogStart = r['lastid'];
            angular.forEach(r.lines,function(line) {
                newline = line.time+': '+line.text;
                $rootScope.active.log.push({c:'log'+line.type,t:newline});
            });
            $rootScope.active.log.splice(0,$rootScope.active.log.length-500);

            $rootScope.active.state = r.state;
            $timeout(responsePoller, 3000);
        });
    };
    var jobPoller = function() {
        WS.send("listJobs",{}).then(function(r) {
           $scope.queue = r.data;
            $timeout(jobPoller,12344);
        });
    }
    jobPoller();
    var fetchModels = function() {
        WS.send("listModels",{}).then(function(r) {
           $scope.models = r.data;
        });
    }
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
            $scope.queue = r.data;
            $scope.activeQueue = false;
        });
    }
    $scope.printActiveQueue = function() {
        WS.send("startJob",{id:$scope.activeQueue.id});
    }
    $scope.deleteActiveGCode = function() {
        WS.send("removeModel",{id:$scope.activeGCode.id}).then(function(r) {
            $scope.models = r.data;
            $scope.activeGCode = false;
            $('#deleteGCodeQuestion').foundation('reveal', 'close');
        });
    }
    $scope.uploadGCode = function() {
        $('#formuploadgcode').ajaxSubmit(function(r) {
            r = jQuery.parseJSON(r);
            console.log("finished");
            console.log(r);
            $('#uploadGCode').foundation('reveal', 'close');
            $scope.models = r.data;
        });
    }
    $scope.printGCode = function() {
        console.log("printGCode");
        WS.send("copyModel",{id:$scope.activeGCode.id}).then(function(r) {
            $scope.queue = r.data;
            if($scope.queue.length == 1) {
                WS.send("startJob",{id:$scope.queue[0].id}).then(function(r) {
                    $scope.queue = r.data;
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