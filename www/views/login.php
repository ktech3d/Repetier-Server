<div class="container">
    <div class="row">
        <div class="col-12 col-md-6 col-md-offset-3">
    <form class="" method="post" action="" id="form-signin">
        <h2 class="form-signin-heading">Please sign in</h2>

        <div class="control-group">
            <label class="control-label" for="login"><?php _("Login") ?>:</label>
            <div class="controls">
                <input name="login" id="login" ng-model="login" type="text" class="form-control" placeholder="<?php _("Login") ?>">
            </div>
        </div>
        <div class="control-group">
            <label class="control-label" for="password"><?php _("Password") ?>:</label>
            <div class="controls">
                <input name="password" id="password" ng-model="password" type="password" class="form-control" placeholder="<?php _("Password") ?>">
            </div>
        </div>
        <button class="btn btn-large btn-primary btn-block margin-top" ng-click="sendLogin()"><?php _("Sign in") ?></button>
    </form>
        </div>
    </div>
</div>
