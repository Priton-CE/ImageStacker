#include "ImageStacker.h"

std::string Log = "";
void PrintLog(std::string message) {
	Log += message;
	std::cout << message;
}

int main()
{
	String InputPath =	"F:/source/VS Projects/VS19/C++/overlap/x64/Debug/input_länger.mp4";
	String OutputPath = "F:/source/VS Projects/VS19/C++/overlap/x64/Release/Output/Output.png";

	PrintLog("Start overlap\n");

	std::vector<Mat> images = ImageStacker::getImages(InputPath);

	std::vector<Point> AlignmentInfo = ImageStacker::getAlignmentInfo(images, 0);

	Mat ResultImage = ImageStacker::calculateAveragePixels(images, AlignmentInfo);

	imwrite(OutputPath, ResultImage);

	ResultImage = reduceLight(ResultImage, Scalar(200, 200, 200));
	imwrite("F:/source/VS Projects/VS19/C++/overlap/x64/Release/Output/Output2.png", ResultImage);
	
	PrintLog("Das Bild wurde als " + OutputPath + " gespeichert.\nDrueck 'enter' um das programm zu beenden...");
	
	std::cin.get();
	return 0;
}