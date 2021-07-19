## FBXSDK
1. Go to the link:  https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0
2. Download FBX SDK 2020.0.1 (Linux)
3. Unzip **fbx20202_fbxsdk_linux.tar.gz**
4. Install FBXSDK
4.1. Go to the folder, and type command into the terminal: 
4.2. Create folder **fbxsdk**
4.3. `chmod ugo+x FBX20202_FBXFILESDK_LINUX`
4.4. `./FBX20202_FBXFILESDK_LINUX ./fbxsdk`
6. Copy files inside the folder **./fbxsdk** into the **aOpenGL/ext/fbxsdk**

## Other Libraries (GLM, GLFW)
1. Go to the **ext/[LIBRARY]**
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make -j`

## Build
1. `mkdir build`
2. `cd build`
3. `cmake ..`
4. `make -j`
