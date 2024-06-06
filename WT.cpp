#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <sstream>
#include <capture.h>
#include <util.h>

#include <chrono>
#include <iomanip>
using namespace cv;
using namespace std;
using lol = long long;


void main() {
	HWND handle = FindWindowA(NULL, "War Thunder");
	lol index = 0;
	//auto start = std::chrono::high_resolution_clock::now();
	while (index < 1000) {
		
		std::wostringstream woss;
		woss << "./img/file" << index << ".bmp";
		screenshot_window_base(handle, woss.str().c_str());//
		index++;
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//cout << std::setprecision(5);
	//cout << "Duration : " << (double)duration.count()/1000 << "\n";

}