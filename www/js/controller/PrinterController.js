PrinterController = function($scope,$routeParams,WS,$rootScope,$timeout) {
    slug = $routeParams.slug;
    $rootScope.selectPrinter(slug);
    $scope.logCommands = 0;
    $scope.logACK = 0;
    $scope.logPause = 0;
    $scope.logAutoscroll = 1;
    $scope.cmd = '';
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
            $timeout(responsePoller, 5000);
        });
    };
    $scope.sendCmd = function() {
        WS.send("send",{cmd:$scope.cmd});
        $scope.cmd = "";
    }
    responsePoller();
    $(document).foundation();
}