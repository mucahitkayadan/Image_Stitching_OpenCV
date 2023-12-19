main.cpp
========
Purpose
-------
This file serves as the primary entry point for the image stitching application. It handles user input, image processing, and output generation.

Key Functions
-------------
*   `main`: Orchestrates the overall execution flow.
*   `readImage`: Reads images from a specified folder (from libraries/readImage.h).
*   `showCylindirical`: Transforms an image into a cylindrical representation (from libraries/cylinder.h).
*   `stitch`: Stitches two images together (from libraries/stitch.h).
*   `divideImage`: Splits a panoramic image into multiple pieces (from libraries/divideImage.h).
*   `applyTransformation`: Applies transformations (rotation, scaling, noise) to images (from libraries/imageTransformation.h).

Execution Flow
--------------
1.  Prompts the user for input choices:
    *   RGB or grayscale images
    *   Dataset or single image
    *   Dataset selection (if applicable)
    *   Image selection (if applicable)
    *   Number of pieces to split (if applicable)
    *   Transformations to apply (if applicable)
2.  Reads images based on user input using `readImage`.
3.  If a single image is selected, splits it into multiple pieces using `divideImage` and applies transformations using `applyTransformation`.
4.  Transforms the first image into a cylindrical representation using `showCylindirical`.
5.  Iterates through the remaining images:
    *   Stitches each image with the base image using `stitch`.
    *   Updates the base image with the stitched result.
    *   Saves intermediate stitching results (optional).
6.  Saves the final stitched image.

