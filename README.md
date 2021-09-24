aOpenGL은 Ubuntu에서 사용하는 OpenGL 라이브러리입니다.

aOpenGL을 사용하기 위해서는 FBXSDK 와 여러가지 외부 라이브러리들이 필요합니다.

## 외부 라이브러리 다운로드

### FBXSDK
FBX 파일들을 읽는데 필요한 라이브러리이며, Autodesk 공식 페이지에서 다운로드하시면 됩니다.

1. FBXSDK 페이지에 들어갑니다: `https://www.autodesk.com/developer-network/platform-technologies/fbx-sdk-2020-2-1`
3. FBX SDK Linux 버전을 다운로드합니다.
4. 다운로드한 폴더에 들어가서 다음 명령어를 순서대로 실행하여 압축을 풀어줍니다.
```
mkdir fbxsdk_linux
tar -xvf [FBXSDK.TAR.GZ 파일] -C fbxsdk_linux
cd fbxsdk_linux
```
4. 다음 명령어를 순서대로 실행하여 파일들을 설치합니다.
```
mkdir fbxsdk
chmod ugo+x [FBXSDK 설치 파일]
./[FBXSDK 설치 파일] fbxsdk
```
5. `fbxsdk` 폴더 안의 파일들을 `aOpenGL/ext/fbxsdk` 폴더 안으로 복사합니다.

### XML2 Library
FBXSDK 는 XML2 라이브러리가 필요합니다.
다음 명령어를 실행해서 설치합니다.
```
sudo apt-get install libxml2-dev
```

### OpenGL Dependencies
아래 라이브러리들은 OpenGL에 필요한 라이브러리입니다.
다음 명령어를 실행해서 설치합니다
```
sudo apt-get install -y libxrandr-dev;
sudo apt-get install -y libxinerama-dev;
sudo apt-get install -y libxcursor-dev;
sudo apt-get install -y libxi-dev;
sudo apt-get install -y libgl-dev;
sudo apt-get install -y libglu1-mesa-dev;
```

## 설치
aOpenGL을 설치하기 전에, GLM과 GLFW 라이브러리를 빌드해줘야 합니다.
GLM과 GLFW는 서브모듈로 ext 폴더에 있습니다.

### GLM 빌드
1. aOpenGL 폴더에 들어갑니다.
2. 다음 명령어를 순서대로 실행합니다.
```
cd ./ext/GLM
mkdir build
cd build
cmake ..
make -j
```

### GLFW 빌드
1. aOpenGL 폴더에 들어갑니다.
2. 다음 명령어를 순서대로 실행합니다.
```
cd ./ext/GLFW
mkdir build
cd build
cmake ..
make -j
```

### aOpenGL 빌드
1. aOpenGL 폴더에 들어갑니다.
2. 다음 명령어를 실행합니다.
```
mkdir build
cd build
cmake ..
make -j
```

## Making New Projects using aOpenGL
1. Go to wiki and find a page named 'How to make your new project with aOpenGL & aLibTorch'
> page: https://github.com/ltepenguin/aOpenGL/wiki/How-to-make-your-new-project-with-aOpenGL-&-aLibTorch
2. Follow the instructions step by step reading the page.
