#ifndef FRAMESIZE_H
#define FRAMESIZE_H
 /*
   @fnvoid newSizeAndMatrix(...)
   @brief Calculates new size and matrix 
   @param HomographyMatrix - Calculated homography matrix
   @param Sec_ImageShape - Shape of second image
   @param Base_ImageShape - Shape of base image
   @param NewFrameSize - saves the size of new frame
   @param Correction -  saves the amount of correction needed
*/ 
void newSizeAndMatrix(cv::Mat& HomographyMatrix, int* Sec_ImageShape, int* Base_ImageShape, int* NewFrameSize, int* Correction)
{
	// Reading the size of the image
	int Height = Sec_ImageShape[0], Width = Sec_ImageShape[1];

	// Taking the matrix of initial coordinates of the corners of the secondary image
	// Stored in the following format : [[x1, x2, x3, x4], [y1, y2, y3, y4], [1, 1, 1, 1]]
	// Where(xi, yi) is the coordinate of the i th corner of the image.
	double initialMatrix[3][4] = { {0, (double)Width - 1, (double)Width - 1, 0},
								   {0, 0, (double)Height - 1, (double)Height - 1},
								   {1.0, 1.0, 1.0, 1.0} };
	cv::Mat InitialMatrix = cv::Mat(3, 4, CV_64F, initialMatrix);// .inv();


	// Finding the final coordinates of the corners of the image after transformation.
	// NOTE: Here, the coordinates of the corners of the frame may go out of the
	// frame(negative values).We will correct this afterwards by updating the
	// homography matrix accordingly.
	cv::Mat FinalMatrix = HomographyMatrix * InitialMatrix;

	cv::Mat x = FinalMatrix(cv::Rect(0, 0, FinalMatrix.cols, 1));
	cv::Mat y = FinalMatrix(cv::Rect(0, 1, FinalMatrix.cols, 1));
	cv::Mat c = FinalMatrix(cv::Rect(0, 2, FinalMatrix.cols, 1));


	cv::Mat x_by_c = x.mul(1 / c);
	cv::Mat y_by_c = y.mul(1 / c);

	// Finding the dimentions of the stitched image frame and the "Correction" factor
	double min_x, max_x, min_y, max_y;
	cv::minMaxLoc(x_by_c, &min_x, &max_x);
	cv::minMaxLoc(y_by_c, &min_y, &max_y);
	min_x = (int)round(min_x); max_x = (int)round(max_x);
	min_y = (int)round(min_y); max_y = (int)round(max_y);


	int New_Width = max_x, New_Height = max_y;
	Correction[0] = 0; Correction[1] = 0;
	if (min_x < 0)
	{
		New_Width -= min_x;
		Correction[0] = abs(min_x);
	}
	if (min_y < 0)
	{
		New_Height -= min_y;
		Correction[1] = abs(min_y);
	}

	// Again correcting New_Widthand New_Height
	// Helpful when secondary image is overlaped on the left hand side of the Base image.
	New_Width = (New_Width < Base_ImageShape[1] + Correction[0]) ? Base_ImageShape[1] + Correction[0] : New_Width;
	New_Height = (New_Height < Base_ImageShape[0] + Correction[1]) ? Base_ImageShape[0] + Correction[1] : New_Height;


	// Finding the coordinates of the corners of the image if they all were within the frame.
	cv::add(x_by_c, Correction[0], x_by_c);
	cv::add(y_by_c, Correction[1], y_by_c);


	cv::Point2f OldInitialPoints[4], NewFinalPonts[4];
	OldInitialPoints[0] = cv::Point2f(0, 0);
	OldInitialPoints[1] = cv::Point2f(Width - 1, 0);
	OldInitialPoints[2] = cv::Point2f(Width - 1, Height - 1);
	OldInitialPoints[3] = cv::Point2f(0, Height - 1);
	for (int i = 0; i < 4; i++)
		NewFinalPonts[i] = cv::Point2f(x_by_c.at<double>(0, i), y_by_c.at<double>(0, i));


	// Updating the homography matrix.Done so that now the secondary image completely
	// lies inside the frame
	HomographyMatrix = cv::getPerspectiveTransform(OldInitialPoints, NewFinalPonts);

	// Setting variable for returning
	NewFrameSize[0] = New_Height; NewFrameSize[1] = New_Width;

}


#endif // !FRAMESIZE_H

