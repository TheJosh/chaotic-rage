/*

LibHFZ, a library to load and save HF2/HFZ compressed heightfield files.

Version: 1.4.3
Released: 10th of August 2009

Copyright (C) 2007-2009 Aaron Torpy (aaron@bundysoft.com)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

For more information about LibHFZ, please consult:
http://www.bundysoft.com/docs/doku.php?id=libhfz

*/

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
	

#include "libhfz.h"
#include <zlib.h>
#include <zconf.h>

// global vars
static long hfzByteOrder = LIBHFZ_BYTEORDER_LITTLEENDIAN; // *NIX developers: change this default

// handles for overridable functions
static LIBHFZ_CUSTOM_FOPEN		hfzFopenProc = 0;
static LIBHFZ_CUSTOM_FWRITE		hfzFwriteProc = 0;
static LIBHFZ_CUSTOM_FWRITE		hfzFreadProc = 0;
static LIBHFZ_CUSTOM_FCLOSE		hfzFcloseProc = 0;
static LIBHFZ_CUSTOM_MALLOC		hfzMallocProc = 0;
static LIBHFZ_CUSTOM_MEMCPY		hfzMemcpyProc = 0;
static LIBHFZ_CUSTOM_FREE		hfzFreeProc = 0;

//
//
// High-level HFZ functions
//
//


long hfzSave(const char* lpFileName, hfzFormat Format, hfzHeader& fh, float* pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam) {

	// check format
	switch(Format) {
	default:
		return LIBHFZ_ERROR_INVALID_FORMAT;
	case LIBHFZ_FORMAT_HF2:
	case LIBHFZ_FORMAT_HF2_GZ:
		break;
	}

	// check settings
	if(fh.Precis<=0) {
		return LIBHFZ_ERROR_INVALID_PRECIS;
	}
	if(fh.TileSize<8 || fh.TileSize>65535) {
		return LIBHFZ_ERROR_INVALID_TILE_SIZE;
	}
	if(fh.nx<=0 || fh.ny<=0) {
		return LIBHFZ_ERROR_INVALID_MAP_SIZE;
	}
	
	// determine number of blocks
	long nBlocksX = fh.nx / fh.TileSize;
	long nBlocksY = fh.ny / fh.TileSize;
	if(fh.nx%fh.TileSize)	nBlocksX++;
	if(fh.ny%fh.TileSize)	nBlocksY++;

	// open file for writing
	hfzFile* fs;
	if(!(fs = hfzOpen(lpFileName, Format, "wb"))) {
		return LIBHFZ_ERROR_OPEN_FAILED;
	}

	// write header
	if(LIBHFZ_STATUS_OK!=hfzWriteHeader(fs, fh)) {
		hfzClose(fs);
		return LIBHFZ_ERROR_WRITE_FAILED;
	}

	// writing tiles
	long rval = LIBHFZ_STATUS_OK;
	for(long bj = 0; bj<nBlocksY && rval==LIBHFZ_STATUS_OK; bj++) {
	for(long bi = 0; bi<nBlocksX && rval==LIBHFZ_STATUS_OK; bi++) {

		if(lpProgCallback) {
			long k = bi + bj * nBlocksX + 1;
			long kmax = nBlocksX * nBlocksY;
			rval = lpProgCallback((float)k / (float)kmax, lpCallbackParam);
		}

		rval = hfzWriteTile(fs, fh, bi, bj, pData);
	}
	}

	// close file
	hfzClose(fs);

	return rval;
}

long hfzLoad(const char* lpFileName, hfzHeader& fh, float *pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam) {

	// null header (in case caller forgot to do so)
	hfzHeader_Init(fh, 0, 0, 0, 0, 0, 0);

	if(!pData)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	hfzFile* fs;
	fs = hfzOpen(lpFileName, LIBHFZ_FORMAT_HF2_GZ, "rb"); // always treat as gzip; zlib handles raw too
	if(!fs) {
		return LIBHFZ_ERROR_OPEN_FAILED;
	}

	long rval = hfzReadHeader(fs, fh);
	if(LIBHFZ_STATUS_OK!=rval) {
		hfzClose(fs);
		return rval;
	}

	// shorthand (shorter hand, anyhow)
	long nx = fh.nx;
	long ny = fh.ny;
	long TileSize = fh.TileSize;

	// determine number of blocks
	long nBlocksX = nx / TileSize;
	long nBlocksY = ny / TileSize;
	if(nx%TileSize)	nBlocksX++;
	if(ny%TileSize)	nBlocksY++;

	rval = LIBHFZ_STATUS_OK;
	for(long bj = 0; bj<nBlocksY && rval == LIBHFZ_STATUS_OK; bj++) {
	for(long bi = 0; bi<nBlocksX && rval == LIBHFZ_STATUS_OK; bi++) {

		if(lpProgCallback) {
			long k = bi + bj * nBlocksX + 1;
			long kmax = nBlocksX * nBlocksY;
			rval = lpProgCallback((float)k / (float)kmax, lpCallbackParam);
		}

		if(rval==LIBHFZ_STATUS_OK)
			rval = hfzReadTile(fs, fh, bi, bj, pData);
	}
	}

	hfzClose(fs);

	return rval;
}

