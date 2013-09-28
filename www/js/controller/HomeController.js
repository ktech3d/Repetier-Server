function HomeController($scope,$rootScope,WS) {
    $scope.r = $rootScope;

    $scope.addPrinter = function() {
        $scope.newprinter = {name:'',slug:''};
        $('#dialogaddprinter').modal('show');
    }

    $scope.createPrinter = function() {
        $scope.newprinter.errors = {};
        if($scope.newprinter.name.length==0)
            $scope.newprinter.errors.name = "No printer name given.";
        if(!$scope.newprinter.slug.match(/^[a-zA-Z0-9]+$/))
            $scope.newprinter.errors.slug = "Only a-z, A-Z and 0-9 are allowed.";
        if(typeof($rootScope.printerConfig[$scope.newprinter.slug])!='undefined')
            $scope.newprinter.errors.slug = "Slug name already in use.";
        if(Object.keys($scope.newprinter.errors).length>0) return;
        WS.send("createConfiguration",$scope.newprinter).then(function(data) {
            $('#dialogaddprinter').modal('hide');
        });
    }

    $scope.replacePrinter = function(p) {
        $scope.confupload = {mode:0,name: p.name,slug: p.slug};
        $('#dialoguploadprinter').modal('show');
    }

    $scope.addPrinterConfig = function() {
        $scope.confupload = {mode:1,name: '',slug: ''};
        $('#dialoguploadprinter').modal('show');
    }

    $scope.createPrinterFromConfig = function() {
        $scope.confupload.errors = {};
        if(!$scope.confupload.slug.match(/^[a-zA-Z0-9]*$/))
            $scope.confupload.errors.slug = "Only a-z, A-Z and 0-9 are allowed.";
        if($scope.confupload.mode==1 && typeof($rootScope.printerConfig[$scope.confupload.slug])!='undefined')
            $scope.confupload.errors.slug = "Slug name already in use.";
        if(Object.keys($scope.confupload.errors).length>0) return;
        $('#formuploadconfig').ajaxSubmit(function (r) {
            r = jQuery.parseJSON(r);
            if(r.error) {
                $scope.confupload.errors.createerror = r.error;
            } else
                $('#dialoguploadprinter').modal('hide');
        });
    }

    $scope.removePrinter = function(p) {
        $scope.confirm("Confirmation required","Really delete printer setup for "+ p.name+" and all associated g-code files?","Yes","No").then(function() {
            WS.send("removeConfiguration",{slug: p.slug});
        });
    }

    $scope.canUpload = function() {
        var fileName = $("#confuploadbutton").val();
        return (fileName.lastIndexOf(".xml")===fileName.length-4);
    }
}