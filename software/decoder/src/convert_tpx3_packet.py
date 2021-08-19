#!/usr/bin/python3

import sys
from src.tpx3luts import *

class PixelDataToAToT:

    def __init__(self):

        self.ftoa = 0
        self.toa = 0
        self.tot = 0
        self.x = 0
        self.y = 0
        self.mode_mask = 0

class PixelDataToA:

    def __init__(self):

        self.ftoa = 0
        self.toa = 0
        self.x = 0
        self.y = 0
        self.mode_mask = 0

class PixelDataMpxiToT:

    def __init__(self):

        self.event_counter = 0
        self.itot = 0
        self.x = 0
        self.y = 0
        self.mode_mask = 0

def convert_packet(data, colShiftNum, itot):

    colshifttbl = LUT_COLSHIFTS[colShiftNum]
    address = (data[0] & 0x0F) << 12 | (data[1] << 4) | ((data[2] >> 4) & 0x0F)
    toa = ((data[2] & 0x0F) << 10) | (data[3] << 2) | ((data[4] >> 6) & 0x03)
    tot = ((data[4] & 0x3F) << 4) | ((data[5] >> 4) & 0x0F)
    ftoa = (data[5] & 0x0F)
    eoc = (address >> 9) & 0x7F
    sp = (address >> 3) & 0x3F
    pix = address & 0x07
    x = eoc * 2 + (pix // 4)
    y = (sp * 4 + (pix % 4))
    idx = y * 256 + x

    if itot:
        toa = LUT_ITOT[toa] if toa >= 1 and toa < MAX_LUT_ITOT else WRONG_LUT_ITOT
        ftoa = LUT_EVENT[ftoa] if ftoa >= 1 and ftoa < MAX_LUT_EVENT else WRONG_LUT_EVENT
    else:
        toa = LUT_TOA[toa] if toa >= 0 and toa < MAX_LUT_TOA else WRONG_LUT_TOA
        ftoa = (ftoa + colshifttbl[x])

    tot = LUT_TOT[tot] if tot >= 1 and tot < MAX_LUT_TOT else WRONG_LUT_TOT

    data = PixelDataToAToT()
    data.x = x
    data.y = y
    data.toa = toa
    data.tot = tot
    data.ftoa = ftoa

    return data
