// Multiboot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <string>
#include <iostream>
#include <Windows.h>
using namespace std;
struct MULTIBOOT_HEADER {
	UINT32 magic;
	UINT32 flags;
	UINT32 checksum;
	UINT32 header_addr;
	UINT32 load_addr;
	UINT32 load_end_addr;
	UINT32 bss_end_addr;
	UINT32 entry_addr;
};
std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return str;
}
#define LOADBASE 0x00100000 // default load base of grub
int main(int argc, char** argv)
{	

	//Open PE file.
	printf("Adding MultiBoot header to : %s.\n", argv[1]);
	HANDLE hFile = CreateFileA(argv[1], GENERIC_WRITE | GENERIC_READ, NULL, NULL, OPEN_EXISTING, NULL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		printf("File not found.\n");
		return TRUE;
	}
	DWORD filesize = GetFileSize(hFile, 0);
	DWORD retsize;
	char* file = (char*)VirtualAlloc(0, filesize, MEM_COMMIT, PAGE_READWRITE);
	if (ReadFile(hFile, file, filesize, &retsize, 0) == 0)
	{
		printf("Could not read the file Error :%X.\n", GetLastError());
		VirtualFree(file, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return TRUE;
	}
	PIMAGE_DOS_HEADER DOS = (PIMAGE_DOS_HEADER)file;
	PIMAGE_NT_HEADERS NT = (PIMAGE_NT_HEADERS)(file + DOS->e_lfanew);
	if (NT->Signature != IMAGE_NT_SIGNATURE)
	{
		printf("Not valid executable.\n");
		VirtualFree(file, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return TRUE;
	}
	PIMAGE_SECTION_HEADER ISH = (PIMAGE_SECTION_HEADER)((char*)NT + sizeof(IMAGE_NT_HEADERS));
	if (((int)ISH - int(file) + NT->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER) + 0x20) > ISH[0].PointerToRawData)
	{
		printf("There is no enough space for multiboot header.\nNote:File Alignment should be 0x1000.\n");
		VirtualFree(file, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return TRUE;
	}
	DWORD HeaderOffset = (DWORD)ISH - (DWORD)file + NT->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	//Setup multiboot header.
	MULTIBOOT_HEADER header = {
		0x1BADB002,
		0x10003,
		-(0x1BADB002 + 0x10003),
		HeaderOffset + LOADBASE,
		LOADBASE,
		0,
		0,
		LOADBASE + NT->OptionalHeader.AddressOfEntryPoint
	};
	//Write the header to output file.
	memcpy(file + HeaderOffset, &header, sizeof(MULTIBOOT_HEADER));
	SetFilePointer(hFile, 0, 0, 0);
	if (WriteFile(hFile, file, filesize, &retsize, 0) == 0)
	{
		printf("Could not write to the file Error :%X.\n", GetLastError());
		VirtualFree(file, 0, MEM_RELEASE);
		CloseHandle(hFile);
		return TRUE;
	}
	CloseHandle(hFile);
	printf("Multiboot header has been added successfully.\n");

	//Create iso file using grub-mkrescue in WSL.
	std::string linux_outpath = argv[1];
	linux_outpath = ReplaceAll(linux_outpath, "\\", "/");
	linux_outpath = ReplaceAll(linux_outpath, "C:", "c");
	
	char wsl_command[0x200];
	snprintf(wsl_command, sizeof(wsl_command), "c:/windows/sysnative/bash -c \"cp /mnt/%s /mnt/c/users/0xCC/Desktop/os/boot/OSKernel.exe && grub-mkrescue -o /mnt/c/users/0xCC/Desktop/bootable0.iso /mnt/c/users/0xCC/Desktop/os/\"", linux_outpath.c_str());
	system(wsl_command);

	return FALSE;
}
