#!/usr/bin/python3

# for converting hexadecimal to binary
import binascii

from src.structures import *
from src.pixel_decoder import *

def bytesToInt16(byte1, byte2):

    return byte1<<8 | byte2

def parseFile(infile, tpx_mode):

    data_out = []

    for line in infile:

        # cut the endl character
        hex_data = line[:-1]

        # convert hexadecimal to binary
        try:
            data = binascii.unhexlify(hex_data)
        except:
            print("Unhexification failed on data: {}".format(hex_data))
            continue

        # 20 = ID specific to the FrameDataMsg()
        if data[0] == 20:

            frame_data = FrameDataMsg()

            # extract the bytes
            # the following ones are not encoded and can be just coppied
            frame_data.frame_id         = bytesToInt16(data[2], data[1])
            frame_data.packet_id        = bytesToInt16(data[4], data[3])
            frame_data.mode             = data[5]
            frame_data.n_pixels         = data[6]
            frame_data.checksum_matched = data[7]

            # the following pixel data are encoded and need to be
            # deserialized and derandomized
            # for each pixel in the packet
            for i in range(0, frame_data.n_pixels):

                # list of 6 bytes
                pixel_data = []

                # for each one of the 6 bytes encoding the pixel data
                for j in range(0, 6):

                    pixel_data.append(data[8 + i*6 + j])

                # deserialize and derandomize the pixel data
                # pixel_data should be one of 3 classes
                # * PixelDataToAToT()
                # * PixelDataToA()
                # * PixelDataMpxiToT()
                pixel_data = convert_packet(pixel_data, 4, tpx_mode)

                if pixel_data:
                    frame_data.pixel_data.append(pixel_data)

            data_out.append(frame_data)

    return data_out
