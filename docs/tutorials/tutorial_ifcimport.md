<h1>Import IFC</h1>
This tutorial explains how to import an IFC file into OpenStudio using BIMserver.  This functionality is available in OpenStudio 1.7.1 and higher.  This functionality was developed by Chong Zhou as part of the [BIMDataHub](https://github.com/BIMDataHub) project at the [Consortium for Building Energy Innovation](http://www.cbei.psu.edu/).

## Install BIMserver

### Stand-alone version

Most users will want to install a stand-alone version of BIMserver on their local computer.

- Install the Java JDK. Java 1.8 is not well supported yet, Java JDK 1.7 can be [downloaded here](http://www.oracle.com/technetwork/java/javase/downloads/jdk7-downloads-1880260.html), for Windows you will want the file named jdk-7u75-windows-x64.exe.

- Download [BIMserver 1.3.4](https://github.com/opensourceBIM/BIMserver/releases/download/1.3.4-FINAL-2014-10-17/bimserver-1.3.4-FINAL-2014-10-17.jar) from the BIMserver Github site.

- Assuming this is your first installation, create a new folder in your system, e.g. C:\BIMserver. Copy the downloaded bimserver-[version].jar file into the above folder. Note: avoid the creating BIMserver subfolder under C:\Program Files or C:\Program Files (x86).

- Double-click on the bimserver-[version].jar file to execute it. Enter the path to your JVM install, e.g. C:\Program Files\Java\jdk1.7.0_75.  Press the start button to start the BIMserver. Wait for BIMserver to expand all the files and configure itself and until the phrase "Server started successfully" appears. Click the Stop button to stop the BIMserver.

![BIMserver install](img/ifcimport/ifcimport1.jpg)

- Download the [latest OSMSerializer](https://github.com/BIMDataHub/OsmSerializer/releases) and copy the OSMSerializer.jar file into the plugins subfolder under the bimserver-[version] folder.

- Click the Start button to restart BIMserver. Once the BIMserver has restarted, click Launch Browser.

- Setup the BIMserver for the first time by clicking the "the admin page" in the browser.

![BIMserver setup](img/ifcimport/ifcimport2.jpg)

- Enter the Administrator username (e.g. 'Administrator'), email (e.g. 'admin@bimserver.org'), and password (e.g. 'admin') then click Setup.

![BIMserver setup](img/ifcimport/ifcimport3.jpg)

- The BIMserver is now correctly setup and running. You can close the browser and continue to use OpenStudio to import IFC file.  BIMserver needs to be running during the IFC import, you can stop BIMserver when you are done by pressing the Stop button.

### Server Version

If you want to host BIMserver on a public website you should install the server version.

- Download the [.war version](https://github.com/opensourceBIM/BIMserver/releases/download/1.3.4-FINAL-2014-10-17/bimserver-1.3.4-FINAL-2014-10-17.war) of the BIMserver and host it with TOMCAT on a unix server. Please refer to the [BIMserver site](https://github.com/opensourceBIM/BIMserver/wiki) on Github for more assistance.

## Using the Import IFC Utility in OpenStudio

### Launch the Import IFC Utility.
- Click File -> Import -> IFC.

![Launch the Import IFC Utility](img/ifcimport/ifcimport4.jpg)

- You will need to specify the BIMserver settings information the first time running the Import IFC Utility in OpenStudio. Please enter the BIMserver Address (e.g. http://127.0.0.1), Port (e.g. 8082), Username (e.g 'admin@bimserver.org'), and Password (e.g. 'admin') for the BIMserver and click Okay.

![BIMserver settings](img/ifcimport/ifcimport5.jpg)

- The left part of the Import IFC Utility lists all BIM projects hosted on BIMserver. The right part displays a list of IFC revisions of a project.

![Import Project](img/ifcimport/ifcimport6.jpg)

### Create a new Project
- Create a new project by clicking the New Project Button and specify the name for the project e.g. "Test".

![Import Project](img/ifcimport/ifcimport7.jpg)

- You will see the newly created project on the left. It contains Project ID on the BIMserver and the Project Name separated by colon.

![Import Project](img/ifcimport/ifcimport8.jpg)

- Click the ">" button to view the current IFC files hosted on BIMserver. It will not display anything for now since we have not yet checked in the IFC file.

### Check in IFC File
- After selecting a project and clicked ">" button, click the Check in IFC File button and select an IFC file to upload to BIMserver.

![Import Project](img/ifcimport/ifcimport9.jpg)

- Now you can click the ">" button again to refresh the IFC revision list. Select the newly added file. It displays the IFC Revision ID and the time when the IFC file is uploaded, separated by colon.

### Download OSM File
- Click the Download OSM File button to start importing this IFC file into OpenStudio. It will ask you to save your current OpenStudio model and restart the application with the newly imported model.

### Change the BIMserver Settings
- If you want to change the BIMserver Settings, please simply click the Setting button and type in the new settings. It will automatically access the new BIMserver after you click OK.

## Scope of our current IFC importation Utility

We provide an IFC to OSM transformation utility, using BIMserver as a middle-ware. Our goal is to assist energy simulation modelers to import the building geometry from IFC. The current effort requires energy simulation modelers to create the building geometry from scratch. Our current utility can partially automate this process by importing important building elements into OpenStudio from IFC. We will try to fully automate this process in the future.

### Currently supported IFC elements
- Spaces will be transformed as OSM Spaces.
- Straight and non-curved Walls will be transformed into OSM Surface. Outside boundary conditions are preserved for walls.
- Ceilings and Floors will be transformed into OSM Surface. Outside boundary conditions are not preserved.
- Doors and Windows will be transformed into OSM SubSurface. The relationship to their corresponding Walls will be preserved. Outside boundary conditions are not preserved.

### IFC elements under development
- Sloped Wall and Roofs (Currently, the sloped walls cannot be transformed correctly. We are solving this problem now but follow the steps in 3.3 to modify your model).
- Outside boundary conditions of ceilings, floors, doors, and windows.
- Curtain Walls.

### What to do if you have some elements being transformed incorrectly or not transformed
- Save the imported OSM model in OpenStudio by clicking File -> Save As in the menu.
- Use SketchUp with the OpenStudio plug-in to open the new OSM file.
- Modify the building geometry in SketchUp. For more information, please refer to [this tutorial](http://nrel.github.io/OpenStudio-user-documentation/getting_started/getting_started/#installation-instructions).


