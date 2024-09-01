#ifndef VGM_OPL3_H
#define VGM_OPL3_H

#define VGM_OPL3_GET_LSITEST1(X) X

#define VGM_OPL3_GET_TIMER1(X) X

#define VGM_OPL3_GET_TIMER2(X) X

#define VGM_OPL3_GET_ST1(X) (X & 0x01)
#define VGM_OPL3_GET_ST2(X) ((X >> 1) & 0x01)
#define VGM_OPL3_GET_MT2(X) ((X >> 5) & 0x01)
#define VGM_OPL3_GET_MT1(X) ((X >> 6) & 0x01)
#define VGM_OPL3_GET_RST(X) ((X >> 7) & 0x01)

#define VGM_OPL3_GET_NTS(X) ((X >> 6) & 0x01)

#define VGM_OPL3_GET_MULT(X) (X & 0x0f)
#define VGM_OPL3_GET_KSR(X) ((X >> 4) & 0x01)
#define VGM_OPL3_GET_EGT(X) ((X >> 5) & 0x01)
#define VGM_OPL3_GET_VIB(X) ((X >> 6) & 0x01)
#define VGM_OPL3_GET_AM(X) ((X >> 7) & 0x01)

#define VGM_OPL3_GET_TL(X) (X & 0x3f)
#define VGM_OPL3_GET_KSL(X) (X >> 6)

#define VGM_OPL3_GET_DR(X) (X & 0x0f)
#define VGM_OPL3_GET_AR(X) (X >> 4)

#define VGM_OPL3_GET_RR(X) (X & 0x0f)
#define VGM_OPL3_GET_SL(X) (X >> 4)

#define VGM_OPL3_GET_FNUML(X) (X)

#define VGM_OPL3_GET_FNUMH(X) (X & 0x03)
#define VGM_OPL3_GET_BLOCK(X) ((X >> 2) & 0x07)
#define VGM_OPL3_GET_KON(X) ((X >> 5) & 0x01)

#define VGM_OPL3_GET_HH(X) (X & 0x01)
#define VGM_OPL3_GET_TC(X) ((X >> 1) & 0x01)
#define VGM_OPL3_GET_TOM(X) ((X >> 2) & 0x01)
#define VGM_OPL3_GET_SD(X) ((X >> 3) & 0x01)
#define VGM_OPL3_GET_BD(X) ((X >> 4) & 0x01)
#define VGM_OPL3_GET_RYT(X) ((X >> 5) & 0x01)
#define VGM_OPL3_GET_DVB(X) ((X >> 6) & 0x01)
#define VGM_OPL3_GET_DAM(X) ((X >> 7) & 0x01)

#define VGM_OPL3_GET_CNT(X) (X & 0x01)
#define VGM_OPL3_GET_FB(X) ((X >> 1) & 0x07)
#define VGM_OPL3_GET_STR(X) ((X >> 4) & 0x01)
#define VGM_OPL3_GET_STL(X) ((X >> 5) & 0x01)
#define VGM_OPL3_GET_EX0(X) ((X >> 6) & 0x01)
#define VGM_OPL3_GET_EX1(X) ((X >> 7) & 0x01)

#define VGM_OPL3_GET_WS(X) (X & 0x07)

#define VGM_OPL3_GET_LSITEST2(X) (X & 0x3f)

#define VGM_OPL3_GET_CONNECTIONSEL(X) (X & 0x3f)

#define VGM_OPL3_GET_NEW(X) (X & 0x01)

#define VGM_OPL3_SET_LSITEST1(X, Y) Y

#define VGM_OPL3_SET_TIMER1(X, Y) Y

#define VGM_OPL3_SET_TIMER2(X, Y) Y

#define VGM_OPL3_SET_ST1(X, Y) ((X & ~1) | Y)
#define VGM_OPL3_SET_ST2(X, Y) ((X & ~(1 << 1)) | (Y << 1))
#define VGM_OPL3_SET_MT2(X, Y) ((X & ~(1 << 5)) | (Y << 5))
#define VGM_OPL3_SET_MT1(X, Y) ((X & ~(1 << 6)) | (Y << 6))
#define VGM_OPL3_SET_RST(X, Y) ((X & ~(1 << 7)) | (Y << 7))

