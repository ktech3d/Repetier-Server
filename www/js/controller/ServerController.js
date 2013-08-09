function ServerController($scope,$rootScope,$timeout,$http,WS) {
    $rootScope.printerList = [];
    $rootScope.messages = [];
    $rootScope.setup = {printer:[{name:'Ordbot'},{name:'DeltaTower'}]};
    $rootScope.active = {status:{}};
    $rootScope.activeSlug = '';
    $rootScope.serverSetup = {};
    $rootScope.printerSetup = {};
    $rootScope.printerConfig = {};
    $rootScope.printer = {};
    window.rs = $rootScope; // for debugging
    $rootScope.selectPrinter = function(slug) {
        console.log("Activate "+slug);
        WS.selectPrinter(slug);
        $rootScope.activeSlug = slug;
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
                    if(firstPrinterPoll) {
                        WS.send("getPrinterConfig",{printer: p.slug}).then(function(c) {
                           $rootScope.printerConfig[p.slug] = c;
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
        });
    }
    $scope.$on("messagesChanged",function(event,data) {
        console.log("messages changed event");
       messagesPoller();
    });
    // Update configuration when changed
    $scope.$on("config",function(event,data) {
       $rootScope.printerConfig[data.data.general.slug] = data.data;
    });
    $scope.$on("connected",function(event) {
       printerPoller();
       messagesPoller();
    });
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
}

function AboutController($scope) {

}