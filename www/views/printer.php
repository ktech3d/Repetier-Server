<div class="container">
<div class="row" style="margin-top:10px">
<div class="col-lg-3">
    <div class="row">
        <div class="col-12">
            <div class="panel panel-primary">
                <div class="panel-heading">{{active.status.name}}
                    <div class="btn-group" style="float: right;top: -9px;right: -13px;">
                        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                            <i class="icon-cog"></i> <span class="caret"></span>
                        </button>
                        <ul class="dropdown-menu" style="right:0;left:auto">
                            <li ng-hide="active.status.active"><a ng-click="activate()" href="javascript:void(0)">Activate</a></li>
                            <li ng-show="active.status.active"><a ng-click="deactivate()" href="javascript:void(0)">Deactivate</a></li>
                            <li ng-show="active.status.active"><a ng-click="showCommunication()" href="javascript:void(0)">Connection Informations</a></li>
                            <li ng-show="active.status.active"><a ng-click="editEeprom()" href="javascript:void(0)">EEPROM Settings</a></li>
                            <li><a href="#/scriptConfig/{{activeSlug}}">Scripts</a></li>
                            <li><a href="#/printerConfig/{{activeSlug}}">Configuration</a></li>
                        </ul>
                    </div>
                </div>

                <div class="small-font">
                    <div class="row small-margin">
                        <div class="col-5">Status:</div>
                        <div class="col-7" ng-bind-html-unsafe="active.status | online"></div>
                    </div>
                    <div ng-show="active.status.online">
                        <div class="row small-margin" ng-repeat="e in active.state.extruder">
                            <div class="col-5">Extr. {{$index+1}}:</div>
                            <div class="col-7">{{e.tempRead | temp}} / {{e.tempSet | temp}}</div>
                        </div>
                        <div class="row small-margin">
                            <div class="col-5">Bed:</div>
                            <div class="col-7">{{active.state.bedTempRead | temp}} / {{active.state.bedTempSet
                                | temp}}
                            </div>
                        </div>
                        <div class="row small-margin">
                            <div class="col-5">Speed:</div>
                            <div class="col-7">{{active.state.speedMultiply}}%</div>
                        </div>
                        <div class="row small-margin">
                            <div class="col-5">Flow:</div>
                            <div class="col-7">{{active.state.flowMultiply}}%</div>
                        </div>
                    </div>
                </div>
            </div>
            <div class="panel" ng-show="isJobActive">
                <div class="panel-heading">Current print</div>

                <div class="row">
                    <div class="col-5">Printing:</div>
                    <div class="col-7">{{active.status.job}}</div>
                </div>
                <div class="row">
                    <div class="col-12">
                        <div class="progress progress-striped">
                            <div class="metertext">{{active.status.done | number:2}}%</div>
                            <div class="progress-bar progress-bar-success" role="progressbar"
                                 style="width: {{active.status.done}}%"></div>
                        </div>
                    </div>
                    <div class="col-6">
                        <button ng-click="stopPrint()" class="btn btn-danger btn-block"><i class="icon-stop"></i> Stop
                        </button>
                    </div>
                    <div class="col-6">
                        <button ng-click="pausePrint()" class="btn btn-primary btn-block"><i class="icon-pause"></i>
                            Pause
                        </button>
                    </div>
                </div>
            </div>
            <div class="panel small-font"
                 ng-show="active.status.online && (queue.length>1 || (queue.length==1 && queue[0].id!=active.status.jobid))">
                <div class="panel-heading">Print queue</div>

                <div class="row queue" ng-repeat="q in queue" ng-click="selectQueue(q);"
                     ng-hide="q.id==active.status.jobid" ng-class="{queueactive:queueFileSelected(q)}">
                    <div class="col-12">
                        {{q.name}}
                    </div>
                </div>
                <div class="row" ng-show="activeQueue" style="margin-top:10px">
                    <div class="col-6">
                        <button class="btn btn-block" ng-click="dequeActive()"><i class="icon-minus"></i>
                            Deque
                        </button>
                    </div>
                    <div class="col-6">
                        <button class="btn btn-block" ng-hide="isJobActive" ng-click="printActiveQueue()"><i
                                class="icon-print"></i> Print
                        </button>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>
