ScriptConfigController = function ($scope, $routeParams, WS, $rootScope, $timeout,$q) {
    slug = $routeParams.slug;
    $rootScope.selectPrinter(slug);
    $scope.scripts = [
        {name: "start", title: "Run before job"},
        {name: "end", title: "Run after job"},
        {name: "pause", title: "Run on pause"},
        {name: "kill", title: "Run if job is aborted"},
        {name: "script_1", title: "Custom script 1"},
        {name: "script_2", title: "Custom script 2"},
        {name: "script_3", title: "Custom script 3"},
        {name: "script_4", title: "Custom script 4"},
        {name: "script_5", title: "Custom script 5"}
    ];
    $scope.scriptLoaded = false;
    $scope.activeScript = $scope.scripts[0];
    $scope.scriptContent = "";
    var origScript = "";
    $scope.selectScript = function (script) {
        $scope.saveScript().then(function () {
            $scope.activeScript = script;
            WS.send("getScript", {name: script.name}).then(function (r) {
                $scope.scriptContent = origScript = r.script;
                $scope.scriptLoaded = true;
            })
        });
    }
    $scope.saveScript = function () {
        var deferred = $q.defer();
        deferred.resolve();
        if ($scope.scriptLoaded == false) return deferred.promise;
        if (origScript == $scope.scriptContent) return deferred.promise;
        $scope.scriptLoaded = false;
        return WS.send("setScript", {name: $scope.activeScript.name, script: $scope.scriptContent});
    }
    $scope.$on('$destroy', function () {
        $scope.saveScript();
    });
    WS.send("getScript", {name: $scope.activeScript.name}).then(function (r) {
        $scope.scriptContent = origScript = r.script;
        $scope.scriptLoaded = true;
    });
}