#define VGM_OPL3_SET_NTS(X, Y) ((X & ~(1 << 6)) | (Y << 6))

#define VGM_OPL3_SET_MULT(X, Y) ((X & 0xf0) | (Y & 0x0f))
#define VGM_OPL3_SET_KSR(X, Y) ((X & ~(1 << 4)) | (Y << 4))
#define VGM_OPL3_SET_EGT(X, Y) ((X & ~(1 << 5)) | (Y << 5))
#define VGM_OPL3_SET_VIB(X, Y) ((X & ~(1 << 6)) | (Y << 6))
#define VGM_OPL3_SET_AM(X, Y) ((X & ~(1 << 7)) | (Y << 7))

#define VGM_OPL3_SET_TL(X, Y) ((X & 0xc0) | (Y & 0x3f))
#define VGM_OPL3_SET_KSL(X, Y) ((X & 0x3f) | ((Y & 0x03) << 6))

#define VGM_OPL3_SET_DR(X, Y) ((X & 0xf0) | (Y & 0x0f))
#define VGM_OPL3_SET_AR(X, Y) ((X & 0x0f) | ((Y & 0x0f) << 4))

#define VGM_OPL3_SET_RR(X, Y) ((X & 0xf0) | (Y & 0x0f))
#define VGM_OPL3_SET_SL(X, Y) ((X & 0x0f) | ((Y & 0x0f) << 4))

#define VGM_OPL3_SET_FNUML(X, Y) (Y)

#define VGM_OPL3_SET_FNUMH(X, Y) ((X & 0xfc) | (Y & 0x03))
#define VGM_OPL3_SET_BLOCK(X, Y) ((X & 0x03) | ((Y & 0x07) << 2) | (X & 0xe0))
#define VGM_OPL3_SET_KON(X, Y) ((X & ~(1 << 5)) | (Y << 5))

#define VGM_OPL3_SET_HH(X, Y) ((X & ~1) | Y)
#define VGM_OPL3_SET_TC(X, Y) ((X & ~(1 << 1)) | (Y << 1))
#define VGM_OPL3_SET_TOM(X, Y) ((X & ~(1 << 2)) | (Y << 2))
#define VGM_OPL3_SET_SD(X, Y) ((X & ~(1 << 3)) | (Y << 3))
#define VGM_OPL3_SET_BD(X, Y) ((X & ~(1 << 4)) | (Y << 4))
#define VGM_OPL3_SET_RYT(X, Y) ((X & ~(1 << 5)) | (Y << 5))
#define VGM_OPL3_SET_DVB(X, Y) ((X & ~(1 << 6)) | (Y << 6))
#define VGM_OPL3_SET_DAM(X, Y) ((X & ~(1 << 7)) | (Y << 7))

#define VGM_OPL3_SET_CNT(X, Y) ((X & ~1) | Y)
#define VGM_OPL3_SET_FB(X, Y) ((X & 1) | ((Y & 0x07) << 1) | (X & 0xf0))
#define VGM_OPL3_SET_STR(X, Y) ((X & ~(1 << 4)) | (Y << 4))
#define VGM_OPL3_SET_STL(X, Y) ((X & ~(1 << 5)) | (Y << 5))
#define VGM_OPL3_SET_EX0(X, Y) ((X & ~(1 << 6)) | (Y << 6))
#define VGM_OPL3_SET_EX1(X, Y) ((X & ~(1 << 7)) | (Y << 7))

#define VGM_OPL3_SET_WS(X, Y) (Y & 0x07)

#define VGM_OPL3_SET_LSITEST2(X, Y) (Y & 0x3f)

#define VGM_OPL3_SET_CONNECTIONSEL(X, Y) (Y & 0x3f)

#define VGM_OPL3_SET_NEW(X, Y) ((X & ~1) | Y)

#endif