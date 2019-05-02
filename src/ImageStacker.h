#pragma once
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;

void PrintLog(std::string message); //Function to write to the log and console

class ImageStacker
{
public:
	static std::vector<Mat> getImages(String path_to_source); //Creates a vector of images

	static std::vector<Point> getAlignmentInfo(std::vector<Mat> OriginalinputImages, int mainImage = 0, int Number_of_mainObjects = 1); //Generates info about how to align the images
	static std::vector<Mat> ReziseMatsWithAlignmentInfo(std::vector<Mat> InputImages, Rect Rectangle, std::vector<Point> AlignmentInfo); //Rezises the image-vector with the alignmentinfo

	static Mat calculateAveragePixels(std::vector<Mat> InputImages, std::vector<Point> AlignmentInfo); //stacks the images
};

Mat reduceLight(Mat img, Scalar Value);
