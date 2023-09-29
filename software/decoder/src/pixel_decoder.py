#!/usr/bin/python3

from src.tpx3_luts import *
from src.structures import *

def convert_packet(data, colShiftNum, tpx_mode):

    colshifttbl = LUT_COLSHIFTS[colShiftNum]

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

    # frame[idx] = (decoded3 & 0x3FFF) | ((decoded2 & 0x3FF) << 14) | (( decoded1 & 0x1F) << 24);

    if tpx_mode == LLCP_TPX3_PXL_MODE_TOA_TOT:

        ftoa = (value1 + colshifttbl[x])
        tot = LUT_TOT[value2] if value2 >= 1 and value2 < MAX_LUT_TOT else WRONG_LUT_TOT
        toa = LUT_TOA[value3] if value3 >= 1 and value3 < MAX_LUT_TOA else WRONG_LUT_TOA

        data           = PixelDataToAToT()
        data.x         = x
        data.y         = y
        data.toa       = toa
        data.tot       = tot
        data.ftoa      = ftoa

    elif tpx_mode == LLCP_TPX3_PXL_MODE_TOA:

        ftoa = (value1 + colshifttbl[x])
        toa = LUT_TOA[value3] if value3 >= 0 and value3 < MAX_LUT_TOA else WRONG_LUT_TOA

        data           = PixelDataToA()
        data.x         = x
        data.y         = y
        data.toa       = toa
        data.ftoa      = ftoa

    elif tpx_mode == LLCP_TPX3_PXL_MODE_MPX_ITOT:

        decoded1 = LUT_EVENT[value1]
        decoded2 = LUT_TOT[value2]
        decoded3 = LUT_ITOT[value3] if value3 >= 0 and value3 < MAX_LUT_ITOT else WRONG_LUT_ITOT

        frame = (decoded3 & 0x3FFF) | ((decoded2 & 0x3FF) << 14) | (( decoded1 & 0x1F) << 24);

        mpx = decoded2
        itot = decoded3

        data           = PixelDataMpxiToT()
        data.x         = x
        data.y         = y
        data.itot      = itot
        data.mpx       = mpx

    else:
        print("Error: wrong Timepix3 mode specified, need {0, 1, 2}")
        return False

    return data
