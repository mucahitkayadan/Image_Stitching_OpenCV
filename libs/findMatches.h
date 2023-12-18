#ifndef FINDMATCHES_H
#define FINDMATCHES_H
/*
* @fn void findMatches(cv::Mat, cv::Mat, std::vector<cv::DMatch>&, std::vector<cv::KeyPoint>&, std::vector<cv::KeyPoint>&)
* @brief This header file consists findMatch function. 
* SIFT detector is used.
* BFMatcher class is used.
* distance ratio is 0.75.
* knnMatch is used.
* @param cv::Mat mainImage : First image to be matched. Input
* @param cv::Mat secondImage : Second image to be matched. Input
* @param std::vector<cv::DMatch>& goodMatches : contains good matches. Output.
* @param mainImageKeypoints : contains the keypoints of the first image. Output
* @param secondImageKeypoints : contains the keypoints of the second image. Output
*/
void findMatches(cv::Mat mainImage, cv::Mat secondImage, std::vector<cv::DMatch>& goodMatches, std::vector<cv::KeyPoint>& mainImageKeypoints, std::vector<cv::KeyPoint>& secondImageKeypoints)
{
	//using namespace cv;
	cv::Mat mainImageDecsriptors, secondImageDescriptors; //defining descriptors
	cv::Mat mainImageGray, secondImageGray; //defining images for grayscale
	cv::cvtColor(mainImage, mainImageGray, cv::COLOR_BGR2GRAY); //convert first image to gray
	cv::cvtColor(secondImage, secondImageGray, cv::COLOR_BGR2GRAY); //convert second image to gray
	cv::BFMatcher BF_Matcher; // call and assign BFMatcher class
	std::vector<std::vector<cv::DMatch>> initialMatches; //defining inital matches. It is an inception of vectors, because I will use knnMatcher
	const float ratio = 0.75; //ratio is defined as 0.75

	// Using SIFT to find the keypointsand decriptors in the images

	cv::Ptr<cv::SIFT> detector = cv::SIFT::create(); //SIFT detector is created in a name of detector.
	//I did not use detector->detect and detector->compute seperately. It is compact and useful.
	detector->detectAndCompute(mainImageGray, cv::noArray(), mainImageKeypoints, mainImageDecsriptors); //keypoints and desctiptors are computed for the first image.
	detector->detectAndCompute(secondImageGray, cv::noArray(), secondImageKeypoints, secondImageDescriptors); //keypoints and desctiptors are computed for the second image.
	BF_Matcher.knnMatch(mainImageDecsriptors, secondImageDescriptors, initialMatches, 2); //knnMatch is runned on 1st and 2nd image descriptors.
	

	// In this for loop, it will visit all matches and goodMatch will store suitable ones, which are meeting the condution of being smaller than next distance multiplied by ratio. (ratio is defined as const float 0.75)
	for (int i = 0; i < initialMatches.size(); ++i)
	{
		if (initialMatches[i][0].distance < ratio * initialMatches[i][1].distance)
		{
			goodMatches.push_back(initialMatches[i][0]);
		}
	}
}

#endif // !FINDMATCHES_H

