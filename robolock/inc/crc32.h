/*****************************************************************************
 *   crc32.h:  Ethernet CRC module file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __CRC32_H 
#define __CRC32_H

void   crc32_init(DWORD *pCRC);
void   crc32_add(DWORD *pCRC, BYTE val8);
void   crc32_end(DWORD *pCRC);
DWORD  crc32_bfr(void *pBfr, DWORD size);
DWORD do_crc_behav( long long Addr );
void Set_HashValue( DWORD CrcValue );

#endif
    
/*-----------------------------------------------------------*/
