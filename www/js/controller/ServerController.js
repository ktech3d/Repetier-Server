function ServerController($scope,$rootScope,$timeout,$http,WS) {
    $rootScope.printerList = [];
    $rootScope.messages = [];
    $rootScope.setup = {printer:[{name:'Ordbot'},{name:'DeltaTower'}]};
    $rootScope.active = {status:{}};
    $rootScope.activeSlug = '';
    $rootScope.serverSetup = {};
    $rootScope.printerSetup = {};
    $rootScope.printer = {};
    window.rs = $rootScope; // for debugging
    $rootScope.selectPrinter = function(slug) {
        console.log("Activate "+slug);
        WS.selectPrinter(slug);
        $rootScope.activeSlug = slug;
        if($rootScope.printer[slug])
            $rootScope.active = $rootScope.printer[slug];
    }
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
                });
            }
            //$rootScope.messages = r.data.messages;
            $timeout(printerPoller, 2000);
        });
    };
    printerPoller();

    var messagesPoller = function() {
        WS.send("messages",{}).then(function(r) {
            $rootScope.messages = r;
        });
    }
    messagesPoller();
    $scope.$on("messagesChanged",function(event,data) {
        console.log("messages changed event");
       messagesPoller();
    });
    $scope.$on("connected",function(event) {
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
        $('#'+id).foundation('reveal', 'close');
    }
}

function AboutController($scope) {

}