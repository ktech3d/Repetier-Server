<div class="container">
    <ul class="breadcrumb">
        <li><a href="#/"><?php _("Dashboard") ?></a></li>
        <li class="active"><?php _("User Database") ?></li>
    </ul>

    <div class="row" ng-show="userlist">
        <div class="col-xs-12" ng-hide="userlist.loginRequired">
            <div
                class="alert alert-danger"><?php _("Currently your user database is empty. That means everybody with a network access has full access to all functions. If you have an open network you should consider adding a user database.") ?></div>
        </div>
        <div class="col-xs-12">
            <table ng-show="userlist.loginRequired" class="table table table-bordered table-hover">
                <tr>
                    <th><?php _("Login") ?></th>
                    <th><?php _("Permissions") ?></th>
                    <th><?php _("Action") ?></th>
                </tr>
                <tr ng-repeat="u in userlist.users">
                    <td>{{u.login}}</td>
                    <td>{{u.permissions | permissions}}</td>
                    <td>
                        <button ng-click="openEditUser(u)"><i class="icon-edit"></i> <?php _("Edit") ?></button>
                    </td>
                </tr>
            </table>
        </div>
        <div class="col-xs-12">
            <button class="btn btn-primary" ng-click="createUser()"><i class="icon-plus"></i> <?php _("Create User") ?>
            </button>
        </div>
    </div>
</div>

<div id="dialogCreateUser" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <h4 class="modal-title"><?php _("Create new User") ?></h4>
            </div>
            <div class="modal-body">
                <div ng-show="edituser.error" class="alert alert-danger">{{edituser.error}}</div>
                <div class="form-group">
                    <label><?php _("Login") ?></label>
                    <input type="text" class="form-control" ng-model="edituser.login" placeholder="<?php _("Login") ?>">
                </div>
                <div class="form-group">
                    <label><?php _("Password") ?></label>
                    <input type="text" class="form-control" ng-model="edituser.password"
                           placeholder="<?php _("Password") ?>">
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.print" ng-true-value="1" ng-false-value="0">
                        <?php _("User can print") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.addfiles" ng-true-value="1" ng-false-value="0">
                        <?php _("User can add new files") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.deletefiles" ng-true-value="1" ng-false-value="0">
                        <?php _("User can delete files") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.configure" ng-true-value="1" ng-false-value="0">
                        <?php _("User can configure the server") ?>
                    </label>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default"
                        ng-click="sendCreateUser()"><?php _("Create User") ?></button>
                <button type="button" class="btn btn-primary" data-dismiss="modal"><?php _("Cancel") ?></button>
            </div>
        </div>
    </div>
</div>

<div id="dialogEditUser" class="modal fade">
    <div class="modal-dialog">
        <div class="modal-content">
            <div class="modal-header">
                <h4 class="modal-title"><?php _("Edit User") ?></h4>
            </div>
            <div class="modal-body">
                <div ng-show="edituser.error" class="alert alert-danger">{{edituser.error}}</div>
                <div class="form-group">
                    <label><?php _("Login") ?></label>
                    <input type="text" class="form-control" ng-model="edituser.login" readonly>
                </div>
                <div class="form-group">
                    <label><?php _("Password") ?></label>
                    <input type="text" class="form-control" ng-model="edituser.password"
                           placeholder="<?php _("Password") ?>">
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.print" ng-true-value="1" ng-false-value="0">
                        <?php _("User can print") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.addfiles" ng-true-value="1" ng-false-value="0">
                        <?php _("User can add new files") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.deletefiles" ng-true-value="1" ng-false-value="0">
                        <?php _("User can delete files") ?>
                    </label>
                </div>
                <div class="checkbox">
                    <label>
                        <input type="checkbox" ng-model="edituser.configure" ng-true-value="1" ng-false-value="0">
                        <?php _("User can configure the server") ?>
                    </label>
                </div>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn btn-default"
                        ng-click="sendEditUser()"><?php _("Apply changes") ?></button>
                <button type="button" class="btn btn-danger"
                        ng-click="deleteUser(edituser.login)"><?php _("Delete User") ?></button>
                <button type="button" class="btn btn-primary" data-dismiss="modal"><?php _("Cancel") ?></button>
            </div>
        </div>
    </div>
</div>