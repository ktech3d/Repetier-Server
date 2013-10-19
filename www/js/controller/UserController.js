routeModule.factory('User', [ function () {
    var permissions = 15; // No user system active - allow anything
    var login = '', password = '', session = '';
    return {
        canPrint: function () {
            return (permissions & 1) == 1;
        },
        canAddFiles: function () {
            return (permissions & 2) == 2;
        },
        canDeleteFiles: function () {
            return (permissions & 4) == 8;
        },
        canChangeConfig: function () {
            return (permissions & 8) == 8;
        },
        setCredentials: function (_login, _pw) {
            login = _login;
            password = _pw;
        },
        logout: function () {
            permissions = 0;
            login = "";
            password = "";
        },
        getSession: function () {
            return session;
        },
        setSession: function (_session) {
            session = _session;
        },
        setPermissions: function(perm) { permissions = perm;}
    }
}]);

function UserLoginController($scope,$rootScope,WS,User,$location) {
    User.logout();
    $scope.login = "";
    $scope.password = "";
    $scope.loginerror = false;

    $scope.sendLogin = function() {
        WS.send("login",{login:$scope.login,password:CryptoJS.MD5(User.getSession()+CryptoJS.MD5($scope.login+$scope.password).toString()).toString()}).then(function(data) {
            if(data.error)
                $scope.loginerror = data.error;
            else {
                User.setPermissions(data.permissions);
                User.setCredentials(data.login,CryptoJS.MD5($scope.login+$scope.password).toString());
                if(typeof($rootScope.pathAfterLogin)=="undefined" || $rootScope.pathAfterLogin=="/login")
                    $rootScope.pathAfterLogin = "/";
                $location.path($rootScope.pathAfterLogin);
            }
        });
    }
}
function UserConfigController($scope, $rootScope, WS,$location) {
    if(!$rootScope.user.canChangeConfig()) {
        $location.path('/');
        return;
    }

    $scope.userlist = false;
    $scope.edituser = {login: '', print: "1", addfiles: "1", deletefiles: "1", configure: "1", password: ''};
    var updateUserlist = function () {
        WS.send("userlist", {}).then(function (data) {
            $scope.userlist = data;
        });
    }
    updateUserlist();

    $scope.createUser = function () {
        $scope.edituser = {login: '', print: "1", addfiles: "1", deletefiles: "1", configure: "1", password: ''}
        $('#dialogCreateUser').modal('show');
    }
    $scope.deleteUser = function (user) {
        $scope.confirm('<?php _("Delete User") ?>', '<?php _("Really delete this user?") ?>', '<?php _("Yes, delete user") ?>', '<?php _("No") ?>').then(function () {
            WS.send("deleteUser", {login: user}).then(function (data) {
                updateUserlist();
                $('#dialogEditUser').modal('hide');
            });
        });
    }
    $scope.sendCreateUser = function () {
        var perm = 0;
        if ($scope.edituser.print == "1") perm += 1;
        if ($scope.edituser.addfiles == "1") perm += 2;
        if ($scope.edituser.deletefiles == "1") perm += 4;
        if ($scope.edituser.configure == "1") perm += 8;
        WS.send("createUser", {login: $scope.edituser.login, password: CryptoJS.MD5($scope.edituser.login + $scope.edituser.password).toString(),
            permissions: perm}).then(function (data) {
                updateUserlist();
                $('#dialogCreateUser').modal('hide');
            });
    }
    $scope.sendEditUser = function() {
        var perm = 0;
        if ($scope.edituser.print == "1") perm += 1;
        if ($scope.edituser.addfiles == "1") perm += 2;
        if ($scope.edituser.deletefiles == "1") perm += 4;
        if ($scope.edituser.configure == "1") perm += 8;
        WS.send("updateUser", {login: $scope.edituser.login, password: ($scope.edituser.password.length == 0 ? '' : CryptoJS.MD5($scope.edituser.login + $scope.edituser.password).toString()),
            permissions: perm}).then(function (data) {
                updateUserlist();
                $('#dialogEditUser').modal('hide');
            });
    }
    $scope.openEditUser = function(u) {
        $scope.edituser = {login: u.login, print: (u.permissions & 1 ? "1" : "0"), addfiles: (u.permissions & 2 ?"1":"0"),
            deletefiles: (u.permissions & 4 ?"1":"0"), configure: (u.permissions & 8 ?"1":"0"), password: ''};
        $('#dialogEditUser').modal('show');
    }
}