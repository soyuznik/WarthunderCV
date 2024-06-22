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

void MouseMove(double x, double y)
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x;
    double fy = y;
    INPUT Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE ;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));
}
int main()
{

    //double sens = 1;
    //RunMouse* Mouse = new RunMouse();
    //cout << "You Can press\n";

       
        
        //Sleep(500);
    



   // while (true) {
        // pause;
   // }
    LPCWSTR window_title = L"War Thunder - Test Drive";
    HWND handle = FindWindow(NULL, window_title);

    //Mat img = WindowsGraphicsCapture(handle);
    //cvtColor(img, img, COLOR_RGBA2RGB);
   // imshow("img", img);
    //cv::waitKey(0);
   // while (true) {

  //  }
    std::string model = "./Models/yolov4_train_best0.weights";  
    std::string config = "./Models/yolov4_train0.cfg";

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

    string scoped = "SCOPE: OFF";

    for (;;)
    {
        
        Mat img = WindowsGraphicsCapture(handle);
        cvtColor(img, img, COLOR_RGBA2RGB);
        // if all edged pixels are black means scoped in
        Vec3b pinc = Vec3b(0, 255, 0);
        Vec3b& px00 = img.at<Vec3b>(30, 30);
        Vec3b& pxE0 = img.at<Vec3b>(img.rows - 5, 30);
        Vec3b& px0E = img.at<Vec3b>(30, img.cols - 5);
        Vec3b& pxEE = img.at<Vec3b>(img.rows - 5, img.cols - 5);
        // found out when scoped in pixels from borders are csum < 300 , and
        // when not scoped csum >1000 on average but can reach about 300 so 300 is a good threshold.
        int csum = 0;
        csum += px00[0] + px00[1] + px00[2];
        csum += pxE0[0] + pxE0[1] + pxE0[2];
        csum += px0E[0] + px0E[1] + px0E[2];
        csum += pxEE[0] + pxEE[1] + pxEE[2];
        px00 = px0E = pxEE = pxE0 = pinc;
        
        //Vec3b& cpx00 = img.at<Vec3b>(25, 200);
        //Vec3b& cpxE0 = img.at<Vec3b>(img.rows - 200, 200);
        //Vec3b& cpx0E = img.at<Vec3b>(25, img.cols - 200);
        //Vec3b& cpxEE = img.at<Vec3b>(img.rows - 200, img.cols - 200);
        //cpx00 = cpx0E = cpxEE = cpxE0 = Vec3b(0, 0, 255);
        int xoffset = 20, yoffset = 200;
        Mat cimg = img(Range(xoffset, img.rows - 200), Range(yoffset, img.cols - 200));
        static Mat blobFromImg;
        bool swapRB = true;
        blobFromImage(cimg, blobFromImg, 1/255.0, Size(416, 416), Scalar(), swapRB, false);
        network.setInput(blobFromImg);
        std::vector<Mat> voutMat;
        Mat outMat;
        network.forward(voutMat , ln);
        vconcat(voutMat, outMat);
        
        int rowsNoOfDetection = outMat.rows;
        int colsCoordinatesPlusClassScore = outMat.cols;
        std::vector<cv::Rect> boxes;
        using pii = pair<double, double>;
        //std::vector<pii> distm;
        std::vector<float> confidences;
        for (int j = 0; j < rowsNoOfDetection; ++j)
        {
            Mat scores = outMat.row(j).colRange(5, colsCoordinatesPlusClassScore);

            Point PositionOfMax;
            double confidence;
            minMaxLoc(scores, 0, &confidence, 0, &PositionOfMax);

            if (confidence > CONF)
            {
                
                ld centerX = (outMat.at<float>(j, 0) * cimg.cols) + yoffset;
                ld centerY = (outMat.at<float>(j, 1) * cimg.rows) + xoffset;
                ld width = (outMat.at<float>(j, 2) * cimg.cols);
                ld height = (outMat.at<float>(j, 3) * cimg.rows);
                //distm.push_back({ centerX - (img.rows)/2, centerY - (img.cols) / 2 });
                ld left = centerX - width / 2;
                ld top = centerY - height / 2;
                
                cv::Rect2d box_(left, top, width, height);
                boxes.push_back(box_);
                confidences.push_back(confidence);

               // putText(img, "tank", Point(left, top), FONT_HERSHEY_SIMPLEX, 1.4, Scalar(0, 0, 255), 2, false);
               

                //rectangle(img, Rect(left, top, width, height), Scalar(0, 0, 255), 2, 8, 0);
            }
        }
       // if (GetAsyncKeyState(VK_SPACE)) {
           // double sens = 0.1;
         //   MouseMove(distm[0].first * sens, distm[0].second * sens);
            //if (GetAsyncKeyState(VK_LEFT)) {
            //    MouseMove(-1, 0);
            //}
            //if (GetAsyncKeyState(VK_UP)) {
            //    MouseMove(0, -1);
           // }
            //if (GetAsyncKeyState(VK_DOWN)) {
            //    MouseMove(0, 1);
            //}
            //if (GetAsyncKeyState(VK_RIGHT)) {
            //    MouseMove(1, 0);
            //}
        //}
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
            if (GetAsyncKeyState(0x4F)) {
                double cx = left + (width / 2) + 10;
                double cy = top + (height / 2);
                double sens = 1;
                double dx = cx - ((img.cols) / 2);
                double dy = cy - ((img.rows) / 2);
                MouseMove(dx * sens, dy * sens);
                if (dx < 25 && dy < 25) {
                    INPUT input = {};
                    input.type = INPUT_MOUSE;
                    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    SendInput(1, &input, sizeof(INPUT));
                }
                //cout << "left: " << left << "  top:" << top << "\n";
                //cout << "rows: " << img.rows / 2 << "  cols:" << img.cols / 2 << "\n";
                //cout << "dx: " << dx * sens << "  " << "dy: " << dy * sens << "\n";

            }
        }
        if (csum > 300) {
            scoped = "SCOPE: OFF";
        }
        else scoped = "SCOPE: ON";
        putText(img, scoped, Point(0, 40), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 255, 0), 1, LINE_AA);
        
        namedWindow("window", WINDOW_AUTOSIZE);
        //movement_detection.detect(img);
        cv::imshow("window", img);
        cv::waitKey(25);
        //Sleep(2000);
    }
    return 0;
}