<div class="col-lg-9">
<ul class="nav nav-tabs" id="printerTabs">
    <li ng-show="active.status.online"><a href="#panel1" data-toggle="tab">Control</a></li>
    <li ng-show="active.status.online"><a href="#panelConsole" data-toggle="tab">Console</a></li>
    <li><a href="#panel4" data-toggle="tab">Camera</a></li>
    <li><a href="#panel3" data-toggle="tab">G-Codes</a></li>
</ul>
<div class="tab-content">
<div class="tab-pane active fade" id="panel1">
    <div class="row" ng-show="active.status.online">
        <div class="col-12 col-sm-8 hidden-sm" id="control-row">
            <div id="control-hscroll-container" style="margin-left:40px;padding:10px 11px">
                <div id="xpos" slider orientation="horizontal" min="{{activeConfig.movement.xMin}}"
                     max="{{activeConfig.movement.xMax}}" step="0.1" precision="1" size="{{hsliderSize}}"
                     ng-model="movoToXPos" enabled="{{!isJobActive}}" moved="xMoveTo(value)" letter="X"></div>
            </div>
            <div style="float:left;width:40px;height:300px;padding:11px 10px">
                <div id="control-vscoll-container" slider orientation="vertical"
                     min="{{activeConfig.movement.yMin}}" max="{{activeConfig.movement.yMax}}" step="0.1"
                     precision="1" ng-model="movoToYPos" style="height:300px" enabled="{{!isJobActive}}"
                     moved="yMoveTo(value)" flip="true" letter="Y"></div>
                <br/>
            </div>
            <div id="control-view" class="well"
                 style="float:left;width:300px;height:300px;padding:10px"></div>
            <div style="float:left;width:40px;height:300px;padding:11px 10px">
                <div id="control-vscoll-container2" slider orientation="vertical"
                     min="{{activeConfig.movement.zMin}}" max="{{activeConfig.movement.zMax}}" step="0.1"
                     precision="1" ng-model="movoToZPos" style="height:300px" enabled="{{!isJobActive}}"
                     moved="zMoveTo(value)" flip="true" letter="Z"></div>
                <br/>
            </div>
        </div>
        <div class="col-12 col-sm-4">
            <div class="row margin-top">
                <div class="col-8">
                    X: {{movoToXPos | number:2}} mm
                </div>
                <div class="col-4">
                    <button class="btn btn-block" ng-click="sendGCode('G28 X0')" ng-hide="isJobActive"><i
                            class="icon-home"></i> X
                    </button>
                </div>
            </div>
            <div class="row margin-top">
                <div class="col-8">
                    Y: {{movoToYPos | number:2}} mm
                </div>
                <div class="col-4">
                    <button class="btn btn-block" ng-click="sendGCode('G28 Y0')" ng-hide="isJobActive"><i
                            class="icon-home"></i> Y
                    </button>
                </div>
            </div>
            <div class="row margin-top">
                <div class="col-8">
                    Z: {{movoToZPos | number:2}} mm
                </div>
                <div class="col-4">
                    <button class="btn btn-block" ng-click="sendGCode('G28 Z0')" ng-hide="isJobActive"><i
                            class="icon-home"></i> Z
                    </button>
                </div>
            </div>
            <div class="row margin-top">
                <div class="col-8">
                </div>
                <div class="col-4">
                    <button class="btn btn-block" ng-click="sendGCode('G28')" ng-hide="isJobActive"><i
                            class="icon-home"></i> All
                    </button>
                </div>
            </div>
            <div class="row margin-top">
                <div class="col-12">
                    Speed multiplier:
                    <button class="btn btn-small" ng-click="speedChange(-1)"><i class="icon-minus-sign"></i>
                    </button>
                    {{active.state.speedMultiply}}%
                    <button class="btn btn-small" ng-click="speedChange(1)"><i class="icon-plus-sign"></i>
                </div>
                <div id="control-hscroll-container" style="padding:10px 11px">
                    <div slider orientation="horizontal" min="25"
                         max="300" step="1" precision="0" size="220"
                         ng-model="active.state.speedMultiply"
                         moved="sendGCode('M220 S'+value)" letter="S"></div>
                </div>
            </div>
            <div class="row margin-top">
                <div class="col-12">
                    Flow multiplier:
                    <button class="btn btn-small" ng-click="flowChange(-1)"><i class="icon-minus-sign"></i>
                    </button>
                    {{active.state.flowMultiply}}%
                    <button class="btn btn-small" ng-click="flowChange(1)"><i class="icon-plus-sign"></i>
                </div>
                <div id="control-hscroll-container" style="padding:10px 11px">
                    <div slider orientation="horizontal" min="50"
                         max="150" step="1" precision="0" size="220"
                         ng-model="active.state.flowMultiply"
                         moved="sendGCode('M221 S'+value)" letter="F"></div>
                </div>
            </div>
            <div class="row margin-top" ng-show="activeConfig.general.fan">
                <div class="col-8 vcenter-btnline">
                    Fan: {{fanPercent | number:1}}%
                </div>
                <div class="col-4">
                    <boolswitch class="small" on="On" off="Off" value="active.state.fanOn"
                                changed="fanEnabledChanged()"></boolswitch>
                </div>
                <div id="control-hscroll-container" style="padding:10px 11px">
                    <div slider orientation="horizontal" min="0"
                         max="100" step="0.5" precision="1" size="220"
                         ng-model="fanPercent"
                         moved="fanSpeedChanged(value)" letter="F"></div>
                </div>
            </div>
        </div>
    </div>
