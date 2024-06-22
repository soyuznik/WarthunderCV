#include "windows.graphics.capture/pch.h"

namespace winrt
{
    using namespace Windows::Foundation;
    using namespace Windows::Graphics::Capture;
    using namespace Windows::Graphics::DirectX;
    using namespace Windows::Graphics::DirectX::Direct3D11;
}

winrt::GraphicsCaptureItem GetCaptureItemForPrimaryMonitor();
winrt::com_ptr<ID3D11Texture2D> CaptureSingleFrame(
    winrt::com_ptr<ID3D11Device> const& d3d11Device,
    winrt::GraphicsCaptureItem const& item);

cv::Mat windows_graphics_capture_getmat()
{
    // Initialize COM
    winrt::init_apartment(winrt::apartment_type::multi_threaded);

    // Create our D3D11 device
    auto d3d11Device = util::CreateD3DDevice();

    // Capture the screen
    auto item = GetCaptureItemForPrimaryMonitor();
    auto texture = CaptureSingleFrame(d3d11Device, item);

    // OpenCV requires a texture that can be read from the CPU
    auto stagingTexture = util::CopyD3DTexture(d3d11Device, texture, true);

    // Initialize OpenCV
    auto cvContext = cv::directx::ocl::initializeContextFromD3D11Device(d3d11Device.get());
    // Get the dimensions of our texture
    D3D11_TEXTURE2D_DESC desc = {};
    stagingTexture->GetDesc(&desc);
    auto width = desc.Width;
    auto height = desc.Height;

    // Convert our D3D11 texture to OpenCV's Mat
    cv::Mat input(height, width, CV_8UC4);
    cv::directx::convertFromD3D11Texture2D(stagingTexture.get(), input);
    return input;




    // Skip the other part from : WGCOpenCVInterop











    // DEBUG: Test the conversion
    //cv::imwrite("input.jpg", input);

    // We need a grayscale image for the contours demo
    //cv::Mat grayscale(height, width, CV_8UC1);
    //cv::cvtColor(input, grayscale, cv::COLOR_BGRA2GRAY);
    // DEBUG: Save the grayscale image
    //cv::imwrite("grayscale.jpg", grayscale);

    // The rest is based on the OpenCV contours demo
    // https://docs.opencv.org/3.4/da/d32/samples_2cpp_2contours2_8cpp-example.html
    //std::vector<std::vector<cv::Point>> contours0;
    //std::vector<cv::Vec4i> hierarchy;
    //cv::findContours(grayscale, contours0, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    //
    //std::vector<std::vector<cv::Point>> contours(contours0.size());
    //for (auto i = 0; i < contours0.size(); i++)
    //{
    //    cv::approxPolyDP(cv::Mat(contours0[i]), contours[i], 3, true);
    //}
    //
    //// Draw the contours to an image
    //cv::Mat outputImage = cv::Mat::zeros(height, width, CV_8UC3);
    //cv::drawContours(outputImage, contours, 3, cv::Scalar(128, 255, 255), 3, cv::LINE_AA, hierarchy);
    //cv::imwrite("output.jpg", outputImage);
    //
    //wprintf(L"Success!\n");

    //return 0;
}

winrt::GraphicsCaptureItem GetCaptureItemForPrimaryMonitor()
{
    auto monitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    auto interopFactory = winrt::get_activation_factory<
        winrt::GraphicsCaptureItem,
        IGraphicsCaptureItemInterop>();

    winrt::GraphicsCaptureItem item{ nullptr };
    winrt::check_hresult(interopFactory->CreateForMonitor(
        monitor,
        winrt::guid_of<winrt::GraphicsCaptureItem>(),
        winrt::put_abi(item)));

    return item;
}

// This method is meant for one-off screen captures. Do not use this in a loop
// if you want to get continual frames at high speed.
winrt::com_ptr<ID3D11Texture2D> CaptureSingleFrame(
    winrt::com_ptr<ID3D11Device> const& d3d11Device,
    winrt::GraphicsCaptureItem const& item)
{
    auto device = CreateDirect3DDevice(d3d11Device);
    auto itemSize = item.Size();

    // Because we want to synchronously wait for a frame, we use CreateFreeThreaded
    // instead of Create here. By doing this, the FrameArrived event is fired from
    // the frame pool's internal thread, and the calling thread doesn't need to 
    // pump messages. Check the Win32CaptureSample README for more details.
    auto framePool = winrt::Direct3D11CaptureFramePool::CreateFreeThreaded(
        device,
        winrt::DirectXPixelFormat::B8G8R8A8UIntNormalized,
        1,
        itemSize);
    auto session = framePool.CreateCaptureSession(item);
    session.IsCursorCaptureEnabled(false);

    // Setup the event we'll use to wait for the frame and the FrameArrived callback.
    winrt::Direct3D11CaptureFrame frame{ nullptr };
    wil::shared_event captureEvent(wil::EventOptions::None);
    framePool.FrameArrived([&frame, captureEvent](auto&& framePool, auto&&)
        {
            frame = framePool.TryGetNextFrame();
            captureEvent.SetEvent();
        });

    // Start the capture and wait
    session.StartCapture();
    captureEvent.wait(2000);

    // Stop the capture
    framePool.Close();
    session.Close();

    // Extract the ID3D11Texture2D
    auto surface = frame.Surface();
    auto texture = GetDXGIInterfaceFromObject<ID3D11Texture2D>(surface);

    return texture;
}
