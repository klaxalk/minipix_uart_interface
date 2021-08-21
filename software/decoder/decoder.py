#!/usr/bin/python3

# #{ imports

# math, arrays, etc.
import numpy
import math

# gui creator
import tkinter

# Figure and canvas
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

from src.structures import *
from src.parse_file import *

# #} end of imports

file_path = "data/data.txt"

 # Timepix3 mode {"toa_tot", "toa", "mpx_itot"}
tpx_mode = MODE_TOA_TOT

# #{ open the input file => list of "frame_data"

try:
    infile = open(file_path, "r", encoding="ascii")
except:
    print("[Error]: can not open input file!")
    exit()

# parse the input file, dehexify the data and decode the pixel values
# frame_data = list of all decoded messages from the MUI
frame_data = parseFile(infile, tpx_mode)

# #} open the input file

# #{ frame_data => list of numpy images

# image data map: image id => numpy image
images_data = {}

# list of image IDs (= list of keys for the image_data map)
id_list = []

for idx,frame in enumerate(frame_data):

    # if this is the first occurance of this frame_id, initialize new image for it
    if images_data.get(frame.frame_id) == None:

        id_list.append(frame.frame_id)

        if tpx_mode == MODE_TOA_TOT:
            images_data[frame.frame_id] = ImageToAToT()
        elif tpx_mode == MODE_TOA:
            images_data[frame.frame_id] = ImageToA()
        elif tpx_mode == MODE_MPX_ITOT:
            images_data[frame.frame_id] = ImageMpxiToT()

    # iterate over all the pixels within the frame and copy the pixel values to the numpy image
    for idx,pixel in enumerate(frame.pixel_data):

        if tpx_mode == MODE_TOA_TOT:

            if isinstance(images_data[frame.frame_id], ImageToAToT):
                images_data[frame.frame_id].tot[pixel.x, pixel.y]  = math.log(pixel.tot) if pixel.tot > 0 else 0
                images_data[frame.frame_id].toa[pixel.x, pixel.y]  = pixel.toa
                images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa

        elif tpx_mode == MODE_TOA:

            if isinstance(images_data[frame.frame_id], ImageToA):
                images_data[frame.frame_id].toa[pixel.x, pixel.y]  = pixel.toa
                images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa

        elif tpx_mode == MODE_MPX_ITOT:

            if isinstance(images_data[frame.frame_id], ImageMpxiToT):
                images_data[frame.frame_id].mpx[pixel.x, pixel.y]  = pixel.mpx
                images_data[frame.frame_id].itot[pixel.x, pixel.y] = pixel.itot

# #} end of frame_data => list of numpy images

## | --------------------------- gui -------------------------- |

# #{ initialize the window

root = tkinter.Tk()

# create the main Frame in the root window
frame_main = tkinter.Frame(root);
frame_main.pack(side=tkinter.TOP, fill=tkinter.BOTH, expand=1)

# create the left subframe for the image list
frame_left = tkinter.Frame(frame_main, bd=1);
frame_left.pack(side=tkinter.LEFT, fill=tkinter.Y, expand=0, padx=5, pady=5)

# create the right subframe for the figure
frame_right = tkinter.Frame(frame_main);
frame_right.pack(side=tkinter.RIGHT, fill=tkinter.BOTH, expand=1, padx=5, pady=5)

# #} end of initialize the window

# #{ create the figure

my_figure = Figure(facecolor='none', figsize=(8.2, 6.8), dpi=120)

# create the matplotlib's figure canvas
figure_canvas = FigureCanvasTkAgg(my_figure, master=frame_right)
figure_canvas.draw()
figure_canvas.get_tk_widget().pack(side=tkinter.TOP)
figure_canvas._tkcanvas.pack(side=tkinter.TOP)

subplot1 = my_figure.add_subplot(221)
subplot2 = my_figure.add_subplot(222)

# the TOA TOT mode has 3 plots
if tpx_mode == MODE_TOA_TOT:
    subplot3 = my_figure.add_subplot(223)

# #} end of create figure

# #{ create the listbox with a scrollbar

scrollbar = tkinter.Scrollbar(master=frame_left, orient=tkinter.VERTICAL)
listbox = tkinter.Listbox(master=frame_left, yscrollcommand=scrollbar.set, selectmode=tkinter.SINGLE)
listbox.pack(side=tkinter.LEFT, fill=tkinter.Y, expand=0)
scrollbar.config(command=listbox.yview)
scrollbar.pack(side=tkinter.LEFT, fill=tkinter.Y, expand=0)

# fill the listbox items with the image IDs (keys for the image map)
for key in images_data.keys():
    listbox.insert(tkinter.END, key)

# select the first item on the list
listbox.selection_set(0)

# #} end of create listbox

## | ------------------------- methods ------------------------ |

# #{ def loadImage()

def loadImage(key):

    if isinstance(images_data[key], ImageToAToT):

        subplot1.imshow(images_data[key].toa, interpolation='none')
        subplot1.set_title("ToA")

        subplot2.imshow(images_data[key].tot, interpolation='none')
        subplot2.set_title("ToT")

        subplot3.imshow(images_data[key].ftoa, interpolation='none')
        subplot3.set_title("FToA")

    elif isinstance(images_data[key], ImageToA):

        subplot1.imshow(images_data[key].toa, interpolation='none')
        subplot1.set_title("ToA")

        subplot2.imshow(images_data[key].ftoa, interpolation='none')
        subplot2.set_title("FToA")

    elif isinstance(images_data[key], ImageMpxiToT):

        subplot1.imshow(images_data[key].mpx, interpolation='none')
        subplot1.set_title("MPX")

        subplot2.imshow(images_data[key].itot, interpolation='none')
        subplot2.set_title("iToT")

    figure_canvas.draw()

# #} end of loadImage()

# #{ def listBoxOnSelect()

def listBoxOnSelect(evt):

    w = evt.widget

    # clicked but not selected
    if len(w.curselection()) == 0:
        return

    # extract the index of the selected item
    listbox_idx = int(w.curselection()[0])

    # load the image with the ID under the listbox idx
    loadImage(id_list[listbox_idx])

# bind the ListBoxOnSelect() method to the listbox selection event
listbox.bind('<<ListboxSelect>>', listBoxOnSelect)

# #} end of listBoxOnSelect()

# load the first image
loadImage(id_list[0])

# #{ window loop and deletion

def win_deleted():
    root.withdraw()
    root.quit()
    root.destroy()

root.protocol("WM_DELETE_WINDOW", win_deleted)
root.mainloop()
exit()

# #} end of window loop and deletion
