# PE-To-Multiboot-tool
This too converts PE files in windows into a kernel could be booted from grub.
It's for the ones who prefer to use visual studio in windows to do OSDev tutorials.
# How To Use
1- Install Qemu.<br>
2- Create your kernel using Visual Studio (C/C++)and make sure you added these option to the linker:<br>
```/FILEALIGN:4096``` to make file alignment to 0x1000.<br>
```/BASE:"0x100000"``` to make ImageBase to be 0x100000.<br>
3- Add the following command to Post-Build events in Visual Studio (Properties -> Build Events -> Post Build Events -> Command Line)<br>
```%Multiboot.exe% $(OutDir)$(TargetFileName)```<br>
Replace %Multiboot.exe% by Multiboot.exe full path.<br>
This command runs the Multiboot tool with output file of the kernel each build to add the grub multiboot header to it and execute Qemu with your modified kernel.<br>
4- Build the Kernel in Visual Studio and Qemu will start your kernel.
