#!/usr/bin/python3

# #{ imports

# math, arrays, etc.
import numpy
import math
import csv

from src.structures import *
from src.parse_file import *

from src.export_methods import *

# #} end of imports

# #{ exportDsc()

def exportDsc(file_path, image_mode, image_id):

    with open(file_path, "w") as dsc_file:

        dsc_file.write("A{0:09d}\r\n\
[F0]\r\n\
Type=double matrix width={1} height={2}\r\n\
\"Acq mode\" (\"Acquisition mode\"):\r\n\
i32[1]\r\n\
{3}\r\n\
".format(1, 256, 256, image_mode))

        dsc_file.write("\r\n")

        dsc_file.write("\"ImageID\" (\"Unique ID assigned to the image by the satellite\"):\r\n\
u32[1]\r\n\
{}\r\n".format(image_id))

# #} end of exportDsc()

# #{ exportData()

def exportData(file_path, image):

    with open(file_path, "w") as data_file:

        writer = csv.writer(data_file, quoting=csv.QUOTE_NONE, delimiter=',')

        for i in range(0, 256):
            writer.writerow(["{}".format(x) for x in image[i, :]])


# the file should containt 1 packet of FrameDataMsg_t() per line in HEXadecimal form
file_path = "data/oneweb.csv"

# #} end of exportData()

if __name__ == '__main__':

    # #{ open the input file => list of "frame_data"

    try:
        infile = open(file_path, "r", encoding="ascii")
    except:
        print("[Error]: can not open input file!")
        exit()

    # parse the input file, dehexify the data and decode the pixel values
    # frame_data = list of all decoded messages from the MUI
    frame_data = parseStream(infile)

    # #} open the input file

    # #{ frame_data => list of numpy images

    # image data map: image id => numpy image
    images_data = {}

    # enumerate the frame_data (decoded packets) and stitch them together
    for idx,frame in enumerate(frame_data):

        # if this is the first occurance of this frame_id, initialize new numpy image for it
        if images_data.get(frame.frame_id) == None:

            # initialize the right type of image
            if frame.mode == LLCP_TPX3_PXL_MODE_TOA_TOT:
                images_data[frame.frame_id] = ImageToAToT()
            elif frame.mode == LLCP_TPX3_PXL_MODE_TOA:
                images_data[frame.frame_id] = ImageToA()
            elif frame.mode == LLCP_TPX3_PXL_MODE_MPX_ITOT:
                images_data[frame.frame_id] = ImageMpxiToT()

        # iterate over all the pixels within the frame and copy the pixel values to the numpy image
        for idx,pixel in enumerate(frame.pixel_data):

            if frame.mode == LLCP_TPX3_PXL_MODE_TOA_TOT:

                if isinstance(images_data[frame.frame_id], ImageToAToT):

                    # TOA is the prettiest part and it shows nicely in log()
                    # this is not, ofcourse, an official transformation, please DO NOT USE it if you are going to process the data
                    images_data[frame.frame_id].tot[pixel.x, pixel.y]  = math.log(pixel.tot) if pixel.tot > 0 else 0
                    # images_data[frame.frame_id].tot[pixel.x, pixel.y]  pixel.tot

                    images_data[frame.frame_id].toa[pixel.x, pixel.y]  = pixel.toa
                    images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa

            elif frame.mode == LLCP_TPX3_PXL_MODE_TOA:

                if isinstance(images_data[frame.frame_id], ImageToA):
                    images_data[frame.frame_id].toa[pixel.x, pixel.y]  = pixel.toa
                    images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa

            elif frame.mode == LLCP_TPX3_PXL_MODE_MPX_ITOT:

                if isinstance(images_data[frame.frame_id], ImageMpxiToT):
                    images_data[frame.frame_id].mpx[pixel.x, pixel.y]  = pixel.mpx
                    images_data[frame.frame_id].itot[pixel.x, pixel.y] = pixel.itot

    # sort the image data by id
    sorted_keys = list(images_data.keys())
    sorted_keys.sort()
    sorted_data = {i: images_data[i] for i in sorted_keys}

    images_data = sorted_data

    # #} end of frame_data => list of numpy images

    ## | -------------------- export the images ------------------- |

    for idx,key in enumerate(images_data):

        image = images_data.get(key)

        print("exporting: {}".format(key))

        if isinstance(image, ImageToAToT):

            dsc_file_path = "data_export/{}.dsc".format(key)
            toa_file_path = "data_export/toa_{}.txt".format(key)
            tot_file_path = "data_export/tot_{}.txt".format(key)

            exportDsc(dsc_file_path, key, "ToAToT")
            exportData(toa_file_path, image.toa)
            exportData(tot_file_path, image.tot)

        if isinstance(image, ImageToA):

            dsc_file_path = "data_export/{}.dsc".format(key)
            toa_file_path = "data_export/toa_{}.txt".format(key)

            exportDsc(dsc_file_path, key, "ToA")
            exportData(toa_file_path, image.toa)

        if isinstance(image, ImageMpxiToT):

            dsc_file_path = "data_export/{}.dsc".format(key)
            mpx_file_path = "data_export/mpx_{}.txt".format(key)
            itot_file_path = "data_export/itot_{}.txt".format(key)

            exportDsc(dsc_file_path, key, "MpxiToT")
            exportData(mpx_file_path, image.mpx)
            exportData(itot_file_path, image.itot)
