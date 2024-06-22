//#include <NumCpp.hpp>
#include <iostream>
//#include <MoveDetect/src-lib/MoveDetect.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <windows.h>
#include <win10capture.h>

//#include <RealisticMouse/RealisticMouse/RunMouse.h>
using namespace cv;
using namespace std;
using namespace dnn;
#define lol long long
#define ld double
#define CONF 0.1

void MouseMove(int x, int y)
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x * (65535.0f / fScreenWidth);
    double fy = y * (65535.0f / fScreenHeight);
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE ;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));
}
int main()
{


    //RunMouse* Mouse = new RunMouse();
    //cout << "You Can press\n";
    //while (true) {
     //   POINT p;
        //0x20 == spacebar
     //   if (GetAsyncKeyState(0x20)) {
     //       GetCursorPos(&p);
            //cout << "Cursor pos : " << p.x << " " << p.y << "\n";
      //      MouseMove(0, 1);
            //GetCursorPos(&p);
            //cout << "Cursor pos : " << p.x << " " << p.y << "\n";
     //   }

        //500ms delay between click
     //   Sleep(500);
   // }



   // while (true) {
        // pause;
   // }
    LPCWSTR window_title = L"War Thunder";
    HWND handle = FindWindow(NULL, window_title);

    //Mat img = WindowsGraphicsCapture(handle);
    //cvtColor(img, img, COLOR_RGBA2RGB);
   // imshow("img", img);
    //cv::waitKey(0);
   // while (true) {

  //  }
    std::string model = "./Models/yolov4_train_best1.weights";  
    std::string config = "./Models/yolov4_train1.cfg";

    Net network = readNet(model, config , "Darknet");
    network.setPreferableBackend(DNN_BACKEND_OPENCV);
    network.setPreferableTarget(DNN_TARGET_OPENCL);
    //std::vector<cv::String> ln = network.getUnconnectedOutLayersNames();
    std::vector<cv::String> ln;
    auto layers = network.getLayerNames();
    for (auto i : network.getUnconnectedOutLayers()) {
        ln.push_back(layers[i-1]);
    }
    //MoveDetect::Handler movement_detection;
    //movement_detection.mask_enabled = true;
    //movement_detection.bbox_enabled = true;
    //movement_detection.contours_enabled = true;
    //movement_detection.contours_size = 4;
    //
    //// If you are generating either the contours or the bounding boxes,
    //// then you'll want to increase the frequency and keep more frames.
    //movement_detection.key_frame_frequency = 1;
    //movement_detection.number_of_control_frames = 10;
    //
    //// Larger "thumbnails" improves precision, but takes longer to process each frame.
    //movement_detection.thumbnail_ratio = 0.25;
    //
    //// More expensive but slightly prettier anti-aliased lines.
    //movement_detection.line_type = cv::LINE_AA;


    for (;;)
    {
        //if (!cap.isOpened()) {
        //    cout << "Video Capture Fail" << endl;
       //     break;
       // }
        Mat img = WindowsGraphicsCapture(handle);
        cvtColor(img, img, COLOR_RGBA2RGB);
        
        static Mat blobFromImg;
        bool swapRB = true;
        blobFromImage(img, blobFromImg, 1/255.0, Size(416, 416), Scalar(), swapRB, false);
        network.setInput(blobFromImg);
        std::vector<Mat> voutMat;
        Mat outMat;
        network.forward(voutMat , ln);
        vconcat(voutMat, outMat);
        
        int rowsNoOfDetection = outMat.rows;
        int colsCoordinatesPlusClassScore = outMat.cols;
        std::vector<cv::Rect> boxes;
        std::vector<float> confidences;
        for (int j = 0; j < rowsNoOfDetection; ++j)
        {
            Mat scores = outMat.row(j).colRange(5, colsCoordinatesPlusClassScore);

            Point PositionOfMax;
            double confidence;
            minMaxLoc(scores, 0, &confidence, 0, &PositionOfMax);

            if (confidence > CONF)
            {
                ld centerX = (outMat.at<float>(j, 0) * img.cols);
                ld centerY = (outMat.at<float>(j, 1) * img.rows);
                ld width = (outMat.at<float>(j, 2) * img.cols);
                ld height = (outMat.at<float>(j, 3) * img.rows);

                ld left = centerX - width / 2;
                ld top = centerY - height / 2;
                
                cv::Rect2d box_(left, top, width, height);
                boxes.push_back(box_);
                confidences.push_back(confidence);

               // putText(img, "tank", Point(left, top), FONT_HERSHEY_SIMPLEX, 1.4, Scalar(0, 0, 255), 2, false);
               

                //rectangle(img, Rect(left, top, width, height), Scalar(0, 0, 255), 2, 8, 0);
            }
        }
        std::vector<int> good;
        cv::dnn::NMSBoxes(boxes, confidences, CONF, 0 , good);
        for (auto ind : good) {
            Rect r = boxes[ind];
            int left = r.x;
            int top = r.y;
            int width = r.width;
            int height = r.height;
            stringstream ss; ss << "tank " << trunc(confidences[ind] * 100) << "%";
            putText(img, ss.str(), Point(left, top - 5), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255),1, LINE_AA);
            rectangle(img, Rect(left, top, width, height), Scalar(0, 0, 255), 2, 8, 0);
        }
        namedWindow("window", WINDOW_AUTOSIZE);
        //movement_detection.detect(img);
        cv::imshow("window", img);
        cv::waitKey(25);
        //Sleep(2000);
    }
    return 0;
}