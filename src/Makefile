FFMPEG_LIBS=    libavdevice                        \
				libavformat                        \
				libavfilter                        \
				libavcodec                         \
				libswresample                      \
				libswscale                         \
				libavutil                          \

FFMPEG_CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
FFMPEG_LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)
$(warning $(FFMPEG_CFLAGS))
$(warning $(FFMPEG_LDLIBS))
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDLIBS := $(shell sdl2-config --libs)
$(warning $(SDL_CFLAGS))
$(warning $(SDL_LDLIBS))

CFLAGS += -Wall -g -fpermissive

CC := clang++

MediaPlayer: MediaPlayer.o MediaDecoder.o FFSDL.o MediaBuffer.o LoopBuffer.o
	$(CC) -o MediaPlayer MediaPlayer.cpp MediaDecoder.o FFSDL.o MediaBuffer.o $(CFLAGS) $(FFMPEG_LDLIBS) $(SDL_LDLIBS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

MediaPlayer.o: MediaPlayer.cpp
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

MediaDecoder.o: MediaDecoder.cpp MediaDecoder.h
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS)

FFSDL.o: FFSDL.cpp FFSDL.h
	$(CC) -c $^ $(CFLAGS) $(SDL_CFLAGS)

MediaBuffer.o: MediaBuffer.cpp MediaBuffer.h Queue.h QueueNode.h
	$(CC) -c $^ $(CFLAGS) $(FFMPEG_CFLAGS) $(SDL_CFLAGS)

LoopBuffer.o: LoopBuffer.cpp LoopBuffer.h
	$(CC) -c $^ $(CFLAGS)

clean:
	rm *.o *.gch MediaDecoder
