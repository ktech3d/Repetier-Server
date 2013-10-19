<div class="container">
<ul class="breadcrumb">
    <li><a href="#/"><?php _("Dashboard") ?></a></li>
    <li><a href="#/printer/{{activeSlug}}">{{active.status.name}}</a></li>
    <li class="active"><?php _("Configuration") ?></li>
</ul>

<div class="row">
<div class="col-xs-12">
<div class="row">
    <div class="col-sm-9 col-xs-12">
        <h1>
            <?php _("Configure") ?> {{editor.general.name}}
        </h1>
    </div>
    <div class="col-sm-3 col-xs-12" style="vertical-align: middle">
        <h1>
            <button class="btn btn-success btn-block" ng-click="saveConfig()"><i
                    class="icon-save"></i> <?php _("Save Configuration") ?>
            </button>
        </h1>
    </div>
</div>

<form>
<ul class="nav nav-tabs" id="printerTabs">
    <li><a href="#general"><?php _("General") ?></a></li>
    <li><a href="#extruder"><?php _("Extruder") ?></a></li>
    <li><a href="#shape"><?php _("Printer Shape") ?></a></li>
    <li><a href="#advanced"><?php _("Advanced") ?></a></li>
</ul>
<div class="tab-content">
<div class="tab-pane active fade" id="general">
<h4><?php _("General") ?></h4>

<div class="row">
    <div class="col-sm-9 form-group">
        <label for="confname"><?php _("Printer Name") ?></label>
        <input id="confname" class="form-control" type="text" placeholder="Displayed printer name"
               ng-model="editor.general.name">
    </div>
    <div class="col-sm-3 form-group">
        <label><?php _("Printer Variant") ?></label>
        <select class="form-control" ng-model="editor.general.printerVariant">
            <option value="delta"><?php _("Delta Printer") ?></option>
            <option value="cartesian"><?php _("Cartesian Printer") ?></option>
        </select>
    </div>
</div>
<div class="row">
    <div class="col-lg-4 col-xs-6">
        <label><?php _("Fan Installed") ?></label>

        <boolswitch class="small" on="Yes" off="No" value="editor.general.fan"></boolswitch>
    </div>
    <div class="col-lg-4 col-xs-6">
        <label><?php _("SD Card reader Installed") ?></label>

        <boolswitch class="small" on="Yes" off="No" value="editor.general.sdcard"></boolswitch>
    </div>
    <div class="col-lg-4 col-xs-6">
        <label><?php _("Software Power") ?></label>

        <boolswitch class="small" on="Yes" off="No"
                    value="editor.general.softwarePower"></boolswitch>
    </div>
</div>
<div class="row margin-top">
    <div class="col-xs-12"><h4><?php _("Connection") ?></h4></div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Device / Port") ?></label>
        <input class="form-control" type="text" placeholder=""
               ng-model="editor.connection.serial.device" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("For windows systems, this is the port COMx. On unix this is the path to the serial device like /dev/tty.USB0") ?>"
               data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Baud rate") ?></label>
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
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Input Buffer Size") ?></label>
        <input class="form-control" type="text" placeholder=""
               ng-model="editor.connection.serial.inputBufferSize" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Size of bytes the incoming queue of your printer can store. Typical values are 127 and 63 byte.") ?>"
               data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Protocol") ?></label>
        <select class="form-control" ng-model="editor.connection.serial.protocol">
            <option value="0"><?php _("Autodetect") ?></option>
            <option value="1"><?php _("Force ASCII") ?></option>
            <option value="2"><?php _("Force Repetier") ?></option>
        </select>
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Ping-Pong Mode") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.connection.serial.pingPong"></boolswitch>
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Send ok after Resend") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.connection.serial.okAfterResend"></boolswitch>
    </div>
</div>
<div class="row">
    <div class="col-xs-12"><h4><?php _("Movement") ?></h4></div>