stitch.h
==========================
Overview
--------
This header file defines the \`stitch\` function, which is used to stitch together two images. It utilizes cylindrical projection, image masking, keypoint matching, homography matrix calculation, and image warping to seamlessly blend the images.

Functions
---------
**Function**  
`cv::Mat stitch(cv::Mat mainImage, cv::Mat secondImage, int imageCounter, std::string outputFile)` 

**Description**  
Stitches two images together, returning the resulting stitched image.  

Detailed Function Descriptions
------------------------------

### `stitch`

**Parameters**:

`mainImage`: The first image to be stitched.

`secondImage`: The second image to be stitched.

`imageCounter`: A counter used for numbering the output images.

`outputFile`: The name of the output file to write the stitched image to.

Returns:

The stitched image.

Additional Notes
----------------

This header file also includes the following functions, but their detailed descriptions are not provided here:

*   `showCylindirical`
*   `findMatches`
*   `findHomography`
*   `newSizeAndMatrix`

readImage.h
=============================

Overview
--------

This header file provides functions for reading images from a specified folder and preparing them for processing. It includes:

*   Path string manipulation
*   Image reading and sorting
*   Grayscale to RGB conversion

Functions
---------

**Function**  
`std::string replaceAll(std::string inputString, const std::string& from, const std::string& to)`  

**Description**  
Replaces all occurrences of a substring within a string.

`int readImage(std::string imageFolderPath, std::vector& Images, int grayOrRGB)`

Reads images from a folder, sorts them, and optionally converts grayscale images to RGB.

Detailed Function Descriptions
------------------------------

### `replaceAll`

**Parameters**:

`inputString`: The string to modify.

`from`: The substring to replace.

`to`: The replacement substring.

Returns:

The modified string with all occurrences of `from` replaced by `to`.

### `readImage`

**Parameters**:

`imageFolderPath`: The path to the folder containing the images.

`Images`: A vector to store the read images.

`grayOrRGB`: An integer flag indicating whether to read images as grayscale (1) or RGB (2).

Returns:

0 on success, or a non-zero error code on failure.

parallel.h
============================

Overview
--------

This header file provides a class for multithreading and multiprocessing operations, specifically designed for parallelizing image processing tasks.

Classes
-------
**Description**

`ParallelMandelbrot`

Enables parallel execution of a Mandelbrot-like image transformation across multiple threads or processes.

Detailed Class Descriptions
---------------------------

### `ParallelMandelbrot`

This class implements a parallel loop body for a specific image transformation task. It's designed to be used with the OpenCV `parallel_for_` function for parallel execution.

Constructor:

`ParallelMandelbrot(cv::Mat& TransformedImage, cv::Mat& InitialImage, std::vector ti_x, std::vector ti_y, std::vector ii_tl_x, std::vector ii_tl_y, std::vector weight_tl, std::vector weight_tr, std::vector weight_bl, std::vector weight_br)`

Member function:

`void operator()(const cv::Range& range) const CV_OVERRIDE`: Performs the image transformation calculations for a given range of pixels.

imageTransformation.h
=======================================

Overview
--------

This header file provides functions for applying various image transformations, including rotation, scaling, and Gaussian noise addition.

Functions
---------

**Function**  
`randomFloat(float a, float b)`

Generates a random float number between the specified limits.  
**Description**  
`cv::Mat rotate(cv::Mat inputImage)`

Rotates an image by a random angle (deprecated).

`cv::Mat rotateWithoutCropping(cv::Mat inputImage)`

Rotates an image by a random angle without cropping the outer frame.

`cv::Mat scale(cv::Mat inputImage)`

Scales an image by a random factor.

`cv::Mat AddGaussianNoise(cv::Mat inputImage)`

Adds Gaussian noise to an image.

`std::vector applyTransformation(std::vector inputImages, std::string transformation)`

Applies specified transformations to a vector of images.

`void cleanOutputFolders(std::string outputPath)`

Cleans output folders by removing existing files.

findMatches.h
===============================

Overview
--------

This header file provides the `findMatches` function for identifying matching features between two images using SIFT features and a brute-force matcher.

Functions
---------

**Function**  
`findMatches(cv::Mat mainImage, cv::Mat secondImage, std::vector& goodMatches, std::vector& mainImageKeypoints, std::vector& secondImageKeypoints)`  

**Description**  
Finds good matches between two images using SIFT features and a brute-force matcher.  

Detailed Function Description
-----------------------------

### `findMatches`

This function performs the following steps:

1.  Converts the input images to grayscale.
2.  Detects SIFT keypoints and computes descriptors for both images.
3.  Uses a brute-force matcher to find initial matches between the descriptors.
4.  Filters the initial matches using the ratio test to keep only good matches.
5.  Returns the good matches, keypoints for both images.

findHomography.h
==================================

Overview
--------

This header file provides the `findHomography` function for estimating the homography matrix between two images using matching features.

Functions
---------

**Function**  
`findHomography(std::vector Matches, std::vector mainImageKeypoints, std::vector secondImageKeypoints, cv::Mat& homographyMatrix)`  

**Description**  
Estimates the homography matrix between two images based on provided matches.  

Detailed Function Description
-----------------------------

### `findHomography`

This function performs the following steps:

1.  Checks if there are enough matches (at least 4) to compute the homography.
2.  Extracts corresponding point coordinates from the matches.
3.  Uses OpenCV's `findHomography` function with RANSAC to estimate the homography matrix, handling potential outliers.
4.  Returns the estimated homography matrix.


divideImage.h
=============

Functionality
-------------

Divides an image into vertical blocks with overlaps. Used for processing singular added images.

Key Points
----------

*   Overlapping blocks ensure sufficient matching features for stitching.
*   Error handling checks for invalid images.


### divideImage

**Parameters**:

*   const cv::Mat& img: Input image.
*   const int columnNumber: Number of vertical blocks.
*   std::vector& blocks: Output blocks.

Returns: EXIT\_SUCCESS on success, EXIT\_FAILURE on error.

frameSize.h
===========

Functionality
-------------

Calculates the new frame size and corrected homography matrix for image stitching.

Key Points
----------

*   Uses corner coordinates to determine frame size.
*   Applies a correction factor to adjust for corners outside the frame.
*   Handles overlap in different positions.

### newSizeAndMatrix

**Parameters**:

*   cv::Mat& HomographyMatrix: Calculated homography matrix.
*   int\* Sec\_ImageShape: Shape of the second image.
*   int\* Base\_ImageShape: Shape of the base image.
*   int\* NewFrameSize: Output new frame size.
*   int\* Correction: Output correction factor.

showAsCylinder.h
================

Functionality
-------------

Transforms an image to a cylindrical representation for creating cylindrical panoramas.

Key Points
----------

*   Coordinate warping using trigonometric functions.
*   Coordinate rounding for accurate mapping.
*   Boundary removal to avoid black regions.
*   Bilinear interpolation for smooth pixel transitions.
*   Parallel processing for potential performance enhancement.

### convertXY

**Parameters**:

*   std::vector transformedImage\_X: X coordinates of the transformed image.
*   std::vector transformedImage\_Y: Y coordinates of the transformed image.
*   std::vector& xt: Output vector for transformed X coordinates.
*   std::vector& yt: Output vector for transformed Y coordinates.
*   int center\_X: X coordinate of the image center.
*   int center\_Y: Y coordinate of the image center.
*   int f: Focal length parameter for cylindrical warping.

### showCylindirical

**Parameters**:

*   cv::Mat InitialImage: Input image to be transformed.
*   cv::Mat& TransformedImage: Output transformed image.
*   std::vector& mask\_x: Output vector for masked X coordinates.
*   std::vector& mask\_y: Output vector for masked Y coordinates.

