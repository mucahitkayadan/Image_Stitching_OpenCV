#ifndef FINDHOMOGRAPHY_H
#define FINDHOMOGRAPHY_H

/*
   @fnvoid findHomography(...)
   @brief Finds the homography with inputted parameters
   @param Matches - Matches between images
   @param mainImageKeypoints - Keypoints of first image
   @param secondImageKeypoints - Keypoints of second image
   @param homographyMatrix -  saves the result (homography matrix) into this cv Mat
*/  							  
void findHomography(std::vector<cv::DMatch> Matches, std::vector<cv::KeyPoint> mainImageKeypoints, std::vector<cv::KeyPoint> secondImageKeypoints, cv::Mat& homographyMatrix)
{
	// If less than 4 matches found, exit the code.                     
	if (Matches.size() < 4)
	{
		std::cout << "\nNot enough matches found between the images.\n";
		exit(0);
	}
	// Storing coordinates of points corresponding to the matches found in both the images
	std::vector<cv::Point2f> mainImagePoints, secondImagePoints;
	for (int i = 0; i < Matches.size(); i++)
	{
		cv::DMatch Match = Matches[i];
		mainImagePoints.push_back(mainImageKeypoints[Match.queryIdx].pt);
		secondImagePoints.push_back(secondImageKeypoints[Match.trainIdx].pt);
	}

	// Finding the homography matrix(transformation matrix).
	homographyMatrix = cv::findHomography(secondImagePoints, mainImagePoints, cv::RANSAC, (4.0));
}


#endif // !FINDHOMOGRAPHY_H

