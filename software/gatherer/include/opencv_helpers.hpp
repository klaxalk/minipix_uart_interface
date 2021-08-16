#include <opencv2/opencv.hpp>

#include <stdio.h>
#include <stdarg.h>

using namespace cv;
using namespace std;

/*
Function///////////////////////////////////////////////////////////////

Name:       ShowManyImages

Purpose:

This is a function illustrating how to display more than one
image in a single window using Intel OpenCV

Parameters:

string title: Title of the window to be displayed
int    n_images: Number of images to be displayed
Mat    img1: First Mat, which contains the first image
...
Mat    imgN: First Mat, which contains the Nth image

Language:   C++

The method used is to set the ROIs of a Single Big image and then resizing
and copying the input images on to the Single Big Image.

This function does not stretch the image...
It resizes the image without modifying the width/height ratio..

This function can be called like this:

ShowManyImages("Images", 5, img2, img2, img3, img4, img5);

This function can display upto 12 images in a single window.
It does not check whether the arguments are of type Mat or not.
The maximum window size is 700 by 660 pixels.
Does not display anything if the number of arguments is less than
one or greater than 12.

Idea was from [[BettySanchi]] of OpenCV Yahoo! Groups.

If you have trouble compiling and/or executing
this code, I would like to hear about it.

You could try posting on the OpenCV Yahoo! Groups
[url]http://groups.yahoo.com/group/OpenCV/messages/ [/url]


Parameswaran,
Chennai, India.

cegparamesh[at]gmail[dot]com

...
///////////////////////////////////////////////////////////////////////
*/

template <int PixelFormat>
void showManyImages(const string &title, const std::vector<cv::Mat> &images) {

  int n_images = images.size();

  int size;
  int i;
  int m, n;
  int x, y;

  // w - Maximum number of images in a row
  // h - Maximum number of images in a column
  int w, h;

  // scale - How much we have to resize the image
  float scale;
  int   max;

  // If the number of arguments is lesser than 0 or greater than 12
  // return without displaying
  if (n_images <= 0) {
    printf("Number of arguments too small....\n");
    return;
  } else if (n_images > 14) {
    printf("Number of arguments too large, can only handle maximally 12 images at a time ...\n");
    return;
  }

  // Determine the size of the image,
  // and the number of rows/cols
  // from number of arguments
  else if (n_images == 1) {
    w = h = 1;
    size  = 500;
  } else if (n_images == 2) {
    w    = 1;
    h    = 2;
    size = 500;
  } else if (n_images == 3 || n_images == 4) {
    w    = 2;
    h    = 2;
    size = 500;
  } else if (n_images == 5 || n_images == 6) {
    w    = 3;
    h    = 2;
    size = 200;
  } else if (n_images == 7 || n_images == 8) {
    w    = 4;
    h    = 2;
    size = 200;
  } else {
    w    = 4;
    h    = 3;
    size = 150;
  }

  // Create a new 3 channel image
  Mat DispImage = Mat::zeros(Size(100 + size * w, 60 + size * h), PixelFormat);

  // Loop for n_images number of arguments
  for (i = 0, m = 20, n = 20; i < n_images; i++, m += (20 + size)) {

    // Check whether it is NULL or not
    // If it is NULL, release the image, and return
    if (images[i].empty()) {
      printf("Invalid arguments");
      return;
    }

    // Find the width and height of the image
    x = images[i].cols;
    y = images[i].rows;

    // Find whether height or width is greater in order to resize the image
    max = (x > y) ? x : y;

    // Find the scaling factor to resize the image
    scale = (float)((float)max / float(size));

    // Used to Align the images
    if (i % w == 0 && m != 20) {
      m = 20;
      n += 20 + size;
    }

    // Set the image ROI to display the current image
    // Resize the input image and copy the it to the Single Big Image
    Rect ROI(m, n, (int)(float(x) / scale), (int)(float(y) / scale));
    Mat  temp;
    resize(images[i], temp, Size(ROI.width, ROI.height));
    temp.copyTo(DispImage(ROI));
  }

  // Create a new window, and show the Single Big Image
  imshow(title, DispImage);
}
