AC_INIT(game/main.cpp)

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Setup for automake
AM_INIT_AUTOMAKE("Critical_Mass",__NO_VERSION__)
AM_CONFIG_HEADER(defines.h)

AC_EXEEXT

case $target in
	*mingw*)
		TARGET=WIN32 ;;
	*apple*)
		TARGET=APPLE ;;
	*)
		TARGET=UNIX ;;
esac

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB

dnl Checks for libraries.

dnl Checks for header files.

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for library functions.

AM_CONDITIONAL(APPLE,test "x$TARGET" = xAPPLE)
AM_CONDITIONAL(WIN32,test "x$TARGET" = xWIN32)

CFLAGS=""
CXXFLAGS="-std=c++0x -DGAME_HAS_HERO_PARTICLE -DUSE_ONLINE_UPDATE"

if test "x$GCC" = xyes; then
   CFLAGS="$CFLAGS -W -Wall"
   CXXFLAGS="$CXXFLAGS -W -Wall"
fi

AC_ARG_ENABLE(debug,
[  --enable-debug               Enable debugging [default=off]],
        enable_debug=$enableval, enable_debug=off)
if test "x$enable_debug" = xyes; then
        CFLAGS="$CFLAGS -g"
        CXXFLAGS="$CXXFLAGS -g"
fi

AC_ARG_ENABLE(optimize,
[  --enable-optimize=level      Enable optimization [default=2]],
        enable_optmize=$enableval, enable_optimize=2)
if test "x$enable_optimize" != "xno" ; then
        CFLAGS="$CFLAGS -O$enable_optimize"
        CXXFLAGS="$CXXFLAGS -O$enable_optimize"
fi

AC_ARG_ENABLE(dyngl,
[  --enable-dyngl               Load GL library dynamically [default=off]],
        enable_dyngl=$enableval, enable_dyngl=off)
if test "x$enable_dyngl" = xyes; then
        CFLAGS="$CFLAGS -DDYNAMIC_GL"
        CXXFLAGS="$CXXFLAGS -DDYNAMIC_GL"
fi

AC_ARG_ENABLE(lzma,
[  --enable-lzma                Enable LZMA [default=off]],
        enable_lzma=$enableval, enable_lzma=off)
AM_CONDITIONAL(USE_LZMA,test "x$enable_lzma" = xyes)

AC_ARG_ENABLE(trace,
[  --enable-trace               Enable tracing [default=off]],
        enable_trace=$enableval, enable_trace=off)
if test "x$enable_trace" = xyes; then
        CFLAGS="$CFLAGS -DTRACE"
        CXXFLAGS="$CXXFLAGS -DTRACE"
fi

AC_CHECK_LIB(SDL, main,
    LIBS="$LIBS `sdl-config --libs`" ; \
    CFLAGS="$CFLAGS `sdl-config --cflags`" ; \
    CXXFLAGS="$CXXFLAGS `sdl-config --cflags`" ; \
    HAVE_SDL=1, AC_MSG_ERROR(SDL is needed), `sdl-config --libs`)

AC_CHECK_LIB(SDL_image, main, LIBS="$LIBS -lSDL_image", 
    AC_MSG_ERROR(SDL_image is needed))

AC_CHECK_LIB(SDL_mixer, main, LIBS="$LIBS -lSDL_mixer", 
    AC_MSG_ERROR(SDL_mixer is needed))

if test "x$TARGET" = xWIN32; then
    AC_MSG_RESULT(Adding special ingredients for mingw32)
    LIBS="$LIBS -lopengl32 -lpng -lz -ljpeg -lstdc++ -lws2_32 -lwinmm"
    CXXFLAGS="$CXXFLAGS -DCURL_STATICLIB"
else
    LIBS="-L/usr/local/lib -L/usr/X11R6/lib $LIBS"
    CXXFLAGS="$CXXFLAGS -I/usr/X11R6/include"
	AC_CHECK_LIB(curl-gnutls, main,,
                     AC_CHECK_LIB(curl,main,,AC_MSG_ERROR(curl is needed))
                    )
	AC_CHECK_LIB(tinyxml, main,, AC_MSG_ERROR(libtinyxml is needed))
	AC_CHECK_LIB(png, png_get_io_ptr,, AC_MSG_ERROR(libpng is needed))
	AC_CHECK_LIB(z, main,, AC_MSG_ERROR(libz is needed))
	if test "x$TARGET" = xUNIX; then
	    if test "x$enable_dyngl" = xoff; then
		AC_CHECK_LIB(GL, glBegin,, AC_MSG_ERROR(OpenGL is needed))
	    fi
	fi
fi

if test "x$TARGET" = xAPPLE; then
    CXXFLAGS="$CXXFLAGS -I/Developer/Headers/FlatCarbon/"
    LIBS="$LIBS -lvorbisfile -lvorbis -logg -framework OpenGL"

dnl AC_CHECK_LIB(mx, asinf,, 
dnl     AC_MSG_ERROR(libmx is needed))
fi

AC_CHECK_LIB(m, sin,, 
    AC_MSG_ERROR(libm is needed))

dnl Checking for Doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

AM_CONDITIONAL([HAVE_DOXYGEN],
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_OUTPUT(Makefile utils/Makefile utilssdl/Makefile utilsgl/Makefile game/Makefile tools/Makefile docs/Makefile)

echo "Configuration:

Version:                    $VERSION
Source code location:       $srcdir
Install path:               $prefix
Compiler:                   $CC $CXX
C++ flags:                  $CXXFLAGS
host:                       $host
target:                     $target
"