</div>
<div class="tab-pane fade" id="panelConsole">
    <div class="row margin-top" ng-show="active.status.online">
        <div class="col-2 vcenter-btnline">
            Commands
        </div>
        <div class="col-2">
            <switch class="small" value="logCommands"></switch>
        </div>
        <div class="col-2 vcenter-btnline">
            ACK
        </div>
        <div class="col-2">
            <switch class="small" value="logACK"></switch>
        </div>
        <div class="col-2 vcenter-btnline">
            Pause
        </div>
        <div class="col-2">
            <switch class="small" value="logPause"></switch>
        </div>
        <div class="col-12 margin-top">
            <div class="row">
                <div class="col-12">
                    <div class=" input-group">
                        <input type="text" class="form-control" ng-model="cmd"
                               placeholder="Enter your g-code here"
                               enter="sendCmd()">
                                    <span class="input-group-btn">
                                        <div class="btn-group">
                                            <button type="button" ng-click="sendCmd()" class="btn btn-primary">Send
                                            </button>
                                            <button type="button" class="btn btn-primary dropdown-toggle"
                                                    data-toggle="dropdown">
                                                <span class="caret"></span>
                                            </button>
                                            <ul class="dropdown-menu" role="menu">
                                                <li><a href="javascript:void(0)" ng-click="sendGCode('G28')">Home
                                                        All</a></li>
                                                <li><a href="javascript:void(0)" ng-click="sendGCode('M115')">Show
                                                        Capabilities</a></li>
                                                <li><a href="javascript:void(0)" ng-click="sendGCode('M119')">Show
                                                        Endstop Status</a></li>
                                                <li><a href="javascript:void(0)" ng-click="sendGCode('M114')">Show
                                                        Coordinates</a></li>
                                            </ul>
                                        </div>
                                    </span>
                    </div>
                </div>
            </div>
        </div>
        <div class="col-12 margin-top">
            <div id="logpanel" class="panel" style="height:300px;overflow:scroll" autoscroll2="logAutoscroll">
                <div ng-repeat="l in active.log" ng-bind-html-unsafe="l.t" class="{{l.c}}"></div>
            </div>
        </div>
        <div class="col-12">
            <p>
                <small>Commands shows commands send to the printer. ACK shows normally suppressed acknowledgement
                    data send back from the printer. You should only enable them for debug purposes or they might
                    slow down your interface. If pause is enabled, new log messages get suppressed, so you can check the
                    log without a hurry.<br/>
                    Use the command interface to send any arbitrary commands to your printer. Remember to use uppercase
                    letter for the parameter. Some usefull commands are available in the drop down list.
                </small>
            </p>
        </div>
    </div>

</div>
<div class="tab-pane fade" id="panel4">
    <video width="320" height="240" autoplay="autoplay" controls>
        <source src="http://localhost:1234" type="video/ogg"/>
        No support for selected video format.
    </video>