long hfzLoadEx(const char* lpFileName, hfzHeader& fh, float **h_pData, LIBHFZ_PROG_CALLBACK lpProgCallback, void* lpCallbackParam) {

	// null header (in case caller forgot to do so)
	hfzHeader_Init(fh, 0, 0, 0, 0, 0, 0);

	long rval = hfzReadHeader2(lpFileName, fh);
	if(LIBHFZ_STATUS_OK!=rval) {
        return rval;		
	}

	// allocate buffer
	float* pData = (float*)hfzMalloc(static_cast<long>(fh.nx * fh.ny * 4));
	if(!pData) {
		hfzHeader_Reset(fh);
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}

	// load 
	rval = hfzLoad(lpFileName, fh, pData, lpProgCallback, lpCallbackParam);
	if(LIBHFZ_STATUS_OK!=rval) {
		hfzHeader_Reset(fh);
		hfzFree(pData);
		return rval;
	}

	*h_pData = pData;
	return LIBHFZ_STATUS_OK;
}

//
//
// Medium-level HFZ functions
//
//

long hfzWriteHeader(hfzFile* fs, hfzHeader& fh) {

	// copy header into buffer
	char HeaderBuf[28];
	long HeaderBufPos = 0;
	sprintf(HeaderBuf+HeaderBufPos, "HF2"); HeaderBufPos+=4;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.FileVersionNo), 2); HeaderBufPos+=2;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.nx), 4); HeaderBufPos+=4;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.ny), 4); HeaderBufPos+=4;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.TileSize), 2); HeaderBufPos+=2;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.Precis), 4); HeaderBufPos+=4;
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.HorizScale), 4); HeaderBufPos+=4;

	// put extended header into a buffer
	char* pExtHeaderData = 0;
	long rval = hfzHeader_EncodeExtHeaderBuf(fh, &pExtHeaderData);
	if(rval<0) return rval;

	// now write in header length of ext header
	hfzMemcpy(HeaderBuf+HeaderBufPos, &(fh.ExtHeaderLength), 4); HeaderBufPos+=4;

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		HeaderBufPos = 4; // skip "HF2\0" string

		hfzByteSwap(HeaderBuf+HeaderBufPos, 2); HeaderBufPos+=2; // FileVersionNo
		hfzByteSwap(HeaderBuf+HeaderBufPos, 4); HeaderBufPos+=4; // nx
		hfzByteSwap(HeaderBuf+HeaderBufPos, 4); HeaderBufPos+=4; // ny
		hfzByteSwap(HeaderBuf+HeaderBufPos, 2); HeaderBufPos+=2; // TileSize
		hfzByteSwap(HeaderBuf+HeaderBufPos, 4); HeaderBufPos+=4; // Precis
		hfzByteSwap(HeaderBuf+HeaderBufPos, 4); HeaderBufPos+=4; // HorizScale
		hfzByteSwap(HeaderBuf+HeaderBufPos, 4); HeaderBufPos+=4; // ExtHeaderLength
	}

	// write header
	if(28!=hfzWrite(fs, HeaderBuf, 28))
		return LIBHFZ_ERROR_WRITE_HEADER_FAILED;

	// write extended header
	if(pExtHeaderData) {
		if(fh.ExtHeaderLength!=(unsigned long)hfzWrite(fs, pExtHeaderData, fh.ExtHeaderLength)) {
			hfzFree(pExtHeaderData);
			return LIBHFZ_ERROR_WRITE_EXTHEAD_FAILED;
		}
		hfzFree(pExtHeaderData);
	}

	return LIBHFZ_STATUS_OK;
}

long hfzReadHeader(hfzFile* fs, hfzHeader& fh) {

	// initialising empty
	hfzHeader_Init(fh, 0, 0, 0, 0, 0, 0);

	// read the core header
	char HeaderBuf[28];
	if(28!=hfzRead(fs, HeaderBuf, 28)) {
		return LIBHFZ_ERROR_READ_FAILED;
	}

	// check for "HF2\0" in leading 4 bytes
	HeaderBuf[3] = 0;
	if(strcmp(HeaderBuf, "HF2")) {
		return LIBHFZ_ERROR_INVALID_FORMAT;
	}

	long HeaderPos;

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		HeaderPos = 4; // skip "HF2\0" string

		hfzByteSwap(HeaderBuf+HeaderPos, 2); HeaderPos+=2; // FileVersionNo
		hfzByteSwap(HeaderBuf+HeaderPos, 4); HeaderPos+=4; // nx
		hfzByteSwap(HeaderBuf+HeaderPos, 4); HeaderPos+=4; // ny
		hfzByteSwap(HeaderBuf+HeaderPos, 2); HeaderPos+=2; // TileSize
		hfzByteSwap(HeaderBuf+HeaderPos, 4); HeaderPos+=4; // Precis
		hfzByteSwap(HeaderBuf+HeaderPos, 4); HeaderPos+=4; // HorizScale
		hfzByteSwap(HeaderBuf+HeaderPos, 4); HeaderPos+=4; // ExtHeaderLength
	}

	HeaderPos = 4; // skip "HF2\0" string
	hfzMemcpy(&(fh.FileVersionNo), HeaderBuf+HeaderPos, 2); HeaderPos+=2;
	hfzMemcpy(&(fh.nx), HeaderBuf+HeaderPos, 4); HeaderPos+=4;
	hfzMemcpy(&(fh.ny), HeaderBuf+HeaderPos, 4); HeaderPos+=4;
	hfzMemcpy(&(fh.TileSize), HeaderBuf+HeaderPos, 2); HeaderPos+=2;
	hfzMemcpy(&(fh.Precis), HeaderBuf+HeaderPos, 4); HeaderPos+=4;
	hfzMemcpy(&(fh.HorizScale), HeaderBuf+HeaderPos, 4); HeaderPos+=4;
	hfzMemcpy(&(fh.ExtHeaderLength), HeaderBuf+HeaderPos, 4); HeaderPos+=4;

	unsigned long hl = fh.ExtHeaderLength;

    if(hl) {

		char* pBuf;
		if(!(pBuf = (char*)hfzMalloc(hl))) {
			return LIBHFZ_ERROR_ALLOC_FAILED;
		}

        if(hl!=(unsigned long)hfzRead(fs, pBuf, hl)) {
			hfzFree(pBuf);
			return LIBHFZ_ERROR_READ_FAILED;
		}

		long rval = hfzHeader_DecodeExtHeaderBuf(fh, pBuf);

		hfzFree(pBuf);

		if(rval<0) {
			return rval;
		}
	}

	return LIBHFZ_STATUS_OK;
}

