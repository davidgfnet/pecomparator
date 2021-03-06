
#define BYTE  unsigned char
#define WORD  unsigned short
#define DWORD unsigned int
#define LONG  unsigned long
#define ULONG unsigned long
#define PBYTE unsigned char *
#define DWORD_PTR DWORD*
#define LONG_PTR LONG*
#define LPVOID void *

#define IMAGE_DIRECTORY_ENTRY_IMPORT 1
#define IMAGE_SIZEOF_SHORT_NAME 8
#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

typedef struct _IMAGE_FILE_HEADER {
  WORD  Machine;
  WORD  NumberOfSections;
  DWORD TimeDateStamp;   // Changes!
  DWORD PointerToSymbolTable;
  DWORD NumberOfSymbols;
  WORD  SizeOfOptionalHeader;
  WORD  Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
  DWORD VirtualAddress;
  DWORD Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

typedef struct _IMAGE_OPTIONAL_HEADER {
  WORD                 Magic;
  BYTE                 MajorLinkerVersion;
  BYTE                 MinorLinkerVersion;
  DWORD                SizeOfCode;
  DWORD                SizeOfInitializedData;
  DWORD                SizeOfUninitializedData;
  DWORD                AddressOfEntryPoint;
  DWORD                BaseOfCode;
  DWORD                BaseOfData;
  DWORD                ImageBase;
  DWORD                SectionAlignment;
  DWORD                FileAlignment;
  WORD                 MajorOperatingSystemVersion;
  WORD                 MinorOperatingSystemVersion;
  WORD                 MajorImageVersion;
  WORD                 MinorImageVersion;
  WORD                 MajorSubsystemVersion;
  WORD                 MinorSubsystemVersion;
  DWORD                Win32VersionValue;
  DWORD                SizeOfImage;
  DWORD                SizeOfHeaders;
  DWORD                CheckSum;   // Changes!
  WORD                 Subsystem;
  WORD                 DllCharacteristics;
  DWORD                SizeOfStackReserve;
  DWORD                SizeOfStackCommit;
  DWORD                SizeOfHeapReserve;
  DWORD                SizeOfHeapCommit;
  DWORD                LoaderFlags;
  DWORD                NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

typedef struct _IMAGE_NT_HEADERS {
  DWORD                 Signature;
  IMAGE_FILE_HEADER     FileHeader;
  IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

typedef struct _IMAGE_SECTION_HEADER {
  BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
  union {
    DWORD PhysicalAddress;
    DWORD VirtualSize;
  } Misc;
  DWORD VirtualAddress;
  DWORD SizeOfRawData;
  DWORD PointerToRawData;
  DWORD PointerToRelocations;
  DWORD PointerToLinenumbers;
  WORD  NumberOfRelocations;
  WORD  NumberOfLinenumbers;
  DWORD Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

typedef struct _IMAGE_DOS_HEADER
{
     WORD e_magic;
     WORD e_cblp;
     WORD e_cp;
     WORD e_crlc;
     WORD e_cparhdr;
     WORD e_minalloc;
     WORD e_maxalloc;
     WORD e_ss;
     WORD e_sp;
     WORD e_csum;
     WORD e_ip;
     WORD e_cs;
     WORD e_lfarlc;
     WORD e_ovno;
     WORD e_res[4];
     WORD e_oemid;
     WORD e_oeminfo;
     WORD e_res2[10];
     DWORD e_lfanew;
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        ULONG   Characteristics;
        ULONG   OriginalFirstThunk;
    } DUMMYUNIONNAME;
    ULONG   TimeDateStamp;
    ULONG   ForwarderChain;
    ULONG   Name;
    ULONG   FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR, *PIMAGE_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_EXPORT_DIRECTORY {
	DWORD Characteristics;
	DWORD TimeDateStamp;
	WORD MajorVersion;
	WORD MinorVersion;
	DWORD Name;
	DWORD Base;
	DWORD NumberOfFunctions;
	DWORD NumberOfNames;
	DWORD AddressOfFunctions;
	DWORD AddressOfNames;
	DWORD AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

typedef struct _IMAGE_IMPORT_BY_NAME {
	WORD Hint;
	BYTE Name[1];
} IMAGE_IMPORT_BY_NAME,*PIMAGE_IMPORT_BY_NAME;

typedef struct _IMAGE_THUNK_DATA {
    union {
        ULONG ForwarderString;
        ULONG Function;
        ULONG Ordinal;
        ULONG AddressOfData;
    } u1;
} IMAGE_THUNK_DATA, *PIMAGE_THUNK_DATA;

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type,field) ((LONG)&(((type *)0)->field))
#endif

#define IMAGE_FIRST_SECTION(h) ((PIMAGE_SECTION_HEADER)((ULONG)h+FIELD_OFFSET(IMAGE_NT_HEADERS,OptionalHeader)+((PIMAGE_NT_HEADERS)(h))->FileHeader.SizeOfOptionalHeader))

// Matt Pietrek's functions
PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(DWORD_PTR rva, PIMAGE_NT_HEADERS pNTHeader) {
	PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(pNTHeader);
	WORD nSection = 0;

	for(nSection = 0; nSection < pNTHeader->FileHeader.NumberOfSections; nSection++, section++ ) {
		// This 3 line idiocy is because Watcom's linker actually sets the
		// Misc.VirtualSize field to 0.  (!!! - Retards....!!!)
		DWORD size = section->Misc.VirtualSize;
		if (size == 0) size = section->SizeOfRawData;

		// Is the RVA within this section?
		if ( (((DWORD)rva) >= section->VirtualAddress) && ((DWORD)rva < (section->VirtualAddress + size)) )
			return section;
	}

	return 0;
}
static LPVOID GetPtrFromRVA(DWORD rva, PIMAGE_NT_HEADERS pNTHeader, PBYTE imageBase) {
	PIMAGE_SECTION_HEADER section;
	LONG_PTR delta;

	section = GetEnclosingSectionHeader((DWORD_PTR)rva, pNTHeader);
	if(!section) return 0;

	delta = (LONG_PTR)( section->VirtualAddress - section->PointerToRawData );
	return (LPVOID)( imageBase + rva - (DWORD)delta );
}


