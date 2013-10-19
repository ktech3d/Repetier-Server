<div class="container">
    <div class="row" style="margin-top:10px">
        <div ng-repeat="p in printerList" class="col-xs-12 col-sm-6 col-lg-4">
            <div class="panel panel-primary">
                <div class="panel-heading">{{p.name}}
                    <div class="btn-group" style="float: right;top: -9px;right: -13px;">
                        <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                            <i class="icon-cog"></i> <span class="caret"></span>
                        </button>
                        <ul class="dropdown-menu" style="right:0;left:auto">
                            <li><a href="#/scriptConfig/{{p.slug}}"><?php _("Scripts") ?></a></li>
                            <li><a href="#/printerConfig/{{p.slug}}"><?php _("Configuration") ?></a></li>
                            <li>
                                <a href="/printer/pconfig/{{p.slug}}?a=download"><?php _("Download Configuration") ?></a>
                            </li>
                            <li><a href="javascript:void(0)"
                                   ng-click="replacePrinter(p)"><?php _("Replace Configuration") ?></a></li>
                            <li><a href="javascript:void(0)"
                                   ng-click="removePrinter(p)"><?php _("Remove Printer") ?></a></li>
                        </ul>
                    </div>
                </div>

                <div class="panel-body small-font">
                    <div class="container">
                        <div class="row">
                            <div class="col-xs-5"><?php _("Status:") ?></div>
                            <div class="col-xs-7" ng-bind-html-unsafe="p | online"></div>
                        </div>
                        <div class="row" ng-show="p.online">
                            <div class="col-xs-5"><?php _("Printing:") ?></div>
                            <div class="col-xs-7" ng-bind-html-unsafe="p | printing"></div>
                        </div>
                        <div class="row" ng-hide="p.online">
                            <div class="col-xs-5"><?php _("Printing:") ?></div>
                            <div class="col-xs-7"><?php _("No") ?></div>
                        </div>
                    </div>
                </div>
                <div class="panel-footer"><a class="btn btn-block btn-success btn-small"
                                             href="/#/printer/{{p.slug}}"><?php _("Details") ?></a></div>
            </div>
        </div>
    </div>
    <div class="row">
        <div class="col-12">
            <button class="btn btn-primary indent-left" ng-click="addPrinter()"><i
                    class="icon-plus"></i> <?php _("Add new Printer") ?></button>
            <button class="btn btn-primary" ng-click="addPrinterConfig()"><i
                    class="icon-plus"></i> <?php _("Upload Printer Configuration") ?></button>
        </div>
    </div>
</div>

<div class="modal fade" id="dialogaddprinter">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title"><?php _("Add new Printer") ?></h4>
            </div>
            <div class="modal-body">
                <div class="row">
                    <div class="col-xs-12">
                        <form>
                            <div class="form-group">
                                <label><?php _("Printer name") ?></label>
                                <input type="text" class="form-control" ng-model="newprinter.name"
                                       placeholder="enter printer name" required>
                                <inputerror error="newprinter.errors.name"></inputerror>
                            </div>
                            <div class="form-group">
                                <label><?php _("Printer slug") ?></label>
                                <input type="text" class="form-control" ng-model="newprinter.slug"
                                       placeholder="enter slug name" required>
                                <inputerror error="newprinter.errors.slug"></inputerror>
                                <p>
                                    <small><?php _("The printer slug name is used inside the server to reference the printer and the configuration. Data gets stored in directories named like the slug. Use only ascii letters a-z and digits for this. The slug name can not be changed later on.") ?>
                                    </small>
                                </p>
                            </div>
                        </form>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal"><?php _("Close") ?></button>
                <button type="button" class="btn btn-primary" ng-click="createPrinter()">
                    C<?php _("reate Printer") ?></button>
            </div>
        </div>
    </div>
</div>

<div class="modal fade" id="dialoguploadprinter">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title"
                    ng-show="confupload.mode"><?php _("Add new Printer from Configuration File") ?></h4>
                <h4 class="modal-title" ng-hide="confupload.mode"><?php _("Replace Printer Configuration File") ?></h4>
            </div>
            <div class="modal-body">
                <div class="row">
                    <div class="col-xs-12">
                        <form id="formuploadconfig" action="/printer/pconfig"
                              enctype="multipart/form-data">
                            <div class="alert alert-danger" ng-show="confupload.errors.createerror">
                                {{confupload.errors.createerror}}
                            </div>
                            <input type="hidden" name="a" value="upload">
                            <input type="hidden" name="mode" value="{{confupload.mode}}">

                            <div class="form-group" ng-show="confupload.mode">
                                <label><?php _("Printer name") ?></label>
                                <input type="text" name="name" class="form-control" ng-model="confupload.name"
                                       placeholder="enter printer name" required>
                                <inputerror error="confupload.errors.name"></inputerror>
                            </div>
                            <div class="form-group" ng-show="confupload.mode">
                                <label><?php _("Printer slug") ?></label>
                                <input type="text" name="slug" class="form-control" ng-model="confupload.slug"
                                       placeholder="enter slug name" required>
                                <inputerror error="confupload.errors.slug"></inputerror>
                                <p>
                                    <small><?php _("The printer slug name is used inside the server to reference the printer and the configuration. Data gets stored in directories named like the slug. Use only ascii letters a-z and digits for this. The slug name can not be changed later on.<br>HINT: You can leave name and slug empty to use the values stored in the configuration file!") ?>
                                    </small>
                                </p>
                            </div>
                            <div class="form-group">
                                <label><?php _("Configuration File") ?></label>
                                <input type="file" class="form-control" id="confuploadbutton" name="upload">
                            </div>
                        </form>
                    </div>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" data-dismiss="modal"><?php _("Close") ?></button>
                <button type="button" class="btn btn-primary" ng-click="createPrinterFromConfig()"
                        ng-class="{disabled:!canUpload()}"
                        ng-show="confupload.mode"><?php _("Create Printer") ?></button>
                <button type="button" class="btn btn-primary" ng-click="createPrinterFromConfig()"
                        ng-class="{disabled:!canUpload()}"
                        ng-hide="confupload.mode"><?php _("Replace Configuration") ?></button>
            </div>
        </div>
    </div>
</div>