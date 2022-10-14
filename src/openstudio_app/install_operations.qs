function Component()
{
  Component.prototype.createOperations = function()
  {
    // call default implementation
    component.createOperations();

    // ... add custom operations

    var kernel = systemInfo.kernelType;
    if( kernel == "darwin" ) {

      // This is equivalent to mkdir -p, will make any directory in between, no
      // override if it exists already. You DO need the trailing slash, as it
      // takes sections up to net trailing slash to create directories...
      component.addElevatedOperation("Mkdir", "@TargetDir@/OpenStudioApp.app/Contents/EnergyPlus/");

      // Copies the content of ./EnergyPlus/* into /OpenStudioApp.app/Contents/EnergyPlus/*
      // Be VERY mindful of the trailing slashes... This behaves very weirdly
      // Source doesn't not include trailing, so it's the directory itself
      // Target does include trailing, so it's within that directory
      component.addElevatedOperation("CopyDirectory", "@TargetDir@/EnergyPlus", "@TargetDir@/OpenStudioApp.app/Contents/");

      // Same ./Radiance/* to OpenStudioApp.app/Contents/Radiance/*
      component.addElevatedOperation("Mkdir", "@TargetDir@/OpenStudioApp.app/Contents/Radiance/");
      component.addElevatedOperation("CopyDirectory", "@TargetDir@/Radiance",   "@TargetDir@/OpenStudioApp.app/Contents/");

      // an equivalent is
      // component.addOperation("Mkdir", "@TargetDir@/OpenStudioApp.app/Contents/EnergyPlus")
      // component.addOperation("CopyDirectory", "@TargetDir@/EnergyPlus/", "@TargetDir@/OpenStudioApp.app/Contents/EnergyPlus/")

      // TODO: remove, done via install(CODE instead, so it works with the TGZ)
      //var linktarget = "@TargetDir@/OpenStudioApp.app/Contents/Frameworks/QtWebEngineCore.framework/Versions/A/Helpers/QtWebEngineProcess.app/Contents/Frameworks";
      //var linksource = "../../../../../../../Frameworks";
      //component.addElevatedOperation("Execute", "ln", "-s", linksource, linktarget);

      //var exePath = installer.value("TargetDir") + "/bin/install_utility";
      //component.addElevatedOperation("Execute", exePath, "Install", "UNDOEXECUTE", exePath, "Remove" );

    }

    if( kernel == "winnt" ) {
      component.addElevatedOperation("CreateShortcut", "@TargetDir@/bin/OpenStudioApp.exe", "@StartMenuDir@/OpenStudio.lnk");
      // Note JM: you have to quote the %1 which represents the file path, otherwise any space in the path will think there are multiple args
      component.addElevatedOperation("RegisterFileType", "osm", "@TargetDir@/bin/OpenStudioApp.exe \"%1\"", "OpenStudio Model File", "text/plain", "@TargetDir@/bin/OpenStudioApp.exe,1");

     // var exePath = installer.value("TargetDir") + "\\bin\\install_utility.exe";
     // component.addElevatedOperation("Execute", exePath, "Install", "UNDOEXECUTE", exePath, "Remove" );

    }
  }
}



