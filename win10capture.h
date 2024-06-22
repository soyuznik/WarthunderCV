#pragma once
#include <iostream>
#include <Windows.h>
#include <dxgi.h>
#include <inspectable.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <roerrorapi.h>
#include <shlobj_core.h>
#include <dwmapi.h>
#pragma comment(lib,"Dwmapi.lib")
#pragma comment(lib,"windowsapp.lib")
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>

winrt::impl::com_ref<IDXGIDevice> dxgiDevice;
winrt::impl::com_ref<IDXGIDevice2> idxgiDevice2;
winrt::com_ptr<ID3D11Device> d3dDevice;
winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
winrt::com_ptr<IDXGIAdapter> adapter;
winrt::com_ptr<IDXGIFactory2> factory;
ID3D11DeviceContext* d3dContext = nullptr;
RECT rect{};
winrt::Windows::Graphics::SizeInt32 sizev;
winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{nullptr};
winrt::Windows::Foundation::IActivationFactory activationFactory;
winrt::com_ptr<IGraphicsCaptureItemInterop> interopFactory;
winrt::Windows::Graphics::Capture::GraphicsCaptureItem captureItem = { nullptr };
bool _winrt__init = 0;
cv::Mat WindowsGraphicsCapture(HWND hwndTarget)
{
    if (!_winrt__init) {//////
        // Init COM
        winrt::init_apartment(winrt::apartment_type::multi_threaded);
        winrt::check_hresult(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            nullptr, 0, D3D11_SDK_VERSION, d3dDevice.put(), nullptr, nullptr));
        dxgiDevice = d3dDevice.as<IDXGIDevice>();
        {
            winrt::com_ptr<::IInspectable> inspectable;
            winrt::check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), inspectable.put()));
            device = inspectable.as<winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice>();
        }
        idxgiDevice2 = dxgiDevice.as<IDXGIDevice2>();
        winrt::check_hresult(idxgiDevice2->GetParent(winrt::guid_of<IDXGIAdapter>(), adapter.put_void()));
        winrt::check_hresult(adapter->GetParent(winrt::guid_of<IDXGIFactory2>(), factory.put_void()));
        d3dDevice->GetImmediateContext(&d3dContext);
        DwmGetWindowAttribute(hwndTarget, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
        sizev = winrt::Windows::Graphics::SizeInt32{ rect.right - rect.left, rect.bottom - rect.top };
        

        activationFactory = winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
        interopFactory = activationFactory.as<IGraphicsCaptureItemInterop>();
        interopFactory->CreateForWindow(hwndTarget, winrt::guid_of<ABI::Windows::Graphics::Capture::IGraphicsCaptureItem>(),
            reinterpret_cast<void**>(winrt::put_abi(captureItem)));
        _winrt__init = 1;
    }////

    /////////////////////////////////////////////////////////////
    m_framePool =
        winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
            device,
            winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
            2,
            sizev);
    auto isFrameArrived = false;
    winrt::com_ptr<ID3D11Texture2D> texture;
    const auto session = m_framePool.CreateCaptureSession(captureItem);
    m_framePool.FrameArrived([&](auto& framePool, auto&)
        {
            if (isFrameArrived) return;
            auto frame = framePool.TryGetNextFrame();

            struct __declspec(uuid("A9B3D012-3DF2-4EE3-B8D1-8695F457D3C1"))
                IDirect3DDxgiInterfaceAccess : ::IUnknown
            {
                virtual HRESULT __stdcall GetInterface(GUID const& id, void** object) = 0;
            };

            auto access = frame.Surface().as<IDirect3DDxgiInterfaceAccess>();
            access->GetInterface(winrt::guid_of<ID3D11Texture2D>(), texture.put_void());
            isFrameArrived = true;
            return;
        });


    session.IsCursorCaptureEnabled(false);
    //session.IsBorderRequired(false);
    try {
        session.StartCapture();
    }
    catch (...) {
        std::cout << "[ERROR] session.StartCapture() - window is hidden or closed";
        exit(1);
    }
    


    // Message pump
    MSG msg;
    clock_t timer = clock();
    while (!isFrameArrived)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
            DispatchMessage(&msg);

        if (clock() - timer > 20000)
        {
            // TODO: try to make here a better error handling
            return cv::Mat();
        }
    }

    session.Close();

    D3D11_TEXTURE2D_DESC capturedTextureDesc;
    texture->GetDesc(&capturedTextureDesc);
    capturedTextureDesc.Usage = D3D11_USAGE_STAGING;
    capturedTextureDesc.BindFlags = 0;
    capturedTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    capturedTextureDesc.MiscFlags = 0;

    winrt::com_ptr<ID3D11Texture2D> userTexture = nullptr;
    winrt::check_hresult(d3dDevice->CreateTexture2D(&capturedTextureDesc, NULL, userTexture.put()));
    d3dContext->CopyResource(userTexture.get(), texture.get());
    D3D11_MAPPED_SUBRESOURCE resource;
    winrt::check_hresult(d3dContext->Map(userTexture.get(), NULL, D3D11_MAP_READ, 0, &resource));

    //https://arxiv.org/pdf/1702.02514
    int scWidth = capturedTextureDesc.Width;
    int scHeight = capturedTextureDesc.Height;
    int CAMERA_CHANNELS = 4;
    std::vector<unsigned char> buffer(scWidth * scHeight * CAMERA_CHANNELS);
    unsigned char* mappedData = reinterpret_cast<unsigned char*>(resource.pData);
    for (int h = 0; h < scHeight; h++) {
        unsigned char* srcRow = mappedData + (resource.RowPitch * h);
        unsigned char* dstRow = buffer.data() + scWidth * CAMERA_CHANNELS * h;
        memcpy(dstRow, srcRow, scWidth * CAMERA_CHANNELS);
    }
    d3dContext->Unmap(userTexture.get(), 0);
    IplImage* frame = cvCreateImageHeader(cvSize(scWidth, scHeight),
        IPL_DEPTH_8U, CAMERA_CHANNELS);
    frame->imageData = reinterpret_cast<char*>(buffer.data());
    cvSetData(frame, buffer.data(), frame->widthStep);
    //delete[] buffer;
    //delete mappedData;
    //userTexture->Release();
    //resource->Release();
    // cv::Mat mt = cv::Mat(frame, true); // constructor dissapered
    //https://stackoverflow.com/questions/15925084/conversion-from-iplimage-to-cvmat
    cv::Mat mt = cv::cvarrToMat(frame).clone();
    return mt;
}