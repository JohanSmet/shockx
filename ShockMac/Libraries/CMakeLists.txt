
set (DIR_LIB ShockMac/Libraries)

# Fixed point math
set (TARGET_LIB_FIX fix)
set (DIR_LIB_FIX ${DIR_LIB}/FIX/Source)

add_library(${TARGET_LIB_FIX} STATIC)
target_sources(${TARGET_LIB_FIX} PRIVATE
	${DIR_LIB_FIX}/atofix.c
	${DIR_LIB_FIX}/f_exp.c
	${DIR_LIB_FIX}/fix24.c
	${DIR_LIB_FIX}/fix.c
	${DIR_LIB_FIX}/fix.h
	${DIR_LIB_FIX}/fix.inc
	${DIR_LIB_FIX}/fix_pow.c
	${DIR_LIB_FIX}/fixsprnt.c
	${DIR_LIB_FIX}/fix_sqrt.c
	${DIR_LIB_FIX}/MakeTables.c
	${DIR_LIB_FIX}/otrigtab.h
	${DIR_LIB_FIX}/trigtab.h
)
target_include_directories(${TARGET_LIB_FIX} PUBLIC ${DIR_LIB_FIX})
target_include_directories(${TARGET_LIB_FIX} PUBLIC ${DIR_LIB}/H)
target_link_libraries(${TARGET_LIB_FIX} PRIVATE ${LIBS_MATH})

# Fixed point math utilities
if (BUILD_UTILS)
	set (TARGET_MAKE_TABLES make_tables)
	add_executable(${TARGET_MAKE_TABLES})
	target_sources(${TARGET_MAKE_TABLES} PRIVATE
		${DIR_LIB}/FIX/Utils/fmaketab.cpp
	)
	target_include_directories(${TARGET_MAKE_TABLES} PUBLIC ${DIR_LIB}/H)
endif()

# Fixed point math (C++)
set (TARGET_LIB_FIXPP fixpp)
set (DIR_LIB_FIXPP ${DIR_LIB}/FIXPP/Source)

add_library(${TARGET_LIB_FIXPP} STATIC)
target_sources(${TARGET_LIB_FIXPP} PRIVATE
	${DIR_LIB_FIXPP}/fixpp.cpp
	${DIR_LIB_FIXPP}/fixpp.h
)
target_link_libraries(${TARGET_LIB_FIXPP} PUBLIC ${TARGET_LIB_FIX})

# LG
set (TARGET_LIB_LG lg)
set (DIR_LIB_LG ${DIR_LIB}/LG/Source)

add_library(${TARGET_LIB_LG} STATIC)
target_sources(${TARGET_LIB_LG} PRIVATE
	${DIR_LIB_LG}/dbg.c
	${DIR_LIB_LG}/dbg.h
	${DIR_LIB_LG}/lg.h
	${DIR_LIB_LG}/lgsprntf.c
	${DIR_LIB_LG}/lgsprntf.h
	${DIR_LIB_LG}/memall.c
	${DIR_LIB_LG}/memall.h
	${DIR_LIB_LG}/stack.c
	${DIR_LIB_LG}/tmpalloc.c
	${DIR_LIB_LG}/tmpalloc.h
)
target_include_directories(${TARGET_LIB_LG} PUBLIC ${DIR_LIB_LG})
target_link_libraries(${TARGET_LIB_LG} PUBLIC ${TARGET_LIB_FIX})

# DSTRUCT
set (TARGET_LIB_DSTRUCT dstruct)
set (DIR_LIB_DSTRUCT ${DIR_LIB}/DSTRUCT/Source)

add_library(${TARGET_LIB_DSTRUCT} STATIC)
target_sources(${TARGET_LIB_DSTRUCT} PRIVATE
	${DIR_LIB_DSTRUCT}/array.c
	${DIR_LIB_DSTRUCT}/array.h
	${DIR_LIB_DSTRUCT}/hash.c
	${DIR_LIB_DSTRUCT}/hash.h
	${DIR_LIB_DSTRUCT}/llist.c
	${DIR_LIB_DSTRUCT}/llist.h
	${DIR_LIB_DSTRUCT}/lllist.c
	${DIR_LIB_DSTRUCT}/pqueue.c
	${DIR_LIB_DSTRUCT}/pqueue.h
	${DIR_LIB_DSTRUCT}/rect.c
	${DIR_LIB_DSTRUCT}/rect.h
	${DIR_LIB_DSTRUCT}/slist.h
)
target_include_directories(${TARGET_LIB_DSTRUCT} PUBLIC ${DIR_LIB_DSTRUCT})
target_link_libraries(${TARGET_LIB_DSTRUCT} PUBLIC ${TARGET_LIB_LG})

# PALETTE
#set (TARGET_LIB_PALETTE palette)
#set (DIR_LIB_PALETTE ${DIR_LIB}/PALETTE/Source)
#
#add_library(${TARGET_LIB_PALETTE} STATIC)
#target_sources(${TARGET_LIB_PALETTE} PRIVATE
#	${DIR_LIB_PALETTE}/palette.c
#	${DIR_LIB_PALETTE}/palette.h
#)
#target_link_libraries(${TARGET_LIB_PALETTE} PUBLIC ${TARGET_LIB_LG})

# RES
set (TARGET_LIB_RES res)
set (DIR_LIB_RES ${DIR_LIB}/RES/Source)

add_library(${TARGET_LIB_RES} STATIC)
target_sources(${TARGET_LIB_RES} PRIVATE
	${DIR_LIB_RES}/lzw.c
	${DIR_LIB_RES}/lzw.h
	${DIR_LIB_RES}/refacc.c
	${DIR_LIB_RES}/resacc.c
	${DIR_LIB_RES}/resbuild.c
	${DIR_LIB_RES}/res.c
	${DIR_LIB_RES}/resfile.c
	${DIR_LIB_RES}/res.h
	${DIR_LIB_RES}/res_.h
	${DIR_LIB_RES}/resload.c
	${DIR_LIB_RES}/resmake.c
	${DIR_LIB_RES}/restypes.c
	${DIR_LIB_RES}/restypes.h
)
target_link_libraries(${TARGET_LIB_RES} PUBLIC ${TARGET_LIB_LG})

# RND
set (TARGET_LIB_RND rnd)
set (DIR_LIB_RND ${DIR_LIB}/RND/Source)

add_library(${TARGET_LIB_RND} STATIC)
target_sources(${TARGET_LIB_RND} PRIVATE
	${DIR_LIB_RND}/rnd.c
	${DIR_LIB_RND}/rnd.h
)
target_link_libraries(${TARGET_LIB_RND} PUBLIC ${TARGET_LIB_LG})
target_include_directories(${TARGET_LIB_RND} PUBLIC ${DIR_LIB_RND})
