<div class="row" style="margin-top:10px">
    <div class="small-4 large-3 columns">
        <div class="row">
            <div class="small-12 columns">
                <h4>{{active.status.name}}</h4>

                <div class="panel small">
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
                <a href="#" class="button expand dense">Scripts</a>
                <a href="#" class="button expand dense">Configuration</a>
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
                    <p>Content of section 2.</p>
                </div>
            </section>
        </div>
    </div>
</div>