long hfzReadHeader2(const char* lpFileName, hfzHeader& fh) {
	hfzFile* fs;
	if(!(fs=hfzOpen(lpFileName, LIBHFZ_FORMAT_HF2_GZ, "rb"))) {
		return LIBHFZ_ERROR_OPEN_FAILED;
	}

	if(LIBHFZ_STATUS_OK!=hfzReadHeader(fs, fh)) {
		return LIBHFZ_ERROR_READ_FAILED;
	}

	hfzClose(fs);

	return LIBHFZ_STATUS_OK;
}


long hfzReadTile(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pMapData) {

	unsigned long i1, i2, j1, j2, i, j;

	char c;
	short s;
	uint32_t li;

	unsigned long TileSize = fh.TileSize;
	unsigned long nx = fh.nx;
	unsigned long ny = fh.ny;

	i1 = TileX * TileSize;
	j1 = TileY * TileSize;
	i2 = i1 + TileSize;
	j2 = j1 + TileSize;

	if(i2>=nx)	i2 = nx;
	if(j2>=ny)	j2 = ny;

	// read vert offset and sale
	float VertOffset, VertScale;
	if(4!=hfzRead(fs, &VertScale, 4))
		return LIBHFZ_ERROR_READ_FAILED;
	if(4!=hfzRead(fs, &VertOffset, 4))
		return LIBHFZ_ERROR_READ_FAILED;

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		hfzByteSwap(&VertScale, 4);
		hfzByteSwap(&VertOffset, 4);
	}
	 
	for(j=j1; j<j2; j++) {

		char LineDepth; // 1, 2, or 4
		uint32_t FirstVal;

		if(1!=hfzRead(fs, &LineDepth, 1))
			return LIBHFZ_ERROR_READ_FAILED;
		if(4!=hfzRead(fs, &FirstVal, 4))
			return LIBHFZ_ERROR_READ_FAILED;

		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(&FirstVal, 4);
		}

		float f = (float)FirstVal * VertScale + VertOffset;

		// set first pixel
		pMapData[i1 + nx * j] = f;

		uint32_t LastVal = FirstVal;

		for(i=i1+1; i<i2; i++) {
			switch(LineDepth) {
			case 1:
				if(1!=hfzRead(fs, &c, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;
				li = (long)c;
				break;
			case 2:
				if(2!=hfzRead(fs, &s, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;

				if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
					hfzByteSwap(&s, 2);
				}

				li = (long)s;
				break;
			case 4:
				if(4!=hfzRead(fs, &li, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;

				if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
					hfzByteSwap(&li, 4);
				}

				break;
			}

			f = (float)(li +  LastVal) * VertScale + VertOffset;
			LastVal = li + LastVal;

			pMapData[i + nx * j] = f;
		}
	}
	
	return LIBHFZ_STATUS_OK;
}
long hfzReadTile2(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pTileData) {

	unsigned long i1, i2, j1, j2, i, j;

	char c;
	short s;
	long li;

	unsigned long TileSize = fh.TileSize;
	unsigned long nx = fh.nx;
	unsigned long ny = fh.ny;

	i1 = TileX * TileSize;
	j1 = TileY * TileSize;
	i2 = i1 + TileSize;
	j2 = j1 + TileSize;

	if(i2>=nx)	i2 = nx;
	if(j2>=ny)	j2 = ny;

	// read vert offset and sale
	float VertOffset, VertScale;
	if(4!=hfzRead(fs, &VertScale, 4))
		return LIBHFZ_ERROR_READ_FAILED;
	if(4!=hfzRead(fs, &VertOffset, 4))
		return LIBHFZ_ERROR_READ_FAILED;

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		hfzByteSwap(&VertScale, 4);
		hfzByteSwap(&VertOffset, 4);
	}
	 
	for(j=j1; j<j2; j++) {

		char LineDepth; // 1, 2, or 4
		long FirstVal;

		if(1!=hfzRead(fs, &LineDepth, 1))
			return LIBHFZ_ERROR_READ_FAILED;
		if(4!=hfzRead(fs, &FirstVal, 4))
			return LIBHFZ_ERROR_READ_FAILED;

		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(&FirstVal, 4);
		}

		float f = (float)FirstVal * VertScale + VertOffset;

		// set first pixel
		pTileData[fh.TileSize * (j-j1)] = f;

		long LastVal = FirstVal;

		for(i=i1+1; i<i2; i++) {
			switch(LineDepth) {
			case 1:
				if(1!=hfzRead(fs, &c, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;
				li = (long)c;
				break;
			case 2:
				if(2!=hfzRead(fs, &s, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;

				if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
					hfzByteSwap(&s, 2);
				}

				li = (long)s;
				break;
			case 4:
				if(4!=hfzRead(fs, &li, LineDepth))	return LIBHFZ_ERROR_READ_FAILED;

				if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
					hfzByteSwap(&li, 4);
				}

				break;
			}

			f = (float)(li +  LastVal) * VertScale + VertOffset;
			LastVal = li + LastVal;

			pTileData[(i-i1) + fh.TileSize * (j-j1)] = f;
		}
	}
	
	return LIBHFZ_STATUS_OK;
}

long hfzWriteTile(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pMapData) {

	if(!fs)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(!pMapData)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	unsigned long i, j;
	long TempInt;
	float f;
	float HFmin, HFmax;
	char c;
	short s;
	unsigned long i1, j1, i2, j2, zi;
	long FirstVal;
	float VertScale;
	float VertOffset;

	unsigned long nx = fh.nx;
	unsigned long ny = fh.ny;
	unsigned long TileSize = fh.TileSize;
	float Precis = fh.Precis;


	i1 = TileX * TileSize;
	j1 = TileY * TileSize;
	i2 = i1 + TileSize;
	j2 = j1 + TileSize;

	if(i2>=nx)	i2 = nx;
	if(j2>=ny)	j2 = ny;

	// prepare buffer for line
	char * pDiffBuf = (char*)hfzMalloc(TileSize * 4);
	if(!pDiffBuf) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}
	char * pOutBuf = (char*)hfzMalloc(TileSize * 4);
	if(!pOutBuf) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}

	// get min/max alt in block (used for vert scale)
	HFmin = 0;
	HFmax = 0;
	for(j=j1; j<j2; j++) {
	for(i=i1; i<i2; i++) {
		// find max diff in line

		f = pMapData[i + j * nx];
	
		if(j==j1 && i==i1) {
			HFmin = HFmax = f;
		} else {
			if(f<HFmin)	HFmin = f;
			if(f>HFmax)	HFmax = f;
		}
	}
	}

	// number of int levels required for this block
	float BlockLevels = (HFmax - HFmin) / Precis + 1;

	// calc scale
	VertScale = (HFmax - HFmin)/(float)BlockLevels;
	VertOffset = HFmin;
	if(VertScale<=0)	VertScale = 1.0f; // this is for niceness

	// write the block scaling
	char ScaleBuf[8];
	hfzMemcpy(ScaleBuf, &VertScale, 4);
	hfzMemcpy(ScaleBuf+4, &VertOffset, 4);

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		hfzByteSwap(ScaleBuf, 4);
		hfzByteSwap(ScaleBuf+4, 4);
	}

	if(8!=hfzWrite(fs, ScaleBuf, 8)) {
		return LIBHFZ_ERROR_WRITE_FAILED;
	}

	// save block line-by-line
	for(j=j1; j<j2; j++) {
	
		// get first val
		f = pMapData[i1 + j * nx];
		FirstVal = (long)((f-VertOffset)/VertScale);
		long LastVal = FirstVal;

		// find max diff in line
		long Diff;
		long MaxDev = 0;
		for(i=i1+1; i<i2; i++) {

			// find max diff in line
			f = pMapData[i + j * nx];
		
			TempInt=(long)((f-VertOffset)/VertScale);
			Diff = TempInt - LastVal;

			zi = i-i1-1;
			hfzMemcpy((char*)pDiffBuf+zi*4, &Diff, 4);

			LastVal = TempInt;

			MaxDev = MaxDev>abs(Diff)?MaxDev:abs(Diff);
		}

		// should we use 8, 16 or 32 bit pixels?
		char LineDepth = 4;
		if(MaxDev<=127) {
			LineDepth = 1;
		} else
		if(MaxDev<=32767) {
			LineDepth = 2;
		}

		// write line header
		char BlockBuf[5];
		hfzMemcpy(BlockBuf, &LineDepth, 1); // store line depth
		hfzMemcpy(BlockBuf+1, &FirstVal, 4); // store first value (32bit precis)
		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(BlockBuf+1, 4);
		}
		if(5!=hfzWrite(fs, BlockBuf, 5)) {
			return LIBHFZ_ERROR_WRITE_FAILED;
		}
		
		// now write block
		char* pOutBuf2 = pOutBuf;
		for(i=i1+1; i<i2; i++) {

			zi = i-i1-1;
			hfzMemcpy(&Diff, (char*)pDiffBuf+zi*4, 4);
			switch(LineDepth) {
			case 1:
				c = (char)Diff;
				hfzMemcpy(pOutBuf2, &c, LineDepth);
				break;
			case 2:
				s = (short)Diff;
				hfzMemcpy(pOutBuf2, &s, LineDepth);
				break;
			case 4:
				hfzMemcpy(pOutBuf2, &Diff, LineDepth);
				break;
			}

			if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
				hfzByteSwap(pOutBuf2, LineDepth);
			}

			pOutBuf2+=LineDepth;
		}

		long len = LineDepth * (i2-i1-1);
		if(len!=hfzWrite(fs, pOutBuf, len))
			return LIBHFZ_ERROR_WRITE_FAILED;
	}

	hfzFree(pDiffBuf);
	hfzFree(pOutBuf);

	return LIBHFZ_STATUS_OK;
}

