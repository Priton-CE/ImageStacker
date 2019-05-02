#include "ImageStacker.h"

int main(int args, char *argv[])
{
	String InputPath = "F:/source/VS Projects/VS19/C++/ImageStacker/input.mp4"; //"F:/source/VS Projects/VS19/C++/ImageStacker/input.mp4"
	String OutputPath = "F:/source/VS Projects/VS19/C++/ImageStacker/output.png"; //"F:/source/VS Projects/VS19/C++/ImageStacker/output.png"

	//if (args > 1) {
	//	InputPath = argv[2];
	//}
	//if (args > 2) {
	//	InputPath = argv[3];
	//}

	PrintLog("Starting overlap...\n");

	std::vector<Mat> images = ImageStacker::getImages(InputPath);

	if (images.size() > 0) {
		std::vector<Point> AlignmentInfo = ImageStacker::getAlignmentInfo(images);

		Mat ResultImage = ImageStacker::calculateAveragePixels(images, AlignmentInfo);

		imwrite(OutputPath, ResultImage);

		ResultImage = reduceLight(ResultImage, Scalar(200, 200, 200));
		imwrite("F:/source/VS Projects/VS19/C++/ImageStacker/Output2.png", ResultImage);

		PrintLog("Picture saved at '" + OutputPath + "'.\nPress 'enter' to end the programm...");
	}
	
	//TODO: Save "Log"

	std::cin.get();
	return 0;
}