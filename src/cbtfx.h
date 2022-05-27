#ifndef CBTFX_H_INCLUDE
#define CBTFX_H_INCLUDE

#include <gb/gb.h>

#define CPTFX_UPDATE_ASM

void CBTFX_init(const unsigned char * SFX) NONBANKED;
void CBTFX_update(void) NONBANKED;
void CBTFX_update_isr(void) __interrupt __naked;

/*Source: http://devrs.com/gb/files/sndtab.html*/

#define FX_C_0 44
#define FX_Cs0 156
#define FX_D_0 262
#define FX_Ds0 363
#define FX_E_0 457
#define FX_F_0 547
#define FX_Fs0 631
#define FX_G_0 710
#define FX_Gs0 786
#define FX_A_0 854
#define FX_As0 923
#define FX_B_0 986
#define FX_C_1 1046
#define FX_Cs1 1102
#define FX_D_1 1155
#define FX_Ds1 1205
#define FX_E_1 1253
#define FX_F_1 1297
#define FX_Fs1 1339
#define FX_G_1 1379
#define FX_Gs1 1417
#define FX_A_1 1452
#define FX_As1 1486
#define FX_B_1 1517
#define FX_C_2 1546
#define FX_Cs2 1575
#define FX_D_2 1602
#define FX_Ds2 1627
#define FX_E_2 1650
#define FX_F_2 1673
#define FX_Fs2 1694
#define FX_G_2 1714
#define FX_Gs2 1732
#define FX_A_2 1750
#define FX_As2 1767
#define FX_B_2 1783
#define FX_C_3 1798
#define FX_Cs3 1812
#define FX_D_3 1825
#define FX_Ds3 1837
#define FX_E_3 1849
#define FX_F_3 1860
#define FX_Fs3 1871
#define FX_G_3 1881
#define FX_Gs3 1890
#define FX_A_3 1899
#define FX_As3 1907
#define FX_B_3 1915
#define FX_C_4 1923
#define FX_Cs4 1930
#define FX_D_4 1936
#define FX_Ds4 1943
#define FX_E_4 1949
#define FX_F_4 1954
#define FX_Fs4 1959
#define FX_G_4 1964
#define FX_Gs4 1969
#define FX_A_4 1974
#define FX_As4 1978
#define FX_B_4 1982
#define FX_C_5 1985
#define FX_Cs5 1988
#define FX_D_5 1992
#define FX_Ds5 1995
#define FX_E_5 1998
#define FX_F_5 2001
#define FX_Fs5 2004
#define FX_G_5 2006
#define FX_Gs5 2009
#define FX_A_5 2011
#define FX_As5 2013
#define FX_B_5 2015

#endif