long hfzWriteTile2(hfzFile* fs, hfzHeader& fh, unsigned long TileX, unsigned long TileY, float* pTileData) {

	if(!fs)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(!pTileData)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	unsigned long i, j;
	long TempInt;
	float f;
	float HFmin, HFmax;
	char c;
	short s;
	unsigned long i1, j1, i2, j2, zi;
	long FirstVal;
	float VertScale;
	float VertOffset;

	unsigned long nx = fh.nx;
	unsigned long ny = fh.ny;
	unsigned long TileSize = fh.TileSize;
	float Precis = fh.Precis;


	i1 = TileX * TileSize;
	j1 = TileY * TileSize;
	i2 = i1 + TileSize;
	j2 = j1 + TileSize;

	if(i2>=nx)	i2 = nx;
	if(j2>=ny)	j2 = ny;

	// prepare buffer for line
	char * pDiffBuf = (char*)hfzMalloc(TileSize * 4);
	if(!pDiffBuf) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}
	char * pOutBuf = (char*)hfzMalloc(TileSize * 4);
	if(!pOutBuf) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}

	// get min/max alt in block (used for vert scale)
	HFmin = 0;
	HFmax = 0;
	for(j=j1; j<j2; j++) {
	for(i=i1; i<i2; i++) {
		// find max diff in line

		f = pTileData[(i-i1) + (j-j1) * fh.TileSize];
	
		if(j==j1 && i==i1) {
			HFmin = HFmax = f;
		} else {
			if(f<HFmin)	HFmin = f;
			if(f>HFmax)	HFmax = f;
		}
	}
	}

	// number of int levels required for this block
	float BlockLevels = (HFmax - HFmin) / Precis + 1;

	// calc scale
	VertScale = (HFmax - HFmin)/(float)BlockLevels;
	VertOffset = HFmin;
	if(VertScale<=0)	VertScale = 1.0f; // this is for niceness

	// write the block scaling
	char ScaleBuf[8];
	hfzMemcpy(ScaleBuf, &VertScale, 4);
	hfzMemcpy(ScaleBuf+4, &VertOffset, 4);

	// swap byte order if required (files use little endian)
	if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
		hfzByteSwap(ScaleBuf, 4);
		hfzByteSwap(ScaleBuf+4, 4);
	}

	if(8!=hfzWrite(fs, ScaleBuf, 8)) {
		return LIBHFZ_ERROR_WRITE_FAILED;
	}

	// save block line-by-line
	for(j=j1; j<j2; j++) {
	
		// get first val
		f = pTileData[(j-j1) * fh.TileSize];
		FirstVal = (long)((f-VertOffset)/VertScale);
		long LastVal = FirstVal;

		// find max diff in line
		long Diff;
		long MaxDev = 0;
		for(i=i1+1; i<i2; i++) {

			// find max diff in line
			f = pTileData[(i-i1) + (j-j1) * fh.TileSize];
		
			TempInt=(long)((f-VertOffset)/VertScale);
			Diff = TempInt - LastVal;

			zi = i-i1-1;
			hfzMemcpy((char*)pDiffBuf+zi*4, &Diff, 4);

			LastVal = TempInt;

			MaxDev = MaxDev>abs(Diff)?MaxDev:abs(Diff);
		}

		// should we use 8, 16 or 32 bit pixels?
		char LineDepth = 4;
		if(MaxDev<=127) {
			LineDepth = 1;
		} else
		if(MaxDev<=32767) {
			LineDepth = 2;
		}

		// write line header
		char BlockBuf[5];
		hfzMemcpy(BlockBuf, &LineDepth, 1); // store line depth
		hfzMemcpy(BlockBuf+1, &FirstVal, 4); // store first value (32bit precis)
		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(BlockBuf+1, 4);
		}
		if(5!=hfzWrite(fs, BlockBuf, 5)) {
			return LIBHFZ_ERROR_WRITE_FAILED;
		}
		
		// now write block
		char* pOutBuf2 = pOutBuf;
		for(i=i1+1; i<i2; i++) {

			zi = i-i1-1;
			hfzMemcpy(&Diff, (char*)pDiffBuf+zi*4, 4);
			switch(LineDepth) {
			case 1:
				c = (char)Diff;
				hfzMemcpy(pOutBuf2, &c, LineDepth);
				break;
			case 2:
				s = (short)Diff;
				hfzMemcpy(pOutBuf2, &s, LineDepth);
				break;
			case 4:
				hfzMemcpy(pOutBuf2, &Diff, LineDepth);
				break;
			}

			if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
				hfzByteSwap(pOutBuf2, LineDepth);
			}

			pOutBuf2+=LineDepth;
		}

		long len = LineDepth * (i2-i1-1);
		if(len!=hfzWrite(fs, pOutBuf, len))
			return LIBHFZ_ERROR_WRITE_FAILED;
	}

	hfzFree(pDiffBuf);
	hfzFree(pOutBuf);

	return LIBHFZ_STATUS_OK;
}

