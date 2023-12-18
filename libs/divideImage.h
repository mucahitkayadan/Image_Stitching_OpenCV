#ifndef DIVIDEIMAGE_H
#define DIVIDEIMAGE_H
/*
* In this header, divideImage is described. It basically divides an image to vertical parts, which means the image is split along its width.
* This function is called for splitting singular added images by me.
* Takes input image as parameter and throws blocks vector as output in parameters.
* It does not just trim, but instead splits with overlaps, because of having matches.
*/
int divideImage(const cv::Mat& img, const int columnNumber, std::vector<cv::Mat>& blocks)
{
    // Checking if the image was passed correctly
    if (!img.data || img.empty())
    {
        std::wcout << "Image Error: Cannot load image to divide." << std::endl;
        return EXIT_FAILURE;
    }

    // init image dimensions
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    //std::wcout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;
    
    //detects the widths of each block by dividing input image width to block number 
    //Then, converts to integer, because of it will be used as size.
    //It will be changed later because of need of overlap.
    int initBlockWidth = static_cast<int>(imgWidth / columnNumber); 
    int xPoint; //starting x point of every block.
    int xWidth;
    int overlap = 50; // 50 pixel overlap.
    
    //In this loop, blocks are parsed and stored.
    //i.e: blockNumber is 4 and imgWidth is 1000. initBlockWidth is 1000/4 = 250 
    //Crop Rects of blocks are: (Let's consider height is 500)
    // 1: (0, 0, 300, 500)
    // 2: (200, 0, 350, 500)
    // 3: (450, 0, 350, 500)
    // 4: (700, 0, 300, 500)
    for (int i = 0; i < columnNumber; i++) {
        xPoint = i == 0 ? 0 : (i * initBlockWidth) - overlap;
        xWidth = (i == 0 || columnNumber - i == 1) ? initBlockWidth + overlap : initBlockWidth + 2 * overlap;
        blocks.push_back(img(cv::Rect(xPoint, 0, xWidth, imgHeight)).clone());
    }
    return EXIT_SUCCESS;

}
#endif // !DIVIDEIMAGE_H

