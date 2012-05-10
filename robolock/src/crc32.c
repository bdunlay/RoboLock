/*****************************************************************************
 *   crc32.c:  Ethernet CRC module file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "LPC23xx.h"                        /* LPC23xx/24xx definitions */
#include "type.h"
#include "crc32.h"

/******************************************************************************
** Function name:		CRC_init
**
** Descriptions:		Begin CRC calculation.		
**
** parameters:			pointer to the CRC area.
** Returned value:		None
** 
******************************************************************************/
void crc32_init(DWORD *pCRC)
{
    *pCRC = 0xffffffff;
}

/******************************************************************************
** Function name:		CRC32_add
**
** Descriptions:		Calculate CRC value one at a time
**
** parameters:			pointer to the CRC area, and passing value to get the CRC
** Returned value:		None
** 
******************************************************************************/
void crc32_add(DWORD *pCRC, BYTE val8)
{
    DWORD i, poly;
    DWORD entry;
    DWORD crc_in;
    DWORD crc_out;

    crc_in = *pCRC;
    poly = 0xEDB88320L;
    entry = (crc_in ^ ((DWORD) val8)) & 0xFF;
    for (i = 0; i < 8; i++)
    {
        if (entry & 1)
            entry = (entry >> 1) ^ poly;
        else
            entry >>= 1;
    }
    crc_out = ((crc_in>>8) & 0x00FFFFFF) ^ entry;
    *pCRC = crc_out;
    return;
}

/******************************************************************************
** Function name:		CRC32_end
**
** Descriptions:		Finish CRC calculation
**
** parameters:			pointer to the CRC area.
** Returned value:		None
** 
******************************************************************************/
void crc32_end(DWORD *pCRC)
{
    *pCRC ^= 0xffffffff;
}

/******************************************************************************
** Function name:		CRC32_bfr
**
** Descriptions:		Get the CRC value based on size of the string. 		
**
** parameters:			Pointer to the string, size of the string.
** Returned value:		CRC value
** 
******************************************************************************/
DWORD crc32_bfr(void *pBfr, DWORD size)
{
    DWORD crc32;
    BYTE  *pu8;

    crc32_init(&crc32);
    pu8 = (BYTE *) pBfr;
    while (size-- != 0)
    {
        crc32_add(&crc32, *pu8);
        pu8++ ;
    }
    crc32_end(&crc32);
    return ( crc32 );
}

/******************************************************************************
** Function name:		do_crc_behav
**
** Descriptions:		calculate CRC
**
** parameters:			MAC address
** Returned value:		CRC value
** 
******************************************************************************/
DWORD do_crc_behav( long long Addr ) 
{
    /* state variables */
    int crc;
    /* declare temporary variables */
    int q0, q1, q2, q3;
    /* loop variables */
    int i, j, d;
  
    /* calculate CRC */
    crc = 0xFFFFFFFF;    
    /* do for each byte */
    for (i = 5; i >= 0; i--) 
    {
		d = Addr >> (i * 8);
		for (j = 0; j < 2; j++) 
		{
	    	/* calculate temporary variables */
	    	/* bits: 26,23,22,16,12,11,10,8,7,5,4,2,1,0 */
	    	q3 = (((crc >> 28) ^ (d >> 3)) & 0x00000001) ? 0x04C11DB7 : 0x00000000;
	    	/* bits: 27,24,23,17,13,12,11,9,8,6,5,3,2,1 */
	    	q2 = (((crc >> 29) ^ (d >> 2)) & 0x00000001) ? 0x09823B6E : 0x00000000;
	    	/* bits: 28,25,24,18,14,13,12,10,9,7,6,4,3,2 */
	    	q1 = (((crc >> 30) ^ (d >> 1)) & 0x00000001) ? 0x130476DC : 0x00000000;
	    	/* bits: 29,26,25,19,15,14,13,11,10,8,7,5,4,3 */
	    	q0 = (((crc >> 31) ^  d)       & 0x00000001) ? 0x2608EDB8 : 0x00000000;
	    	/* do crc */
	    	crc = (crc << 4) ^ q3 ^ q2 ^ q1 ^ q0;
	    	/* shift data */
	    	d >>= 4;
		}
    }
    return ( crc );
}

/******************************************************************************
** Function name:		Set_HashValue
**
** Descriptions:		Set Hash value		
**
** parameters:			CRC value
** Returned value:		None
** 
******************************************************************************/
void Set_HashValue( DWORD CrcValue )
{
    DWORD HashFilterFlag = 0;
    DWORD hashMsb6_1;
    DWORD hashIndex1;
    
    hashMsb6_1 = (CrcValue & 0x1f800000);
    hashMsb6_1 = ( (hashMsb6_1 >> 23) & 0x3f);
	
    if ( hashMsb6_1 >= 32 )
    {
		HashFilterFlag = 1;
		hashMsb6_1 -= 32;
    }
	
    hashIndex1 = 1 << hashMsb6_1;	
	
    if(HashFilterFlag == 1)
    {
		MAC_HASHFILTERH = hashIndex1;
		MAC_HASHFILTERL = 0x0;
    }
    else
    {
		MAC_HASHFILTERH = 0x0;
		MAC_HASHFILTERL = hashIndex1;
    }

    /* When the Hash filter is enabled, the PassRxFilter bit in MAC_COMMAND 
    needs to be enabled too. */
    MAC_COMMAND &= ~0x0080;
    return;
}

/*********************************************************************************
**                            End Of File
*********************************************************************************/
