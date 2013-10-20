<!DOCTYPE html>
<html lang="en" ng-app="server">
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="chrome=1, IE=edge"/>
    <title><?php _("PROJECT_NAME_VERSION") ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="<?php _("PROJECT_NAME") ?> for 3d printer">
    <meta name="author" content="Roland Littwin">
    <link rel="apple-touch-icon" href="/touch-icon-iphone.png"/>
    <link rel="apple-touch-icon" sizes="72x72" href="/touch-icon-ipad.png"/>
    <link rel="apple-touch-icon" sizes="114x114" href="/touch-icon-iphone4.png"/>
    <link rel="stylesheet" href="/style/css/btserver.css">
    <link rel="stylesheet" href="/css/css/font-awesome.css">
    <link rel="stylesheet" href="/libs/colorpicker/css/colorpicker.css"/>
    <title><?php _("PROJECT_NAME_VERSION") ?></title>
</head>
<body ng-controller="ServerController">
<div id="loading" class="loading" ng-show="loading"><i class="icon-spin icon-spinner"></i> Loading ...</div>
<nav class="navbar navbar-default navbar-fixed-top" role="navigation">
    <div class="navbar-header">
        <button type="button" class="navbar-toggle" data-toggle="collapse"
                data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
        </button>
        <a class="navbar-brand" href="#"><?php _("PROJECT_NAME_VERSION") ?></a>
    </div>
    <div class="nav-collapse collapse navbar-collapse">
        <ul class="nav navbar-nav">
            <li class=""><a href="#"><i class="icon-home"></i><?php _("Dashboard") ?></a></li>
            <li class="dropdown"><a href="javascript:void(0)" class="dropdown-toggle" data-toggle="dropdown">Printer</a>
                <ul class="dropdown-menu">
                    <li ng-repeat="p in printerList"><a href="#/printer/{{p.slug}}" style="padding-right:100px">{{p.name}} <span style="position:absolute;right:0;margin-right:10px" ng-bind-html-unsafe="p | online"></span></a>
                </ul>
            </li>
            <li class=""><a data-reveal-id="messageList" href="javascript:void(0);" data-toggle="modal"
                            data-target="#messageList"><?php _("Messages") ?> <span
                        class="badge badge-success">{{messages.length}}</span></a>
            </li>
            <li class="dropdown"><a href="javascript_void()" class="dropdown-toggle" data-toggle="dropdown"><i
                        class="icon-cog"></i></a>
                <ul class="dropdown-menu">
                    <li class="dropdown-submenu" ng-hide="externalCommands.length==0"><a
                            href="javascript:void(0)" class="dropdown-toggle"
                            data-toggle="dropdown"><?php _("Start server command") ?></a>
                        <ul class="dropdown-menu">
                            <li ng-repeat="c in externalCommands"><a ng-click="runExternalCommand(c.id)"
                                                                     href="javascript:void(0);">{{c.name}}</a>
                        </ul>
                    </li>
                    <li ng-show="user.canChangeConfig()"><a href="#/userconfig"><?php _("Configure User") ?></a>
                    </li>
                    <li><a href="#/about"><?php _("About") ?> <?php _("PROJECT_NAME") ?></a></li>
                </ul>
            </li>
        </ul>
    </div>
</nav>
<ng-view></ng-view>
<div class="modal fade" id="messageList">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title"><?php _("Messages") ?></h4>
            </div>
            <div class="modal-body">
                <div style="max-height:300px;overflow:scroll">
                    <div class="row" ng-repeat="m in messages">
                        <div class="alert alert-info alert-dismissable">
                            <button type="button" ng-click="removeMessage($index)" class="close"
                                    aria-hidden="true">&times;</button>
                            {{m.msg}}
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</div>
<div id="connectionLost" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <h4 class="modal-title"><?php _("Connection lost") ?></h4>
            </div>
            <div class="modal-body">
                <p><?php _("Uups! I lost my connection to the server. Trying to reconnect.") ?></p>
            </div>
        </div>
    </div>
</div>
<div id="question" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <h4 class="modal-title">{{question.header}}</h4>
            </div>
            <div class="modal-body">
                <p>{{question.question}}</p>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default" ng-click="questionNo()">{{question.no}}</button>
                <button type="button" class="btn btn-primary" ng-click="questionYes()">{{question.yes}}</button>
            </div>
        </div>
    </div>
</div>
<!-- Placed at the end of the document so the pages load faster -->
<script src="/js/modernizr.js"></script>
<script src="/jquery/jquery.min.js"></script>
<script src="/jquery/jquery.form.js"></script>
<script src="/js/kinetic-v4.6.0.min.js"></script>
<script src="/js/vendor/three.min.js"></script>
<script src="/js/vendor/CryptoJS/md5.js"></script>
<script src="/js/directives/GCodePainter.js"></script>
<script src="/libs/colorpicker/js/bootstrap-colorpicker.js"></script>
<script src="/js/bootstrap.js"></script>
<script src="/js/bootstrap-slider.js"></script>
<script src="/js/gauge.js"></script>
<script src="/js/angular.min.js"></script>
<script src="/js/angular-resource.min.js"></script>
<script src="/js/angular-sanitize.min.js"></script>
<script src="/js/directives/slider.js"></script>
<!--<script src="/libs/slider/angular-slider.min.js"></script>-->
<script src="/js/directives/filter.js"></script>
<script src="/js/router.js"></script>
<script src="/js/directives/bootstrap.js"></script>
<script src="/js/directives/widgets.js"></script>
<script src="/js/directives/gcodepreview.js"></script>
<script src="/js/controller/ServerController.js"></script>
<script src="/js/controller/HomeController.js"></script>
<script src="/js/controller/PrinterController.js"></script>
<script src="/js/controller/UserController.js"></script>
<script src="/js/directives/gauge.js"></script>
</body>
</html>