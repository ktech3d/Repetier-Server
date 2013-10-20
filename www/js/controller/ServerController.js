function ServerController($scope,$rootScope,$timeout,$http,WS,$q,User) {
    $rootScope.user = User;
    $rootScope.printerList = [];
    $rootScope.messages = [];
    $rootScope.setup = {printer:[]};
    $rootScope.active = {status:{}};
    $rootScope.activeSlug = '';
    $rootScope.serverSetup = {};
    $rootScope.printerSetup = {};
    $rootScope.printerConfig = {};
    $rootScope.printer = {};
    $scope.externalCommands = [];
    $scope.question = {};
    $rootScope.loading = false;
    window.rs = $rootScope; // for debugging
    $rootScope.selectPrinter = function(slug) {
        console.log("Activate "+slug);
        WS.selectPrinter(slug);
        $rootScope.activeSlug = slug;
        $rootScope.activeConfig = $rootScope.printerConfig[slug];
        if($rootScope.printer[slug])
            $rootScope.active = $rootScope.printer[slug];
    }
    var firstPrinterPoll = true;
    var printerPoller = function() {
        WS.send("listPrinter",{}).then(function(r) {
            if(angular.toJson($rootScope.setup.printer) != angular.toJson(r)) {
                $rootScope.setup.printer = r;
                $rootScope.printerList = r;
                angular.forEach(r,function(p) {
                    if(!$rootScope.printer[p.slug]) {
                        $rootScope.printer[p.slug] = {status:p,log:[],history:[],layer:[],previousLayer:[]};
                        if(p.slug == $rootScope.activeSlug)
                            $rootScope.active = $rootScope.printer[p.slug];
                    } else $rootScope.printer[p.slug].status = p;
                    if(firstPrinterPoll || typeof($rootScope.printerConfig[p.slug])=="undefined") {
                        WS.send("getPrinterConfig",{printer: p.slug}).then(function(c) {
                            $rootScope.printerConfig[p.slug] = c;
                            $rootScope.activeConfig = $rootScope.printerConfig[$rootScope.activeSlug];
                        });
                    }
                });
                firstPrinterPoll = false;
            }
            //$rootScope.messages = r.data.messages;
            $timeout(printerPoller, 2000);
        });
    };

    var messagesPoller = function() {
        WS.send("messages",{}).then(function(r) {
            $rootScope.messages = r;
            if($rootScope.messages.length == 0)
                $('#messageList').modal('hide');
        });
    }
    $scope.$on("messagesChanged",function(event,data) {
       messagesPoller();
    });
    // Update configuration when changed
    $scope.$on("config",function(event,data) {
        $rootScope.printerConfig[data.data.general.slug] = data.data;
        $rootScope.activeConfig = $rootScope.printerConfig[$rootScope.activeSlug];
    });
    $scope.$on("connected",function(event) {
       printerPoller();
       messagesPoller();
        WS.send("listExternalCommands",{}).then(function(r) {
            $scope.externalCommands = r;
        })
    });
    $scope.runExternalCommand = function(id) {
        WS.send("runExternalCommand",{id:id});
    }
    $scope.removeMessage = function(idx) {
        m = $rootScope.messages[idx];
        a = 'job';
        if(m.link.indexOf("unpause")>0)
            a = 'unpause';

        WS.send("removeMessage",{id:m.id,a:a});
        //,function(r) {
        //    $rootScope.messages = r;
        //});
    }
    $scope.closeReveal = function(id) {
        $('#'+id).modal('hide');
    }

    var resizer = function () {
        $rootScope.$broadcast("windowResized");
    };
    $(window).resize(resizer);
    $scope.$on('$destroy',function() {
        $(window).off("resize",resizer);
    });
    $scope.confirm = function(head,body,yes,no) {
        $scope.question.header = head;
        $scope.question.question = body;
        $scope.question.no = no;
        $scope.question.yes = yes;
        $scope.question.deferred = $q.defer();
        $('#question').modal('show');
        return $scope.question.deferred.promise;
    }
    $scope.questionYes = function() {
        $('#question').modal('hide');
        $scope.question.deferred.resolve();
    }
    $scope.questionNo = function() {
        $('#question').modal('hide');
        $scope.question.deferred.reject();
    }
    $rootScope.$on('load',function(event) {
        $scope.loading = true;
    });
    $rootScope.$on('loaded',function(event) {
        $scope.loading = false;
    });
}

function AboutController($scope) {

}