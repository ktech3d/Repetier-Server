<!DOCTYPE html>
<html lang="en" ng-app="server">
<head>
    <meta charset="utf-8">
    <title>Bootstrap, from Twitter</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="description" content="">
    <meta name="author" content="Roland Littwin">
    <link rel="apple-touch-icon" href="/touch-icon-iphone.png"/>
    <link rel="apple-touch-icon" sizes="72x72" href="/touch-icon-ipad.png"/>
    <link rel="apple-touch-icon" sizes="114x114" href="/touch-icon-iphone4.png"/>
    <link rel="stylesheet" href="/style/css/servermain.css">
    <link rel="stylesheet" href="/style/css/fontello.css">
    <script src="/js/vendor/custom.modernizr.js"></script>
    <title>Repetier-Server</title>
</head>
<body ng-controller="ServerController">
<div class="contain-to-grid sticky">
    <nav class="top-bar">
        <ul class="title-area">
            <!-- Title Area -->
            <li class="name">
                <h1><a href="#">Repetier-Server</a></h1>
            </li>
            <!-- Remove the class "menu-icon" to get rid of menu icon. Take out "Menu" to just have icon alone -->
            <li class="toggle-topbar menu-icon"><a href="#"><span>Menu</span></a></li>
        </ul>
        <section class="top-bar-section">
            <!-- Left Nav Section -->
            <ul class="left">
                <li class="divider"></li>
                <li class="active"><a href="#"><i class="icon-flight"></i> Dashboard</a></li>
                <li class="divider"></li>
                <li class="has-dropdown"><a href="#">Printer</a>
                    <ul class="dropdown">
                        <li ng-repeat="p in printerList"><a href="#/printer/{{p.slug}}">{{p.name}}</a>
                    </ul>
                </li>
                <li class=""><a data-reveal-id="messageList" href="#">Messages <span
                            class="round success label">{{messages.length}}</span></a>
                </li>
            </ul>
        </section>
    </nav>
</div>
<ng-view></ng-view>
<div class="server-footer">
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
</div>
<div id="messageList" class="reveal-modal">
    <h2>Messages</h2>

    <div style="max-height:300px;overflow:scroll">
        <div class="row" ng-repeat="m in messages">
            <div class="small-9 columns">
                {{m.msg}}
            </div>
            <div class="small-3 button" ng-click="removeMessage($index)">Remove</div>
        </div>
        <a class="close-reveal-modal">&#215;</a>
    </div>
</div>
<div id="connectionLost" class="reveal-modal medium">
    <h2>Connection lost</h2>

    <p>Uups! I lost my connection to the server. Trying to reconnect.</p>
</div>

<!-- Placed at the end of the document so the pages load faster -->
<script src="/jquery/jquery.min.js"></script>
<script src="/jquery/jquery.form.js"></script>
<!-- <script src="/js/foundation.min.js"></script>-->
<script src="/js/foundation/foundation.js"></script>
<script src="/js/foundation/foundation.dropdown.js"></script>
<script src="/js/foundation/foundation.alerts.js"></script>
<script src="/js/foundation/foundation.forms.js"></script>
<script src="/js/foundation/foundation.section.js"></script>
<script src="/js/foundation/foundation.interchange.js"></script>
<script src="/js/foundation/foundation.reveal.js"></script>
<script src="/js/foundation/foundation.topbar.js"></script>
<script src="/js/gauge.js"></script>
<script src="/js/angular.min.js"></script>
<script src="/js/angular-resource.min.js"></script>
<script src="/js/angular-sanitize.min.js"></script>
<script src="/js/directives/filter.js"></script>
<script src="/js/router.js"></script>
<script src="/js/directives/foundation.js"></script>
<script src="/js/directives/widgets.js"></script>
<script src="/js/controller/ServerController.js"></script>
<script src="/js/controller/HomeController.js"></script>
<script src="/js/controller/PrinterController.js"></script>
<script src="/js/directives/gauge.js"></script>
</body>
</html>