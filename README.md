## FBXSDK
1. Go to the link:  https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0
2. Download FBX SDK 2020.0.1 (Linux)
3. Unzip **fbx20202_fbxsdk_linux.tar.gz**
4. Go to the folder
5. `mkdir fbxsdk`
6. `chmod ugo+x FBX20202_FBXFILESDK_LINUX`
7. `./FBX20202_FBXFILESDK_LINUX ./fbxsdk`
8. Copy files inside the folder **./fbxsdk** into the **aOpenGL/ext/fbxsdk**

NOTE: FBXSDK requires xml2 library

`sudo apt-get install libxml2-dev`

NOTE: OpenGL dependencies
```
sudo apt-get install -y libxrandr-dev
sudo apt-get install -y libxinerama-dev
sudo apt-get install -y libxcursor-dev
sudo apt-get install -y libxi-dev
sudo apt-get install -y libgl-dev
sudo apt-get install -y libglu1-mesa-dev
```

## Other Libraries (GLM, GLFW)
1. Go to the **aOpenGL/ext/[LIBRARY]**
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make -j`

## Build
1. Go to the **aOpenGL/**
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make -j`
