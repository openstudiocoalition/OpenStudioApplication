<h1>Troubleshooting</h1>
This page is a collection of troubleshooting advice that helps users deal with common issues.
_________________

## OpenStudio SketchUp Plug-in Does Not Load
Ensure that your version of SketchUp is compatible with your version of OpenStudio as listed in [this matrix](https://github.com/NREL/OpenStudio/wiki/OpenStudio-Version-Compatibility-Matrix).  Try opening the "Window->Preferences->Extensions" window in SketchUp, enable the OpenStudio SketchUp Plug-in if it is listed.  If the OpenStudio SketchUp Plug-in is not listed you may have to manually install it (this is the case if you are using a different version of SketchUp than the OpenStudio installer targets).

To install the OpenStudio SketchUp Plug-in manually on Windows please copy the files in:

    C:\openstudio-%VERSION%\SketchUpPlugin\plugin\

to:

    C:\Users\%YOURUSERNAME%\AppData\Roaming\SketchUp\SketchUp %VERSION%\SketchUp\Plugins\

where %YOURUSERNAME% is replaced by your user name and %VERSION% is the version of the software you are using.

To install the OpenStudio SketchUp Plug-in manually on macOS please copy the files in:

    /Applications/OpenStudio-%VERSION%/SketchUpPlugin/plugin/

to:

    ~/Library/Application Support/SketchUp %VERSION%/SketchUp/Plugins/

where %VERSION% is the version of the software you are using.
_________________

## Model Will Not Open in OpenStudio SketchUp Plug-in
If you have an OSM file that will not open, or opens incorrectly in the SketchUp Plug-in then running the OSM diagnostic user script can help identify the problem objects and create a new diagnostic copy of your file. This script creates a report identifying problem objects, and saves a new copy of your file leaving the original untouched. To run the script, go to the "Extensions->OpenStudio User Scripts->Reports" menu and choose "OSM Diagnostic Scrip
_________________

## SketchUp Crashes on Launch
The first thing to do is to remove SketchUp and install it again. To do this, run the SketchUp uninstaller. Also check that all OpenStudio plug-ins are removed from the SketchUp plugin directories. On Windows these directories are:

- C:\Users\%YOURUSERNAME%\AppData\Roaming\SketchUp\SketchUp %VERSION%\SketchUp\Plugins
- C:\ProgramData\SketchUp\SketchUp %VERSION%\SketchUp\Plugins
- C:\Program Files\SketchUp\SketchUp %VERSION%\SketchUp\Plugins

On Mac these are:

- ~/Library/Application Support/SketchUp %VERSION%/SketchUp/Plugins/
- /Library/Application Support/SketchUp %VERSION%/SketchUp/Plugins

If SketchUp crashes without the OpenStudio SketchUp Plug-in installed then something else is wrong, contact the SketchUp support team for help. Check that your system meets the [SketchUp Hardware and Software Requirements](https://help.sketchup.com/en/article/36208). If SketchUp launches, try installing OpenStudio again. If SketchUp crashes on launch again, remove the OpenStudio files from the SketchUp plugin directories again. Open SketchUp, go to "Window->Preferences->Extensions" and disable the OpenStudio SketchUp Plug-in. Re-install OpenStudio and relaunch SketchUp. Open the Ruby console by going to "Window->Ruby Console" and then go back to "Window->Preferences->Extensions" and enable the OpenStudio SketchUp Plug-in, you may see useful output in the Ruby console. If you have other extensions installed, try disabling them and enabling the OpenStudio SketchUp Plug-in.

If none of the steps above work and you are on Windows, it is likely that there is a dynamic library being loaded from another application that is interfering with OpenStudio. If you just want to fix your problem, try copying the files libeay32.dll and ssleay32.dll from `C:\openstudio-%VERSION%\bin` to `C:\Program Files \SketchUp\SketchUp %VERSION%`, this will fix the problem 80% of the time.

If you want to investigate in more depth, download [Dependency Walker](http://www.dependencywalker.com/). Extract the files and launch depends.exe. Navigate to "File->Open" and then browse to choose the SketchUp.exe under `C:\Program Files \SketchUp\SketchUp %VERSION%`. This will examine all of the libraries loaded by SketchUp but not the libraries loaded by OpenStudio. Navigate to "Profile->Start Profiling" then press Ok in the dialog that pops up. This will launch SketchUp and attempt to load OpenStudio. Scroll down in the list of loaded libraries and look for the OpenStudio dlls. Expand the paths under the OpenStudio dlls and look for libraries that are being loaded from other locations. Often, some other program will install a different version of one of the libraries (usually libeay32.dll or ssleay32.dll) that OpenStudio uses. This other library will be in the path ahead of OpenStudio and will be loaded instead, this results in a hard crash of SketchUp. Correcting your path or copying the dlls from the OpenStudio bin directory into the SketchUp install directory will cause the correct dlls to be loaded when SketchUp starts.
_________________

## OpenStudio Application and SketchUp Plug-in Crash
Several users have reported an OpenStudio Application and OpenStudio SketchUp Plug-in crash occurring when saving or opening a file is related to the Dell Backup and Recovery software.  It appears that this program is [known to cause issues](http://en.community.dell.com/support-forums/software-os/f/3526/t/19634253) with Qt based applications (such as OpenStudio).
_________________

## Graphics Problems in OpenStudio Application
Several users have reported graphics issues with the Geometry and Results tabs within the OpenStudio Application.  In many cases, adjusting power saving settings of the graphics cards has resolved these issues.  For computers with NVIDA graphics cards, you can access these settings via the NVIDIA Control Panel under "3D Settings->Manage 3D Settings".  In this panel, make sure that "Preferred graphics processor" is set to "High-performance NVIDIA processor".

![Adjust NVIDIA Settings](img/help/nvidia_settings.png)

Additionally, you may be able to find more information by running the OpenStudio Application from a command prompt and capturing the standard output in a file:

```
set OPENSTUDIO_APPLICATION_DEBUG=1
C:\openstudio-%VERSION%\bin\OpenStudioApp.exe > out.txt 2>&1
```
_________________

## Orphan Objects and Unused Resources
At times some objects in OpenStudio that should have a parent, such as surfaces and internal load instances, may become detached from their parent. Possibly the parent is deleted but for some reason the child is not. These often are not visible in the GUI, and while it is possible to fix remove these orphans with a text editor it isn't ideal. To provide a better workflow for this we created a measure [Remove Orphan Objects and Unused Resources](https://bcl.nrel.gov/node/82267) that can be run either using Apply Measure Now or at run time. If you see an EnergyPlus error about an object you don't think you have in your model, you may try running this to clean up the model. Additionally this measure also provides checkboxes that allow you to remove unused resources. While unused resources can be cleaned up in the GUI, the measure provides a robust and quick way to do this. Over time we will increase the functionality of the orphan cleanup as well as purging of unused resources.

Below is a screenshot of what the measure looks like in the apply measure now window. By default the remove unused objects boxes are un-checked. You will always get the remove orphan functionality.

![Remove Orphan Objects and Unused Resources - Arguments](img/help/orphan_purge_argview.png)

Below is a screenshot of the log after running the measure. The log will list each section being checked, so as the functionality is expanded you will know what object types are being checked for for orphan.

![Remove Orphan Objects and Unused Resources - Output](img/help/orphan_purge_outputview.png)
_________________

## Unable to Communicate with Amazon Cloud from Command Prompt Using the "bundle" Command
This issue is typically a communication error between you and RubyGems.org. A few things to check:

1. Are you behind a proxy? If so, then check if the proxy is applied system wide or just for your web browsers. It is typically easiest to open up your favorite browser and check the settings.
2. Are there any website restrictions at your location? If so, then it is possible that your IT department is blocking access. Note that RubyGems is only the first hurdle, you will also need access to AWS (meaning Amazon).

If you think that the above is happening, then it is best to talk to your IT department and explain the situation. The general explanation for the "situation" is that you are trying to run a large number of simulations using Amazon Web Services. Some good questions may be:

1. How can I access the internet through a command line (or terminal) using our company's proxy?
2. When I ping aws.amazon.com, it times out; is there something that may be restricting access to the site?
3. Can we set the proxy to be machine wide, instead of just on the web browsers?

_________________

## Energy Modeling Forum
Post your questions to the forum below for general and OpenStudio specific energy modeling information.

<a class="btn btn-primary" role="button" href="https://unmethours.com/questions/scope:all/sort:activity-desc/tags:openstudio/">Unmet Hours</a>
_________________

## Submit a Bug
Please let us know if you find a bug in the OpenStudio software.

<a class="btn btn-primary" role="button" href="https://github.com/NREL/OpenStudio/blob/develop/CONTRIBUTING.md">Submit a Bug</a>

_________________

## Useful SketchUp Links
- [SketchUp's On-Line Help](http://help.sketchup.com/en)
- [Forum for SketchUp](https://productforums.google.com/forum/?hl=en#!categories/sketchup/sketchup)
- [Google's user forum for SketchUp](https://productforums.google.com/forum/?hl=en#!categories/sketchup/sketchup)
- [SketchUcation's SketchUp forum](http://sketchucation.com/forums/)