</div>
<div class="tab-pane fade" id="panel3">
    <div class="row" style="margin-top:10px">
        <div class="col-lg-6 col-sm-6" style="margin-bottom:10px">
            <div class="filelist">
                <div class="file" ng-repeat="f in models" ng-class="{active:f==activeGCode}"
                     ng-click="selectGCode(f)">{{f.name}}
                </div>
            </div>
            <button data-toggle="modal" data-target="#uploadGCode" class="btn btn-primary"><i
                    class="icon-plus-squared"></i> Upload G-Code
            </button>
        </div>
        <div class="col-lg-6 col-sm-6 well" ng-show="activeGCode">
            <h4 style="margin-top:0">File Informations</h4>

            <div class="infodesc">Filename:<span>{{activeGCode.name}}</span></div>
            <div class="infodesc">Printed:<span>{{activeGCode.printed}} times</span></div>
            <div class="infodesc">Printing time:<span>{{activeGCode.printTime | hms}}</span></div>
            <div class="infodesc">Filesize /
                Lines<span>{{activeGCode.length | byte}} / {{activeGCode.lines}}</span>
            </div>
            <div class="infodesc">Lines:<span>{{activeGCode.lines}}</span></div>
            <div class="infodesc">Layer:<span>{{activeGCode.layer}}</span></div>
            <div class="infodesc">Filament
                usage:<span>{{activeGCode.filamentTotal | number:0}} mm</span></div>
            <div class="btn-group">
                <div class="btn btn-primary" ng-click="printGCode()"><i class="icon-print"></i>
                    Print
                </div>
                <div class="btn btn-danger" data-reveal-id="deleteGCodeQuestion"><i
                        class="icon-trash"></i> Delete
                </div>
            </div>
        </div>
    </div>
</div>
</div>
</div>
</div>
</div>

<!-- Reveals -->
<div id="deleteGCodeQuestion" class="modal fade">
    <h2>Security question</h2>

    <p class="lead">Do you really want to delete {{activeGCode.name}}</p>

    <div class="row">
        <div class="small-4 columns button" ng-click="closeReveal('deleteGCodeQuestion')">No</div>
        <div class="small-4 small-offset-4 columns button alert" ng-click="deleteActiveGCode()"><i
                class="icon-trash"></i>Yes
        </div>
    </div>
    <a class="close-reveal-modal">&#215;</a>
</div>
<div id="uploadGCode" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title">Upload G-Code</h4>
            </div>
            <div class="modal-body">
                <form enctype="multipart/form-data" action="/printer/model/{{activeSlug}}" id="formuploadgcode">
                    <input type="hidden" name="a" value="upload"/>
                    <label><?php _("Job Name") ?></label>
                    <input type="text" name="name" placeholder="Optional name"/>
                    <label><?php _("G-Code File") ?></label>
                    <input type="file" name="file"/>
                </form>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-primary" ng-click="uploadGCode()"><i class="icon-upload-cloud"></i>Upload
            </div>
        </div>
    </div>
</div>
<div id="dialogConnectionData" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title">Connection Informations</h4>
            </div>
            <div class="modal-body">
                <table class="table">
                    <tr>
                        <td>Data send:</td>
                        <td>{{comData.bytesSend| byte}}</td>
                    </tr>
                    <tr>
                        <td>Data received:</td>
                        <td>{{comData.bytesReceived| byte}}</td>
                    </tr>
                    <tr>
                        <td>Lines send:</td>
                        <td>{{comData.linesSend}}</td>
                    </tr>
                    <tr>
                        <td>Errors:</td>
                        <td>{{comData.resendErrors}}</td>
                    </tr>
                </table>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-primary" data-dismiss="modal">Close</button>
            </div>
        </div>
    </div>
</div>
<div id="dialogEeprom" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title">EEPROM Settings</h4>
            </div>
            <div class="modal-body" style="overflow:auto;height:300px">
                <table class="table">
                    <tr>
                        <th>Parameter</th>
                        <th>Value</th>
                    </tr>
                    <tr ng-repeat="e in eeprom">
                        <td>{{e.text}}</td>
                        <td><input type="text" ng-model="e.value"></td>
                    </tr>
                </table>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-primary" ng-click="saveEeprom()"><i class="icon-save"></i> Save</button>
                <button type="button" class="btn" data-dismiss="modal">Close</button>
            </div>
        </div>
    </div>
</div>
