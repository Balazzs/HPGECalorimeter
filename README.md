# HPGECalorimeter
Project for Modelling Laboratory at ELTE

TODO TOC

## Installation on Windows

### Windows Subsystem for Linux (WSL)

I suggest using WSL for a Windows installation, as the native windows Geant4 doesn't support multithread simulations.
WSL is a real Linux system running under windows (now with WSL 2.0 with GUI included), all the advantages of having Linux and Windows without the hassle of dual booting.

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

Note:
You might have to enable Hyper-V in your BIOS settings (if you don't know how to do it google it).


---


description coming soon...

----

build and run info coming soon...

----

some parts of the analysis notebook might be outdated, there is already an issue for updating it

