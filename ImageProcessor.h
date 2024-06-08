#pragma once
#define lol long long
#include <opencv2/core.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <vector>
#include <fstream>
#include <variant>
using namespace cv::dnn;
using namespace std;
constexpr auto obj_names_path = "yolov4-tiny/obj.names";
using coordinate = std::map<std::string, variant<int, std::string>>;
class ImageProcessor {
	lol W = 0;
	lol H = 0;
	//https://docs.opencv.org/3.4/d6/d0f/group__dnn.html
	Net net;
	vector<cv::String> ln;
	vector<string> classes;
	const vector<cv::Scalar()> colors;

	ImageProcessor(lol _W, lol _H, string cfg_file, string weights_file) {
		net = readNetFromDarknet(cfg_file, weights_file);
		net.setPreferableBackend(DNN_BACKEND_OPENCV);
		ln = net.getLayerNames();
		vector<cv::String> cln;
		for (auto i : net.getUnconnectedOutLayers()) {
			cln.push_back(ln[i - 1]);
		}
		ln = cln;
		W = _W;
		H = _H;
		
		ifstream fin(obj_names_path);
		string line;
		while (getline(fin, line)) {
			classes.push_back(line);
		}
		colors = {
			cv::Scalar(0, 0, 255),
			cv::Scalar(0, 255, 0),
			cv::Scalar(255, 0, 0),
			cv::Scalar(255, 255, 0),
			cv::Scalar(255, 0, 255),
			cv::Scalar(0, 255, 255)
		};
	}

	vector<coordinate> process_image(cv::InputArray img) {
		cv::Mat blob = blobFromImage(img, 1 / 255.0, { 416, 416 }, true, false, 5);
		net.setInput(blob);
		std::vector<vector<cv::Mat>> outputs;
		net.forward(outputs ,ln);
		cv::Mat combinedOutput;
		cv::vconcat(outputs, combinedOutput);
		vector<coordinate> coordinates = get_coordinates(combinedOutput, 0.5);
		draw_identified_objects(img, coordinates);
		return coordinates;
	}
	vector<coordinate> get_coordinates(std::vector<vector<cv::Mat>> outputs, double conf) {

	}

		

};