# Process this file with autoconf to produce a configure script.
AC_INIT([pi],[1.3.0],[john.shumway@asu.edu])

#save flags passed to configure
config_flags="$*"

AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE(pi, 1.3,0)
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_MAINTAINER_MODE 

AC_DEFINE_UNQUOTED([CONFIG_FLAGS],["$config_flags"],[Flags passed to configure])

# Checks for programs.
AC_PROG_CXX
AC_PROG_F77
AC_PROG_INSTALL
AC_PROG_AWK
AC_F77_WRAPPERS
AC_LANG(C++)
AC_ARG_ENABLE(mpi,[  --enable-mpi            Produces an mpi executable],
[
ACX_MPI
AC_DEFINE(ENABLE_MPI,[],[Flag to enable mpi features.]) 
CXX="$MPICXX"
LIBS="$MPILIBS $LIBS"
])
LT_INIT

# Checks for libraries.
AC_ARG_ENABLE(sprng,[  --enable-sprng            Use the SPRNG library],
  [
  AC_CHECK_LIB(sprng,get_rn_dbl, [
    LIBS="-lsprng $LIBS"
    AC_DEFINE(ENABLE_SPRNG,[],[Flag to enable SPRNG library.]) 
  ],[
    echo "You requested --enable-sprng but linking to libsprng failed."
    exit -1
  ])
])

ACX_LAPACK(LIBS="$LAPACK_LIBS $BLAS_LIBS $LIBS $FLIBS")

AM_PATH_XML2(2.4.0,,AC_MSG_ERROR([
*** Libxml is required to build pi; Available at 
http://www.libxml.org/.]))
LIBS="$XML_LIBS $LIBS"
CPPFLAGS="$XML_CFLAGS $CPPFLAGS"

AX_PATH_GSL([1.0],[],[AC_MSG_ERROR(
*** Libgsl is required to build pi; Available at http://www.gnu.org/software/gsl/.)])
LIBS="$GSL_LIBS $LIBS"
CPPFLAGS="$GSL_CFLAGS $CPPFLAGS"
ACX_BLITZ(required)
AC_CHECK_LIB(hdf5,H5Fopen)
AC_CHECK_LIB(fftw3,fftw_plan_dft)

AC_ARG_WITH(ndim, [  --with-ndim=[NDIM]    Set the number of physical dimensions (default is 3)], 
AC_DEFINE_UNQUOTED(NDIM,${withval},[The number of physics dimensions.]),
AC_DEFINE_UNQUOTED(NDIM,3,[The number of physics dimensions.])
)

AC_CHECK_FUNC(getopt_long,
[AC_DEFINE(HAVE_GETOPT_H, 1, 
  [Define this if there is a system getopt.h header])
], [])

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile \
    src/Makefile \
    src/action/Makefile \
    src/algorithm/Makefile \
    src/demo/Makefile \
    src/emarate/Makefile \
    src/estimator/Makefile \
    src/fixednode/Makefile \
	src/parser/Makefile \
	src/sampler/Makefile \
    src/spin/Makefile \
    src/stats/Makefile \
    src/util/Makefile \
    doc/Makefile])
AC_OUTPUT

