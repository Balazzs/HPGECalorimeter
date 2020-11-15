TODO TOC

# Description
My MSc Thesis at Eötvös Lóránd University (started as a project for Modelling Laboratory)

# Building
## Geant4 installation

To run the simulation you need the Geant4 framework. If you do not have it, then here is a short installation guide. (Although please refer to the official guides for detailed steps and more up-to-date information)

### Windows Subsystem for Linux (WSL)

I suggest using WSL for a Windows installation, as the native windows Geant4 doesn't support multithread simulations.
WSL is a real Linux system running under windows (now with WSL 2.0 with GUI included, although you will need an X server and further configurations, I will only install one without GUI), all the advantages of having Linux and Windows without the hassle of dual booting.

Microsoft tutorial: https://docs.microsoft.com/en-us/windows/wsl/install-win10 

Here I just list what is already better written in the previously mentioned tutorial, please use that as this section will not be updated and might be outdated by the time you read it.

Open a powershell in administrator mode and paste the following (then press enter):

`dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart`

then

`dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart`

Restart your computer

Download and run the Linux kernel update package:

https://wslstorestorage.blob.core.windows.net/wslblob/wsl_update_x64.msi

Set WSL 2 as default WSL in powershell:

`wsl --set-default-version 2`

Go to Microsoft Store and install a linux distribution (I installed Ubuntu)

First time you start your subsystem you will be asked for a username and password.

> Note:
> You might have to enable virtualization / Hyper-V in your BIOS settings (if you don't know how to do it google it)
>
> If you want to use graphical interfaces too you will need an X server for you windows and a window manager for your Linux subsystem, there are some tutorials about this around the internet

You now have a working a Linux installation inside your Windows system, follow the linux guide from now on (inside Ubuntu console).

### Installation on Linux

Just stick to the official Geant4 getting started guide: https://geant4.web.cern.ch/support/getting_started

#### Download the source

Download the source code (I used the latest release link from the github repo releases, **this will get outdated**, download it manually, or update the link):

`wget https://github.com/Geant4/geant4/archive/v10.6.3.tar.gz`

`tar -xvf v10.6.3.tar.gz`

#### Get build tools and prerequisites
Get the necessary tools for the build:

`sudo apt-get update`

`sudo apt-get -y install g++ cmake`

Get prerequisites (if you want to use system-wide):

`sudo apt-get install -y libexpat1-dev zlib1g-dev libclhep-dev libxerces-c-dev`

If you wish to use the graphical interface **- NOT TESTED YET (i did this a long time ago and forgot)**:

`sudo apt-get -y install qt5-default`

#### Build

Create a folder for your build next to your unpacked source folder.

`mkdir geant4.10.06-build`

`cd geant4.10.06-build`

- Set an installation prefix (where it should be installed, I used the default /usr/local)
- Set install data to ON (installs datasets)
- Specify the source directory absolute path
- Add Multithreading
- Add GDML support (requires Xerces)
- Use Qt (only if you want GUI with QT)
- If you set the prerequisites up then use the system versions
- Anything other you want...
`cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DGEANT4_INSTALL_DATA=ON -DGEANT4_BUILD_MULTITHREADED=ON -DGEANT4_USE_GDML=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_SYSTEM_CLHEP=ON -DGEANT4_USE_SYSTEM_EXPAT=ON -DGEANT4_USE_SYSTEM_ZLIB=ON /path/to/geant4-source`

In my case:

`cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DGEANT4_INSTALL_DATA=ON -DGEANT4_BUILD_MULTITHREADED=ON -DGEANT4_USE_GDML=ON -DGEANT4_USE_SYSTEM_EXPAT=ON -DGEANT4_USE_SYSTEM_ZLIB=ON  /home/balazzs/geant4-10.6.3`

Then build (16 is the number of threads, I have an 16 thread CPU, so I used 16):

`make -j16`

And install (sudo only needed if you installed into a directory not owned by your user (like /usr/local)):

`sudo make install`

## Building the code


# Running simulations

# Analysis notebook

some parts of the analysis notebook might be outdated, there is already an issue for updating it

