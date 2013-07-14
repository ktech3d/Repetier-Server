    <div class="row" style="margin-top:10px">
    <div ng-repeat="p in printerList" class="small-6 large-4 columns left-align">
        <ul class="pricing-table">
            <li class="title">{{p.name}}</li>
            <li class="price hide-for-small"><gauge title="Print" units="C" max="300" scale="1"></gauge></li>
            <li class="bullet-item">Status: <span ng-bind-html-unsafe="p | online"></span></li>
            <li class="bullet-item">Printing: <span ng-bind-html-unsafe="p | printing"></span></li>
       </ul>
    </div>
    </div>
