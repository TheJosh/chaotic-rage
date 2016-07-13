#ifndef __LIBHFZ_INCLUDED__
#define __LIBHFZ_INCLUDED__

#pragma once

#include <zlib.h>


#define LIBHFZ_STATUS_OK				0
#define LIBHFZ_ERROR_OPEN_FAILED		-1
#define LIBHFZ_ERROR_WRITE_FAILED		-2
#define LIBHFZ_ERROR_READ_FAILED		-3
#define LIBHFZ_ERROR_ALLOC_FAILED		-4
#define LIBHFZ_ERROR_INVALID_PARAM		-5
#define LIBHFZ_ERROR_INVALID_HANDLE		-6
#define LIBHFZ_ERROR_INVALID_FORMAT		-7
#define LIBHFZ_ERROR_INVALID_PRECIS		-8
#define LIBHFZ_ERROR_INVALID_MAP_SIZE	-9
#define LIBHFZ_ERROR_INVALID_TILE_SIZE	-10
#define LIBHFZ_ERROR_INVALID_EXT_HEADER		-11
#define LIBHFZ_ERROR_WRITE_HEADER_FAILED	-12
#define LIBHFZ_ERROR_WRITE_EXTHEAD_FAILED	-13
#define LIBHFZ_USER_ABORT	-14

#define LIBHFZ_BYTEORDER_LITTLEENDIAN	0 // default
#define LIBHFZ_BYTEORDER_INTEL			0
#define LIBHFZ_BYTEORDER_WINDOWS		0
#define LIBHFZ_BYTEORDER_BIGENDIAN		1
#define LIBHFZ_BYTEORDER_MOTOROLA		1
#define LIBHFZ_BYTEORDER_UNIX			1

typedef enum { LIBHFZ_FORMAT_HF2, LIBHFZ_FORMAT_HF2_GZ } hfzFormat;

// extended header block
typedef struct {
	char BlockType[4];
	char BlockName[16];
	unsigned long BlockLength;
	void* pBlockData;
} hfzExtHeaderBlock;

// header block
typedef struct {
	unsigned short FileVersionNo;
	unsigned long nx;
	unsigned long ny;
	unsigned short TileSize;
	float HorizScale;
	float Precis;
	unsigned long ExtHeaderLength;
	unsigned long nExtHeaderBlocks;
	hfzExtHeaderBlock* pExtHeaderBlocks;
} hfzHeader;

// file handle struct
typedef struct {
	hfzFormat Format;
	//struct _iobuf *raw;
	//gzFile gz;
	void* pIoStream;
} hfzFile;

// function handle definitions
typedef long (__stdcall* LIBHFZ_PROG_CALLBACK)(float Progress, void* lpCallbackParam);
typedef void* (__stdcall* LIBHFZ_CUSTOM_MALLOC)(long nBytes);
typedef void* (__stdcall* LIBHFZ_CUSTOM_MEMCPY)(void* pDest, const void* pSrc, long nBytes);
typedef void (__stdcall* LIBHFZ_CUSTOM_FREE)(void* pMem);
typedef void* (__stdcall* LIBHFZ_CUSTOM_FOPEN)(const char* lpFileName, const char* lpAttrib);
typedef long (__stdcall* LIBHFZ_CUSTOM_FWRITE)(void* pIoStream, const void* pData, long nBytes);
typedef long (__stdcall* LIBHFZ_CUSTOM_FREAD)(void* pIoStream, void* pData, long nBytes);
typedef long (__stdcall* LIBHFZ_CUSTOM_FCLOSE)(void* pIoStream);

// high-level
long hfzSave(const char* lpFileName, hfzFormat Format, hfzHeader& fh, float* pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam);
long hfzLoad(const char* lpFileName, hfzHeader& fh, float * pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam); // user must allocate map mem
long hfzLoadEx(const char* lpFileName, hfzHeader& fh, float ** h_pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam); // allocates map mem (user must free)

// medium-level
long hfzReadHeader(hfzFile* fs, hfzHeader& fh);
long hfzReadHeader2(const char* lpFileName, hfzHeader& fh);
long hfzReadTile(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pMapData);
long hfzReadTile2(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pTileData);

long hfzWriteHeader(hfzFile* fs, hfzHeader& fh);
long hfzWriteTile(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pMapData);
long hfzWriteTile2(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pTileData);

// low-level
hfzFile* hfzOpen(const char* lpFileName, hfzFormat FormatID, const char* lpMode);
void hfzClose(hfzFile* fs);
long hfzWrite(hfzFile* fs, const void* pData, long len);
long hfzRead(hfzFile* fs, void* pData, long len);

void* hfzMalloc(long nBytes);
void* hfzMemcpy(void* pDest, const void* pSrc, long nBytes);
void hfzFree(void* pData);

// helpers
const char* hfzGetErrorStr(long ErrorCode);

long hfzHeader_Init(hfzHeader &fh, unsigned long nx, unsigned long ny, unsigned short TileSize, float Precis, float HorizScale, unsigned long nExtHeaderBlocks);
void hfzHeader_Reset(hfzHeader &fh); // clears mem in ext header buf and restores defaults
long hfzHeader_EncodeExtHeaderBuf(hfzHeader &fh, char** ppBuf); 
long hfzHeader_DecodeExtHeaderBuf(hfzHeader &fh, char* pBuf); 

long hfzExtHeaderBlock_Init(hfzExtHeaderBlock* pBlock, const char* lpBlockType, const char* lpBlockName, unsigned long BlockDataLength, void* pBlockData);
long hfzExtHeaderBlock_InitEx(hfzHeader& fh, unsigned long BlockID, const char* lpBlockType, const char* lpBlockName, unsigned long BlockDataLength, void* pBlockData);
void hfzExtHeaderBlock_Reset(hfzExtHeaderBlock* pBlock);

long hfzSetLocalByteOrder(long ByteOrder = LIBHFZ_BYTEORDER_LITTLEENDIAN);
long hfzByteSwap(void* pData, unsigned long DataSize);

#endif