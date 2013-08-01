<div class="row" style="margin-top:10px">
    <div class="small-4 large-3 columns">
        <div class="row">
            <div class="small-12 columns">
                <div style="float:right" data-dropdown="setupMenu"><i class="icon-cog" style="font-size:1.5em"></i></div>
                <ul id="setupMenu" class="f-dropdown">
                    <li><a href="#">Scripts</a></li>
                    <li><a href="#/printerConfig/{{activeSlug}}">Configuration</a></li>
                </ul>
                <h4>{{active.status.name}}</h4>

                <div class="panel small-font">
                    <div class="row small-margin">
                        <div class="small-5 columns">Status:</div>
                        <div class="small-7 columns" ng-bind-html-unsafe="active.status | online"></div>
                    </div>
                    <div class="row small-margin" ng-repeat="e in active.state.extruder">
                        <div class="small-5 columns">Extr. {{$index+1}}:</div>
                        <div class="small-7 columns">{{e.tempRead | temp}} / {{e.tempSet | temp}}</div>
                    </div>
                    <div class="row small-margin">
                        <div class="small-5 columns">Bed:</div>
                        <div class="small-7 columns">{{active.state.bedTempRead | temp}} / {{active.state.bedTempSet | temp}}</div>
                    </div>
                    <div class="row small-margin">
                        <div class="small-5 columns">Speed:</div>
                        <div class="small-7 columns">{{active.state.speedMultiply}}%</div>
                    </div>
                    <div class="row small-margin">
                        <div class="small-5 columns">Flow:</div>
                        <div class="small-7 columns">{{active.state.flowMultiply}}%</div>
                    </div>
                </div>
                <div class="panel small-font" ng-show="isJobActive()">
                    <h3>Current print</h3>
                    <div class="row small-margin">
                        <div class="small-5 columns">Printing:</div>
                        <div class="small-7 columns">{{active.status.job}}</div>
                    </div>
                    <div class="row small-margin">
                        <div class="small-12 columns">
                            <div class="progress small-12 success round"><div class="metertext">{{active.status.done | number:2}}%</div><span class="meter" style="width: {{active.status.done}}%"></span></div>
                        </div>
                    </div>
                </div>
                <div class="panel small-font" ng-show="queue.length>1 || (queue.length==1 && queue[0].id!=active.status.jobid)">
                    <h3>Print queue</h3>
                    <div class="row queue" ng-repeat="q in queue" ng-click="selectQueue(q);" ng-hide="q.id==active.status.jobid">
                        <div class="small-2 columns">
                            <i ng-show="q.id==activeQueue.id" class="icon-circle"></i>
                        </div>
                        <div class="small-10 columns queue">
                            {{q.name}}
                        </div>
                    </div>
                    <div class="row" ng-show="activeQueue">
                        <div class="small-6 columns button" ng-click="dequeActive()"><i class="icon-minus"></i> Deque</div>
                        <div class="small-6 columns button" ng-hide="isJobActive()" ng-click="printActiveQueue()"><i class="icon-print"></i> Print</div>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <div class="small-8 large-9 columns">
        <div class="section-container auto" data-section>
            <section>
                <p class="title" data-section-title><a href="#panel1">Overview</a></p>

                <div class="content" data-section-content>
                    <p>Content of section 1.</p>
                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#panel2">Control</a></p>

                <div class="content" data-section-content>
                    <p>Content of section 2.</p>
                    <div class="panel small-font" ng-show="queue.length>0">
                        <h3>Print queue</h3>
                        <div class="row queue" ng-repeat="q in queue" ng-click="selectQueue(q)">
                            <div class="small-2 columns">
                                <i ng-show="q==activeQueue" class="icon-circle"></i>
                            </div>
                            <div class="small-10 columns queue">
                                {{q.name}}
                            </div>
                        </div>
                        <div class="row" ng-show="activeQueue">
                            <div class="small-6 columns button" cng-click="dequeActive()"><i class="icon-minus"></i> Deque</div>
                            <div class="small-6 columns button" ng-hide="isJobActive()" sng-click="printActiveQueue()"><i class="icon-print"></i> Print</div>
                        </div>
                    </div>

                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#panelConsole">Console</a></p>

                <div class="content" data-section-content>
                    <div class="row">
                        <div class="small-2 columns">
                            Commands
                        </div>
                        <div class="small-2 columns">
                            <div switch class="small" value="logCommands"></div>
                        </div>
                        <div class="small-2 columns">
                            ACK
                        </div>
                        <div class="small-2 columns">
                            <div switch class="small" value="logACK"></div>
                        </div>
                        <div class="small-2 columns">
                            Pause
                        </div>
                        <div class="small-2 columns">
                            <div switch class="small" value="logPause"></div>
                        </div>
                        <div class=" small-12 columns">
                            <div class="panel" style="height:300px;overflow:scroll" autoscroll2="logAutoscroll">
                                <div ng-repeat="l in active.log" ng-bind-html-unsafe="l.t" class="{{l.c}}"></div>
                            </div>
                        </div>
                        <div class=" small-12 columns">
                            <div class="row collapse">
                                <div class="small-10 columns">
                                    <input type="text" ng-model="cmd" placeholder="Enter your g-code here" enter="sendCmd()">
                                </div>
                                <div class="small-2 columns">
                                    <div  ng-click="sendCmd()" class="button prefix">Send</div>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#panel4">Camera</a></p>

                <div class="content" data-section-content>
                    <p>Content of section 2.</p>
                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#panel3">G-Codes</a></p>

                <div class="content" data-section-content>
                    <div class="row">
                        <div class="small-6 columns">
                            <div class="filelist">
                                <div class="file" ng-repeat="f in models" ng-class="{active:f==activeGCode}" ng-click="selectGCode(f)">{{f.name}}</div>
                            </div>
                            <a href="#" data-reveal-id="uploadGCode" class="button success"><i class="icon-plus-squared"></i> Upload G-Code</a>
                        </div>
                        <div class="small-6 columns" ng-show="activeGCode">
                            <div class="infodesc">Filename:<span>{{activeGCode.name}}</span></div>
                            <div class="infodesc">Printed:<span>{{activeGCode.printed}} times</span></div>
                            <div class="infodesc">Printing time:<span>{{activeGCode.printTime | hms}}</span></div>
                            <div class="infodesc">Filesize / Lines<span>{{activeGCode.length | byte}} / {{activeGCode.lines}}</span></div>
                            <div class="infodesc">Layer:<span>{{activeGCode.layer}}</span></div>
                            <div class="infodesc">Filament usage:<span>{{activeGCode.filamentTotal | number:0}} mm</span></div>
                            <div class="small-6 columns button" ng-click="printGCode()"><i class="icon-print"></i> Print</div>
                            <div class="small-6 columns button alert" data-reveal-id="deleteGCodeQuestion"><i class="icon-trash"></i> Delete</div>
                        </div>
                    </div>
                </div>
            </section>
        </div>
    </div>
</div>
<!-- Reveals -->
<div id="deleteGCodeQuestion" class="reveal-modal">
    <h2>Security question</h2>
    <p class="lead">Do you really want to delete {{activeGCode.name}}</p>
    <div class="row">
        <div class="small-4 columns button" ng-click="closeReveal('deleteGCodeQuestion')">No</div>
        <div class="small-4 small-offset-4 columns button alert" ng-click="deleteActiveGCode()"><i class="icon-trash"></i>Yes</div>
    </div>
    <a class="close-reveal-modal">&#215;</a>
</div>
<div id="uploadGCode" class="reveal-modal">
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