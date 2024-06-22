#pragma once

namespace util
{
    inline auto CreateD3DDevice(D3D_DRIVER_TYPE const type, UINT flags, winrt::com_ptr<ID3D11Device>& device)
    {
        WINRT_ASSERT(!device);

        return D3D11CreateDevice(nullptr, type, nullptr, flags, nullptr, 0, D3D11_SDK_VERSION, device.put(),
            nullptr, nullptr);
    }

    inline auto CreateD3DDevice(UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT)
    {
        winrt::com_ptr<ID3D11Device> device;
        HRESULT hr = CreateD3DDevice(D3D_DRIVER_TYPE_HARDWARE, flags, device);
        if (DXGI_ERROR_UNSUPPORTED == hr)
        {
            hr = CreateD3DDevice(D3D_DRIVER_TYPE_WARP, flags, device);
        }

        winrt::check_hresult(hr);
        return device;
    }

    inline auto CopyD3DTexture(winrt::com_ptr<ID3D11Device> const& device, winrt::com_ptr<ID3D11Texture2D> const& texture, bool asStagingTexture)
    {
        winrt::com_ptr<ID3D11DeviceContext> context;
        device->GetImmediateContext(context.put());

        D3D11_TEXTURE2D_DESC desc = {};
        texture->GetDesc(&desc);
        // Clear flags that we don't need
        desc.Usage = asStagingTexture ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
        desc.BindFlags = asStagingTexture ? 0 : D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = asStagingTexture ? D3D11_CPU_ACCESS_READ : 0;
        desc.MiscFlags = 0;

        // Create and fill the texture copy
        winrt::com_ptr<ID3D11Texture2D> textureCopy;
        winrt::check_hresult(device->CreateTexture2D(&desc, nullptr, textureCopy.put()));
        context->CopyResource(textureCopy.get(), texture.get());

        return textureCopy;
    }
}