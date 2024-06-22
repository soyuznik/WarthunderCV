
//#include <yolo.h>
#include <sstream>
#include<windows.h>
#include <util.h>
#include <capture.h>
#define lol long long
using namespace std;


void main() {

	
	HWND handle = FindWindowA(NULL, "War Thunder");
	lol index = 3003000;
	//auto start = std::chrono::high_resolution_clock::now();
	while (true) {
		if (GetKeyState('Z') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
		{
			std::wostringstream woss;
			woss << "./database_6/file" << index << ".bmp";
			screenshot_window_base(handle, woss.str().c_str());//
			index++;
			Sleep(1000);
		}
		
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	//cout << std::setprecision(5);
	//cout << "Duration : " << (double)duration.count()/1000 << "\n";

}

