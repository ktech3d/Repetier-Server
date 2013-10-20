GCodePreviewDirectives = angular.module('Preview', []);

GCodePreviewDirectives.directive('gcodepreview', function($http,$timeout,$rootScope){
    return {
        scope: {data:'@data',slug:'@'},
        restrict: 'AE',
        replace: false,
        transclude: true,
        //template:'<table style="width:100%;"><tr><td style="vertical-align:top;width:40px;"><button class="btn btn-block"><i class="icon-zoom-in"></i></button>'+
         //   '<button class="btn btn-block"><i class="icon-zoom-in"></i></button></td><td><div class="gcp" style="background:blue"></div></td></tr></table>',
        link: function(scope, elem, attrs, ctrls){
            console.log("linking preview");
            var root = $(elem);
            var qe = $(elem);
            qe.prepend('<button class="btn btn-square positioned zoomin" style="left:10px;top:10px;"><i class="icon-zoom-in"></i></button>');
            qe.prepend('<button class="btn btn-square positioned zoomout" style="left:55px;top:10px;"><i class="icon-zoom-out"></i></button>');
            qe.prepend('<button class="btn btn-square positioned up" style="left:33px;top:55px;"><i class="icon-arrow-up"></i></button>');
            qe.prepend('<button class="btn btn-square positioned left" style="left:10px;top:100px;"><i class="icon-arrow-left"></i></button>');
            qe.prepend('<button class="btn btn-square positioned right" style="left:55px;top:100px;"><i class="icon-arrow-right"></i></button>');
            qe.prepend('<button class="btn btn-square positioned down" style="left:33px;top:145px;"><i class="icon-arrow-down"></i></button>');
            var destroyed = false;
            var diameter =1;
            var camera,scene = new THREE.Scene(),renderer = new THREE.WebGLRenderer();
            var bbox = new THREE.Box3();
            var phi=Math.PI/2,theta=0; // Camera angels
            var cameraDistance = 200;
            var isUserInteracting = false;
            var animate = false;
            var animateMode = 0;
            var oldwidth = qe.width();
            renderer.setSize( qe.width(), qe.height());
            renderer.shadowMapEnabled = true;
            renderer.shadowMapSoft = true;
            camera = new THREE.PerspectiveCamera( 35, qe.width() / qe.height(), 0.1, 1000 );
            camera.up.set(0,0,1);
            qe.append( renderer.domElement );
            var canvas = $(renderer.domElement);
            var updateView = function() {

            }
            var resizer = function () {
                canvas.width(qe.width());
                canvas.height(qe.height());
                camera.aspect =  qe.width()/ qe.height();
                camera.updateProjectionMatrix();

                renderer.setSize(  qe.width(), qe.height() );
                render();
            };
            $(window).resize(resizer);
            var materialPrint = new THREE.LineBasicMaterial( { color: 0xff0000 } );
            var materialTravel = new THREE.LineBasicMaterial( { color: 0x00ff00 } );

            function stopAnimate() {
                animate = false;
            }
            function startAnimate(mode) {
                console.log("startAnimate "+mode);
                animateMode = mode;
                animate = true;
                render();
            }
            $(".btn",qe).mouseout(stopAnimate);
            $(".btn",qe).mouseup(stopAnimate);
            $(".zoomin",qe).mousedown(function() {startAnimate(1)});
            $(".zoomout",qe).mousedown(function() {startAnimate(2)});
            $(".up",qe).mousedown(function() {startAnimate(5)});
            $(".down",qe).mousedown(function() {startAnimate(6)});
            $(".left",qe).mousedown(function() {startAnimate(3)});
            $(".right",qe).mousedown(function() {startAnimate(4)});
            function render() {
                if(animate) {
                    requestAnimationFrame(render);
                    switch (animateMode) {
                        case 1: // zoom in
                            camera.fov -= 0.2;
                            if(camera.fov<0.1) camera.fov = 0.1;
                            camera.updateProjectionMatrix();
                            break;
                        case 2: // zoom out
                            camera.fov += 0.2;
                            if(camera.fov>60) camera.fov = 60;
                            camera.updateProjectionMatrix();
                            break;
                        case 3: // rotate theta+
                            theta+=0.01;
                            if(theta>Math.PI) theta-=2*Math.PI;
                            break;
                        case 4:
                            theta-=0.01;
                            if(theta<-Math.PI) theta+=2*Math.PI;
                            break;
                        case 5:
                            phi-=0.01;
                            break;
                        case 6:
                            phi+=0.01;
                            break;
                    }
                }
                updateCamera();
                renderer.render(scene, camera);
            }
            function updateCamera() {
                viewCenter = bbox.center();
                if(phi<0.01) phi=0.01;
                if(phi>Math.PI-0.01) phi = Math.PI-0.01;

                camera.position.x = viewCenter.x + (cameraDistance * Math.cos(theta) * Math.sin(phi));
                camera.position.y = viewCenter.y + (cameraDistance * Math.sin(theta) * Math.sin(phi));
                camera.position.z = viewCenter.z + (cameraDistance * Math.cos(phi));
                camera.lookAt( viewCenter );
                //console.log("camera ");console.log(viewCenter);console.log(camera);
            }
            function recomputeScene(code) {
                bbox.makeEmpty();
                console.log("render scene");
                console.log($rootScope.printerConfig[scope.slug]);
                bs = $rootScope.printerConfig[scope.slug].shape.basicShape;
                ls = new THREE.Geometry();
                lst = '';
                scene = new THREE.Scene();
                scene.add( new THREE.AmbientLight( 0x404040 ) );
                bedMaterial = new THREE.MeshPhongMaterial( {ambient: 0x555555,color:0xff8080, specular: 0x111111, shininess: 200 /*parseInt("0x"+bs.color.substr(1))*/} );
                if(bs.shape=="rectangle") {
                    geom = new THREE.ConvexGeometry([new THREE.Vector3(bs.xMin,bs.yMin,0),new THREE.Vector3(bs.xMax,bs.yMin,0),
                        new THREE.Vector3(bs.xMax,bs.yMax,0),new THREE.Vector3(bs.xMin,bs.yMax,0)]);
                    bedMesh = new THREE.Mesh( geom, bedMaterial);
                } else {

                }
                bedMesh.receiveShadow = true;
                scene.add(bedMesh );

                var geometry = new THREE.CubeGeometry(40,30,35);
                var material = new THREE.MeshPhongMaterial( { color: 0x00ff00 } );
                var cube = new THREE.Mesh( geometry, material );
                cube.position.set( 40, 80, 20 );
                cube.castShadow = true;
                scene.add( cube );
                var lse = new THREE.Geometry();;
                var lsm = new THREE.Geometry();;
                var lastvec = null;
                console.log("parse lines");
                angular.forEach(code,function(val) {
                    if(val.p=='E')
                        bbox.addPoint(val);
                    vec = new THREE.Vector3(val.x,val.y,val.z);
                    if(lastvec!=null) { // switch move type
                        if(val.p=='E') {
                            lse.vertices.push(lastvec);
                            lse.vertices.push(vec);
                        } else {
                            lsm.vertices.push(lastvec);
                            lsm.vertices.push(vec);
                        }
                    }
                    lastvec = vec;
                });
                console.log("create lines");
                lines = new THREE.Line(lse, materialTravel,THREE.LinePieces);
                lines.castShadow = true;
                scene.add(lines);
                lines = new THREE.Line(lsm, materialPrint,THREE.LinePieces);
                //lines.castShadow = true;
                scene.add(lines);
                console.log("create rest");
                dx = bbox.max.x-bbox.min.x;
                dy = bbox.max.y-bbox.min.y;
                dz = bbox.max.z-bbox.min.z;
                diameter = Math.sqrt(dx*dx+dy*dy+dz*dz);
                cameraDistance = 2*diameter;
                scene.fog = new THREE.Fog( 0x72645b, diameter, 4*diameter );
                var d = diameter;
                mx = Math.max(bbox.max.x,Math.abs(bbox.min.x));
                my = Math.max(bbox.max.y,Math.abs(bbox.min.y));
                var md = Math.sqrt(mx*mx+my*my);
                light = new THREE.DirectionalLight( 0xffffff );
                light.position.set( 0.2*2*d, 0.5*2*d, 1*2*d );
                light.castShadow = true;
                light.shadowCameraVisible = true;
                light.shadowDarkness = 0.7;
                scene.add( light );
                light.shadowCameraLeft = -md;
                light.shadowCameraRight = md;
                light.shadowCameraTop = md;
                light.shadowCameraBottom = -md;

                light.shadowCameraNear = 1;
                light.shadowCameraFar = 4*d;

                light.shadowMapWidth = 1024;
                light.shadowMapHeight = 1024;

                light.shadowBias = -0.005;
                hemiLight = new THREE.HemisphereLight( 0xffffff, 0xffffff, 0.6 );
                hemiLight.color.setHSL( 0.6, 1, 0.6 );
                hemiLight.groundColor.setHSL( 0.095, 1, 0.75 );
                hemiLight.position.set( 0, 500, 0 );
                scene.add( hemiLight );

                if(ls.length>0) {
                    scene.add(new THREE.Line(ls,lst=='M' ? materialTravel : materialPrint));
                }
                resizer();
                render();
                $rootScope.$broadcast('loaded');
            }
            /*scope.$watch('data',function(newval) {
                console.log("data changed "+newval);
                $http.get("/printer/parsedgcode/"+slug+"?id="+scope.data).success(function(data) {
                    console.log(data);
                    recomputeScene(data);
                });
            });*/
            attrs.$observe('data', function(value) {
                if(scope.data == 0) return;
                console.log("calling load");
                $rootScope.$broadcast('load');
                $http.get("/printer/parsedgcode/"+scope.slug+"?id="+scope.data).success(function(data) {
                    recomputeScene(data);
                });
            });
            scope.$destroy(function() {
                destroyed = true;
                $(window).off("resize",resizer);
            });
            var onPointerDownPointerX,onPointerDownPointerY;
            var thetaStart,phiStart;
            function onDocumentMouseDown( event ) {
                event.preventDefault();
                isUserInteracting = true;
                thetaStart = theta;
                phiStart = phi;
                onPointerDownPointerX = event.clientX;
                onPointerDownPointerY = event.clientY;
            }

            function onDocumentMouseMove( event ) {
                if ( isUserInteracting ) {
                    theta = ( onPointerDownPointerX - event.clientX ) * 0.0174 + thetaStart;
                    phi = -( event.clientY - onPointerDownPointerY ) * 0.0174 + phiStart;
                    render();
                }
            }

            function onDocumentMouseUp( event ) {
                console.log("mup");
                isUserInteracting = false;
                render();
            }

            function onDocumentMouseWheel( event ) {
                camera.fov -= event.wheelDeltaY * 0.05;
                if(camera.fov<0.1) camera.fov = 0.1;
                if(camera.fov>60) camera.fov = 60;
                camera.updateProjectionMatrix();
                render();
            }

            function onDocumentTouchStart( event ) {

                if ( event.touches.length == 1 ) {
                    event.preventDefault();
                    onPointerDownPointerX = event.touches[ 0 ].pageX;
                    onPointerDownPointerY = event.touches[ 0 ].pageY;
                    thetaStart = theta;
                    phiStart = phi;
                }
            }

            function onDocumentTouchMove( event ) {
                if ( event.touches.length == 1 ) {
                    event.preventDefault();
                    theta = ( onPointerDownPointerX - event.touches[0].pageX ) * 0.0174 + thetaStart;
                    phi = -( event.touches[0].pageY - onPointerDownPointerY ) * 0.0174 + phiStart;
                    render();
                }
            }
            function checkResize() {
                nw = qe.width();
                if(oldwidth!=nw) {
                    resizer();
                    oldwidth = nw;
                }
                if(!destroyed)
                    $timeout(checkResize,500,false);
            }
            checkResize();
            qe.mousedown(onDocumentMouseDown);
            qe.mousemove(onDocumentMouseMove);
            qe.mouseup(onDocumentMouseUp);

            function wheel(event)
            {
                event.preventDefault();
                event.returnValue=false;
            }
            qe.mouseout(function() {
                isUserInteracting = false;
                document.removeEventListener( 'mousewheel', onDocumentMouseWheel, false );
                window.removeEventListener('DOMMouseScroll',wheel,false);
            });
            qe.mouseenter(function() {
                document.addEventListener( 'mousewheel', onDocumentMouseWheel, false );
                window.addEventListener('DOMMouseScroll',wheel,false);
            });
            qe.bind( 'touchstart', onDocumentTouchStart, false );
            qe.bind( 'touchmove', onDocumentTouchMove, false );
        }
    };
});