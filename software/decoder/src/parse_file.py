#!/usr/bin/python3

# for converting hexadecimal to binary
import binascii
import csv

from src.structures import *
from src.pixel_decoder import *
from src.llcp import *

def bytesToInt16(byte1, byte2):

    return byte1<<8 | byte2

def parseFile(infile):

    data_out = []

    line_num = 0

    for line in infile:

        line_num = line_num + 1

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

                decoding_error = False

                # for each one of the 6 bytes encoding the pixel data
                for j in range(0, 6):

                    try:
                        pixel_data.append(data[8 + i*6 + j])
                    except:
                        print("line could not be parsed")
                        decoding_error = True
                        continue

                if decoding_error:
                    continue

                # deserialize and derandomize the pixel data
                # pixel_data should be one of 3 classes
                # * PixelDataToAToT()
                # * PixelDataToA()
                # * PixelDataMpxiToT()
                pixel_data = convert_packet(pixel_data, 4, frame_data.mode)

                if pixel_data:
                    frame_data.pixel_data.append(pixel_data)

            data_out.append(frame_data)

    return data_out

def parseStream(infile):

    # Define the separator string
    separator = '55AA55AA5555AA55AA55'

    timestamp_list = []
    data_list = []

    csv_reader = csv.reader(infile)

    # Initialize variables to store data and timestamp
    data = ""
    timestamp = None

    for row in csv_reader:
        # Check if the separator is in the row row = [timestamp, data]

        row_timestamp = row[0]
        row_data = row[1]

        if row_timestamp == "TIMESTAMP":
            continue

        if separator in row_data:
            # Find the index of the separator in the data
            separator_index = row_data.index(separator)

            # Concatenate the data before the separator
            data += row_data[0:separator_index]
            #print("data: ", data)

            # output_file.write(timestamp + "," + data + "\n")
            timestamp_list.append(timestamp)
            data_list.append(data)

            data = ""  # Reset data for the next string
            timestamp = None

        else:
            # Save the data in this row
            if timestamp == None:
                timestamp = row_timestamp
            data += row_data

    data_out = []

    for data_idx,data in enumerate(data_list):

        try:
            byte_stream = binascii.unhexlify(data)
        except:
            print("Unhexification failed on data: {}".format(hex_data))
            continue

        idx = 0

        while idx < len(byte_stream):

            # print("idx: {} < {}".format(idx, len(byte_stream)))

            if byte_stream[idx] == LLCP_STATUS_MSG_ID:
                print("status")
                idx = idx + LLCP_STATUS_MSG_SIZE
                continue

            if byte_stream[idx] == LLCP_TEMPERATURE_MSG_ID:

                temperature = bytesToInt16(byte_stream[idx+2], byte_stream[idx+1])

                if temperature >= -100 and temperature <= 200:
                    print("Parsed temperature: timestamp: {}, temp: {}".format(timestamp, temperature))

                idx = idx + LLCP_TEMPERATURE_MSG_SIZE

                continue

            if byte_stream[idx] == LLCP_FRAME_DATA_TERMINATOR_MSG_ID:

                frame_id = error_id = bytesToInt16(byte_stream[idx+2], byte_stream[idx+1])
                num_of_packets = error_id = bytesToInt16(byte_stream[idx+4], byte_stream[idx+3])

                print("terminator, frame_id = {}, num_of_packetss = {}".format(frame_id, num_of_packets))

                idx = idx + LLCP_FRAME_DATA_TERMINATOR_MSG_SIZE

                continue

            if byte_stream[idx] == LLCP_FRAME_MEASUREMENT_FINISHED_MSG_ID:
                print("measurement finished")
                idx = idx + LLCP_FRAME_MEASUREMENT_FINISHED_MSG_SIZE
                continue

            if byte_stream[idx] == LLCP_ACK_MSG_ID:
                print("ack")
                idx = idx + LLCP_ACK_MSG_SIZE
                continue

            if byte_stream[idx] == LLCP_MINIPIX_ERROR_MSG_ID:

                error_id = byte_stream[idx+1]

                if error_id >= 0 and error_id <= 10:
                  print("Parsed error: {}".format(LLCP_MinipixErrors[error_id]))

                idx = idx + LLCP_MINIPIX_ERROR_MSG_SIZE
                continue

            if byte_stream[idx] == LLCP_FRAME_DATA_MSG_ID:

                frame_data = FrameDataMsg()

                # extract the bytes
                # the following ones are not encoded and can be just coppied
                frame_data.frame_id         = bytesToInt16(byte_stream[idx+2], byte_stream[idx+1])
                frame_data.packet_id        = bytesToInt16(byte_stream[idx+4], byte_stream[idx+3])
                frame_data.mode             = byte_stream[idx+5]
                frame_data.n_pixels         = byte_stream[idx+6]
                frame_data.checksum_matched = byte_stream[idx+7]

                final_idx = 0

                # the following pixel data are encoded and need to be
                # deserialized and derandomized
                # for each pixel in the packet
                for i in range(0, frame_data.n_pixels):

                    # list of 6 bytes
                    pixel_data = []

                    decoding_error = False

                    # for each one of the 6 bytes encoding the pixel data
                    for j in range(0, 6):

                        try:
                            final_idx = idx + 8 + i*6 + j
                            pixel_data.append(byte_stream[final_idx])
                        except:
                            decoding_error = True
                            continue

                    if decoding_error:
                        continue

                    # deserialize and derandomize the pixel data
                    # pixel_data should be one of 3 classes
                    # * PixelDataToAToT()
                    # * PixelDataToA()
                    # * PixelDataMpxiToT()
                    pixel_data = convert_packet(pixel_data, 4, frame_data.mode)

                    if pixel_data:
                        frame_data.pixel_data.append(pixel_data)

                if frame_data.mode >= 0 and frame_data.mode <= 2:
                    data_out.append(frame_data)

                if final_idx > 0:
                  idx = final_idx + 1
                else:
                  idx = idx + 8
                continue

            idx = idx + 1
            # print("byte skip {}".format(idx))

    return data_out