</div>
<div class="row">
    <div class="col-lg-4 col-xs-6 form-group">
        <label>X-Min [mm]</label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.xMin">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Y-Min [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.yMin">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Z-Min [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.zMin">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("X-Max [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.xMax">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Y-Max [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.yMax">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Z-Max [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.zMax">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label>X<?php _("-Home [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.xHome" data-toggle="popover" data-placement="bottom"
               data-content="X coordinate after homing x axis." data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Y-Home [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.yHome" data-toggle="popover" data-placement="bottom"
               data-content="Y coordinate after homing y axis." data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Z-Home [mm]") ?></label>
        <input class="form-control" type="number" step="any" placeholder=""
               ng-model="editor.movement.zHome" data-toggle="popover" data-placement="bottom"
               data-content="Z coordinate after homing z axis." data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-3 col-xs-6 form-group">
        <label><?php _("X Endstop") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.movement.xEndstop"></boolswitch>
    </div>
    <div class="col-lg-3 col-xs-6 form-group">
        <label><?php _("Y Endstop") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.movement.yEndstop"></boolswitch>
    </div>
    <div class="col-lg-3 col-xs-6 form-group">
        <label><?php _("Z Endstop") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.movement.zEndstop"></boolswitch>
    </div>
    <div class="col-lg-3 col-xs-6 form-group">
        <label><?php _("All Endstops") ?></label>
        <boolswitch class="small" on="Yes" off="No"
                    value="editor.movement.allEndstops"></boolswitch>
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("XY Speed [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.xySpeed" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Speed for manual moves in x-y-plane.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Max. XY Speed [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.maxXYSpeed" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Maximum speed for moves in x-y-plane.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("XY Jerk [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="0" placeholder=""
               ng-model="editor.movement.xyJerk" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Max. allowed jerk for moves in x-y-plane.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Z Speed [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.zSpeed" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Speed for manual moves in z-direction.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Max. Z Speed [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.maxZSpeed" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Maximum speed for moves in z-direction.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Z Jerk [mm/s]") ?></label>
        <input class="form-control" type="number" step="any" min="0" placeholder=""
               ng-model="editor.movement.zJerk" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Max. allowed jerk for moves in z-direction.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("XY Print Acceleration [mm/s²]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.xyPrintAcceleration" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Acceleration during print moves in x-y-plane.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("XY Travel Acceleration [mm/s²]") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.xyTravelAcceleration" data-toggle="popover" data-placement="bottom"
               data-content="<?php _("Acceleration during travel moves in x-y-plane.") ?>" data-original-title
               data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Move buffer size") ?></label>
        <input class="form-control" type="number" step="any" min="1" placeholder=""
               ng-model="editor.movement.movebuffer" data-toggle="popover" data-placement="bottom"
               data-content="Size of the path planner buffer in your firmware. 16 is a typical value. Used for printing time prediction."
               data-original-title data-trigger="focus">
    </div>
    <div class="col-lg-4 col-xs-6 form-group">
        <label><?php _("Printing time multiplier") ?></label>
        <input class="form-control" type="number" step="any" min="0.1" placeholder=""
               ng-model="editor.movement.timeMultiplier" data-toggle="popover" data-placement="bottom"
               data-content="Computed printing time gets multiplied by this value. If you configured relevant parameter correctly, the multiplier should be near 1."
               data-original-title data-trigger="focus">
    </div>
</div>
</div>
<div class="tab-pane active fade" id="extruder">
    <h3><?php _("Heated Bed") ?></h3>

    <div class="row">
        <div class="col-xs-3">
            <label><?php _("Heated bed installed") ?></label>

            <boolswitch class="small" on="<?php _("Yes") ?>" off="<?php _("No") ?>"
                        value="editor.heatedBed.installed"></boolswitch>
        </div>
    </div>
    <div ng-show="editor.heatedBed.installed">
        <h4><?php _("Predefined temperatures") ?></h4>

        <div class="row">
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Heatup per Second") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="editor.heatedBed.heatupPerSecond" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Time to raise temperature by one degree celsius in seconds.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Cooldown per Second") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="editor.heatedBed.cooldownPerSecond" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Time to reduce temperature by one degree celsius in seconds.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
        </div>
        <div class="row" ng-repeat="r in editor.heatedBed.temperatures" style="margin-bottom:0.3em">
            <div class="col-xs-2"><?php _("Name") ?></div>
            <div class="col-xs-3"><input type="text" ng-model="r.name" class="form-control"></div>
            <div class="col-xs-2"><?php _("Temp. [°C]") ?></div>
            <div class="col-xs-3"><input type="number" ng-model="r.temp" class="form-control"></div>
            <div class="col-xs-2">
                <button ng-hide="$first" class="btn" ng-click="bedTempUp($index)"><i class="icon-arrow-up"></i></button>
                <button ng-hide="$last" class="btn" ng-click="bedTempDown($index)"><i class="icon-arrow-down"></i>
                </button>
                <button class="btn btn-danger" ng-click="bedTempDel($index)"><i class="icon-trash"></i></button>
            </div>
        </div>
        <button class="btn btn-primary" ng-click="bedTempAdd()"><i class="icon-plus-sign"></i> Add</button>
    </div>
    <h3><?php _("Extruder") ?></h3>

    <div ng-repeat="ex in editor.extruders" style="margin-bottom:0.5em">
        <h4><?php _("Extruder") ?> {{$index+1}}</h4>

        <div class="row">
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Jerk [mm/s]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.eJerk" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Extruder will start with 50% of jerk.") ?>" data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Max. speed [mm/s]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.maxSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Maximum extruder speed.") ?>" data-original-title data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Extrude speed [mm/s]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.extrudeSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Extruder speed if you click on extrude in manual controls.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Extrude distance [mm]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.extrudeSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Extrude distance if you click on extrude in manual controls.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Retract speed [mm/s]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.retractSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Retract speed if you click on extrude in manual controls.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Retract distance [mm]") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.retractSpeed" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Retract distance if you click on extrude in manual controls.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Heatup per Second") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.heatupPerSecond" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Time to raise temperature by one degree celsius in seconds.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
            <div class="col-lg-4 col-xs-6 form-group">
                <label><?php _("Cooldown per Second") ?></label>
                <input class="form-control" type="number" placeholder=""
                       ng-model="ex.cooldownPerSecond" data-toggle="popover" data-placement="bottom"
                       data-content="<?php _("Time to reduce temperature by one degree celsius in seconds.") ?>"
                       data-original-title
                       data-trigger="focus">
            </div>
        </div>
        <h4><?php _("Predefined temperatures") ?></h4>

        <div class="row" ng-repeat="r in ex.temperatures" style="margin-bottom:0.3em">
            <div class="col-xs-2"><?php _("Name") ?></div>
            <div class="col-xs-3"><input type="text" ng-model="r.name" class="form-control"></div>
            <div class="col-xs-2"><?php _("Temp. [°C]") ?></div>
            <div class="col-xs-3"><input type="number" ng-model="r.temp" class="form-control"></div>
            <div class="col-xs-2">
                <button ng-hide="$first" class="btn" ng-click="extTempUp(ex,$index)"><i class="icon-arrow-up"></i>
                </button>
                <button ng-hide="$last" class="btn" ng-click="extTempDown(ex,$index)"><i class="icon-arrow-down"></i>
                </button>
                <button class="btn btn-danger" ng-click="extTempDel(ex,$index)"><i class="icon-trash"></i></button>
            </div>
        </div>
        <div class="row">
            <div class="col-xs-2 col-sm-2">
                <button class="btn btn-primary" ng-click="extTempAdd(ex)"><i
                        class="icon-plus-sign"></i> <?php _("Add") ?></button>
            </div>
            <div class="col-xs-0 col-sm-8"></div>
            <div class="col-xs-2 col-sm-2">
                <button class="btn btn-danger" ng-click="removeExtruder($index)"><i
                        class="icon-trash"></i> <?php _("Delete Extruder") ?>
                </button>
            </div>
        </div>
        <hr>
    </div>
    <button class="btn btn-primary" ng-click="addExtruder()"><i class="icon-plus-sign"></i> <?php _("Add Extruder") ?>
    </button>
</div>
<div class="tab-pane active fade" id="shape">
    <h4><?php _("Printer Shape") ?></h4>

    <div id="shapePreview" class="well" style="height:300px"></div>
    <div class="row">
        <div class="col-lg-4 col-xs-6 form-group">
            <label><?php _("Grid color") ?></label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.shape.gridColor" colorpicker>
        </div>
        <div class="col-lg-4 col-xs-6 form-group">
            <label><?php _("Grid spacing [mm] (0=disable)") ?></label>
            <input class="form-control" type="number" min="0" step="any" placeholder=""
                   ng-model="editor.shape.gridSpacing">
        </div>
    </div>
    <div class="row">
        <div class="col-lg-4 col-xs-6 form-group">
            <label><?php _("Bed shape") ?></label>
            <select ng-model="editor.shape.basicShape.shape" class="form-control">
                <option value="rectangle"><?php _("Rectangle") ?></option>
                <option value="circle"><?php _("Circle") ?></option>
            </select>
        </div>
        <div class="col-lg-4 col-xs-6 form-group">
            <label><?php _("Bed color") ?></label>
            <input class="form-control" type="text" placeholder=""
                   ng-model="editor.shape.basicShape.color" colorpicker>
        </div>
    </div>
    <div class="row" ng-show="editor.shape.basicShape.shape == 'rectangle'">
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("X left [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.xMin">
        </div>
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("Y front [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.yMin">
        </div>
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("X right [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.xMax">
        </div>
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("Y rear [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.yMax">
        </div>
    </div>
    <div class="row" ng-show="editor.shape.basicShape.shape == 'circle'">
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("X center [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.x">
        </div>
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("Y center [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.y">
        </div>
        <div class="col-sm-3 col-xs-6 form-group">
            <label><?php _("Radius [mm]") ?></label>
            <input class="form-control" type="number" step="any" placeholder=""
                   ng-model="editor.shape.basicShape.radius">
        </div>
    </div>
</div>
<div class="tab-pane active fade" id="advanced">
    <h4><?php _("Webcam support") ?></h4>

    <div class="row">
        <div class="col-lg-6 col-xs-6 form-group">
            <label><?php _("Webcam connection") ?></label>
            <select class="form-control" ng-model="editor.webcam.method">
                <option value="0"><?php _("No webcam") ?></option>
                <option value="1"><?php _("JPG Images") ?></option>
                <option value="2"><?php _("MJPG Stream") ?></option>
                <option value="3"><?php _("JPG + MJPG Stream") ?></option>
            </select>
        </div>
        <div class="col-lg-6 col-xs-6 form-group">
            <label><?php _("Time lapse video") ?></label>
            <select class="form-control" ng-model="editor.webcam.timelapseMethod">
                <option value="0"><?php _("Disabled, don't offer it") ?></option>
                <option value="1"><?php _("Every x seconds") ?></option>
                <option value="2"><?php _("Every level") ?></option>
            </select>
        </div>
        <div class="col-lg-6 col-xs-6 form-group">
            <label><?php _("Reload interval [s]") ?></label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.webcam.reloadInterval" data-toggle="popover" data-placement="bottom"
                   data-content="<?php _("Time between image reload if JPG images is selected.") ?>" data-original-title
                   data-trigger="focus">
        </div>
        <div class="col-lg-6 col-xs-6 form-group">
            <label><?php _("Time lapse interval [s]") ?></label>
            <input class="form-control" type="number" step="any" min="1" placeholder=""
                   ng-model="editor.webcam.timelapseInterval" data-toggle="popover" data-placement="bottom"
                   data-content="<?php _("Time between 2 images saved for timelapse.") ?>" data-original-title
                   data-trigger="focus">
        </div>
    </div>
    <div class="row">
        <div class="col-lg-12 col-xs-12 form-group">
            <label><?php _("Dynamic image URL") ?></label>
            <input class="form-control" type="text" placeholder="http://"
                   ng-model="editor.webcam.dynamicUrl" data-toggle="popover" data-placement="bottom"
                   data-content="<?php _("Source for dynamic MJPG images.") ?>" data-original-title
                   data-trigger="focus">
        </div>
        <div class="col-lg-12 col-xs-12 form-group">
            <label><?php _("Static image URL") ?></label>
            <input class="form-control" type="text" placeholder="http://"
                   ng-model="editor.webcam.staticUrl" data-toggle="popover" data-placement="bottom"
                   data-content="<?php _("Source for static JPG images.") ?>" data-original-title
                   data-trigger="focus">
        </div>
        <div class="col-xs-12">
            <p><?php _("Time lapse videos require an url which delivers a jpg image. You need to enable time lapse for a print, if you want a video.") ?></p>
        </div>
    </div>
</div>
</div>
</form>
</div>
</div>
</div>