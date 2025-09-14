#include "C:/Users/llama/source/repos Dependencies/opencv/build/include/opencv2/opencv.hpp"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "resource.h"

#define BMHEIGHT 100
#define BAR_HEIGHT 200
#define BAR_WIDTH 50
#define NUM_COLOURS 27

struct RGB
{
    unsigned char red, green, blue;
};

void makeBMP(std::vector<RGB>& pixels)
{
    BITMAPFILEHEADER fileHeader
    {
        .bfType = 0x4d42,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
    };

    BITMAPINFOHEADER infoHeader
    {
        .biSize = sizeof(BITMAPINFOHEADER),
        .biHeight = BMHEIGHT,
        .biPlanes = 1,
        .biBitCount = 24
    };

    infoHeader.biWidth = (long)pixels.size();
    std::ofstream file("colours.bmp", std::ios::binary);

    if (file.is_open())
    {
        file.write((char*)&fileHeader, sizeof(fileHeader));
        file.write((char*)&infoHeader, sizeof(infoHeader));
        
        int padding = 4 - ((infoHeader.biWidth * 3) % 4);

        std::vector<unsigned char> extraChars(padding, 0);

        for (int i = 0; i < BMHEIGHT; i++)
        {
            file.write((char*)pixels.data(), infoHeader.biWidth * 3);
            file.write((char*)extraChars.data(), padding);
        }

        file.close();
    }
}

HBITMAP Bitmap{};

void ThePaint(HWND hwnd)
{
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(hwnd, &paint);

    HDC dc2 = CreateCompatibleDC(NULL);
    HBITMAP oldBM = (HBITMAP)SelectObject(dc2, Bitmap);
    BITMAP bmData;
    GetObject(Bitmap, sizeof(BITMAP), &bmData);
    //BitBlt(dc, 0, 0, bmData.bmWidth, bmData.bmHeight, dc2, 0, 0, SRCCOPY);
    RECT rect;
    GetClientRect(hwnd, &rect);
    StretchBlt(dc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, dc2, 0, 0, bmData.bmWidth, bmData.bmHeight, SRCCOPY);

    SelectObject(dc2, oldBM);
    EndPaint(hwnd, &paint);
}


LRESULT CALLBACK NewWindowCallback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_PAINT:
        ThePaint(hwnd);
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}


HWND CreateNewWindow(HINSTANCE inst, int windowWidth, int windowHeight)
{
    WNDCLASSEX windowClass
    {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = NewWindowCallback,
        .hInstance = inst,
        .hIcon = LoadIcon(inst, MAKEINTRESOURCE(IDI_ICON1)),
        .hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1),
        .lpszClassName = L"classOfTheWindow",
        .hIconSm = LoadIcon(inst, MAKEINTRESOURCE(IDI_ICON1)),
    };

    RegisterClassEx(&windowClass);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    RECT windowRect;
    SetRect(&windowRect, (width - windowWidth) / 2, (height - windowHeight) / 2, 
        (width + windowWidth) / 2, (height + windowHeight) / 2);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

    AdjustWindowRectEx(&windowRect, style, FALSE, 0);

    return CreateWindowEx(0, L"classOfTheWindow", L"Motion Of Colour", style,
        windowRect.left, windowRect.top, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
        NULL, NULL, inst, NULL);
}

void CreatePixels(std::vector<BYTE>& pixels)
{
    std::vector<BYTE> row;

    BYTE hexColour[3]{ 0, 127, 255 };
    for (BYTE red : hexColour)
    {
        for (BYTE green : hexColour)
        {
            for (BYTE blue : hexColour)
            {
                std::vector<BYTE> pixel;
                pixel.reserve(3);
                pixel.push_back(red);
                pixel.push_back(green);
                pixel.push_back(blue);

                for (int barWidth = 0; barWidth < BAR_WIDTH; barWidth++)
                {
                    row.insert(row.end(), pixel.begin(), pixel.end());
                }
            }
        }
    }

    for (int padding = 4 - (row.size() % 4); padding; padding--)
    {
        row.push_back(0);
    }

    for (int height = 0; height < BAR_HEIGHT; height++)
    {
        pixels.insert(pixels.end(), row.begin(), row.end());
    }
}


HBITMAP CreateBitmapFromPixels(int width, int height, BYTE* pixels)
{
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // negative = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 24; // or 32 for RGBA
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HBITMAP hBitmap = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);

    if (hBitmap && pixels)
    {
        memcpy(pBits, pixels, width * height * 3); // copy RGBA data, or * 4 for RGBA
    }

    return hBitmap;
}


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE prevhInstance, PSTR commandLine, int commandShow)
{
    cv::VideoCapture video{"video.mp4"}; // PUT CORRECT VIDEO PATH HERE

    if (!video.isOpened())
    {
        std::cout << "Couldn't open video.";
    }

    long long frameCount = static_cast<long long> (video.get(cv::CAP_PROP_FRAME_COUNT));

    cv::Mat frame;

    std::string name{ "video" };
    cv::namedWindow(name.c_str(), cv::WINDOW_AUTOSIZE);

    std::vector<unsigned char> colours;

    for (int i = 0; i < 500; i++)
    {
        if (!video.read(frame)) break;

        cv::imshow(name.c_str(), frame);
        cv::waitKey(30);

        long long totalRed{};
        long long totalGreen{};
        long long totalBlue{};


        for (int row = 0; row < frame.rows; row++)
        {
            for (int col = 0; col < frame.cols; col++)
            {
                cv::Vec3b& pixel = frame.at<cv::Vec3b>(row, col);
                totalBlue += pixel[0];
                totalGreen += pixel[1];
                totalRed += pixel[2];
            }
        }

        int totalPixels = frame.rows * frame.cols;
        int avgRed = totalRed / totalPixels;
        int avgGreen = totalGreen / totalPixels;
        int avgBlue = totalBlue / totalPixels;

        for (int i = 0; i < 3; i ++)
        {
            colours.push_back(avgRed);
            colours.push_back(avgGreen);
            colours.push_back(avgBlue);
        }
    }
    
    for (int padding = 4 - (colours.size() % 4); padding; padding--)
    {
        colours.push_back(0);
    }

    std::vector<unsigned char> coloursFull;
    coloursFull.reserve(colours.size() * BMHEIGHT);

    for (int i = 0; i < BMHEIGHT; i++) {
        coloursFull.insert(coloursFull.end(), colours.begin(), colours.end());
    }

    // now call with correct width and height
    Bitmap = CreateBitmapFromPixels(colours.size() / 3, BMHEIGHT, coloursFull.data());

    video.release();
    cv::destroyAllWindows();

    HWND handle = CreateNewWindow(hinstance, colours.size() / 3, BMHEIGHT);

    if (handle)
    {
        MSG message;
        while (GetMessage(&message, nullptr, 0, 0))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}
