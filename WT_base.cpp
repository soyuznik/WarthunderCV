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
#include <unordered_set>
//#include <Z_Utils.h>
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
    


    //cv::cuda::GpuMat;
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
    std::string model = "./Models/yolov4_train_best2.weights";  
    std::string config = "./Models/yolov4_train2.cfg";

    Net network = readNet(model, config , "Darknet");
    network.setPreferableBackend(DNN_BACKEND_CUDA);
    network.setPreferableTarget(DNN_TARGET_CUDA);
    std::vector<cv::String> ln;
    auto layers = network.getLayerNames();
    for (auto i : network.getUnconnectedOutLayers()) {
        ln.push_back(layers[i-1]);
    }

    std::string crossm = "./Models/yolov4-tiny-cross_best.weights";
    std::string crossc = "./Models/yolov4-tiny-cross.cfg";
    Net crossd = readNet(crossm, crossc, "Darknet");
    crossd.setPreferableBackend(DNN_BACKEND_CUDA);
    crossd.setPreferableTarget(DNN_TARGET_CUDA);
    std::vector<cv::String> lnc;
    auto crossl = crossd.getLayerNames();
    for (auto i : crossd.getUnconnectedOutLayers()) {
        lnc.push_back(crossl[i - 1]);
    }

    string scoped = "scope: OFF";
    int distoffset = 0;
    std::set<pair<int,int>> trace;
    for (;;)
    {
        
        Mat img = WindowsGraphicsCapture(handle);
        cvtColor(img, img, COLOR_RGBA2RGB);
        // record map movement; // on resultion 1280 x 720 on warthunder and mat size is 1282x747
        std::vector<cv::Rect> cross_box;
        std::vector<float> cross_conf;
        //int mapy = 515;
        //int mapx = 1050;
        {
            Vec3b pinc = Vec3b(0, 0, 255);
            Vec3b& px00 = img.at<Vec3b>(515, 1050);
            Vec3b& pxE0 = img.at<Vec3b>(733, 1267);
            Vec3b& px0E = img.at<Vec3b>(733, 1050);
            Vec3b& pxEE = img.at<Vec3b>(515 , 1267);
            px00 = px0E = pxEE = pxE0 = pinc;
            Mat map = img(Range(515, 733) , Range(1050, 1257));
            static Mat blobFromImg;
            bool swapRB = true;
            cv::dnn::blobFromImage(map, blobFromImg, 1 / 255.0, Size(416, 416), cv::Scalar(), swapRB, false);
            crossd.setInput(blobFromImg);
            std::vector<Mat> voutMat;
            Mat outMat;
            crossd.forward(voutMat, lnc);
            cv::vconcat(voutMat, outMat);
            int rowsNoOfDetection = outMat.rows;
            int colsCoordinatesPlusClassScore = outMat.cols;
            using pii = pair<double, double>;
            //std::vector<pii> distm;
            for (int j = 0; j < rowsNoOfDetection; ++j)
            {
                Mat scores = outMat.row(j).colRange(5, colsCoordinatesPlusClassScore);
                Point PositionOfMax;
                double confidence;
                minMaxLoc(scores, 0, &confidence, 0, &PositionOfMax);

                if (confidence > CONF)
                {

                    ld centerX = (outMat.at<float>(j, 0) * map.cols);
                    ld centerY = (outMat.at<float>(j, 1) * map.rows);
                    ld width = (outMat.at<float>(j, 2) * map.cols);
                    ld height = (outMat.at<float>(j, 3) * map.rows);
                    //ld left = centerX - width / 2;
                    //ld top = centerY - height / 2;

                    cv::Rect2d box_(centerX, centerY, width, height);
                    cross_box.push_back(box_);
                    cross_conf.push_back(confidence);
                }
            }
            std::vector<int> good_cross;
            cv::dnn::NMSBoxes(cross_box, cross_conf, CONF, 0, good_cross);
            Rect r = cross_box[good_cross[0]];
            int cx = r.x;
            int cy = r.y;
            double radius = (sqrt(2)) / 4;
            radius *= ((r.width > r.height) ? r.width : r.height);
            trace.insert(make_pair(cx, cy));
            for (auto p : trace) {
                Vec3b& px = map.at<Vec3b>(cv::Point(p.first , p.second));
                px = Vec3b(255, 0, 0);
            }
            circle(map, cv::Point(cx, cy) , radius, cv::Scalar(0, 255, 0), 1);
            cv::namedWindow("map", WINDOW_AUTOSIZE);
            cv::imshow("map", map);

        }
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
        blobFromImage(cimg, blobFromImg, 1/255.0, Size(704, 416), Scalar(), swapRB, false);
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
                ld left = centerX - width / 2;
                ld top = centerY - height / 2;
                
                cv::Rect2d box_(left, top, width, height);
                boxes.push_back(box_);
                confidences.push_back(confidence);
            }
        }

        std::vector<int> good;
        cv::dnn::NMSBoxes(boxes, confidences, CONF, 0 , good);
        std::vector<pii> dxdy;
        for (auto ind : good) {
            Rect r = boxes[ind];
            int left = r.x;
            int top = r.y;
            int width = r.width;
            int height = r.height;
            stringstream ss; ss << "tank " << trunc(confidences[ind] * 100) << "% " << ind;
            cv::putText(img, ss.str(), Point(left, top - 5), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(0, 0, 255),1, LINE_AA);
            rectangle(img, Rect(left, top, width, height), Scalar(0, 0, 255), 2, 8, 0);
            double cx = left + (width / 2);
            double cy = top + (height / 2) - distoffset;
            double dx = cx - ((img.cols) / 2);
            double dy = cy - ((img.rows) / 2);
            dxdy.push_back({ dx , dy });
        }
        sort(dxdy.begin(), dxdy.end(), [](pii a, pii b) {
            return a.first*a.first + a.second*a.second < b.first*b.first + b.second*b.second;
        });
        bool engaged = false;
        if (GetAsyncKeyState(0x4F)) {
            if (dxdy.empty()) continue;
            double dx = dxdy[0].first;
            double dy = dxdy[0].second;
            double sens = 1;
            MouseMove(dx * sens, dy * sens);
            if (dx < 10 && dy < 10) {
                // simulate click
                INPUT input = {};
                engaged = true;
                input.type = INPUT_MOUSE;
                input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                SendInput(1, &input, sizeof(INPUT));
            }
        }
        if (csum > 300) {
            scoped = "scope: off";
        }
        else scoped = "scope: on";
        cv::putText(img, scoped, Point(0, 40), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1, LINE_AA);
        if (engaged) {
            cv::putText(img, "engaged", Point(0, 60), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
            //distoffset += 2;
            stringstream ss; ss << "distoffset: " << distoffset;
            cv::putText(img, ss.str(), Point(0, 80), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255), 1, LINE_AA);
        }
        else distoffset = 0;
        
        cv::namedWindow("window", WINDOW_AUTOSIZE);
        //movement_detection.detect(img);
        cv::imshow("window", img);
        INPUT input = {};
        input.type = INPUT_MOUSE;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
        cv::waitKey(25);
        //Sleep(2000);
    }
    return 0;
}