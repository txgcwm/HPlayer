set(EXECUTABLE_OUTPUT_PATH ${OUTDIR}/test)

include_directories(
	${TOPDIR}/src
)

link_directories(
	/usr/local/lib
)


#################################
add_executable(MediaPlayer MediaPlayer.cpp)
target_link_libraries(MediaPlayer player SDL2 avdevice avformat avfilter avcodec swresample swscale avutil X11 xcb xcb-render xcb-shm xcb-xfixes xcb-shape lzma asound pthread dl m z)


 