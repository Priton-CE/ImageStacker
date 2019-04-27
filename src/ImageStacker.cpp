#include "ImageStacker.h"

std::vector<Mat> ImageStacker::getImages(String path_to_source)
{
	bool isVideo = false;
	std::vector<Mat> Return;

	std::vector<String> Filenames;
	int Supported = 0;
	int notSupported = 0;

	glob(path_to_source, Filenames);

	for (int i = 0; i < Filenames.size(); i++) {
		String Fileformat = "";
		for (int j = Filenames.at(i).find_last_of('.'); j < Filenames.at(i).size(); j++) {
			Fileformat += Filenames.at(i)[j];
		}

		if (Fileformat == ".mp4") {
			PrintLog("  OK   - Format: *" + Fileformat + "\t\t" + path_to_source + "\n");
			isVideo = true;
			break;
		}
		else if (Fileformat == ".avi") {
			PrintLog("  OK   - Format: *" + Fileformat + "\t\t" + path_to_source + "\n");
			isVideo = true;
			break;
		}
		else if (Fileformat == ".png") {
			Supported++;
			PrintLog("  OK   - Format: *" + Fileformat + "\t\t" + Filenames.at(i) + "\n");
		}
		else if (Fileformat == ".jpg" || Fileformat == ".jpeg" || Fileformat == ".jpe") {
			Supported++;
			PrintLog("  OK   - Format: *" + Fileformat + "\t\t" + Filenames.at(i) + "\n");
		}
		else if (Fileformat == ".bmp" || Fileformat == ".dip") {
			Supported++;
			PrintLog("  OK   - Format: *" + Fileformat + "\t\t" + Filenames.at(i) + "\n");
		}
		else {
			notSupported++;
			PrintLog("Fehler - Format: *" + Fileformat + "\t\t" + Filenames.at(i) + "\n");

			Filenames.erase(Filenames.begin() + i);
			i--;
		}
	}

	PrintLog("\n");

	if (!isVideo) {
		PrintLog(std::to_string(Supported) + " / " + std::to_string(Supported + notSupported) + " Dateien unterstuetzt.\n");
		PrintLog(std::to_string(notSupported) + " / " + std::to_string(Supported + notSupported) + " Dateien nicht unterstuetzt.\n\n");

		for (int i = 0; i < Filenames.size(); i++) {
			Mat tmp = imread(Filenames.at(i));

			if (!tmp.data) {
				PrintLog("Error: Bild konnte nicht geladen werden!\nVielleicht ist das Dateiformat nicht von dieser OpenCV-Version unterstuetzt. *.bmp oder *.dip Files gehen immer!\nFile: " + Filenames.at(i) + "\n\n");
			}
			else {
				Return.push_back(tmp);
			}
		}
	}
	else {
		VideoCapture cap;
		cap.open(path_to_source);

		if (!cap.isOpened()) {
			PrintLog("Error: Video konnte nicht geladen werden!\nVielleicht ist das Dateiformat nicht von dieser OpenCV-Version unterstuetzt.\nFile: " + path_to_source + "\n\n");
			return Return;
		}

		PrintLog("Starte convertierung von Video zu Bildern...");
		while (true) {
			Mat frame;
			bool bSuccess = cap.read(frame);

			if (bSuccess == false)
			{
				PrintLog("   Fertig!\n");
				break;
			}

			Return.push_back(frame);
		}
	}
	
	return Return;
}

