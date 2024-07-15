## Description
This is a simple project that uses OpenCV to detect tanks in War Thunder. It gets a performance of     
~142 fps on a videocard with 16 Gb VRAM. It also has an auto aiming mode , basically aimbot.

## Building
1. Install darknet and other dependencies using steps at https://github.com/hank-ai/darknet
2. To use the gpu you need to download cuda wheels from https://github.com/cudawarped/opencv_contrib/releases
 version 4.10.0 >> OpenCV (Release and Debug builds including python bindings and docs) 
     built with VS2022 against CUDA 12.5., Nvidia Video Codec SDK 12.2 and cuDNN 9.2.0.
3. After all these steps you can build the .sln file
4. You will also need the models to run the project
## Screenshots:
 ![detection at distance]("Screenshots/screenshot1.png")           
 ![aimbot mode]("Screenshots/screenshot2.png")                           
