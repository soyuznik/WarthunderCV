#pragma once
#include<iostream>
#include<opencv2/opencv.hpp>
#include "opencv2/core.hpp"
#include "opencv2/core/ocl.hpp"
#include "opencv2/core/utility.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv::dnn;
using namespace std;
#define YOLO_P6 false //是否使用P6模型

struct Output {
	int id;             //结果类别id
	float confidence;   //结果置信度
	cv::Rect box;       //矩形框
};

class Yolov5 {
public:
	Yolov5() {
	}
	~Yolov5() {}
	bool readModel(Net& net, string& netPath, string& cfgPath, bool isCuda = false);
	bool Detect(cv::Mat& SrcImg, cv::dnn::Net& net, std::vector<Output>& output);
	void drawPred(cv::Mat& img, std::vector<Output> result, std::vector<cv::Scalar> color);

private:

	void LetterBox(const cv::Mat& image, cv::Mat& outImage,
		cv::Vec4d& params, //[ratio_x,ratio_y,dw,dh]
		const cv::Size& newShape = cv::Size(640, 640),
		bool autoShape = false,
		bool scaleFill = false,
		bool scaleUp = true,
		int stride = 32,
		const cv::Scalar& color = cv::Scalar(114, 114, 114));

#if(defined YOLO_P6 && YOLO_P6==true)
	const int _netWidth = 1280;  //ONNX图片输入宽度
	const int _netHeight = 1280; //ONNX图片输入高度
#else

	const int _netWidth = 640;   //ONNX图片输入宽度
	const int _netHeight = 640;  //ONNX图片输入高度
#endif // YOLO_P6

	float _classThreshold = 0.25;
	float _nmsThreshold = 0.45;
public:
	std::vector<std::string> _className = {"tank"};
};
