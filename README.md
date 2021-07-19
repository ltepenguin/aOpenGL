## FBXSDK
1. Go to the link:  https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-0
2. Download FBX SDK 2020.0.1 (Linux)
3. Unzip **fbx20202_fbxsdk_linux.tar.gz**
4. Install FBXSDK
5. Go to the folder, and type command into the terminal: 
6. Create folder **fbxsdk**
7. `chmod ugo+x FBX20202_FBXFILESDK_LINUX`
8. `./FBX20202_FBXFILESDK_LINUX ./fbxsdk`
9. Copy files inside the folder **./fbxsdk** into the **aOpenGL/ext/fbxsdk**

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
