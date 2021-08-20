# for convertin hexadecimal to binary
import binascii

from src.structures import *
from src.convert_tpx3_packet import *

def bytesToInt16(byte1, byte2):

    return byte1<<8 | byte2

def bytesToInt32(byte1, byte2, byte3, byte4):

    return byte1<<24 | byte2<<16 | byte3<<8 | byte4

def parseFile(infile):

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
            frame_data.frame_id         = bytesToInt16(data[2], data[1])
            frame_data.packet_id        = bytesToInt16(data[4], data[3])
            frame_data.mode             = data[5]
            frame_data.n_pixels         = data[6]
            frame_data.checksum_matched = data[7]

            # for each pixel in the packet
            for i in range(0, frame_data.n_pixels):

                pixel_data = []

                # for each byt coding the pixel data
                for j in range(0, 6):

                    pixel_data.append(data[8 + i*6 + j])

                # deserialize and derandomize the pixel data
                pixel_data = convert_packet(pixel_data, 4)

                if pixel_data:
                    frame_data.pixel_data.append(pixel_data)

            data_out.append(frame_data)

    return data_out
