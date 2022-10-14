function Component()
{
  Component.prototype.createOperations = function()
  {
    // call default implementation
    component.createOperations();

    // ... add custom operations

    var kernel = systemInfo.kernelType;

    if( kernel == "winnt" ) {
      component.addElevatedOperation("CreateShortcut", "@TargetDir@/bin/OpenStudioApp.exe", "@StartMenuDir@/OpenStudio.lnk");
      // Note JM: you have to quote the %1 which represents the file path, otherwise any space in the path will think there are multiple args
      component.addElevatedOperation("RegisterFileType", "osm", "@TargetDir@/bin/OpenStudioApp.exe \"%1\"", "OpenStudio Model File", "text/plain", "@TargetDir@/bin/OpenStudioApp.exe,1");

     // var exePath = installer.value("TargetDir") + "\\bin\\install_utility.exe";
     // component.addElevatedOperation("Execute", exePath, "Install", "UNDOEXECUTE", exePath, "Remove" );

    }
  }
}



