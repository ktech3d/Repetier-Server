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
                            <li><a href="#">Scripts</a></li>
                            <li><a href="#/printerConfig/{{activeSlug}}">Configuration</a></li>
                        </ul>
                    </div>
                </div>

                <div class="small-font">
                    <div class="row small-margin">
                        <div class="col-5">Status:</div>
                        <div class="col-7" ng-bind-html-unsafe="active.status | online"></div>
                    </div>
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
            <div class="panel" ng-show="isJobActive()">
                <div class="panel-heading">Current print</div>

                <div class="row small-margin">
                    <div class="col-5">Printing:</div>
                    <div class="col-7">{{active.status.job}}</div>
                </div>
                <div class="row small-margin">
                    <div class="col-12">
                        <div class="progress col-12 success round">
                            <div class="metertext">{{active.status.done | number:2}}%</div>
                            <span class="meter" style="width: {{active.status.done}}%"></span></div>
                    </div>
                </div>
            </div>
            <div class="panel small-font"
                 ng-show="queue.length>1 || (queue.length==1 && queue[0].id!=active.status.jobid)">
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
                        <button class="btn btn-block" ng-hide="isJobActive()" ng-click="printActiveQueue()"><i
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
        <li class="active"><a href="#panel1">Overview</a></li>
        <li><a href="#panel2">Control</a></li>
        <li><a href="#panelConsole">Console</a></li>
        <li><a href="#panel4">Camera</a></li>
        <li><a href="#panel3">G-Codes</a></li>
    </ul>
    <div class="tab-content">
        <div class="tab-pane active fade" id="panel1">
            <p>Content of section 1.</p>
        </div>
        <div class="tab-pane fade" id="panel2">
            <p>Content of section 2.</p>

            <div class="panel small-font" ng-show="queue.length>0">
                <h3>Print queue</h3>

                <div class="row queue" ng-repeat="q in queue" ng-click="selectQueue(q)">
                    <div class="col-2 columns">
                        <i ng-show="q==activeQueue" class="icon-circle"></i>
                    </div>
                    <div class="col-10 columns queue">
                        {{q.name}}
                    </div>
                </div>
                <div class="row" ng-show="activeQueue">
                    <div class="col-6 btn" ng-click="dequeActive()"><i class="icon-minus"></i>
                        Deque
                    </div>
                    <div class="col-6 btn" ng-hide="isJobActive()"
                         sng-click="printActiveQueue()"><i class="icon-print"></i> Print
                    </div>
                </div>
            </div>
        </div>
        <div class="tab-pane fade" id="panelConsole">
            <div class="row">
                <div class="col-2">
                    Commands
                </div>
                <div class="col-2">
                    <switch class="small" value="logCommands"></switch>
                </div>
                <div class="col-2">
                    ACK
                </div>
                <div class="col-2">
                    <switch class="small" value="logACK"></switch>
                </div>
                <div class="col-2">
                    Pause
                </div>
                <div class="col-2">
                    <switch class="small" value="logPause"></switch>
                </div>
                <div class="col-12">
                    <div class="panel" style="height:300px;overflow:scroll" autoscroll2="logAutoscroll">
                        <div ng-repeat="l in active.log" ng-bind-html-unsafe="l.t" class="{{l.c}}"></div>
                    </div>
                </div>
                <div class="col-12">
                    <div class="row">
                        <div class="col-12">
                            <div class=" input-group">
                                <input type="text" class="form-control" ng-model="cmd"
                                       placeholder="Enter your g-code here"
                                       enter="sendCmd()">
                                    <span class="input-group-btn">
                                    <button ng-click="sendCmd()" class="btn">Send</button>
                                    </span>
                            </div>
                        </div>
                    </div>
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
                    <button data-reveal-id="uploadGCode" class="btn btn-primary"><i
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
    <h2>Upload G-Code</h2>

    <form enctype="multipart/form-data" action="/printer/model/{{activeSlug}}" id="formuploadgcode">
        <input type="hidden" name="a" value="upload"/>
        <label><?php _("Job Name") ?></label>
        <input type="text" name="name" placeholder="Optional name"/>
        <label><?php _("G-Code File") ?></label>
        <input type="file" name="file"/>
    </form>
    <div class="button alert" ng-click="uploadGCode()"><i class="icon-upload-cloud"></i>Upload</div>
    <a class="close-reveal-modal">&#215;</a>
</div>