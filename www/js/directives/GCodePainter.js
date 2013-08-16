function GCodePainter(elem) {
    var lastX = 0;
    var lastY = 0;
    var lastZ = 0;
    var lastPrintZ = 0;

    var element = $('#' + elem);
    var stage = new Kinetic.Stage({
        container: elem,
        width: element.width(),
        height: element.height()
    });
    var that = this;
    var resizer = function () {
        console.log("new width "+element.width());
        that.updateShape();
    };
    $(window).resize(resizer);
    this.$destroy = function() {
        $(window).off("resize",resizer);
        stage.destroy();
        stage = null;
    }
    var printer = null;
    var base = new Kinetic.Layer();
    var marker = new Kinetic.Layer();
    var codeOld = new Kinetic.Layer();
    var code = new Kinetic.Layer();
    var top = new Kinetic.Layer();
    var lastLayerG = new Kinetic.Group();
    var currentLayerG = new Kinetic.Group();
    var lastLayer = [];
    var currentLayer = [];

    var scale = 2; // Koordinate shift
    var offsetX = 0;
    var offsetY = 0;

    stage.add(base);
    stage.add(marker);
    stage.add(codeOld);
    stage.add(code);
    stage.add(top);

    codeOld.add(lastLayerG);
    code.add(currentLayerG);

    this.adjustScaling = function () {
        w = stage.getWidth() - 2;
        h = stage.getHeight() - 2;
        dimx = printer.movement.xMax - printer.movement.xMin;
        dimy = printer.movement.yMax - printer.movement.yMin;
        aspectP = dimx / dimy;
        aspectS = w / h;
        if (aspectS > aspectP) { // Stage height
            scale = h / dimy;
            offsetY = 1 + h + printer.movement.yMin * scale;
            offsetX = 1 + (w - dimx * scale) * 0.5 - printer.movement.xMin * scale;
        } else {
            scale = w / dimx;
            offsetX = 1 - printer.movement.xMin * scale;
            offsetY = 1 + h - (h - dimy * scale) * 0.5 + printer.movement.yMin * scale;
        }
    }
    var convCoord = function (x, y) {
        return {x: offsetX + scale * x, y: offsetY - y * scale};
    }
    this.disableCursor = function() {
        top.remove();
    }
    var smallLine = function (layer, color, x1, y1, x2, y2) {
        line = new Kinetic.Line({
            points: [x1, y1, x2, y2],
            fill: lc,
            //fillEnabled: false,
            strokeWidth: 1,
            stroke: color
        });
        layer.add(line);
    }
    var printLine = function (layer, color, x1, y1, x2, y2) {
        line = new Kinetic.Line({
            points: [x1, y1, x2, y2],
            fill: lc,
            //fillEnabled: false,
            strokeWidth: 2,
            stroke: color
        });
        layer.add(line);
    }
    var filledCircle = function (layer, color, x, y, radius) {
        radius = Math.abs(radius);
        circ = new Kinetic.Circle({
            radius: radius,
            fill: color,
            x: x,
            y: y,
            strokeEnabled: false
        });
        layer.add(circ);
        return circ;
    }
    var addShape = function (layer, shape, marker) {
        if (shape.shape == 'rectangle') {
            p1 = convCoord(Math.min(shape.xMin, shape.xMax), Math.min(shape.yMin, shape.yMax));
            p2 = convCoord(Math.max(shape.xMax, shape.xMin), Math.max(shape.yMax, shape.yMin));
            if (marker) {
                var rect = new Kinetic.Rect({
                    x: p1.x,
                    y: p1.y,
                    width: p2.x - p1.x,
                    height: p2.y - p1.y,
                    fill: shape.color,
                    strokeEnabled: false
                });
                layer.add(rect);
            } else {
                lc = printer.shape.gridColor;
                var rect = new Kinetic.Rect({
                    x: p1.x,
                    y: p1.y,
                    width: p2.x - p1.x,
                    height: p2.y - p1.y,
                    fill: shape.color,
                    stroke: lc,
                    strokeWidth: 1
                });
                spacing = printer.shape.gridSpacing;
                off = 0;
                zero = convCoord(0, 0);
                m = Math.max(Math.abs(shape.xMin), Math.abs(shape.xMax), Math.abs(shape.yMin), Math.abs(shape.yMax))
                layer.add(rect);
                if (parseFloat(spacing) > 1) {
                    if (0 > shape.yMin && 0 < shape.yMax)
                        smallLine(layer, lc, p1.x, zero.y, p2.x, zero.y);
                    if (0 > shape.xMin && 0 < shape.xMax)
                        smallLine(layer, lc, zero.x, p1.y, zero.x, p2.y);
                    while (off + spacing < m) {
                        off += spacing;
                        soff = off * scale;
                        if (off > shape.yMin && off < shape.yMax)
                            smallLine(layer, lc, p1.x, zero.y - soff, p2.x, zero.y - soff);
                        if (off > shape.xMin && off < shape.xMax)
                            smallLine(layer, lc, zero.x + soff, p1.y, zero.x + soff, p2.y);
                        if (-off > shape.yMin && off < shape.yMax)
                            smallLine(layer, lc, p1.x, zero.y + soff, p2.x, zero.y + soff);
                        if (-off > shape.xMin && off < shape.xMax)
                            smallLine(layer, lc, zero.x - soff, p1.y, zero.x - soff, p2.y);
                    }
                    filledCircle(layer, lc, zero.x, zero.y, 0.25 * spacing * scale);
                }
            }
        } else if (shape.shape == 'circle') {
            p = convCoord(shape.x,shape.y);
            r = shape.radius*scale;
            if (marker) {
                var rect = new Kinetic.Circle({
                    x: p.x,
                    y: p.y,
                    radius:r,
                    fill: shape.color,
                    strokeEnabled: false
                });
                layer.add(rect);
            } else {
                lc = printer.shape.gridColor;
                var rect = new Kinetic.Circle({
                    x: p.x,
                    y: p.y,
                    radius:r,
                    fill: shape.color,
                    stroke: lc,
                    strokeWidth: 1
                });
                spacing = printer.shape.gridSpacing;
                off = 0;
                zero = convCoord(0, 0);
                m = Math.max(Math.abs(shape.x-shape.radius), Math.abs(shape.x+shape.radius), Math.abs(shape.y-shape.radius), Math.abs(shape.y+shape.radius))
                layer.add(rect);
                if (parseFloat(spacing) > 1) {
                    if (0 > shape.yMin && 0 < shape.yMax)
                        smallLine(layer, lc, p1.x, zero.y, p2.x, zero.y);
                    if (0 > shape.xMin && 0 < shape.xMax)
                        smallLine(layer, lc, zero.x, p1.y, zero.x, p2.y);
                    while (off < m) {
                        soff = off * scale;
                        if (off > shape.yMin && off < shape.yMax) {
                            alpha = Math.asin((zero.y-soff-p.y)/r);
                            d = r*Math.cos(alpha);
                            smallLine(layer, lc, p.x-d, zero.y - soff, p.x+d, zero.y - soff);
                        }
                        if (off > shape.xMin && off < shape.xMax) {
                            alpha = Math.asin((zero.x+soff-p.x)/r);
                            d = r*Math.cos(alpha);
                            smallLine(layer, lc, zero.x + soff, p.y-d, zero.x + soff, p.y+d);
                        }
                        if (-off > shape.yMin && off < shape.yMax) {
                            alpha = Math.asin((zero.y+soff-p.y)/r);
                            d = r*Math.cos(alpha);
                            smallLine(layer, lc, p.x-d, zero.y + soff, p.x+d, zero.y + soff);
                        }
                        if (-off > shape.xMin && off < shape.xMax) {
                            alpha = Math.asin((zero.x-soff-p.y)/r);
                            d = r*Math.cos(alpha);
                            smallLine(layer, lc, zero.x - soff, p.y-d, zero.x - soff, p.y+d);
                        }
                        off += spacing;
                    }
                    filledCircle(layer, lc, zero.x, zero.y, 0.25 * spacing * scale);
                }
            }

        }
    }
    this.connectPrinter = function (config) {
        if (config && config.shape) {
            printer = config;
            this.updateShape();
        }
    }

    this.updateShape = function () {
        if(!printer) return;
        stage.setWidth(element.width());
        stage.setHeight(element.height());
        this.adjustScaling();
        base.removeChildren();
        addShape(base, printer.shape.basicShape, false);
        if(undefined != printer.shape.marker) {
            angular.forEach(printer.shape.marker,function(s) {
               addShape(base,s,true);
            });
        }
        repositionCursor();
        stage.draw();
    }

    var newLayer = function() {
        lastLayerG.removeChildren();
        lastLayerG.remove();
        lastLayerG.destroy();
        lastLayerG = currentLayerG;
        //codeOld.add(lastLayerG);
        //lastLayerG.draw();
        currentLayerG = new Kinetic.Group();
        code.add(currentLayerG);
        lastLayer = currentLayer;
        currentLayer = [];
    }
    var repositionCursor = function() {
        if(!cursor) return;
        cp = convCoord(lastX,lastY);
        cursor.setX(cp.x);
        cursor.setY(cp.y);
    }
    var cursor = filledCircle(top,"#ff0000",0,0,4);
    var codeChanged = false;
    this.addMove = function(move) {
        if(move.de>0.000001 && move.z>lastPrintZ) {
            newLayer();
        }
        old = convCoord(lastX,lastY);
        next = convCoord(move.x,move.y);
        if(move.de>0.000001) {
            lastPrintZ = move.z;
            currentLayer.push({p:true,x1:lastX,y1:lastY,x2:move.x,x2:move.y});
            printLine(currentLayerG,"#000080",old.x,old.y,next.x,next.y);
        } else {
            currentLayer.push({p:false,x1:lastX,y1:lastY,x2:move.x,x2:move.y});
            smallLine(currentLayerG,"#A0A0A0",old.x,old.y,next.x,next.y);
        }
        codeChanged = true;
        lastX = move.x;
        lastY = move.y;
        lastZ = move.z;
        cursor.setX(next.x);
        cursor.setY(next.y);

        //code.draw();
        top.draw();
        //stage.draw();
    }
    var updateDrawing = function() {
        if(stage == null) return;
        if(codeChanged) {
            codeChanged = false;
            code.draw();
        }
        setTimeout(updateDrawing,500);
    }
    setTimeout(updateDrawing,500);
}
