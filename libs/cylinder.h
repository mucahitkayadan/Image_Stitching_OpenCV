#ifndef SHOWASCYLINDER_H
#define SHOWASCYLINDER_H
/*
* This header file consists convertXY function and showCylindirical function.
* convertXY helps us warping image and converting its X and Y
* showCylindirical presents images as cylindrical. It takes InitialImage as parameter and pushes transformed image.
*/

/*
   @fn void convertXY(...)
   @brief Warps an image in the X Y order using inputted parameters
*/
void convertXY(std::vector<int> transformedImage_X, std::vector<int> transformedImage_Y, std::vector<float>& xt, std::vector<float>& yt, int center_X, int center_Y, int f)
{
	for (int i = 0; i < transformedImage_Y.size(); i++) //For loop visits all transformed image Y
	{
		//xt and yt contains converted coordinates
		//(x', y') = (x*cos(rads) - y*sin(rads), x*sin(rads) + y*cos(rads))
		xt.push_back((f * tan((float)(transformedImage_X[i] - center_X) / f)) + center_X); //f is defined as 5000 in function.
		yt.push_back(((float)(transformedImage_Y[i] - center_Y) / cos((float)(transformedImage_X[i] - center_X) / f)) + center_Y);
	}
}


 /*
   @fn void showCylindirical(...)
   @brief after the operations, displays the image as cylindiric (for panaromic images)
*/																		   
void showCylindirical(cv::Mat InitialImage, cv::Mat& TransformedImage, std::vector<int>& mask_x, std::vector<int>& mask_y)
{
	int h = InitialImage.rows, w = InitialImage.cols; //height and width
	int center_x = w / 2, center_y = h / 2; //center point of the image
	int f = 10000; //f value is experimental. 
	// Creating a blank transformed image.
	TransformedImage = cv::Mat::zeros(cv::Size(InitialImage.cols, InitialImage.rows), InitialImage.type());

	// Storing all coordinates of the transformed image in 2 arrays (x and y coordinates)
	std::vector<int> transformedImage_X, transformedImage_Y;
	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			transformedImage_X.push_back(i);
			transformedImage_Y.push_back(j);
		}
	}

	// Finding corresponding coordinates of the transformed image in the initial image
	std::vector<float> ii_x, ii_y;
	convertXY(transformedImage_X, transformedImage_Y, ii_x, ii_y, center_x, center_y, f);

	// Rounding off the coordinate values to get exact pixel values (top-left corner).
	std::vector<int> ii_tl_x, ii_tl_y;
	for (int i = 0; i < ii_x.size(); i++)
	{
		ii_tl_x.push_back((int)ii_x[i]);
		ii_tl_y.push_back((int)ii_y[i]);
	}

	// Finding transformed image points whose corresponding
	// initial image points lies inside the initial image
	std::vector<bool> goodIndices;
	for (int i = 0; i < ii_tl_x.size(); i++) //goodIndices keeps ii_tl values which are in w-2 and h-2
		goodIndices.push_back((ii_tl_x[i] >= 0) && (ii_tl_x[i] <= (w - 2)) && (ii_tl_y[i] >= 0) && (ii_tl_y[i] <= (h - 2)));

	try { //this try block erases oval parts of cylinder. In other saying, corners.
		transformedImage_X.erase(std::remove_if(transformedImage_X.begin(), transformedImage_X.end(), [&goodIndices, &transformedImage_X](auto const& i) { return !goodIndices.at(&i - transformedImage_X.data()); }), transformedImage_X.end());
		transformedImage_Y.erase(std::remove_if(transformedImage_Y.begin(), transformedImage_Y.end(), [&goodIndices, &transformedImage_Y](auto const& i) { return !goodIndices.at(&i - transformedImage_Y.data()); }), transformedImage_Y.end());

		ii_x.erase(std::remove_if(ii_x.begin(), ii_x.end(), [&goodIndices, &ii_x](auto const& i) { return !goodIndices.at(&i - ii_x.data()); }), ii_x.end());
		ii_y.erase(std::remove_if(ii_y.begin(), ii_y.end(), [&goodIndices, &ii_y](auto const& i) { return !goodIndices.at(&i - ii_y.data()); }), ii_y.end());

		ii_tl_x.erase(std::remove_if(ii_tl_x.begin(), ii_tl_x.end(), [&goodIndices, &ii_tl_x](auto const& i) { return !goodIndices.at(&i - ii_tl_x.data()); }), ii_tl_x.end());
		ii_tl_y.erase(std::remove_if(ii_tl_y.begin(), ii_tl_y.end(), [&goodIndices, &ii_tl_y](auto const& i) { return !goodIndices.at(&i - ii_tl_y.data()); }), ii_tl_y.end());

	}
	catch (std::exception& e) //catch any errors during erasing cylinder corners.
	{
		
		const char* err_msg = e.what();
		std::cout << "exception caught: " << err_msg << std::endl;
	}
	
	// Removing all the outside points from everywhere

	// Bilinear interpolation
	std::vector<float> dx(ii_x.size()), dy(ii_y.size());
	//std::transform applies the given function to a range and stores the result in another range, keeping the original elements order and beginning at d_first.
	std::transform(ii_x.begin(), ii_x.end(), ii_tl_x.begin(), dx.begin(), std::minus<float>());
	std::transform(ii_y.begin(), ii_y.end(), ii_tl_y.begin(), dy.begin(), std::minus<float>());

	//Calculates weights of corners.
	std::vector<float> weightTopLeft, weightTopRight, weightBottomLeft, weightBottomRight;
	for (int i = 0; i < dx.size(); i++)
	{
		weightTopLeft.push_back((1.0 - dx[i]) * (1.0 - dy[i]));
		weightTopRight.push_back((dx[i]) * (1.0 - dy[i]));
		weightBottomLeft.push_back((1.0 - dx[i]) * (dy[i]));
		weightBottomRight.push_back((dx[i]) * (dy[i]));
	}
	// Multithreading for parallel mandel brot. Calls the class from parallel.h header.
	// https://docs.opencv.org/master/d7/dff/tutorial_how_to_use_OpenCV_parallel_for_.html
	ParallelMandelbrot parallelMandelbrot(TransformedImage, InitialImage, transformedImage_X, transformedImage_Y, ii_tl_x, ii_tl_y, weightTopLeft, weightTopRight, weightBottomLeft, weightBottomRight);
	cv::parallel_for_(cv::Range(0, weightTopLeft.size()), parallelMandelbrot);

	// Getting x coorinate to remove black region from right and left in the transformed image
	int min_x = *min_element(transformedImage_X.begin(), transformedImage_X.end());

	// Cropping out the black region from both sides(using symmetricity)
	TransformedImage(cv::Rect(min_x, 0, TransformedImage.cols - min_x * 2, TransformedImage.rows)).copyTo(TransformedImage);

	// Setting return values
	// mask_x = transformedImage_X - min_x
	std::vector<int> min_x_v(transformedImage_X.size(), min_x);
	std::transform(transformedImage_X.begin(), transformedImage_X.end(), min_x_v.begin(), std::back_inserter(mask_x),
		[](int a, int b) { return (a - b); });
	//mask_y = transformedImage_Y
	mask_y = transformedImage_Y;
}


#endif // !SHOWASCYLINDER_H

