> [Wiki](Home) ▸ **Configuring OpenStudio Build Environments**

**NOTE: this build configuration is specific for work on the **develop3** branch.  Unless you are involved in development of OpenStudio 3, you will want to continue using this document [[Configuring OpenStudio Build Environments]] to build *develop*.**

Build instructions are changing for OpenStudio 3. For instructions on building the 1.14.X Long Term Support branch please see [[(v1.14.X-LTS)-Configuring-OpenStudio-Build-Environments]]. For instructions on building previous 2.X branches please see [[Configuring OpenStudio Build Environments]].  The new build system is much easier to get started with.  To build OpenStudio, most developers need only follow the [OpenStudio Core](#openstudio-core) instructions, this process will download all necessary dependencies.  Only developers needing to build OpenStudio dependencies need worry about the advanced [OpenStudio Dependencies](#openstudio-dependencies) instructions.

## Contents
**Recommended**: [Building OpenStudio Core](#openstudio-core)
- [Windows](#windows)
- [Linux](#linux)
- [Mac](#mac)

**Advanced**: [Building OpenStudio Dependencies](#openstudio-dependencies)
- [Build Qt](#build-qt)

<h1> OpenStudio (Core) </h1>

This is the fastest, most reliable method of getting a working OpenStudio build.  These instructions assume that you have successfully [cloned the OpenStudio repository](Using-OpenStudio-with-Git-and-GitHub#cloning-the-repository-to-your-local-computer) already. For these instructions we will refer to the root of the OpenStudio checkout as `.`.

*Note that you will want to select `.` as the source directory in CMake, unlike in previous versions of OpenStudio where it was `./openstudiocore`.  Suggested build directory names are `.\build` or `.\core-build`, these names are ignored by git.*

## Dependencies

### Conan

We are now using [conan](https://conan.io/) to manage build dependencies. To install conan you need to install python (install version 3 or greater) and `pip install conan` or `sudo pip install conan`, and make sure that your python bin dir is in your path. After that the rest of it should be automatic.

#### Windows

Install [Visual Studio 2017](https://docs.microsoft.com/en-us/visualstudio/releasenotes/vs2017-relnotes) Professional or Community (free for open source development) - or better yet install [Visual Studio 2019](https://docs.microsoft.com/en-us/visualstudio/releases/2019/release-notes) but make sure to install also 2017 build tools -, [CMake 3.10 or greater](https://cmake.org/download/), and [msysGit](http://msysgit.github.io/).  

*NEED SCREENSHOT:* 
show install options, "Desktop Development with C++"

*NEEDS UPDATE:*
Look at the [Suggested Visual Studio 2013 Configuration](https://github.com/NREL/OpenStudio/wiki/Suggested-Visual-Studio-2013-Configuration) to configure Visual Studio. Set the `PreferredToolArchitecture` environment variable to `x64`, this will use 64 bit compiler tools and performs much better (you may still create 32 bit builds with the 64 bit compiler tools). _Note: Visual Studio 2013 Express does not have the 64-bit compiler tools and will not work._

Suggest these instead of our own:
https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools-19123

https://marketplace.visualstudio.com/items?itemName=ArkadyShapkin.CDebuggerVisualizersforVS2017

Clone the OpenStudio repository, launch the CMake GUI, and select the source and build directories.  Configure build options and press generate.  Certain configuration options may require installation of additional dependencies, these are documented below.  After generating, open OpenStudio.sln in the build directory with Visual Studio and choose Build Solution.  

#### Linux

Ubuntu Bionic 18.04 is the primary version of Linux supported.  TO install build dependencies:

*TODO:* This can be skimmed down now that we rely on conan. gdbm readline qdbm etc.

```shell
sudo apt install build-essential git cmake-curses-gui cmake-gui libssl-dev libxt-dev \
    libncurses5-dev libgl1-mesa-dev autoconf libexpat1-dev libpng-dev libfreetype6-dev \
    libdbus-glib-1-dev libglib2.0-dev libfontconfig1-dev libxi-dev libxrender-dev \
    libgeographic-dev libicu-dev chrpath bison libffi-dev libgdbm-dev libqdbm-dev \
    libreadline-dev libyaml-dev libharfbuzz-dev libgmp-dev patchelf python-pip
```
    
#### Ubuntu 16.04
On Ubuntu Xenial 16.04, add a PPA that supports GCC-7 and configure to use GCC 7

```shell
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install gcc-7 g++-7
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-5 60 --slave /usr/bin/g++ g++ /usr/bin/g++-5
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 40 --slave /usr/bin/g++ g++ /usr/bin/g++-7
sudo update-alternatives --config gcc
g++ -v
# => 7.3.0
```

Update HarfBuzz

```shell
sudo apt remove libharfbuzz-dev
sudo apt-add-repository ppa:jonathonf/texlive
sudo apt update
sudo apt install libharfbuzz-dev
```

Install other dependencies

    sudo apt install libnssn3 libxtst6

----

Install CMake

```shell
CMAKE_VERSION=3.15.0 # Update with latest by checking https://cmake.org/download/
wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/cmake-$CMAKE_VERSION.tar.gz
tar xfz cmake-$CMAKE_VERSION.tar.gz
cd cmake-$CMAKE_VERSION
./configure
make -j$(nproc)
sudo make install
```
    
Not required but to install Ruby 2.5:

```shell
sudo apt-add-repository ppa:brightbox/ruby-ng
sudo apt-get update
sudo apt-get install ruby2.5 ruby2.5-dev
```
#### Mac
Install Xcode and [CMake](https://cmake.org/download/).

Clone the OpenStudio repository, launch the CMake GUI, and select the source and build directories.  Configure build options and press generate.  Certain configuration options may require installation of additional dependencies, these are documented below. After generating, change directories into the build directory and run `make -j$(nproc)`.

## Clone, configure, and build

Clone the OpenStudio repository, create a build directory, and run ccmake:

```shell
git clone git@github.com:NREL/OpenStudio.git
cd OpenStudio
mkdir build
cd build
ccmake ..
```

Configure build options and generate.  Certain configuration options may require installation of additional dependencies, these are documented below.  After generating, change directories into the build directory and run `make`.  

### Building C# Bindings (Windows only)
*NEEDS UPDATE:*
Ensure that you have the [.NET Framework 3.5 SP1](http://www.microsoft.com/en-us/download/details.aspx?id=22) installed

### Building PAT
*NEEDS UPDATE:*
#### Windows
Install [Node](https://nodejs.org/dist/v6.9.2/win-x64/node.exe)

#### Linux
Install [Node](https://nodejs.org/dist/v6.9.2/node-v6.9.2.pkg)


### Building Documentation
*NEEDS UPDATE:*
#### Windows
Install [Doxygen v1.8.8](http://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.8-setup.exe)

(The searchengine feature in >=1.8.9 is broken)

Install [Graphviz v2.38](http://www.graphviz.org/pub/graphviz/stable/windows/graphviz-2.38.msi)

#### Mac
Download [Doxygen v1.8.8](http://ftp.stack.nl/pub/users/dimitri/Doxygen-1.8.8.dmg) and drag it to Applications

(The searchengine feature in >=1.8.9 is broken)

Install [Graphviz v2.36.0](http://www.graphviz.org/pub/graphviz/stable/macos/mountainlion/graphviz-2.36.0.pkg)


### Building Installer Packages
*NEEDS UPDATE:*
#### Windows
Install [QtIFW](https://download.qt.io/official_releases/qt-installer-framework/3.1.1/QtInstallerFramework-win-x86.exe)

#### Linux
Install [QtIFW](https://download.qt.io/official_releases/qt-installer-framework/3.1.1/QtInstallerFramework-linux-x64.run)

#### Mac
Install [QtIFW](https://download.qt.io/official_releases/qt-installer-framework/3.1.1/QtInstallerFramework-mac-x64.dmg)

---

<h1>OpenStudio Dependencies</h1>

Most developers do not have to worry about building OpenStudio dependencies, the correct dependencies will be downloaded as needed when building the [OpenStudio Core](#openstudio-core) project.  Only developers needing to update the version of OpenStudio dependencies or provide dependency builds for new platforms need to read this section.  After new OpenStudio dependencies are built they are uploaded to Amazon S3 and the `./CMakeLists.txt` file is updated to reference the new packages.

*Note that you will want to select `./meta` as the source directory.  Suggested build directory names are `build` or `super-build`, these names are ignored by git.*

Clone the repository, launch the CMake GUI, and select the source and build directories.  Configure build options and press generate.  Certain configuration options may require installation of additional dependencies, these are documented below.  After generating, open OpenStudioMeta.sln in the build directory with Visual Studio and choose Build Solution.  

### Build OpenStudio Gems

* Update the gems referenced in `\dependencies\ruby\Gemfile`.  
* Build the openstudio-gems target in the OpenStudioMeta.sln.
* Copy the openstudio-gems-YYYYMMDD.tar.gz file to S3:openstudio-resources/dependencies/. 
* Find the MD5 Hash (OSX: `md5 openstudio-gems-YYYYMMDD.tar.gz`, Windows: `md5sum openstudio-gems-YYYYMMDD.tar.gz`)
* Update filename and MD5 hash in openstudiocore/CMakeLists.txt

### Build Qt
Configure whether to build static or shared Qt binaries using the Qt5Static variable.  Build the Qt target in the OpenStudioMeta.sln. Package the Qt directory.

#### Ubuntu 18.04
    sudo apt install libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev libicu-dev libdrm-dev libxcomposite-dev libxcursor-dev libxi-dev libxrandr-dev libxtst-dev libdbus-1-dev libfontconfig-dev libpro libnss3-dev libatspi2.0-dev python bison flex gperf ninja-build