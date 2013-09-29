
#include <stdio.h>
#include <stdlib.h>
#include "pe_headers.h"

void notify(const char * c) {
	printf("%s\n",c);
	exit(0);
}

void patch (unsigned char * buf) {
	PIMAGE_NT_HEADERS pNT = (PIMAGE_NT_HEADERS)((char*)buf + ((PIMAGE_DOS_HEADER)buf)->e_lfanew);
	PIMAGE_SECTION_HEADER pSections = (PIMAGE_SECTION_HEADER)((char*)pNT + sizeof(DWORD) +
		sizeof(IMAGE_FILE_HEADER) + pNT->FileHeader.SizeOfOptionalHeader);

	pNT->FileHeader.TimeDateStamp = 0;
	pNT->OptionalHeader.CheckSum = 0;

	// Patch export table timestamp (usually present in DLLs)
	LPVOID eat = GetPtrFromRVA(pNT->OptionalHeader.DataDirectory[0].VirtualAddress,pNT,buf);
	if (eat) {
		IMAGE_EXPORT_DIRECTORY * ied = (IMAGE_EXPORT_DIRECTORY *)eat;

		ied->TimeDateStamp = 0;
	}
}

int main(int argc, char ** argv) {
	if (argc < 3) {
		fprintf(stderr,"Usage: %s file1.exe file2.exe\n",argv[0]);
		exit(1);
	}
	FILE * fd1 = fopen(argv[1],"rb");
	FILE * fd2 = fopen(argv[2],"rb");

	if (fd1 == 0) {
		fprintf(stderr,"Could not open input file %s\n",argv[1]);
		exit(1);
	}
	if (fd2 == 0) {
		fprintf(stderr,"Could not open input file %s\n",argv[2]);
		exit(1);
	}

	fseek(fd1, 0, SEEK_END);
	fseek(fd2, 0, SEEK_END);

	int lof1 = ftell(fd1);
	int lof2 = ftell(fd2);

	if (lof1 != lof2)
		notify("0");

	unsigned char * buf1 = malloc(lof1);
	unsigned char * buf2 = malloc(lof2);

	fseek(fd1, 0, SEEK_SET);
	fseek(fd2, 0, SEEK_SET);
	fread(buf1,1,lof1,fd1);
	fread(buf2,1,lof2,fd2);
	fclose(fd1);
	fclose(fd2);

	// Patch all the timestamps with zero (and checksums too)
	patch(buf1);
	patch(buf2);

	if (memcmp(buf1,buf2,lof1) != 0)
		notify("0");

	notify("1");
}



