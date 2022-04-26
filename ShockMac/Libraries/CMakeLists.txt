# Fixed point math
set (TARGET_LIB_FIX fix)
add_library(${TARGET_LIB_FIX} STATIC)
target_sources(${TARGET_LIB_FIX} PRIVATE
	FIX/Source/atofix.c
	FIX/Source/f_exp.c
	FIX/Source/fix24.c
	FIX/Source/fix.c
	FIX/Source/fix.h
	FIX/Source/fix.inc
	FIX/Source/fix_pow.c
	FIX/Source/fixsprnt.c
	FIX/Source/fix_sqrt.c
	FIX/Source/MakeTables.c
	FIX/Source/otrigtab.h
	FIX/Source/trigtab.h
)
target_include_directories(${TARGET_LIB_FIX} PUBLIC FIX/Source)
target_include_directories(${TARGET_LIB_FIX} PUBLIC H)

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
target_link_libraries(${TARGET_LIB_LG} PUBLIC ${TARGET_LIB_FIX})
