#!/usr/bin/python

# #{ imports

# math, arrays, etc.
import numpy

import matplotlib.pyplot as plt
from matplotlib.figure import Figure

import tkinter as Tk

try:
    from matplotlib.backends.backend_tkagg import (NavigationToolbar2TkAgg, FigureCanvasTkAgg)
except ImportError:
    from matplotlib.backends.backend_tkagg import NavigationToolbar2Tk as NavigationToolbar2TkAgg
    from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg as FigureCanvasTkAgg

from src.parse_file import *

# #} end of imports

# #{ config

file_path = "data/data.txt"

# #} end of config

# #{ parse input file

# open the file
try:
    infile = open(file_path, "r", encoding="ascii")
except:
    print("[Error]: can not open input file!")
    exit()

frame_data = parseFile(infile)

# for all lines in the file

# #} end of parse input file

# #{ class ImageToAToT

class ImageToAToT:

    def __init__(self):

        self.tot = numpy.zeros(shape=[256, 256])
        self.toa = numpy.zeros(shape=[256, 256])
        self.ftoa = numpy.zeros(shape=[256, 256])

# #} end of class Image

# #{ class ImageToA

class ImageToA:

    def __init__(self):

        self.toa = numpy.zeros(shape=[256, 256])
        self.ftoa = numpy.zeros(shape=[256, 256])

# #} end of class Image

# #{ class ImageMpxiToT

class ImageMpxiToT:

    def __init__(self):

        self.event_counter = numpy.zeros(shape=[256, 256])
        self.itot = numpy.zeros(shape=[256, 256])

# #} end of class Image

images_data = {}

for idx,frame in enumerate(frame_data):

    measurement_mode = frame.mode

    if images_data.get(frame.frame_id) == None:

        if measurement_mode == MODE_TOA_TOT:
            images_data[frame.frame_id] = ImageToAToT()
        elif measurement_mode == MODE_TOT:
            images_data[frame.frame_id] = ImageToT()
        elif measurement_mode == MODE_TOT:
            images_data[frame.frame_id] = ImageMpxiToT()

        print("instancing image id {} mode {}".format(frame.frame_id, measurement_mode))

    for idx,pixel in enumerate(frame.pixel_data):

        if measurement_mode == MODE_TOA_TOT:
            images_data[frame.frame_id].tot[pixel.x, pixel.y] = pixel.tot
            images_data[frame.frame_id].toa[pixel.x, pixel.y] = pixel.toa
            images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa
        elif measurement_mode == MODE_TOA:
            images_data[frame.frame_id].toa[pixel.x, pixel.y] = pixel.toa
            images_data[frame.frame_id].ftoa[pixel.x, pixel.y] = pixel.ftoa
        elif measurement_mode == MODE_MPX_ITOT:
            images_data[frame.frame_id].mpx[pixel.x, pixel.y] = pixel.mpx
            images_data[frame.frame_id].itot[pixel.x, pixel.y] = pixel.itot

image_keys = images_data.keys()

root = Tk.Tk()

# create the main Frame in the root window
frame_main = Tk.Frame(root);
frame_main.pack(side=Tk.TOP, fill=Tk.BOTH, expand=1)

# create the left subframe for the list
frame_left = Tk.Frame(frame_main, bd=1);
frame_left.pack(side=Tk.LEFT, fill=Tk.Y, expand=0, padx=5, pady=5)

# create the right subframe for the figure and its control panel
frame_right = Tk.Frame(frame_main);
frame_right.pack(side=Tk.RIGHT, fill=Tk.BOTH, expand=1, padx=5, pady=5)

my_figure = Figure(facecolor='none', figsize=(8.2, 6.8), dpi=120)

# create the matplotlib's figure canvas
figure_canvas = FigureCanvasTkAgg(my_figure, master=frame_right)
figure_canvas.draw()
figure_canvas.get_tk_widget().pack(side=Tk.TOP)
figure_canvas._tkcanvas.pack(side=Tk.TOP)

scrollbar = Tk.Scrollbar(master=frame_left, orient=Tk.VERTICAL)
listbox = Tk.Listbox(master=frame_left, yscrollcommand=scrollbar.set, selectmode=Tk.SINGLE)
scrollbar.config(command=listbox.yview)
scrollbar.pack(side=Tk.LEFT, fill=Tk.Y, expand=0)
listbox.pack(side=Tk.LEFT, fill=Tk.Y, expand=0)

# fill the listbox
for key in images_data.keys():
    listbox.insert(Tk.END, key)

subplot1 = my_figure.add_subplot(221)
subplot2 = my_figure.add_subplot(222)
subplot3 = my_figure.add_subplot(223)
# ax.set_box_aspect([1,1,1])

def loadImage(index):

    if isinstance(images_data[index], ImageToAToT):

        subplot1.imshow(images_data[index].toa, interpolation='none')
        subplot1.set_title("ToA")

        subplot2.imshow(images_data[index].tot, interpolation='none')
        subplot2.set_title("ToT")

        subplot3.imshow(images_data[index].ftoa, interpolation='none')
        subplot3.set_title("FToA")

    elif isinstance(images_data[index], ImageToA):

        subplot1.imshow(images_data[index].toa, interpolation='none')
        subplot1.set_title("ToA")

        subplot2.imshow(images_data[index].ftoa, interpolation='none')
        subplot2.set_title("FToA")

    elif isinstance(images_data[index], ImageMpxiToT):

        subplot1.imshow(images_data[index].mpx, interpolation='none')
        subplot1.set_title("MPX")

        subplot2.imshow(images_data[index].itot, interpolation='none')
        subplot2.set_title("iToT")

    figure_canvas.draw()

loadImage(0)

def listBoxOnSelect(evt):

    w = evt.widget

    if len(w.curselection()) == 0:
        return

    # extract the index of the selected item
    index = int(w.curselection()[0])

    loadImage(index)

listbox.bind('<<ListboxSelect>>', listBoxOnSelect)

def close_window():
    root.withdraw()
    root.quit()
    root.destroy()

def win_deleted():
    print("Closed from outside...");
    close_window();

root.protocol("WM_DELETE_WINDOW", win_deleted)
root.mainloop()
print("Exitting")
sys.exit()
