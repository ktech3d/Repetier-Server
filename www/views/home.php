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
                            <li><a href="/printer/pconfig/{{p.slug}}?a=download">Download Configuration</a></li>
                            <li><a href="javascript:void(0)" ng-click="replacePrinter(p)">Replace Configuration</a></li>
                            <li><a href="javascript:void(0)" ng-click="removePrinter(p)">Remove Printer</a></li>
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
    <div class="row">
        <div class="col-12">
            <button class="btn btn-primary" ng-click="addPrinter()"><i class="icon-plus"></i> Add new printer</button>
            <button class="btn btn-primary" ng-click="addPrinterConfig()"><i class="icon-plus"></i> Upload printer configuration</button>
        </div>
    </div>
</div>

<div class="modal fade" id="dialogaddprinter">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title">Add new printer</h4>
            </div>
            <div class="modal-body">
                <div class="row">
                    <div class="col-12">
                        <form class="form-horizontal">
                            <div class="form-group">
                                <label>Printer name</label>
                                <input type="text" class="form-control" ng-model="newprinter.name"
                                       placeholder="enter printer name" required>
                                <inputerror error="newprinter.errors.name"></inputerror>
                            </div>
                            <div class="form-group">
                                <label>Printer slug</label>
                                <input type="text" class="form-control" ng-model="newprinter.slug"
                                       placeholder="enter slug name" required>
                                <inputerror error="newprinter.errors.slug"></inputerror>
                                <p>
                                    <small>The printer slug name is used inside the server to reference the printer and
                                        the
                                        configuration. Data gets stored in directories named like the slug. Use only
                                        ascii letters a-z
                                        and digits for this. The slug name can not be changed later on.
                                    </small>
                                </p>
                            </div>
                        </form>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary" ng-click="showCreatePrinter()">Create printer</button>
            </div>
        </div>
    </div>
</div>

<div class="modal fade" id="dialoguploadprinter">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title" ng-show="confupload.mode">Add new printer from configuration file</h4>
                <h4 class="modal-title" ng-hide="confupload.mode">Replace printer configuration file</h4>
            </div>
            <div class="modal-body">
                <div class="row">
                    <div class="col-12">
                        <form class="form-horizontal" id="formuploadconfig" action="/printer/pconfig" enctype="multipart/form-data">
                            <div class="alert alert-danger" ng-show="confupload.errors.createerror">{{confupload.errors.createerror}}</div>
                            <input type="hidden" name="a" value="upload">
                            <input type="hidden" name="mode" value="{{confupload.mode}}">
                            <div class="form-group" ng-show="confupload.mode">
                                <label>Printer name</label>
                                <input type="text" name="name" class="form-control" ng-model="confupload.name"
                                       placeholder="enter printer name" required>
                                <inputerror error="confupload.errors.name"></inputerror>
                            </div>
                            <div class="form-group" ng-show="confupload.mode">
                                <label>Printer slug</label>
                                <input type="text" name="slug" class="form-control" ng-model="confupload.slug"
                                       placeholder="enter slug name" required>
                                <inputerror error="confupload.errors.slug"></inputerror>
                                <p>
                                    <small>The printer slug name is used inside the server to reference the printer and
                                        the
                                        configuration. Data gets stored in directories named like the slug. Use only
                                        ascii letters a-z
                                        and digits for this. The slug name can not be changed later on.<br>
                                        HINT: You can leave name and slug empty to use the values stored in the configuration file!
                                    </small>
                                </p>
                            </div>
                            <div class="form-group">
                                <label>Configuration file</label>
                                <input type="file" class="form-control" id="confuploadbutton" name="upload">
                            </div>
                        </form>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
                <button type="button" class="btn btn-primary" ng-click="createPrinterFromConfig()" ng-class="{disabled:!canUpload()}" ng-show="confupload.mode">Create Printer</button>
                <button type="button" class="btn btn-primary" ng-click="createPrinterFromConfig()" ng-class="{disabled:!canUpload()}" ng-hide="confupload.mode">Replace Configuration</button>
            </div>
        </div>
    </div>
</div>