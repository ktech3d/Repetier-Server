<div class="container">
    <ul class="breadcrumb">
        <li><a href="#/">Dashboard</a></li>
        <li><a href="#/printer/{{activeSlug}}">{{active.status.name}}</a></li>
        <li class="active">Scripts</li>
    </ul>

    <div class="row">
        <div class="col-sm-6 col-8">
            <div class="btn-group">
                <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                    {{activeScript.title}} <span class="caret"></span>
                </button>
                <ul class="dropdown-menu">
                    <li ng-repeat="s in scripts" ng-click="selectScript(s)"><a href="javascript:void(0);">{{s.title}}</a></li>
                </ul>
            </div>
        </div>
        <div class="col-sm-4 col-1"></div>
        <div class="col-sm-2 col-3">
            <button class="btn btn-primary btn-block" ng-click="saveScript()"><i class="icon-save"></i> Save</button>
        </div>
    </div>
    <div class="row" style="margin-top:10px;">
        <div class="col-12">
            <textarea ng-model="scriptContent" style="width:100%;height:400px"></textarea>
        </div>
    </div>
</div>