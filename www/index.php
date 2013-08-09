<!DOCTYPE html>
<html lang="en" ng-app="server">
<head>
    <meta charset="utf-8">
    <title>Bootstrap, from Twitter</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="Repetier-Server for 3d printer">
    <meta name="author" content="Roland Littwin">
    <link rel="apple-touch-icon" href="/touch-icon-iphone.png"/>
    <link rel="apple-touch-icon" sizes="72x72" href="/touch-icon-ipad.png"/>
    <link rel="apple-touch-icon" sizes="114x114" href="/touch-icon-iphone4.png"/>
    <link rel="stylesheet" href="/style/css/btserver.css">
    <link rel="stylesheet" href="/css/css/font-awesome.css">
    <link rel="stylesheet" href="/libs/colorpicker/css/colorpicker.css"/>
    <title>Repetier-Server</title>
</head>
<body ng-controller="ServerController">
<div class="navbar navbar-fixed-top">
    <div class="container">
        <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-responsive-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
        </button>
        <a class="navbar-brand" href="#">Repetier-Server</a>

        <div class="nav-collapse collapse navbar-responsive-collapse">
            <ul class="nav navbar-nav">
                <li class=""><a href="#"><i class="icon-home"></i>Dashboard</a></li>
                <li class="dropdown"><a href="#" class="dropdown-toggle" data-toggle="dropdown">Printer</a>
                    <ul class="dropdown-menu">
                        <li ng-repeat="p in printerList"><a href="#/printer/{{p.slug}}">{{p.name}}</a>
                    </ul>
                </li>
                <li class=""><a data-reveal-id="messageList" href="#" data-toggle="modal" data-target="#messageList">Messages <span
                            class="badge badge-success">{{messages.length}}</span></a>
                </li>
            </ul>
        </div>
    </div>
</div>
<ng-view></ng-view>
<!--<div class="server-footer">
    <div class="row equalheight">
        <div class="small-4 columns">
            <h2>Repetier-Server</h2>

            <p>The user friendly way to manager your 3d printer.</p>
        </div>
        <div class="small-4 columns vsplit">
            <a href="#/about">About</a><br>

        </div>
        <div class="small-4 columns vsplit">
            About<br>

        </div>
    </div>
</div> -->
<div class="modal fade" id="messageList">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
                <h4 class="modal-title">Messages</h4>
            </div>
            <div class="modal-body">
                <div style="max-height:300px;overflow:scroll">
                    <div class="row" ng-repeat="m in messages">
                        <div class="small-9 columns">
                            {{m.msg}}
                        </div>
                        <div class="small-3 button" ng-click="removeMessage($index)">Remove</div>
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
                <h4 class="modal-title">Connection lost</h4>
            </div>
            <div class="modal-body">
                <p>Uups! I lost my connection to the server. Trying to reconnect.</p>
            </div>
        </div>
    </div>
</div>

<!-- Placed at the end of the document so the pages load faster -->
<script src="/jquery/jquery.min.js"></script>
<script src="/jquery/jquery.form.js"></script>
<script src="/js/kinetic-v4.5.5.min.js"></script>
<script src="/js/directives/GCodePainter.js"></script>
<script src="/libs/colorpicker/js/bootstrap-colorpicker.js"></script>
<script src="/js/bootstrap.js"></script>
<script src="/js/gauge.js"></script>
<script src="/js/angular.min.js"></script>
<script src="/js/angular-resource.min.js"></script>
<script src="/js/angular-sanitize.min.js"></script>
<script src="/js/directives/filter.js"></script>
<script src="/js/router.js"></script>
<script src="/js/directives/bootstrap.js"></script>
<script src="/js/directives/widgets.js"></script>
<script src="/js/controller/ServerController.js"></script>
<script src="/js/controller/HomeController.js"></script>
<script src="/js/controller/PrinterController.js"></script>
<script src="/js/directives/gauge.js"></script>
</body>
</html>