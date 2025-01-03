function Controller() {
  installer.autoRejectMessageBoxes();

  // silent install is not an option until QtIFW v3.0.1
  gui.setSilent(true);

  installer.setMessageBoxAutomaticAnswer("OverwriteTargetDirectory",
                                         QMessageBox.Yes);

  installer.installationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });
  // Uninstaller
  installer.uninstallationFinished.connect(function() {
    gui.clickButton(buttons.NextButton);
  });
}

function logCurrentPage() {
    var pageName = gui.currentPageWidget().objectName;
    var pagePrettyTitle = gui.currentPageWidget().title;
    console.log("At page: " + pageName + " ('" + pagePrettyTitle + "')");
}

Controller.prototype.IntroductionPageCallback = function() {
  console.log("---- INTRODUCTION PAGE");
  logCurrentPage();
  gui.clickButton(buttons.NextButton);
};

// Unused
//Controller.prototype.WelcomePageCallback = function() {
  //console.log("---- WELCOME PAGE");
  //logCurrentPage();
  //// click delay because the next button is initially disabled for ~1s
  //gui.clickButton(buttons.NextButton, 3000);
//};

//Controller.prototype.CredentialsPageCallback = function() {
  //console.log("---- CREDENTIAL PAGE");
  //logCurrentPage();
  //gui.clickButton(buttons.NextButton);
//};

Controller.prototype.LicenseAgreementPageCallback = function() {
  console.log("---- LICENSE PAGE");
  logCurrentPage();
  // click delay because the next button is initially disabled for ~1s
  gui.clickButton(buttons.NextButton, 3000);
};



Controller.prototype.TargetDirectoryPageCallback = function()
{
  console.log("---- TARGET DIRECTORY PAGE");
  logCurrentPage();

  console.log("User-suplied TargetDir: " + installer.value("TargetDir"));

  // gui.currentPageWidget().TargetDirectoryLineEdit.setText(installer.value("harcoded/path/EnergyPlus");
  console.log("Target dir: " +
              gui.currentPageWidget().TargetDirectoryLineEdit.text);

  gui.clickButton(buttons.NextButton);
};

// Unused
Controller.prototype.ComponentSelectionPageCallback = function() {
  console.log("---- COMPONENT SELECTION PAGE");
  logCurrentPage();

  //var widget = gui.currentPageWidget();

  //var components = installer.components();
  //console.log("There are " + components.length + " available components.");
  //console.log("Components selected for installation:");
  //for (var i = 0; i < components.length; i++) {
    //var compName = components[i].name;
    //var installStatus = "Yes";

    //// Get command line args passed
    //var thisCompFlag = installer.value(compName);

    //if (thisCompFlag === "false") {
      //// Protect against trying to deselect something required
      //if (["Unspecified", "Licenses"].indexOf(compName) >= 0) {
        //console.log("-- Component '" + compName +
                    //"' is mandatory and cannot be unselected");
        //installStatus = "Yes (FORCED AS REQUIRED)";
      //} else if (compName === "CopyAndRegisterSystemDLLs") {
        //console.log("-- Component 'CopyAndRegisterSystemDLLs' is highly recommended on Windows, and it will not overwrite any existing DLLs so it should not have side effects");
        //// Allow unselect anyways
        //installStatus = "No (use at your own risk)";
        //widget.deselectComponent(compName);
      //} else if (compName === "Libraries") {
        //console.log("-- Component 'Libraries' is highly recommended on Windows, it will include msvc runtime, and also Windows Universal CRT libraries (needed for shipping to Windows 7/8).");
        //// Allow unselect anyways
        //installStatus = "No (use at your own risk)";
        //widget.deselectComponent(compName);
      //} else {
        //installStatus = "No";
        //widget.deselectComponent(compName);
      //}
    //}
    //console.log("* " + compName + ": " + installStatus);
  //}

  gui.clickButton(buttons.NextButton);
};

Controller.prototype.LicenseAgreementPageCallback = function() {
  console.log("---- LICENSE AGREEMENT PAGE");
  logCurrentPage();
  gui.currentPageWidget().AcceptLicenseCheckBox.setChecked(true);
  gui.clickButton(buttons.NextButton);
};

Controller.prototype.StartMenuDirectoryPageCallback = function() {
  console.log("---- START MENU DIRECTORY PAGE");
  logCurrentPage();

  // You won't get in this callback if it wasn't already Windows, but let's be explicit & safe
  if (systemInfo.kernelType == "winnt") {
    // TODO: extra logging for debug for now
    console.log("installer StartMenuDir: " + installer.value("StartMenuDir"));
    console.log("Text: " + gui.currentPageWidget().StartMenuPathLineEdit.text);
    console.log("AllUsersStartMenuProgramsPath: " + installer.value("AllUsersStartMenuProgramsPath"));
    console.log("UserStartMenuProgramsPath: " + installer.value("UserStartMenuProgramsPath"));
    if (installer.value("UseAllUsersStartMenu") === "true") {
      console.log("Will use the **All** Users Start Menu at: " + installer.value("AllUsersStartMenuProgramsPath"));
    } else {
      console.log("Will use this Users' Start Menu at: " + installer.value("UserStartMenuProgramsPath"));
    }
  }
  gui.clickButton(buttons.NextButton);
};

Controller.prototype.ReadyForInstallationPageCallback = function()
{
  console.log("---- READY FOR INSTALLATION PAGE");
  logCurrentPage();
  gui.clickButton(buttons.CommitButton);
};

Controller.prototype.PerformInstallationPageCallback = function()
{
  console.log("---- PERFORM INSTALLATION PAGE");
  logCurrentPage();
  gui.clickButton(buttons.CommitButton);
};

Controller.prototype.FinishedPageCallback = function() {
  console.log("---- FINISHED PAGE");
  logCurrentPage();
  gui.currentPageWidget().RunItCheckBox.setChecked(false);

  gui.clickButton(buttons.FinishButton);
};
