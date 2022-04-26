# LG
set (TARGET_LIB_LG lg)
add_library(${TARGET_LIB_LG} STATIC)
target_sources(${TARGET_LIB_LG} PRIVATE
	LG/Source/dbg.c
	LG/Source/dbg.h
	LG/Source/LG.h
	LG/Source/lgsprntf.c
	LG/Source/lgsprntf.h
	LG/Source/memall.c
	LG/Source/memall.h
	LG/Source/stack.c
	LG/Source/tmpalloc.c
	LG/Source/tmpalloc.h
)
target_include_directories(${TARGET_LIB_LG} PUBLIC H)
