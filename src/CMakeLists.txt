include_directories(
	/usr/local/include
)

file(GLOB_RECURSE HEADERFILES "../include/*")
aux_source_directory(. DIR_SRCS)
add_library(player ${DIR_SRCS} ${HEADERFILES})