#ifndef STITCH_H
#define STITCH_H 
/*
* @fn stitch(cv::Mat mainImage, cv::Mat secondImage, int imageCounter, std::string outputFile)
* @brief In this header, stitch function is described. It takes main and second image, and stitches them.
* stitch function is used in for loop, so it stitches all the images in order.
* @param cv::Mat mainImage = first image to stitch
* @param cv::Mat secondImage = second image to stitch
* @param int imageCounter = counter of images. It is used for numbering images when they are written.
* @param std::string outputFile = name of thee written image
*/
cv::Mat stitch(cv::Mat mainImage, cv::Mat secondImage, int imageCounter, std::string outputFile)
{
	// Applying Cylindrical projection on second image
	cv::Mat secondImageCylinder;
	std::vector<int> mask_x, mask_y;//defining masks
	showCylindirical(secondImage, secondImageCylinder, mask_x, mask_y); //shows secondImage on Cylinder.

	// Getting secondImage Mask
	cv::Mat secondImageMask = cv::Mat::zeros(secondImageCylinder.rows, secondImageCylinder.cols, secondImageCylinder.type());
	//std::vector<int> white_color(255, secondImageMask.channels());
	for (int i = 0; i < mask_x.size(); i++)
	{
		//vector with 3 byte value contains mask_y's row and mask_x's column
		cv::Vec3b& value = secondImageMask.at<cv::Vec3b>(mask_y[i], mask_x[i]);
		for (int k = 0; k < secondImageCylinder.channels(); k++)
			value.val[k] = 255;
	}

	// Finding matches between the 2 images and their keypoints
	std::vector<cv::DMatch> Matches;
	std::vector<cv::KeyPoint> firstImageKeypoints, secondImageKeypoints;
	findMatches(mainImage, secondImageCylinder, Matches, firstImageKeypoints, secondImageKeypoints);

	cv::Mat drawMatchOutput;
	cv::drawMatches(mainImage, firstImageKeypoints, secondImage, secondImageKeypoints, Matches, drawMatchOutput);
	cv::imwrite(outputFile, drawMatchOutput);
	//std::string titleOfMatchesScreen = "The Matches Between the Stitched Image Until Now and " + std::to_string(imageCounter + 1) + ". Image";
	//cv::imshow(titleOfMatchesScreen, drawMatchOutput);
	//cv::waitKey(0);
		
	// Finding homography matrix.
	cv::Mat homographyMatrix;
	findHomography(Matches, firstImageKeypoints, secondImageKeypoints, homographyMatrix);

	// Finding size of new frame of stitched images and updating the homography matrix
	int secondImageShape[2] = { secondImageCylinder.rows, secondImageCylinder.cols };
	int mainImageShape[2] = { mainImage.rows, mainImage.cols };
	int NewFrameSize[2], Correction[2];
	newSizeAndMatrix(homographyMatrix, secondImageShape, mainImageShape, NewFrameSize, Correction);

	// Finally placing the images upon one another.
	cv::Mat secongImageTransformed, secondImageTransformedMask;
	cv::warpPerspective(secondImageCylinder, secongImageTransformed, homographyMatrix, cv::Size(NewFrameSize[1], NewFrameSize[0]));
	cv::warpPerspective(secondImageMask, secondImageTransformedMask, homographyMatrix, cv::Size(NewFrameSize[1], NewFrameSize[0]));

	cv::Mat BaseImage_Transformed = cv::Mat::zeros(NewFrameSize[0], NewFrameSize[1], mainImage.type());
	mainImage.copyTo(BaseImage_Transformed(cv::Rect(Correction[0], Correction[1], mainImage.cols, mainImage.rows)));

	cv::Mat StitchedImage, temp;
	//cv::imwrite("1.jpg", secondImageTransformedMask);
	cv::bitwise_not(secondImageTransformedMask, secondImageTransformedMask);
	//cv::imwrite("2.jpg", secongImageTransformed);
	//cv::imwrite("3.jpg", BaseImage_Transformed);
	cv::bitwise_and(BaseImage_Transformed, secondImageTransformedMask, temp);
	//cv::imwrite("7.jpg", temp);
	cv::bitwise_or(secongImageTransformed, temp, StitchedImage);
	//cv::imwrite("4.jpg", secongImageTransformed);
	//cv::imwrite("5.jpg", secondImageTransformedMask);
	//cv::imwrite("6.jpg", StitchedImage);
	return StitchedImage;
}


#endif // !STITCH_H

