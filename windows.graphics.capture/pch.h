#pragma once

// Windows SDK
#include <Windows.h>
#include <Unknwn.h>
#include <inspectable.h>

// WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>

// WinRT Interop
#include <windows.graphics.capture.interop.h>

// Direct3D11
#include <d3d11_4.h>
#include <dxgi1_6.h>

// WIL
//#include <wil/resource.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/core/directx.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

// STL
#include <vector>

// Helpers headers adapted from robmikh.common
#include "d3dHelpers.h"
#include "direct3d11.interop.h"
