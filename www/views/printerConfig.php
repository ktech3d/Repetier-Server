<div class="container">
<div class="row">
<div class="col-12">
<div class="row">
<div class="col-sm-9 col-12">
    <h1>
        Configure {{editor.general.name}}
    </h1>
</div>
<div class="col-sm-3 col-12" style="vertical-align: middle">
    <h1>
        <button class="btn btn-success btn-block" ng-click="saveConfig()"><i class="icon-save"></i> Save Configuration</button>
    </h1>
</div>
</div>

<form class="">
<ul class="nav nav-tabs" id="printerTabs">
    <li class="active"><a href="#general">General</a></li>
    <li><a href="#extruder">Extruder</a></li>
    <li><a href="#shape">Printer Shape</a></li>
</ul>
<div class="tab-content">
<div class="tab-pane active fade" id="general">
    <h4>General</h4>

    <div class="row">
        <div class="col-sm-9 form-group">
            <label for="confname">Printer Name</label>
            <input id="confname" class="form-control" type="text" placeholder="Displayed printer name"
                   ng-model="editor.general.name">
        </div>
        <div class="col-sm-3 form-group">
            <label>Printer Variant</label>
            <select class="form-control" ng-model="editor.general.printerVariant">
                <option value="delta">Delta Printer</option>
                <option value="cartesian">Cartesian Printer</option>
            </select>
        </div>
        <div class="col-4"></div>
    </div>
    <div class="row">
        <div class="col-lg-4 col-6">
            <label>Fan Installed {{edit.general.fan}}</label>

            <boolswitch class="small" on="Yes" off="No" value="editor.general.fan"></boolswitch>
        </div>
        <div class="col-lg-4 col-6">
            <label>SD Card reader Installed</label>

            <boolswitch class="small" on="Yes" off="No" value="editor.general.sdcard"></boolswitch>
        </div>
        <div class="col-lg-4 col-6">
            <label>Software Power</label>

            <boolswitch class="small" on="Yes" off="No"
                        value="editor.general.softwarePower"></boolswitch>
        </div>
        <div class="col-12"><h4>Connection</h4></div>
        <div class="col-lg-4 col-6 form-group">
            <label>Device / Port</label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.connection.serial.device" data-toggle="popover" data-placement="bottom"
                   data-content="For windows systems, this is the port COMx. On unix this is the path to the serial device like /dev/tty.USB0"
                   data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Baud rate</label>
            <select class="form-control" ng-model="editor.connection.serial.baudrate">
                <option>19200</option>
                <option>38400</option>
                <option>56000</option>
                <option>57600</option>
                <option>76800</option>
                <option>111112</option>
                <option>115200</option>
                <option>230400</option>
                <option>250000</option>
                <option>256000</option>
                <option>460800</option>
                <option>500000</option>
            </select>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Input Buffer Size</label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.connection.serial.inputBufferSize" data-toggle="popover" data-placement="bottom"
                   data-content="Size of bytes the incoming queue of your printer can store. Typical values are 127 and 63 byte."
                   data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Protocol</label>
            <select class="form-control" ng-model="editor.connection.serial.protocol">
                <option value="0">Autodetect</option>
                <option value="1">Force ASCII</option>
                <option value="2">Force Repetier</option>
            </select>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Ping-Pong Mode</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.connection.serial.pingPong"></boolswitch>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Send ok after Resend</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.connection.serial.okAfterResend"></boolswitch>
        </div>
        <div class="col-12"><h4>Movement</h4></div>
        <div class="col-lg-4 col-6 form-group">
            <label>X-Min [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.xMin">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Y-Min [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.yMin">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Z-Min [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.zMin">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>X-Max [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.xMax">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Y-Max [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.yMax">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Z-Max [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.zMax">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>X-Home [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.xHome" data-toggle="popover" data-placement="bottom"
                   data-content="X coordinate after homing x axis." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Y-Home [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.yHome" data-toggle="popover" data-placement="bottom"
                   data-content="Y coordinate after homing y axis." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Z-Home [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.movement.zHome" data-toggle="popover" data-placement="bottom"
                   data-content="Z coordinate after homing z axis." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-3 col-6 form-group">
            <label>X Endstop</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.movement.xEndstop"></boolswitch>
        </div>
        <div class="col-lg-3 col-6 form-group">
            <label>Y Endstop</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.movement.yEndstop"></boolswitch>
        </div>
        <div class="col-lg-3 col-6 form-group">
            <label>Z Endstop</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.movement.zEndstop"></boolswitch>
        </div>
        <div class="col-lg-3 col-6 form-group">
            <label>All Endstops</label>
            <boolswitch class="small" on="Yes" off="No"
                        value="editor.movement.allEndstops"></boolswitch>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>XY Speed [mm/s]</label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.movement.xySpeed" data-toggle="popover" data-placement="bottom"
                   data-content="Speed for manual moves in x-y-plane." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Z Speed [mm/s]</label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.movement.zSpeed" data-toggle="popover" data-placement="bottom"
                   data-content="Speed for manual moves in z-direction." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>XY Print Acceleration [mm/s²]</label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.movement.xyPrintAcceleration" data-toggle="popover" data-placement="bottom"
                   data-content="Acceleration during print moves in x-y-plane." data-original-title
                   data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>XY Travel Acceleration [mm/s²]</label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.movement.xyTravelAcceleration" data-toggle="popover" data-placement="bottom"
                   data-content="Acceleration during travel moves in x-y-plane." data-original-title
                   data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>XY Jerk [mm/s]</label>
            <input class="form-control" type="number" step="any" min="0" placeholder=""
                   ng-model="editor.movement.xyJerk" data-toggle="popover" data-placement="bottom"
                   data-content="Max. allowed jerk for moves in x-y-plane." data-original-title data-trigger="focus">
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Z Jerk [mm/s]</label>
            <input class="form-control" type="number" step="any" min="0" placeholder=""
                   ng-model="editor.movement.zJerk" data-toggle="popover" data-placement="bottom"
                   data-content="Max. allowed jerk for moves in z-direction." data-original-title data-trigger="focus">
        </div>

    </div>

</div>
<div class="tab-pane active fade" id="extruder">
    <h3>Heated Bed</h3>

    <div class="row">
        <div class="col-3">
            <label>Heated bed installed</label>

            <boolswitch class="small" on="Yes" off="No"
                        value="editor.heatedBed.installed"></boolswitch>
        </div>
    </div>
    <div ng-show="editor.heatedBed.installed">
        <h4>Predefined temperatures</h4>

        <div class="row" ng-repeat="r in editor.heatedBed.temperatures" style="margin-bottom:0.3em">
            <div class="col-2">Name</div>
            <div class="col-3"><input type="text" ng-model="r.name" class="form-control"></div>
            <div class="col-2">Temp. [°C]</div>
            <div class="col-3"><input type="text" ng-model="r.temp" class="form-control"></div>
            <div class="col-2">
                <button ng-hide="$first" class="btn" ng-click="bedTempUp($index)"><i class="icon-arrow-up"></i></button>
                <button ng-hide="$last" class="btn" ng-click="bedTempDown($index)"><i class="icon-arrow-down"></i>
                </button>
                <button class="btn btn-danger" ng-click="bedTempDel($index)"><i class="icon-trash"></i></button>
            </div>
        </div>
        <button class="btn btn-primary" ng-click="bedTempAdd()"><i class="icon-plus-sign"></i> Add</button>
    </div>
    <h3>Extruder</h3>

    <div ng-repeat="ex in editor.extruders" style="margin-bottom:0.5em">
        <h4>Extruder {{$index+1}}</h4>

        <div class="row">
            <div class="col-lg-4 col-6 form-group">
                <label>Jerk [mm/s]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.eJerk" data-toggle="popover" data-placement="bottom"
                       data-content="Extruder will start with 50% of jerk." data-original-title data-trigger="focus">
            </div>
            <div class="col-lg-4 col-6 form-group">
                <label>Jerk [mm/s]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.maxSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="Maximum extruder speed." data-original-title data-trigger="focus">
            </div>
            <div class="col-lg-4 col-6 form-group">
                <label>Extrude speed [mm/s]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.extrudeSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="Extruder speed if you click on extrude in manual controls." data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-6 form-group">
                <label>Extrude distance[mm]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.extrudeSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="Extrude distance if you click on extrude in manual controls." data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-6 form-group">
                <label>Retract speed [mm/s]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.retractSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="Retract speed if you click on extrude in manual controls." data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-6 form-group">
                <label>Retract distance[mm]</label>
                <input class="form-control" type="text" placeholder=""
                       ng-model="ex.retractSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="Retract distance if you click on extrude in manual controls." data-original-title
                       data-trigger="focus">
            </div>
        </div>
        <h4>Predefined temperatures</h4>

        <div class="row" ng-repeat="r in ex.temperatures" style="margin-bottom:0.3em">
            <div class="col-2">Name</div>
            <div class="col-3"><input type="text" ng-model="r.name" class="form-control"></div>
            <div class="col-2">Temp. [°C]</div>
            <div class="col-3"><input type="text" ng-model="r.temp" class="form-control"></div>
            <div class="col-2">
                <button ng-hide="$first" class="btn" ng-click="extTempUp(ex,$index)"><i class="icon-arrow-up"></i>
                </button>
                <button ng-hide="$last" class="btn" ng-click="extTempDown(ex,$index)"><i class="icon-arrow-down"></i>
                </button>
                <button class="btn btn-danger" ng-click="extTempDel(ex,$index)"><i class="icon-trash"></i></button>
            </div>
        </div>
        <div class="row">
            <div class="col-2 col-sm-2">
                <button class="btn btn-primary" ng-click="extTempAdd(ex)"><i class="icon-plus-sign"></i> Add</button>
            </div>
            <div class="col-0 col-sm-8"></div>
            <div class="col-2 col-sm-2">
                <button class="btn btn-danger" ng-click="removeExtruder($index)"><i class="icon-trash"></i> Delete
                    Extruder
                </button>
            </div>
        </div>
    </div>
    <button class="btn btn-primary" ng-click="addExtruder()"><i class="icon-plus-sign"></i> Add Extruder</button>
</div>
<div class="tab-pane active fade" id="shape">
    <h4>Printer Shape</h4>

    <div id="shapePreview" class="well" style="height:300px"></div>
    <div class="row">
        <div class="col-lg-4 col-6 form-group">
            <label>Grid color</label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.shape.gridColor" colorpicker>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Grid spacing [mm] (0=disable)</label>
            <input class="form-control" type="number" min="0" step="any" placeholder=""
                   ng-model="editor.shape.gridSpacing">
        </div>
    </div>
    <div class="row">
        <div class="col-lg-4 col-6 form-group">
            <label>Bed shape</label>
            <select ng-model="editor.shape.basicShape.shape" class="form-control">
                <option value="rectangle">Rectangle</option>
                <option value="circle">Circle</option>
            </select>
        </div>
        <div class="col-lg-4 col-6 form-group">
            <label>Bed color</label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.shape.basicShape.color" colorpicker>
        </div>
    </div>
    <div class="row" ng-show="editor.shape.basicShape.shape == 'rectangle'">
        <div class="col-sm-3 col-6 form-group">
            <label>X left [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.xMin">
        </div>
        <div class="col-sm-3 col-6 form-group">
            <label>Y front [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.yMin">
        </div>
        <div class="col-sm-3 col-6 form-group">
            <label>X right [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.xMax">
        </div>
        <div class="col-sm-3 col-6 form-group">
            <label>Y rear [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.yMax">
        </div>
    </div>
    <div class="row" ng-show="editor.shape.basicShape.shape == 'circle'">
        <div class="col-sm-3 col-6 form-group">
            <label>X center [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.x">
        </div>
        <div class="col-sm-3 col-6 form-group">
            <label>Y center [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.y">
        </div>
        <div class="col-sm-3 col-6 form-group">
            <label>Radius [mm]</label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.radius">
        </div>
    </div>
    <br><br>{{editor | json}}
</div>
</div>
</form>
</div>
</div>
</div>