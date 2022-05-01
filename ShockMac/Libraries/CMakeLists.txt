
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


# LG
set (TARGET_LIB_LG lg)
set (DIR_LIB_LG ${DIR_LIB}/LG/Source)

add_library(${TARGET_LIB_LG} STATIC)
target_sources(${TARGET_LIB_LG} PRIVATE
	${DIR_LIB_LG}/dbg.c
	${DIR_LIB_LG}/dbg.h
	${DIR_LIB_LG}/LG.h
	${DIR_LIB_LG}/lgsprntf.c
	${DIR_LIB_LG}/lgsprntf.h
	${DIR_LIB_LG}/memall.c
	${DIR_LIB_LG}/memall.h
	${DIR_LIB_LG}/stack.c
	${DIR_LIB_LG}/tmpalloc.c
	${DIR_LIB_LG}/tmpalloc.h
)
target_link_libraries(${TARGET_LIB_LG} PUBLIC ${TARGET_LIB_FIX})
