<div class="row">
    <div class="small-12 columns">
        <h1>Configure {{editor.general.name}}</h1>
        <form class="custom">
        <div class="section-container auto" data-section>
            <section>
                <p class="title" data-section-title><a href="#general">General</a></p>

                <div class="content" data-section-content>
                    <div class="row">
                        <div class="large-12 columns">
                            <label>Printer Name</label>
                            <input type="text" placeholder="Displayed printer name" ng-model="editor.general.name">
                        </div>
                        <div class="large-4 columns">
                            <label>Printer Variant</label>
                            <select class="medium" ng-model="editor.general.printerVariant">
                                <option value="delta">Delta Printer</option>
                                <option value="cartesian">Cartesian Printer</option>
                            </select>
                        </div>
                        <div class="large-4 columns"></div>
                     </div>
                    <div class="row">
                        <div class="large-4 small-6 columns">
                            <label>Fan Installed {{edit.general.fan}}</label>
                            <div boolswitch class="small" on="Yes" off="No" value="editor.general.fan"></div>
                        </div>
                        <div class="large-4 small-6 columns">
                            <label>SD Card reader Installed</label>
                            <div boolswitch class="small" on="Yes" off="No" value="editor.general.sdcard"></div>
                        </div>
                        <div class="large-4 small-6 columns">
                            <label>Software Power</label>
                            <div boolswitch class="small" on="Yes" off="No" value="editor.general.softwarePower"></div>
                        </div>
                    </div>
                    {{editor | json}}
                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#extruder">Extruder</a></p>

                <div class="content" data-section-content>
                    <p>Content of section 2.</p>
                </div>
            </section>
            <section>
                <p class="title" data-section-title><a href="#shape">Shape</a></p>

                <div class="content" data-section-content>
                    <p>Content of section 2.</p>
                </div>
            </section>
        </div>
        </form>
    </div>
</div>