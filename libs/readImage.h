#ifndef READIMAGE_H
#define READIMAGE_H
/*
* In this header file, replaceAll and readImage functions are defined.
* replaceAll function basically changes sub-strings to another char or string.
* This convertion was needed, because the filesystem uses pathing system with '\\', and  OpenCV has bugs so sometimes...
* it does not read the path. I.e: "C:\\Opencv4\\libs" to "C:/Opencv4/libs"
* 
* readImage function takes inputFolderPath, pops Mat vector and takes grayOrRGB as integer.
* It checks all requirements and gives Images in vector.

*/

namespace fs = std::filesystem;		// ISO C++17 Standard

std::string replaceAll(std::string inputString, const std::string& from, const std::string& to) 
	{
		size_t start_pos = 0;
		while ((start_pos = inputString.find(from, start_pos)) != std::string::npos) {
			inputString.replace(start_pos, from.length(), to);
			start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
	}
	return inputString;
}

int readImage(std::string imageFolderPath, std::vector<cv::Mat>& Images, int grayOrRGB)
{
	if (fs::is_directory(fs::status(imageFolderPath))) // Checks if the folder exist
	{
		std::vector<std::string> imageNames; //string vector contains image names.
		// fs::directory_iterator iterates all available folders in path.
		for (const auto& entry : fs::directory_iterator(imageFolderPath))
		{
			std::string strVar = entry.path().string(); //Converts iterator to a string, to use in replaceAll function.
			replaceAll(std::string(strVar), std::string("\\"), std::string("/")); //Changes \\ to /
			if (strVar.find('_') != std::string::npos) { //If the image name contains under score, we add it to the vector.
				//Because we do not want to read original(reference) images.
				imageNames.push_back(entry.path().u8string()); //add image to the string vector
			}
		}

		// Sorting the images according to their names and reading in ascending order.
		std::vector<std::string> imageNamesSorted = imageNames; //Names will be sorted, so we basically copy our image name vector.
		std::sort(imageNamesSorted.begin(), imageNamesSorted.end()); //Names are sorted.

		for (auto x : imageNamesSorted) //In all images
		{
			for (int i = 0; i < imageNames.size(); i++)
			{
				if (imageNames.at(i) == x) //imageNames.at(i).q
				{
					cv::Mat Image = cv::imread(imageNames.at(i), grayOrRGB - 1);	// Reading images one by one.
					//User enters 1 or 2 for grayOrRGB. flags:grayOrRGB - 1 corresponds to flags enum, 0 is grayScale, 1 is RGB
					if (Image.empty())		// Checks if image is read
					{
						std::cout << "Not able to read image: " << imageNames[i] << std::endl;
						std::cin.ignore();
						exit(0);
					}
					if (Image.channels() == 1) { //If user chooses grayScale image, we have to convert it to 3 channel image for other functions.
						cv::cvtColor(Image, Image, cv::COLOR_GRAY2BGR);
					}
					Images.push_back(Image); //we keep all images in Images vector.

					break;

				}
			}
		}
	}

	else                                    // If it is not folder(Invalid Path).
		std::cout << "\nEnter valid Image Folder Path.\n";

	if (Images.size() < 2) //we need at least 2 images to compute.(match & stitch etc.
	{
		std::cout << "\nNot enough images found. Please provide 2 or more images.\n";
		exit(1);
	}

	return 0;
}

#endif // !READIMAGE_H

