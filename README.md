# PE-To-Multiboot-tool
This too converts PE files in windows into a kernel could be booted from grub.
It's for the ones who prefer to use visual studio in windows to do OSDev tutorials.
# How To Use
1- Install WSL (Windows Subsystem for Linux), choose any system (e.g : Ubuntu).<br>
2- Install grub2 in your linux subsystem.<br>
3- Create the following directory structure
iso directory structure:
```
iso/
    boot/
        grub/
            grub.cfg
        kernel.bin
```
grub.cfg:
```
menuentry "OS" {
              multiboot /boot/OSKernel.exe
              boot
}
```
5- Create your kernel using Visual Studio and make sure you add these option to the linker:<br>
```/FILEALIGN:4096``` to make file alignment to 0x1000<br>
```/BASE:"0x100000"``` to make ImageBase to be 0x100000<br>
6- Add the following command to Post-Build events in Visual Studio (Properties -> Build Events -> Post Build Events -> Command Line)<br>
```%Multiboot.exe% $(OutDir)$(TargetFileName)```<br>
Replace %Multiboot.exe% by Multiboot.exe full path.<br>
This command runs the tool with output file of the kernel each build to add the grub multiboot header to it and call mkgrub-rescue to create the bootable iso.<br>
7- Build the Kernel and Now you have bootable ISO could be tested on VMware.(you can specify the iso output path in Multiboot.cpp line 79)
