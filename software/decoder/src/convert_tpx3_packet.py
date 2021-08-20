#!/usr/bin/python3

from src.tpx3luts import *
from src.structures import *

def convert_packet(data, colShiftNum):

    colshifttbl = LUT_COLSHIFTS[colShiftNum]

    mode_mask   = (data[0] & 0xF0) >> 4
    address     = (data[0] & 0x0F) << 12 | (data[1] << 4) | ((data[2] >> 4) & 0x0F)
    value3      = ((data[2] & 0x0F) << 10) | (data[3] << 2) | ((data[4] >> 6) & 0x03)
    value2      = ((data[4] & 0x3F) << 4) | ((data[5] >> 4) & 0x0F)
    value1      = (data[5] & 0x0F)
    eoc         = (address >> 9) & 0x7F
    sp          = (address >> 3) & 0x3F
    pix         = address & 0x07
    x           = eoc * 2 + (pix // 4)
    y           = (sp * 4 + (pix % 4))
    idx         = y * 256 + x

    # mode_mask = MODE_TOA_TOT

    if mode_mask == MODE_TOA_TOT:

        ftoa = (value1 + colshifttbl[x])
        tot = LUT_TOT[value2] if value2 >= 1 and value2 < MAX_LUT_TOT else WRONG_LUT_TOT
        toa = LUT_TOA[value3] if value3 >= 1 and value3 < MAX_LUT_TOA else WRONG_LUT_TOA

        data           = PixelDataToAToT()
        data.x         = x
        data.y         = y
        data.toa       = toa
        data.tot       = tot
        data.ftoa      = ftoa
        data.mode_mask = mode_mask

    elif mode_mask == MODE_TOA:

        ftoa = (value1 + colshifttbl[x])
        toa = LUT_TOA[value3] if value3 >= 0 and value3 < MAX_LUT_TOA else WRONG_LUT_TOA

        data           = PixelDataToA()
        data.x         = x
        data.y         = y
        data.toa       = toa
        data.ftoa      = ftoa
        data.mode_mask = mode_mask

    elif mode_mask == MODE_MPX_ITOT:

        mpx = LUT_EVENT[value2] if value2 >= 1 and value2 < MAX_LUT_EVENT else WRONG_LUT_EVENT
        itot = LUT_ITOT[value3] if value3 >= 1 and value3 < MAX_LUT_ITOT else WRONG_LUT_ITOT

        data           = PixelDataMpxiToT()
        data.x         = x
        data.y         = y
        data.itot      = itot
        data.mpx       = mpx
        data.mode_mask = mode_mask

    else:
        print("wring mode detecting while reconstructing a pixel [{}, {}]: {}".format(x, y, mode_mask))
        return False

    return data