//
//
// Low-level HFZ functions
//
//

hfzFile* hfzOpen(const char* lpFileName, hfzFormat Format, const char* lpMode) {
	
	hfzFile* fs = new hfzFile;

	if(hfzFopenProc) {
		// using custom function
		if(!(fs->pIoStream = hfzFopenProc(lpFileName, lpMode))) {
			delete fs;
			return 0;
		}
	} else {
		switch(Format) {
		case LIBHFZ_FORMAT_HF2: 
			if(!(fs->pIoStream = (void*)fopen(lpFileName, lpMode))) {
				delete fs;
				return 0;
			}
			break;
		case LIBHFZ_FORMAT_HF2_GZ: 
			if(!(fs->pIoStream = (void*)gzopen(lpFileName, lpMode))) {
				delete fs;
				return 0;
			}
			break;
		default: 
			delete fs;
			return 0;
		}
	}

	fs->Format = Format;

	return fs;
}

void hfzClose(hfzFile* fs) {

	if(!fs)
		return;

	if(hfzFcloseProc) {
		// use custom function
		hfzFcloseProc(fs->pIoStream);
	} else {
		switch(fs->Format) {
		case LIBHFZ_FORMAT_HF2:	fclose((FILE*)fs->pIoStream); break;
		case LIBHFZ_FORMAT_HF2_GZ: gzclose((gzFile)fs->pIoStream); break;
		}
	}

	delete fs;
}

