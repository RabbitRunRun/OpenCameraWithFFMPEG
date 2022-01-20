cmake_minimum_required(VERSION 3.5)
project(OpenCameraWithFFmpeg)

set(CMAKE_CXX_STANDARD 11)

set(PLATFORM "X64" CACHE STRING "auto, x86 or x64")
set(CONFIGURATION "Release" CACHE STRING "Debug or Release")
set(CONFIGURATION ${CMAKE_BUILD_TYPE})

#link_directories(${FFMPEG_HOME}/lib)

add_executable(OpenCameraWithFFmpeg main.cpp)

set(FFMPEG_HOME E:/3DataInnerTrans/wangquyuan/ffmpeg-master-latest-win64-gpl-shared)
find_library(LIB_AVCODEC NAME avcodec PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)
find_library(LIB_AVDEVICE NAME avdevice PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)
find_library(LIB_AVFORMAT NAME avformat PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)
find_library(LIB_SWSCALE NAME swscale PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)
find_library(LIB_SWRESAMPLE NAME swresample PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)
find_library(LIB_AVUTIL NAME avutil PATHS ${FFMPEG_HOME}/lib NO_DEFAULT_PATH)

include_directories("${FFMPEG_HOME}/include")
target_link_libraries(${PROJECT_NAME}  ${LIB_AVCODEC} ${LIB_AVDEVICE} ${LIB_AVFORMAT} ${LIB_SWSCALE} ${LIB_SWRESAMPLE} ${LIB_AVUTIL})

set(OPENCV_HOME D:/opencv/opencv/build)
include_directories("${OPENCV_HOME}/include")
find_library(OPENCV_CORE NAMES opencv_core2413${ENV_SUFFIX}  PATHS  ${OPENCV_HOME}/${PLATFORM}/vc14/lib  NO_DEFAULT_PATH )
find_library(OPENCV_IMG NAMES opencv_imgproc2413${ENV_SUFFIX}  PATHS  ${OPENCV_HOME}/${PLATFORM}/vc14/lib  NO_DEFAULT_PATH )
find_library(OPENCV_HIGH NAMES opencv_highgui2413${ENV_SUFFIX}  PATHS  ${OPENCV_HOME}/${PLATFORM}/vc14/lib  NO_DEFAULT_PATH )

target_link_libraries(${PROJECT_NAME} ${OPENCV_CORE} ${OPENCV_IMG} ${OPENCV_HIGH})