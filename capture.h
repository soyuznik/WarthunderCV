#include <windows.h>

void screenshot_window_back(HWND handle) {
    RECT client_rect = { 0 };
    GetWindowRect(handle, &client_rect);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;


    HDC hdcScreen = GetDC(HWND_DESKTOP);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdc, hbmp);

    BitBlt(hdc, 0, 0, width, height, hdcScreen, client_rect.left, client_rect.top, SRCCOPY);

    BITMAPINFO bmp_info = { 0 };
    bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
    bmp_info.bmiHeader.biWidth = width;
    bmp_info.bmiHeader.biHeight = height;
    bmp_info.bmiHeader.biPlanes = 1;
    bmp_info.bmiHeader.biBitCount = 24;
    bmp_info.bmiHeader.biCompression = BI_RGB;

    int bmp_padding = (width * 3) % 4;
    if (bmp_padding != 0) bmp_padding = 4 - bmp_padding;

    BYTE* bmp_pixels = new BYTE[(width * 3 + bmp_padding) * height];;
    GetDIBits(hdc, hbmp, 0, height, bmp_pixels, &bmp_info, DIB_RGB_COLORS);

    BITMAPFILEHEADER bmfHeader;
    HANDLE bmp_file_handle = CreateFile(L"TestFile.bmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = (width * 3 + bmp_padding) * height + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;

    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM

    DWORD dwBytesWritten = 0;
    WriteFile(bmp_file_handle, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(bmp_file_handle, (LPSTR)&bmp_info.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(bmp_file_handle, (LPSTR)bmp_pixels, (width * 3 + bmp_padding) * height, &dwBytesWritten, NULL);

    //Close the handle for the file that was created
    CloseHandle(bmp_file_handle);

    DeleteDC(hdc);
    DeleteObject(hbmp);
    ReleaseDC(HWND_DESKTOP, hdcScreen);
    delete[] bmp_pixels;
    ReleaseDC(handle, hdcScreen);
}

void screenshot_window_base(HWND handle , LPCWSTR output) { 

    // Analysis , Profiled no memory leaks.
    // =============================================
    //  Capture 1024x 768 windowed 
    //  Time 36.996 seconds , Debug mode x64
    //========================================
    // =============================================
    //  Capture 1024x 768 windowed 
    //  Time 42.247 seconds , Release mode x64 , overwrite files.
    //========================================
    // =============================================
    //  Capture 1024x 768 windowed 
    //  Time 24.75 seconds , Release mode x64 , create new files
    //========================================




    RECT client_rect = { 0 };
    GetClientRect(handle, &client_rect);
    int width = client_rect.right - client_rect.left;
    int height = client_rect.bottom - client_rect.top;


    HDC hdcScreen = GetDC(handle);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdc, hbmp);

    BitBlt(hdc, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    BITMAPINFO bmp_info = { 0 };
    bmp_info.bmiHeader.biSize = sizeof(bmp_info.bmiHeader);
    bmp_info.bmiHeader.biWidth = width;
    bmp_info.bmiHeader.biHeight = height;
    bmp_info.bmiHeader.biPlanes = 1;
    bmp_info.bmiHeader.biBitCount = 24;
    bmp_info.bmiHeader.biCompression = BI_RGB;

    int bmp_padding = (width * 3) % 4;
    if (bmp_padding != 0) bmp_padding = 4 - bmp_padding;

    BYTE* bmp_pixels = new BYTE[(width * 3 + bmp_padding) * height];;
    GetDIBits(hdc, hbmp, 0, height, bmp_pixels, &bmp_info, DIB_RGB_COLORS);

    BITMAPFILEHEADER bmfHeader;
    HANDLE bmp_file_handle = CreateFile(output, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size
    DWORD dwSizeofDIB = (width * 3 + bmp_padding) * height + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB;

    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM

    DWORD dwBytesWritten = 0;
    WriteFile(bmp_file_handle, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
    WriteFile(bmp_file_handle, (LPSTR)&bmp_info.bmiHeader, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
    WriteFile(bmp_file_handle, (LPSTR)bmp_pixels, (width * 3 + bmp_padding) * height, &dwBytesWritten, NULL);

    //Close the handle for the file that was created
    CloseHandle(bmp_file_handle);

    DeleteDC(hdc);
    DeleteObject(hbmp);
    ReleaseDC(NULL, hdcScreen);
    delete[] bmp_pixels;
}