#ifndef IMAGETRANSFORMATION_H
#define IMAGETRANSFORMATION_H
/*
* @fn float randomFloat(float a, float b)
* @brief In this header, some transformations are done for splitted image blocks. 
* randomFloat: Generates random float numbers for 
* @param float a : lower limit of the random floats.
* @param float b : higher limit of the random floats.
*/
float randomFloat(float a, float b) {
    float random = ((float)rand()) / (float)RAND_MAX;
    float diff = b - a;
    float r = random * diff;
    return a + r;
}
/*
* @fn cv::Mat rotate(cv::Mat inputImage)
* @brief rotate function returning mat object with parametres imagefile and angle 
* @param cv::Mat inputImage : Takes and image as input
* @warning deprecated : Use rotateWithoutCropping instead. The difference is croping outer frame.
*/
cv::Mat rotate(cv::Mat inputImage) {      

    float angle = randomFloat(-10.0, 10.0);
    cv::Mat rotatedImage;      //Mat object for output image file
    cv::Point2f pt(inputImage.cols / 2., inputImage.rows / 2.);          //point from where to rotate    
    cv::Mat r = cv::getRotationMatrix2D(pt, angle, 1.0);      //Mat object for storing after rotation
    cv::warpAffine(inputImage, rotatedImage, r, cv::Size(inputImage.cols, inputImage.rows));  ///applie an affine transforation to image.
    return rotatedImage;         //returning Mat object for output image file
}
cv::Mat rotateWithoutCropping(cv::Mat inputImage) {   //rotate function returning mat object with parametres imagefile and angle    

    float angle = randomFloat(-5.0, 5.0);
    // get rotation matrix for rotating the image around its center in pixel coordinates
    cv::Point2f center((inputImage.cols - 1) / 2.0, (inputImage.rows - 1) / 2.0);
    cv::Mat rotation = cv::getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle, center not relevant
    cv::Rect2f boundingBox = cv::RotatedRect(cv::Point2f(), inputImage.size(), angle).boundingRect2f();
    // adjust transformation matrix
    rotation.at<double>(0, 2) += boundingBox.width / 2.0 - inputImage.cols / 2.0;
    rotation.at<double>(1, 2) += boundingBox.height / 2.0 - inputImage.rows / 2.0;

    cv::Mat rotatedImage;
    cv::warpAffine(inputImage, rotatedImage, rotation, boundingBox.size());
    return rotatedImage;
}

cv::Mat scale(cv::Mat inputImage) {
    float ratio = randomFloat(0.5, 1.5);
    cv::Mat resizedImage;
    cv::resize(inputImage, resizedImage, cv::Size(), ratio, ratio, cv::INTER_LINEAR);
    return resizedImage;
}

cv::Mat AddGaussianNoise(cv::Mat inputImage){
    double Mean = 0.0;
    double StdDev = 20.0;
    cv::Mat mSrc_16SC;
    cv::Mat noisedImage;
    cv::Mat mGaussian_noise = cv::Mat(inputImage.size(), CV_16SC3);
    randn(mGaussian_noise, cv::Scalar::all(Mean), cv::Scalar::all(StdDev));

    inputImage.convertTo(mSrc_16SC, CV_16SC3);
    addWeighted(mSrc_16SC, 1.0, mGaussian_noise, 1.0, 0.0, mSrc_16SC);
    mSrc_16SC.convertTo(noisedImage, inputImage.type());

    return noisedImage;
}


std::vector<cv::Mat> applyTransformation(std::vector<cv::Mat> inputImages, std::string transformation) {
	//"R for Rotating, S for Scaling, N for Noise" 
	//"You can write more than one, i.e SR, RSN, N, NS..."
	for (int i = 0; i < inputImages.size(); i++) {
		if (transformation.find('S') != std::string::npos) {
			inputImages.at(i) = scale(inputImages.at(i));
		}
		if (transformation.find('N') != std::string::npos) {
			inputImages.at(i) = AddGaussianNoise(inputImages.at(i));
		}
		if (transformation.find('R') != std::string::npos) {
			inputImages.at(i) = rotateWithoutCropping(inputImages.at(i));
		}
		else if (transformation.find('0') != std::string::npos) {
			return inputImages;
		}
        else {
            return inputImages;
        }
	}
	return inputImages;
}
void cleanOutputFolders(std::string outputPath) {
	if (!(fs::is_directory(fs::status(outputPath)))) {
		fs::create_directory(outputPath);
	}
	else if (fs::is_directory(fs::status(outputPath))) {
		for (auto& path : fs::directory_iterator(outputPath)) {
			fs::remove_all(path);
		}
		//fs::remove_all(outputFolder + subInputImageFolder);
	}
}

#endif // !IMAGETRANSFORMATION_H