long hfzRead(hfzFile* fs, void* pData, long len) {
	if(!fs)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(!len)
		return 0;

	if(!pData)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(hfzFreadProc) {
		// try custom function
		return hfzFreadProc(fs->pIoStream, pData, len);
	} else {
		switch(fs->Format) {
		case LIBHFZ_FORMAT_HF2:    return (long)fread(pData, 1, len, (FILE*)fs->pIoStream);
		case LIBHFZ_FORMAT_HF2_GZ: return (long)gzread((gzFile)fs->pIoStream, pData, len);
		}
	}

	return LIBHFZ_ERROR_INVALID_FORMAT;
}

long hfzWrite(hfzFile* fs, const void* pData, long len) {
	if(!fs)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(!len)
		return 0;

	if(!pData)
		return LIBHFZ_ERROR_INVALID_HANDLE;

	if(hfzFwriteProc) {
		// try custom function
		return hfzFwriteProc(fs->pIoStream, pData, len);
	} else {
		switch(fs->Format) {
		case LIBHFZ_FORMAT_HF2:    return (long)fwrite(pData, 1, len, (FILE*)fs->pIoStream);
		case LIBHFZ_FORMAT_HF2_GZ: return (long)gzwrite((gzFile)fs->pIoStream, pData, len);
		}
	}

	return LIBHFZ_ERROR_INVALID_FORMAT;
}

void* hfzMalloc(long nBytes) {

	if(hfzMallocProc) 
			return hfzMallocProc(nBytes);
	else	return calloc(nBytes, 1);
}
void* hfzMemcpy(void* pDest, const void* pSrc, long nBytes) {

	if(hfzMemcpyProc) 
			return hfzMemcpyProc(pDest, pSrc, nBytes);
	else	return memcpy(pDest, pSrc, nBytes);
}
void hfzFree(void* pData) {

	if(hfzFreeProc)
			hfzFreeProc(pData);
	else	free(pData);
}

//
//
// HFZ helper functions
//
//


const char* hfzGetErrorStr(long ErrorCode) {
	switch(ErrorCode) {
	case LIBHFZ_ERROR_OPEN_FAILED:			return "Cannot open file";
	case LIBHFZ_ERROR_WRITE_FAILED:			return "Write failed";
	case LIBHFZ_ERROR_READ_FAILED:			return "Read failed";
	case LIBHFZ_ERROR_ALLOC_FAILED:			return "Cannot allocate buffer";
	case LIBHFZ_ERROR_INVALID_PARAM:		return "Invalid parameter";
	case LIBHFZ_ERROR_INVALID_HANDLE:		return "Invalid handle";
	case LIBHFZ_ERROR_INVALID_FORMAT:		return "Invalid format ID";
	case LIBHFZ_ERROR_INVALID_PRECIS:		return "Invalid precision";
	case LIBHFZ_ERROR_INVALID_MAP_SIZE:		return "Invalid map size";
	case LIBHFZ_ERROR_INVALID_TILE_SIZE:	return "Invalid tile size";
	case LIBHFZ_ERROR_INVALID_EXT_HEADER:	return "Invalid extended header";
	case LIBHFZ_ERROR_WRITE_HEADER_FAILED:	return "Error writing header";
	case LIBHFZ_ERROR_WRITE_EXTHEAD_FAILED: return "Error writign extended header";
	}

	return 0;
}

