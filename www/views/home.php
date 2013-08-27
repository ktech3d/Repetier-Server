<div class="container">
    <div class="row" style="margin-top:10px">
        <div ng-repeat="p in printerList" class="col-12 col-lg-4">
            <div class="panel panel-primary">
                <div class="panel-heading">{{p.name}}
                    <div class="btn-group" style="float: right;top: -9px;right: -13px;">
                        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                            <i class="icon-cog"></i> <span class="caret"></span>
                        </button>
                        <ul class="dropdown-menu" style="right:0;left:auto">
                            <li><a href="#/scriptConfig/{{p.slug}}">Scripts</a></li>
                            <li><a href="#/printerConfig/{{p.slug}}">Configuration</a></li>
                        </ul>
                    </div>
                </div>

                <div class="small-font">
                    <div class="row small-margin">
                        <div class="col-5">Status:</div>
                        <div class="col-7" ng-bind-html-unsafe="p | online"></div>
                    </div>
                    <div ng-show="p.online">
                        <div class="row small-margin">
                            <div class="col-5">Printing:</div>
                            <div class="col-7" ng-bind-html-unsafe="p | printing"></div>
                        </div>
                        <div class="row margin-top  ">
                            <div class="col-12">
                                <a class="btn btn-block btn-success btn-small" href="/#/printer/{{p.slug}}">Details</a>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>