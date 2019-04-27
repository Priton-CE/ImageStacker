#pragma once
#include "define.h"
#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

using namespace cv;

class ImageStacker
{
public:
	static std::vector<Mat> getImages(String path_to_source);

	static std::vector<Point> getAlignmentInfo(std::vector<Mat> OriginalinputImages, int mainImage = 0, int Number_of_mainObjects = 1);
	static std::vector<Mat> ReziseMatsWithAlignmentInfo(std::vector<Mat> InputImages, Rect Rectangle, std::vector<Point> AlignmentInfo);

	static Mat calculateAveragePixels(std::vector<Mat> InputImages, std::vector<Point> AlignmentInfo);
};

Mat reduceLight(Mat img, Scalar Value);