long hfzHeader_Init(hfzHeader &fh, unsigned long nx, unsigned long ny, unsigned short TileSize, float Precis, float HorizScale, unsigned long nExtHeaderBlocks) {

	fh.FileVersionNo = 0; // this is the very first implementation
	fh.nx = nx;
	fh.ny = ny;
	fh.TileSize = TileSize;
	fh.Precis = Precis;
	fh.HorizScale = HorizScale;
	fh.nExtHeaderBlocks = nExtHeaderBlocks;

	try {

		if(nExtHeaderBlocks) {
			fh.pExtHeaderBlocks = (hfzExtHeaderBlock*)hfzMalloc(sizeof(hfzExtHeaderBlock) * nExtHeaderBlocks);
			if(!fh.pExtHeaderBlocks) {
				return LIBHFZ_ERROR_ALLOC_FAILED;
			}
			for(unsigned long i=0; i<nExtHeaderBlocks; i++) {
				// initialising blocks empty
				hfzExtHeaderBlock_InitEx(fh, i, 0, 0, 0, 0);
			}
		} else {
			fh.pExtHeaderBlocks = 0;
		}

	} catch(...) {

		return LIBHFZ_ERROR_INVALID_PARAM;

	}

	return LIBHFZ_STATUS_OK;
}

void hfzHeader_Reset(hfzHeader& fh) {
	fh.FileVersionNo = 0;
	fh.nx = 0;
	fh.ny = 0;
	fh.HorizScale = 1;
	fh.Precis = 0.01f;
	fh.TileSize = 256;
	fh.ExtHeaderLength = 0;

	for(unsigned long i=0; i<fh.nExtHeaderBlocks; i++) {
		hfzExtHeaderBlock_Reset(&(fh.pExtHeaderBlocks[i]));
	}
	fh.nExtHeaderBlocks = 0;
	if(fh.pExtHeaderBlocks) {
		hfzFree(fh.pExtHeaderBlocks);
		fh.pExtHeaderBlocks = 0;
	}
}

long hfzHeader_EncodeExtHeaderBuf(hfzHeader &fh, char** ppBuf) {

	char* pBuf;

	fh.ExtHeaderLength = 0;

	if(!fh.nExtHeaderBlocks) {
		return LIBHFZ_STATUS_OK;
	}
	
	//calculate overall length
	unsigned long extlen = 0;
	for(unsigned long i=0; i<fh.nExtHeaderBlocks; i++) {
		extlen += 24 + fh.pExtHeaderBlocks[i].BlockLength;
	}

	if(!extlen) {
		// can't have zero length
		return LIBHFZ_ERROR_INVALID_EXT_HEADER;
	}

	pBuf = (char*)hfzMalloc(extlen);
	if(!pBuf) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}

	unsigned long offset = 0;
	hfzExtHeaderBlock* pBlock;
	for(unsigned long i=0; i<fh.nExtHeaderBlocks; i++) {
		pBlock = &(fh.pExtHeaderBlocks[i]);

		// forcing nulls
		pBlock->BlockType[3] = 0;
		pBlock->BlockName[15] = 0;

		hfzMemcpy(pBuf+offset, &(pBlock->BlockType), 4); offset+=4;
		hfzMemcpy(pBuf+offset, &(pBlock->BlockName), 16); offset+=16;
		hfzMemcpy(pBuf+offset, &(pBlock->BlockLength), 4); offset+=4;

		// swap byte order if required (files use little endian)
		// (only have to do this for block length; block name and type are ASCII)
		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(pBuf+offset-4, 4); // going back to change BlockLength in buffer
		}

		if(pBlock->BlockLength) {
			hfzMemcpy(pBuf+offset, pBlock->pBlockData, pBlock->BlockLength); offset+=pBlock->BlockLength;
		}
	}

	fh.ExtHeaderLength  = extlen;
	*ppBuf = pBuf;

	return extlen;
}

