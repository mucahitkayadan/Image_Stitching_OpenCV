#define _USE_MATH_DEFINES
////// add gray or color, add orb, add image 

#include "opencv2/opencv.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <exception>
#include <random>

#include "libraries/readImage.h"
#include "libraries/findMatches.h"
#include "libraries/findHomography.h"                                                  
#include "libraries/frameSize.h"
#include "libraries/parallel.h"
#include "libraries/cylinder.h"
#include "libraries/stitch.h"
#include "libraries/divideImage.h"
#include "Libraries/imageTransformation.h"

int main()
{

	// Reading images.
	std::vector<cv::Mat> Images;		// Input Images will be stored in this list.
	std::string mainInputImageFolder = "InputImages/";
	std::string subInputImageFolder;
	std::string outputFolder = "OutputImages/";
	std::vector<std::string> providedDatasets = {"T1", "T2", "T3", "RT1", "RT2", "RT3", "SRT1", "SRT2", "SRT3","NRT1", "NRT2", "NRT3", "LNSRT1", "LNSRT2", "LNSRT3"};
	std::vector<std::string> panaromicImagetoParse = { "DEI", "MYHOUSE", "ISTANBUL" };
	int answerForDatasetOrImage;
	int answerForRGBorGray;
	
	namespace fs = std::filesystem;
	
	
	//https://docs.opencv.org/4.x/d8/d6a/group__imgcodecs__flags.html#gga61d9b0126a3e57d9277ac48327799c80af660544735200cbe942eea09232eb822
	std::cout << "Would you like to continue with RGB Image or Grayscale Image?\n"
		<< "1: GrayScale\n2: RGB\n";
	std::cin >> answerForRGBorGray;
	while (answerForRGBorGray < 1 || answerForRGBorGray > 2) {
		std::cout << "Please enter 1 or 2!!\n";
		std::cin >> answerForRGBorGray;
	}

	std::cout << "Would you like to progress with a dataset which provided by the professor, or choose an image to process? " << std::endl
		<<"1: Original Dataset " << std::endl << "2: I want to split my own panaromic image." << std::endl;		
	std::cin >> answerForDatasetOrImage;
	while (answerForDatasetOrImage < 1 || answerForDatasetOrImage > 2) {
		std::cout << "Please enter 1 or 2!!\n";
		std::cin >> answerForDatasetOrImage;
	}
	if (answerForDatasetOrImage == 1) {
		int answerForWhichDatasetInput;
		std::cout << "Please choose a dataset:" << std::endl
			<< "1: T1\n2: T2\n3: T3\n" << "4: RT1\n5: RT2\n6: RT3\n" << "7: SRT1\n8: SRT2\n9: SRT3\n"
			<< "10: NRT1\n11: NRT2\n12: NRT3\n" << "13: LNSRT1\n14: LNSRT2\n15: LNSRT3\n";
		std::cin >> answerForWhichDatasetInput;
		while (answerForWhichDatasetInput < 1 || answerForWhichDatasetInput > 15) {
			std::cout << "Please enter a number between 1-15!!\n";
			std::cin >> answerForWhichDatasetInput;
		}
		subInputImageFolder = providedDatasets.at(static_cast<std::vector<cv::String, std::allocator<cv::String>>::size_type>(answerForWhichDatasetInput) - 1);
		cleanOutputFolders(outputFolder + subInputImageFolder);
		readImage(mainInputImageFolder.append(subInputImageFolder), Images, answerForRGBorGray);
	}
	else if (answerForDatasetOrImage == 2) {
		int answerforWhichImagetoParse;
		std::wcout << "Please choose a photo to separate.\n" << "1: DEI Building\n2: My House\n3: Istanbul\n";
		std::cin >> answerforWhichImagetoParse;
		while (answerForDatasetOrImage < 1 || answerForDatasetOrImage > 3) {
			std::cout << "Please enter 1, 2 or 3!!\n";
			std::cin >> answerforWhichImagetoParse;
		}
		subInputImageFolder = panaromicImagetoParse.at(static_cast<std::vector<cv::String, std::allocator<cv::String>>::size_type>(answerforWhichImagetoParse) - 1);
		
		cleanOutputFolders(outputFolder + subInputImageFolder);
		cv::Mat inputImage = cv::imread(mainInputImageFolder + subInputImageFolder + "/Image.jpg",  answerForRGBorGray - 1);
		if (inputImage.channels() == 1) {
			cv::cvtColor(inputImage, inputImage, cv::COLOR_GRAY2BGR);
		}
		std::wcout << "How many pieces do you want to split?\nPlease enter between 2-6:\n";
		int howManyPieces;
		std::cin >> howManyPieces;
		while (howManyPieces < 2 || howManyPieces > 6) {
			std::cout << "Please enter a number between 2 and 6!!\n";
			std::cin >> howManyPieces;
		}

		std::cout << "Do you want to add transformations to the sub images?" << std::endl
			<< "R for Rotating, S for Scaling, N for Noise" << std::endl
			<< "You can write more than one, i.e SR, RSN, RN, S..." << std::endl
			<< "0 for not to apply any transformation.\n";
		std::string answerFortransformation;
		std::cin >> answerFortransformation;
		std::transform(answerFortransformation.begin(), answerFortransformation.end(), answerFortransformation.begin(), ::toupper);
		//std::vector<cv::Mat> blocks;
		int divideStatus = divideImage(inputImage, howManyPieces, Images);

		Images = applyTransformation(Images, answerFortransformation);

		for (int j = 0; j < Images.size(); j++)
		{
			std::string blockId = std::to_string(j + 1);
			std::string blockImgName = outputFolder + subInputImageFolder + "/block_" + blockId + ".jpg";
			cv::imwrite(blockImgName, Images[j]);
		}
	}
	std::string outputFilePath = (outputFolder + subInputImageFolder + "/" + "Total_Image_to_");
	

	cv::Mat BaseImage;
	std::vector<int> temp1, temp2;
	showCylindirical(Images[0], BaseImage, temp1, temp2);

	for (int i = 1; i < Images.size(); i++)
	{
		cv::Mat stitchedImage = stitch(BaseImage, Images[i], i, outputFilePath + std::to_string(i+1) + ".jpg");

		stitchedImage.copyTo(BaseImage);
		//cv::imshow(std::to_string(i), BaseImage);
		//cv::waitKey(0);
	}

	cv::imwrite(outputFolder + subInputImageFolder + "/" + subInputImageFolder + ".jpg", BaseImage);
	system("PAUSE");
	return 0;
}