PrinterConfigController = function ($scope, $routeParams, WS, $rootScope, $timeout) {
    slug = $routeParams.slug;
    $rootScope.selectPrinter(slug);
    window.ed = $scope;
    $scope.editor = angular.copy($rootScope.printerConfig[slug]) || {};
    var preview = new GCodePainter("shapePreview");
    preview.connectPrinter($scope.editor);
    preview.disableCursor();

    $scope.$on('$destroy', function () {
        preview.$destroy();
    });

    $scope.saveConfig = function () {
        console.log("saveConfig");
        console.log($scope.editor);
        WS.send("setPrinterConfig", $scope.editor);
    }
    $scope.resetConfig = function () {
        $scope.editor = $rootScope.printerConfig[slug];
    }
    var enrichEditor = function () {
        if (!$scope.editor) return;
        bs = $scope.editor.shape.basicShape;
        if (undefined == bs.radius) {
            bs.radius = 100;
            bs.x = 0;
            bs.y = 0;
        }
        if (undefined == bs.xMin) {
            bs.xMin = bs.yMin = 0;
            bs.xMax = bs.yMax = 200;
        }
    }
    $scope.$watch('printerConfig.' + slug, function (newVal) {
        $scope.editor = angular.copy(newVal);
        enrichEditor();
        $timeout(function () {
            $("[data-toggle=popover]").popover({});
            preview.connectPrinter($scope.editor);
        });
    });
    $('#printerTabs a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
        $timeout(preview.resizer,400);
    })

    $scope.bedTempUp = function (idx) {
        x = $scope.editor.heatedBed.temperatures[idx - 1];
        y = $scope.editor.heatedBed.temperatures[idx];
        $scope.editor.heatedBed.temperatures.splice(idx - 1, 2, y, x);
    }
    $scope.bedTempDown = function (idx) {
        x = $scope.editor.heatedBed.temperatures[idx];
        y = $scope.editor.heatedBed.temperatures[idx + 1];
        $scope.editor.heatedBed.temperatures.splice(idx, 2, y, x);
    }
    $scope.bedTempDel = function (idx) {
        $scope.editor.heatedBed.temperatures.splice(idx, 1);
    }
    $scope.bedTempAdd = function () {
        $scope.editor.heatedBed.temperatures.push({name: "", temp: "50"});
    }
    $scope.extTempUp = function (ex, idx) {
        x = ex.temperatures[idx - 1];
        y = ex.temperatures[idx];
        ex.temperatures.splice(idx - 1, 2, y, x);
    }
    $scope.extTempDown = function (ex, idx) {
        x = ex.temperatures[idx];
        y = ex.temperatures[idx + 1];
        ex.temperatures.splice(idx, 2, y, x);
    }
    $scope.extTempDel = function (ex, idx) {
        ex.temperatures.splice(idx, 1);
    }
    $scope.extTempAdd = function (ex) {
        ex.temperatures.push({name: "", temp: "50"});
    }
    $scope.addExtruder = function () {
        $scope.editor.extruders.push({eJerk: 40, maxSpeed: 30, acceleration:5000, extrudeSpeed: 2, extrudeDistance: 10,
            retractSpeed: 20, retractDistance: 10, heatupPerSecond:1.5,lastTemp:0,cooldownPerSecond:1, temperatures: []});
    }
    $scope.removeExtruder = function (idx) {
        if (confirm("Really delete this extruder?")) {
            $scope.editor.extruders.splice(idx, 1);
        }
    }
    $scope.$watch('editor.shape|json', function () {
        preview.updateShape();
    });
    $scope.$watch('editor.movement|json', function () {
        preview.updateShape();
    });

    //$("#printerTabs a:last").tab('show');
    $("#printerTabs a:first").tab('show');
    $timeout(function () {
        $("[data-toggle=popover]").popover({});
        enrichEditor();
    });
}
PrinterController = function ($scope, $routeParams, WS, $rootScope, $timeout,$filter,$http) {
    var slug = $routeParams.slug;
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
    $scope.movoToXPos = 20;
    $scope.movoToYPos = 50;
    $scope.moveToZPos = 0;
    $scope.hsliderSize = 300;
    $scope.fanPercent = 100;
    $scope.eeprom = [];
    $scope.webcammode = 0;
    var preview = new GCodePainter("control-view");
    preview.connectPrinter($rootScope.activeConfig);

    lastLogStart = 0;
    $scope.$on('$destroy', function () {
        preview.$destroy();
    });
    var getLoglevel = function () {
        return 12 + ($scope.logCommands > 0 ? 1 : 0) + ($scope.logACK > 0 ? 2 : 0);
    }
    var addLogLine = function (line) {
        newline = line.time + ': ' + line.text;
        $rootScope.active.log.push({c: 'log' + line.type, t: newline});
        $rootScope.active.log.splice(0, $rootScope.active.log.length - 500);
        lastLogStart = line.id;
    }
    $scope.$on("move", function (event, pos) {
        preview.addMove(pos.data);
        $scope.movoToXPos = pos.data.x;
        $scope.movoToYPos = pos.data.y;
        $scope.movoToZPos = pos.data.z;
    });
    $scope.$on("eepromClear",function(event) {
        $scope.eeprom = [];
    });
    $scope.$on("eepromData",function(event,data) {
        $scope.eeprom = $scope.eeprom.concat(data.data);
    });
    $scope.editEeprom = function() {
        WS.send("getEeprom",{});
        $('#dialogEeprom').modal('show');
    }
    $scope.saveEeprom = function() {
        WS.send("setEeprom",{eeprom:$scope.eeprom});
        $('#dialogEeprom').modal('hide');
    }
    $scope.xMoveTo = function(x) {
        WS.send("move",{x:x,relative:false});
    }
    $scope.yMoveTo = function(x) {
        WS.send("move",{y:x,relative:false});
    }
    $scope.zMoveTo = function(x) {
        WS.send("move",{z:x,relative:false});
    }
    $scope.activate = function() {
        WS.send("activate",{printer:slug});
    }
    $scope.deactivate = function() {
        WS.send("deactivate",{printer:slug});
    }
    $scope.showCommunication = function() {
        WS.send("communicationData",{}).then(function(data) {
            $scope.comData = data;
            $('#dialogConnectionData').modal('show');
        });
    }
    var responsePoller = function () {
        if(!WS.connected) return;
        filter = 12 + ($scope.logCommands > 0 ? 1 : 0) + ($scope.logACK > 0 ? 2 : 0);
        WS.send("response", {filter: filter, start: lastLogStart}).then(function (r) {
            /*lastLogStart = r['lastid'];
             angular.forEach(r.lines,function(line) {
             addLogLine(line);
             });
             */
            $rootScope.active.state = r.state;
            $scope.movoToXPos = r.state.x;
            $scope.movoToYPos = r.state.y;
            $scope.movoToZPos = r.state.z;
            preview.setCursor($scope.movoToXPos,$scope.movoToYPos);
            $scope.fanPercent = $rootScope.active.state.fanVoltage/2.55;
            $scope.isJobActive = $rootScope.active.status.job != 'none' ? true : false;
            $timeout(responsePoller, 3000);
        });
    };
    var fetchPrintqueue = function () {
        WS.send("listJobs", {}).then(function (r) {
            $scope.queue = r.data;
        });
    }
    var fetchModels = function () {
        oldid = $scope.activeGCode ? $scope.activeGCode.id : 0;
        WS.send("listModels", {}).then(function (r) {
            $scope.models = r.data;
            angular.forEach($scope.models,function(val) {
               if(val.id == oldid)
                $scope.activeGCode = val;
            });
        });
    }
    $scope.$watch('logACK', function () {
        WS.send("setLoglevel", {level: getLoglevel()});
    });
    $scope.$watch('logCommands', function () {
        WS.send("setLoglevel", {level: getLoglevel()});
    });
    $scope.$on("connected", function (event) {
        responsePoller();
        fetchPrintqueue();
        fetchModels();
        updateModes();
        WS.send("setLoglevel", {level: getLoglevel()});
    });
    $scope.$watch("activeConfig", function () {
        preview.connectPrinter($rootScope.activeConfig);
        resizeContols();
        updateModes();
    });
    $scope.$on("printqueueChanged", function (event) {
        fetchPrintqueue();
    });
    $scope.$on("jobsChanged", function (event) {
        fetchModels();
    });
    $scope.$on("log", function (event, data) {
        if(!$scope.logPause) {
        addLogLine(data.data);
        $timeout(function() {
            var elem = document.getElementById('logpanel');
            elem.scrollTop = elem.scrollHeight;
        });
        }
        //$scope.$apply()
    });
    $scope.test = function (v) {
        console.log("test");
        console.log(v);
    }
    $scope.selectQueue = function (gc,event) {
        elem = event.target;
        $scope.activeQueue = gc;
        console.log(elem);
        $(elem).tooltip({title:'<div style="width:200px;text-align:left"><h4>Job Informations</h4>Printing time: '+$filter('hms')(gc.printTime)+
            '<br>Extrusion: '+Math.ceil(gc.filamentTotal)+' mm</div>',html:true,placement:'right',trigger:'hover click'});
        //$(elem).tooltip('show');
    }
    $scope.selectGCode = function (gc) {
        $scope.activeGCode = gc;
    }
    $scope.dequeActive = function () {
        WS.send("removeJob", {id: $scope.activeQueue.id}).then(function (r) {
            //$scope.queue = r.data;
            $scope.activeQueue = false;
        });
    }
    $scope.printActiveQueue = function () {
        WS.send("startJob", {id: $scope.activeQueue.id});
    }
    $scope.deleteActiveGCode = function () {
        WS.send("removeModel", {id: $scope.activeGCode.id}).then(function (r) {
            //$scope.models = r.data;
            $scope.activeGCode = false;
            $('#deleteGCodeQuestion').foundation('reveal', 'close');
        });
    }
    $scope.uploadGCode = function () {
        $('#formuploadgcode').ajaxSubmit(function (r) {
            r = jQuery.parseJSON(r);
            $('#uploadGCode').modal('hide');
        });
    }
    $scope.printGCode = function () {
        WS.send("copyModel", {id: $scope.activeGCode.id});
    }
    $scope.setExtruderTemperature = function(extr,temp) {
        WS.send("send", {cmd: "M104 S"+temp+" T"+extr});
    }
    $scope.setBedTemperature = function(temp) {
        WS.send("send", {cmd: "M140 S"+temp});
    }
    $scope.sendCmd = function () {
        WS.send("send", {cmd: $scope.cmd});
        $scope.cmd = "";
    }
    $scope.sendGCode = function (code) {
        WS.send("send", {cmd: code});
        $scope.cmd = "";
    }
    $scope.queueFileSelected = function (q) {
        return q.id == $scope.activeQueue.id;
    }
    $scope.speedChange = function(diff) {
        $scope.active.state.speedMultiply += diff;
        WS.send("send",{cmd: "M220 S"+$scope.active.state.speedMultiply});
    }
    $scope.flowChange = function(diff) {
        $scope.active.state.flowMultiply += diff;
        WS.send("send",{cmd: "M221 S"+$scope.active.state.flowMultiply});
    }
    $scope.fanEnabledChanged = function(val) {
        if(!$scope.active.state.fanOn) {
            WS.send("send",{cmd:"M106 S"+Math.round($scope.fanPercent*2.55)});
        } else {
            WS.send("send",{cmd:"M107"});
        }
    }
    $scope.fanSpeedChanged = function(val) {
        console.log("fan speed changed "+val);
        $scope.fanPercent = val;
        WS.send("send",{cmd:"M106 S"+Math.round(val*2.55)});
    }
    $scope.stopPrint = function() {
        $scope.confirm("Confirmation required","Really stop current print?","Yes","No").then(function() {
            if($scope.active.status.jobid)
                WS.send("stopJob",{id:$scope.active.status.jobid});
        });
    }
    $scope.pausePrint = function() {
        WS.send("send",{cmd:'@pause User requested pause.'});
    }
    fit_modal_body = function(modal) {
        var body, bodypaddings, header, headerheight, height, modalheight,footer,footerheight;
        header = $(".modal-header", modal);
        footer = $(".modal-footer", modal);
        body = $(".modal-body", modal);
        modalheight = $(window).height(); //parseInt(modal.css("height"));
        headerheight = header.outerHeight();
        footerheight = footer.outerHeight(); //parseInt(footer.css("height")) + parseInt(footer.css("padding-top")) + parseInt(footer.css("padding-bottom"));
        bodypaddings = parseInt(body.css("padding-top")) + parseInt(body.css("padding-bottom"));
        height = modalheight - headerheight - bodypaddings - 5;
        $(body).height(height-140);
        //return body.css("max-height", "" + height + "px");
    };
    $scope.previewData = 0;
    $scope.previewGCode = function(id) {
        fit_modal_body($('#dialogPreview'));
        $('#dialogPreview').modal('show');
        $scope.previewData = id;
    }
    var resizeContols = function () {
        w = $('#control-row').width();
        p = $rootScope.activeConfig;
        if (!p) return;
        //console.log("resizeControls");
        dimx = p.movement.xMax - p.movement.xMin;
        dimy = p.movement.yMax - p.movement.yMin;
        aspectP = dimx / dimy;
        pw = ph = 372;
        if (aspectP > 1) ph = pw / aspectP; else pw = ph * aspectP;
        //console.log("w=" + w + " dimx " + dimx + " dimy " + dimy + " pw " + pw + " ph " + ph);
        $('#control-view').width(pw).height(ph);
        $scope.hsliderSize = pw;
        $('#control-vscoll-container').height(ph);
        $('#control-vscoll-container2').height(ph);

        preview.updateShape();
    }
    $scope.$on('windowResized', function () {
        resizeContols();
    });
    // $scope.webcammode = 0;
    var webcamcount = 1;
    var webcamRefreshRunning = false;
    $scope.webcamUrl = '';
    $scope.$watch('webcammode',function() {
        if(typeof ($rootScope.activeConfig)=='undefined') return undefined;
        //console.log("webcamUrl "+webcamcount);
        //console.log($rootScope.activeConfig);
        if($scope.webcammode == 1 && !webcamRefreshRunning) {
            refreshWebcamTimer();
            webcamRefreshRunning = true;
        }
        switch($scope.webcammode) {
            case 1: $scope.webcamUrl = $rootScope.activeConfig.webcam.staticUrl+"&rand="+webcamcount;break;
            case 2: $scope.webcamUrl = $rootScope.activeConfig.webcam.dynamicUrl;break;
            default: $scope.webcamUrl = "";
        }
    });
    var updateModes = function() {
        if(!$rootScope.activeConfig) return;
        $scope.webcammodes = [{id:0,name:'Disabled'}];
        if($rootScope.activeConfig.webcam.method & 1)
            $scope.webcammodes.push({id:1,name:'Poll Images'});
        if($rootScope.activeConfig.webcam.method & 2)
            $scope.webcammodes.push({id:2,name:'Motion JPG'});
    }
    $scope.refreshWebcamImage = function() {
        if($scope.webcammode == 1) {
            webcamcount++;
            $scope.webcamUrl = $rootScope.activeConfig.webcam.staticUrl+"&rand="+webcamcount;
        }
    }
    var refreshWebcamTimer = function() {
        $scope.refreshWebcamImage();
        if($scope.webcammode != 1) {
            webcamRefreshRunning = false;
            return;
        }
        $timeout(refreshWebcamTimer,1000*$rootScope.activeConfig.webcam.reloadInterval);
    }
    responsePoller();
    fetchModels();
    $('#printerTabs a').click(function (e) {
        e.preventDefault();
        $(this).tab('show');
    })
    //$("#printerTabs").tab();
    $("#printerTabs a:first").tab('show');
    //(function() {console.log("show");$("#printerTabs a:first").tab('show');},200);
}