long hfzHeader_DecodeExtHeaderBuf(hfzHeader &fh, char* pBuf) {

	if(fh.ExtHeaderLength==0)
		return LIBHFZ_STATUS_OK;

	fh.nExtHeaderBlocks = 0;

	// read once to determine number of blocks
	bool DoneFlag = false;
	unsigned int offset = 0;
	unsigned long BlockLen;
	do {
		if(offset==fh.ExtHeaderLength) {
			DoneFlag = true;
		} else
		if(fh.ExtHeaderLength-offset<24) {
			// not enough for a complete header
			return LIBHFZ_ERROR_INVALID_EXT_HEADER;
		} else {
			hfzMemcpy(&BlockLen, pBuf+offset+20, 4); // getting block length

			// swap byte order if required (files use little endian)
			if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
				hfzByteSwap(&BlockLen, 4);
			}

			offset+=24+BlockLen;
			fh.nExtHeaderBlocks++;
		}
	} while(!DoneFlag);

	//now allocate correct number of blocks
	fh.pExtHeaderBlocks = (hfzExtHeaderBlock*)hfzMalloc(sizeof(hfzExtHeaderBlock) * fh.nExtHeaderBlocks);
	if(!fh.pExtHeaderBlocks) {
		return LIBHFZ_ERROR_ALLOC_FAILED;
	}

	offset = 0;
	hfzExtHeaderBlock* pBlock;
	for(unsigned long i=0; i<fh.nExtHeaderBlocks; i++) {
		pBlock = &(fh.pExtHeaderBlocks[i]);
		hfzMemcpy(&(pBlock->BlockType), pBuf+offset, 4); offset+=4;
		hfzMemcpy(&(pBlock->BlockName), pBuf+offset, 16); offset+=16;
		hfzMemcpy(&(pBlock->BlockLength), pBuf+offset, 4); offset+=4;

		// swap byte order if required (files use little endian)
		if(LIBHFZ_BYTEORDER_BIGENDIAN==hfzByteOrder) {
			hfzByteSwap(&(pBlock->BlockLength), 4);
		}

		// forcing nulls
		pBlock->BlockType[3] = 0;
		pBlock->BlockName[15] = 0;

		if(pBlock->BlockLength) {
			pBlock->pBlockData = hfzMalloc(pBlock->BlockLength);
			hfzMemcpy(pBlock->pBlockData, pBuf+offset, pBlock->BlockLength); 
			offset+=pBlock->BlockLength;
		}
	}

	return fh.nExtHeaderBlocks;
}

long hfzExtHeaderBlock_Init(hfzExtHeaderBlock* pBlock, const char* lpBlockType, const char* lpBlockName, unsigned long BlockDataLength, void* pBlockData) {

	for(int i=0; i<4; i++)	pBlock->BlockType[i] = 0;
	for(int i=0; i<16; i++)	pBlock->BlockName[i] = 0;


	if(lpBlockType)	{
		if(strlen(lpBlockType)>=4)
			return LIBHFZ_ERROR_INVALID_PARAM;

		sprintf(pBlock->BlockType, "%s", lpBlockType);
	}

	if(lpBlockName)	{
		if(strlen(lpBlockName)>=16)
			return LIBHFZ_ERROR_INVALID_PARAM;

		sprintf(pBlock->BlockName, "%s", lpBlockName);
	}
	
	
	pBlock->BlockLength = BlockDataLength;

	if(!pBlock->BlockLength) {
		pBlock->pBlockData = 0;
	} else {
		if(!(pBlock->pBlockData = hfzMalloc(pBlock->BlockLength)))
			return LIBHFZ_ERROR_ALLOC_FAILED;

		hfzMemcpy(pBlock->pBlockData, pBlockData, pBlock->BlockLength);
	}

	return LIBHFZ_STATUS_OK;
}

long hfzExtHeaderBlock_InitEx(hfzHeader& fh, unsigned long BlockID, const char* lpBlockType, const char* lpBlockName, unsigned long BlockDataLength, void* pBlockData) {

	if(BlockID>=fh.nExtHeaderBlocks)
		return LIBHFZ_ERROR_INVALID_PARAM;

	return hfzExtHeaderBlock_Init(&(fh.pExtHeaderBlocks[BlockID]), lpBlockType, lpBlockName, BlockDataLength, pBlockData);
}

void hfzExtHeaderBlock_Reset(hfzExtHeaderBlock* pBlock) {
	for(int i=0; i<4; i++)	pBlock->BlockType[i] = 0;
	for(int i=0; i<16; i++)	pBlock->BlockName[i] = 0;

	if(pBlock->pBlockData) {
		hfzFree(pBlock->pBlockData);
		pBlock->pBlockData = 0;
	}

	pBlock->BlockLength = 0;
}

long hfzSetLocalByteOrder(long ByteOrder) {

	switch(ByteOrder) {
	case LIBHFZ_BYTEORDER_LITTLEENDIAN:
	case LIBHFZ_BYTEORDER_BIGENDIAN:
		break; // OK
	default:
		return LIBHFZ_ERROR_INVALID_PARAM;
	}

	hfzByteOrder = ByteOrder;
	return LIBHFZ_STATUS_OK;
}

long hfzByteSwap(void* pData, unsigned long DataSize) {
	if(!pData)		return LIBHFZ_ERROR_INVALID_HANDLE;
	if(!DataSize)	return LIBHFZ_ERROR_INVALID_PARAM;

	void* pSwap = hfzMalloc(DataSize);
	if(!pSwap)		return LIBHFZ_ERROR_ALLOC_FAILED;
	
	// re-sort into swap
	for(unsigned long i=0; i<DataSize; i++) {
		((char*)pSwap)[i] = ((char*)pData)[DataSize - i - 1];
	}

	// now copy back
	hfzMemcpy(pData, pSwap, DataSize);

	// and release swap memory
	hfzFree(pSwap);

	return LIBHFZ_STATUS_OK;
}