std::vector<Point> ImageStacker::getAlignmentInfo(std::vector<Mat> OriginalinputImages, int mainImage, int Number_of_mainObjects)
{
	PrintLog("Stabilisiere Bilder...\n");

	std::vector<Mat> inputImages;
	inputImages.resize(OriginalinputImages.size());
	for (int i = 0; i < OriginalinputImages.size(); i++) {
		cvtColor(OriginalinputImages.at(i), inputImages.at(i), COLOR_BGR2HSV);
	}

	//make sure that "mainImage" is not causing a outofrange-exeption
	if (mainImage > inputImages.size())
		mainImage = inputImages.size() - 1;

	std::vector<Point> AlignmentInfo;
	AlignmentInfo.resize(inputImages.size());

	std::vector<Rect2f> mainObjects;
	mainObjects.resize(Number_of_mainObjects);

	{
		Mat thresholdImage;
		inRange(inputImages.at(mainImage), Scalar(10, 10, 10), Scalar(255, 255, 255), thresholdImage);

		std::vector<std::vector<Point>> contours;
		cv::findContours(thresholdImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

		std::vector<Rect2f> Objects;
		Objects.resize(contours.size());
		for (int j = 0; j < contours.size(); j++) {
			float minX = 10000;
			float maxX = 0;
			float minY = 10000;
			float maxY = 0;

			for (int k = 0; k < contours.at(j).size(); k++) {
				if (contours.at(j).at(k).x < minX) { minX = contours.at(j).at(k).x; }
				if (contours.at(j).at(k).x > maxX) { maxX = contours.at(j).at(k).x; }
				if (contours.at(j).at(k).y < minY) { minY = contours.at(j).at(k).y; }
				if (contours.at(j).at(k).y > maxY) { maxY = contours.at(j).at(k).y; }
			}

			Objects.at(j).width = maxX - minX;
			Objects.at(j).height = maxY - minY;
			Objects.at(j).x = minX;
			Objects.at(j).y = minY;
		}

		std::vector<int> Records;
		Records.resize(Number_of_mainObjects);
		for (int j = 0; j < Objects.size(); j++) {
			for (int k = 0; k < Records.size(); k++) {
				if (Objects.at(Records.at(k)).width * Objects.at(Records.at(k)).height < Objects.at(j).width * Objects.at(j).height) { Records.at(k) = j; }
			}
		}

		//biggest Objects
		for (int j = 0; j < mainObjects.size(); j++) {
			mainObjects.at(j) = Objects.at(Records.at(j));
		}
		
		AlignmentInfo.at(mainImage) = Point2f(0, 0);
	}

	for (int i = 0; i < inputImages.size(); i++) {
		if (i != mainImage) {
			Mat thresholdImage;
			inRange(inputImages.at(i), Scalar(10, 10, 10), Scalar(255, 255, 255), thresholdImage);

			std::vector<std::vector<Point>> contours;
			cv::findContours(thresholdImage, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			if (contours.size() != 0) {
				std::vector<Rect2f> Objects;
				Objects.resize(contours.size());
				for (int j = 0; j < contours.size(); j++) {
					float minX = 10000;
					float maxX = 0;
					float minY = 10000;
					float maxY = 0;

					for (int k = 0; k < contours.at(j).size(); k++) {
						if (contours.at(j).at(k).x < minX) { minX = contours.at(j).at(k).x; }
						if (contours.at(j).at(k).x > maxX) { maxX = contours.at(j).at(k).x; }
						if (contours.at(j).at(k).y < minY) { minY = contours.at(j).at(k).y; }
						if (contours.at(j).at(k).y > maxY) { maxY = contours.at(j).at(k).y; }
					}

					Objects.at(j).width = maxX - minX;
					Objects.at(j).height = maxY - minY;
					Objects.at(j).x = minX;
					Objects.at(j).y = minY;
				}

				std::vector<int> Records;
				Records.resize(Number_of_mainObjects);
				for (int j = 0; j < Objects.size(); j++) {
					for (int k = 0; k < Records.size(); k++) {
						if (Objects.at(Records.at(k)).width * Objects.at(Records.at(k)).height < Objects.at(j).width * Objects.at(j).height) { Records.at(k) = j; }
					}
				}

				//biggest Objects
				std::vector<Rect2f> RecordObjects;
				RecordObjects.resize(Number_of_mainObjects);
				for (int j = 0; j < RecordObjects.size(); j++) {
					RecordObjects.at(j) = Objects.at(Records.at(j));
				}

				//calculate Image offset ("Offset")
				std::vector<Point2f> Offsets;
				Offsets.resize(Number_of_mainObjects);
				for (int j = 0; j < mainObjects.size(); j++) {
					Offsets.at(j).x = mainObjects.at(j).x - RecordObjects.at(j).x;
					Offsets.at(j).y = mainObjects.at(j).y - RecordObjects.at(j).y;
				}

				Point2f Offset;
				for (int j = 0; j < Offsets.size(); j++) {
					Offset += Offsets.at(j);
				}
				Offset /= (int)Offsets.size();

				AlignmentInfo.at(i) = Offset;
			}
		}
		PrintLog("Bild " + std::to_string(i + 1) + " / " + std::to_string(inputImages.size()) + "\n");
	}
	
	PrintLog("----Fertig!----\n");
	return AlignmentInfo;
}

std::vector<Mat> ImageStacker::ReziseMatsWithAlignmentInfo(std::vector<Mat> InputImages, Rect Rectangle, std::vector<Point> AlignmentInfo)
{
	for (int i = 0; i < InputImages.size(); i++) {
		Rect modRect = Rectangle;
		modRect.x -= AlignmentInfo.at(i).x;
		modRect.y -= AlignmentInfo.at(i).y;

		InputImages.at(i)(modRect);
	}
	return InputImages;
}

Mat ImageStacker::calculateAveragePixels(std::vector<Mat> InputImages, std::vector<Point> AlignmentInfo)
{
	PrintLog("Bereite berechnung vor...");

	int minCols = 10000;
	int minRows = 10000;

	for (int i = 0; i < InputImages.size(); i++) {
		if (InputImages.at(i).cols < minCols) {
			minCols = InputImages.at(i).cols;
		}
		if (InputImages.at(i).rows < minRows) {
			minRows = InputImages.at(i).rows;
		}
	}
	
	Mat OutputImage = InputImages.at(0);
	PrintLog("   Fertig!\nStarte berechnung. Das dauert eine Weile...");

	for (int Column = 0; Column < minCols; Column++) {
		for (int Row = 0; Row < minRows; Row++) {
			//TODO: try to implement Mat::ptr() insteat if Mat::at()
			Vec3i Color = OutputImage.at<Vec3b>(Point(Column, Row));
			for (int i = 1; i < InputImages.size(); i++) {
				Point Pixel = Point(Column, Row) - AlignmentInfo.at(i);
				if (Pixel.x < 0)							{ Pixel.x = 0;							}
				if (Pixel.x > InputImages.at(i).cols - 1)	{ Pixel.x = InputImages.at(i).cols - 1; }
				if (Pixel.y < 0)							{ Pixel.y = 0;							}
				if (Pixel.y > InputImages.at(i).rows - 1)	{ Pixel.y = InputImages.at(i).rows - 1; }

				Color += InputImages.at(i).at<Vec3b>(Pixel);
			}
			OutputImage.at<Vec3b>(Point(Column, Row)) = Color / (int)InputImages.size();
		}
	}

	PrintLog("   Fertig!\n");

	return OutputImage;
}

Mat reduceLight(Mat img, Scalar Value)
{
	for (int Column = 0; Column < img.cols; Column++) {
		for (int Row = 0; Row < img.rows; Row++) {
			//TODO: try to implement Mat::ptr() insteat if Mat::at()
			Vec3i Color = img.at<Vec3b>(Point(Column, Row));

			if (Value.val[0] < 0)
				Value.val[0] = 0;
			if (Value.val[1] < 0)
				Value.val[1] = 0;
			if (Value.val[2] < 0)
				Value.val[2] = 0;

			Color -= Vec3i(Value.val[0], Value.val[1], Value.val[2]);
			
			img.at<Vec3b>(Point(Column, Row)) = Color;
		}
	}

	return img;
}