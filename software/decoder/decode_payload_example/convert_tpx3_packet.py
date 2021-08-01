#!/usr/bin/python3

import sys
from tpx3luts import *

def convert_packet(data, colShiftNum, itot):

    colshifttbl = LUT_COLSHIFTS[colShiftNum]
    # print(" ".join("%02X" % x for x in data), cnt)
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

    print("x: {}".format(x))
    print("y: {}".format(y))
    print("toa: {}".format(toa))
    print("tot: {}".format(tot))
    print("ftoa: {}".format(ftoa))

# INSERT DATA - 6 bytes of payload
data = []
for i in range(0, 6):
  data.append(0)

# get data from the payload
convert_packet(data, 4, False)
