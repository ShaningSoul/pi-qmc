# Introduction #


The easiest way to build is to use:
```
./configure
make
```

For a parallel build
```
./configure --enable-mpi MPICXX=mpic++ MPICC=mpicc MPIF77=mpif77
```
where you should use the names of your MPI enabled compilers.

You can also build for different numbers of physical dimensions (default is NDIM=3)
```
./configure --with-ndim=2
```

# Required libraries #
We use the following libraries in the **_pi_** code:
  * [libxml2](http://xmlsoft.org/)
  * [blitz++](http://www.oonumerics.org/blitz/)
  * [hdf5](http://www.hdfgroup.org/)
  * [fftw3](http://www.fftw.org/)
  * BLAS/LAPACK
  * [gsl](http://www.gnu.org/software/gsl/)

# Advanced build using multiple directories #

In research, we often want different versions of the executables, for example, versions with and
without MPI, or versions compiled for two-dimensional systems. To accomplish this, we make
a <tt>pibuilds</tt> directory beside our svn checkout directory (<tt>pi</tt> or <tt>pi-qmc</tt>).
We then make empty subdirectories for each build, for example <tt>ndim2mpi</tt> for a two
dimensional MPI version. A typical directory structure is:

```
codes/
  pi-qmc/
    configure
    src/
    lib/
  pibuilds/
    ndim1/
    ndim2/
    ndim3/
    ndim1mpi/
    ndim2mpi/
    ndim3mpi/
    debug/
```

To build, go into the empty build directory,
```sh

cd ~/codes/pibuilds/ndim2mpi
```
Then run the configure script with the desired options
```sh

../../configure --with-ndim=2 --enable-mpi
```
You will probably want more configure options; see the platform specific instructions
below for some examples.

Then, make the code in that directory,
```sh

make -j2
```

For conveniance, you can make a soft link to the executable
```sh

ln -sf ~/codes/pibuilds/ndim3mpi ~/bin/pi2Dmpi
```

# Platform specific instructions #

## Mac OS X ##

All the dependencies are available through [macports](http://www.macports.org/). It is also handy to install
the latest gcc compilers (with gfortran), openmpi, and python utilities for data analysis and plotting.
```
$ port installed
  libxml2 @2.7.3_0 (active)
  blitz @0.9_0 (active)
  hdf5-18 @1.8.3_0 (active)
  gsl @1.12_0 (active)
  fftw-3 @3.2.2_0 (active)

  gcc44 @4.4.0_0 (active)

  python26 @2.6.2_3 (active)
  py26-numpy @1.3.0_0 (active)
  py26-ipython @0.9.1_0+scientific (active)
  py26-scipy @0.7.0_0+gcc44 (active)
  py26-tables @2.1_0 (active)
```

The following configure works well on an intel mac:
```sh

../../pi/configure CXX=g++-mp-4.4 CC=gcc-mp-4.4 \
CXXFLAGS="-O3 -g -Wall -ffast-math -ftree-vectorize \
-march=native -fomit-frame-pointer -pipe" \
F77=gfortran-mp-4.4
```
or, for an MPI enabled build,
```sh

../../pi/configure --enable-mpi CXX=g++-mp-4.4 CC=gcc-mp-4.4 F77=gfortran-mp-4.4 \
MPICC=openmpicc MPICXX=openmpicxx MPIF77=openmpif77 \
CXXFLAGS="-O3 -g -Wall -ffast-math -ftree-vectorize \
-march=native -fomit-frame-pointer -pipe"
```

On a G5 mac, try:
```sh

../../pi/configure --with-ndim=3  F77=gfortran-mp-4.4 CC=gcc-mp-4.4 CXX=g++-mp-4.4\
CXXFLAGS="-g -O3 -ffast-math -ftree-vectorize -maltivec -mpowerpc-gpopt \
-mpowerpc64 falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32 -funroll-loops"
```
or, for an MPI enabled build,
```sh

../../pi/configure --with-ndim=3 --enable-mpi \
CXXFLAGS="-g -O3 -ffast-math -ftree-vectorize -maltivec -mpowerpc-gpopt \
-mpowerpc64 falign-functions=32 -falign-labels=32 -falign-loops=32 -falign-jumps=32 -funroll-loops" \
F77=gfortran-mp-4.4 CC=gcc-mp-4.4 CXX=g++-mp-4.4  MPICC=openmpicc MPICXX=openmpicxx MPIF77=openmpif77
```

To run piworld you will also need pyqt4:
```sh

sudo port install py26-pyqt4
```

## Ubuntu ##

```sh

sudo apt-get install g++
sudo apt-get install gfortran
sudo apt-get install blitz++
sudo apt-get install hdf5-tools
sudo apt-get install libgsl0-dev
sudo apt-get install fftw3-dev
sudo apt-get install liblapack-dev
sudo apt-get install libhdf5-serial-dev
```
Or:
```sh

sudo apt-get install g++ gfortran blitz++ hdf5-tools libgsl0-dev fftw3-dev liblapack-dev libhdf5-serial-dev
```
### Ubuntu launchpad ppa (experimental) ###
There is an ubuntu repository with binary packages here:
https://launchpad.net/~jarondl/+archive/pi-qmc
To install all you have to do is:
```
sudo add-apt-repository ppa:jarondl/pi-qmc
sudo apt-get update
sudo apt-get install pi-qmc python-pitools
```
The pi-qmc package will install three binary versions, "pi1D","pi2D" and "pi3D".
The python-pitools package will install the python "pitools" module, which can later be imported in a python script using "import pitools".
The building and packaging is done automatically by launchpad several hours after a change is detected in the svn source. Please note that this is the first version of the debian packaging, so feedback is encouraged.

## Linux (CentOS 5.3) ##

You can download dependencies using <tt>yum</tt>. First, you may need to add access to the fedora [Extra Packages for Enterprise Linux (EPEL)](http://fedoraproject.org/wiki/EPEL).
```sh

sudo rpm -Uvh http://download.fedora.redhat.com/pub/epel/5/i386/epel-release-5-3.noarch.rpm
```

Then install the required packages for **_pi_**. (You probably want to compile atlas yourself to get automatic performance tuning for your hardware, but the yum install will work if you are impatient.) Note: replace <tt>x86_64</tt> with <tt>i386</tt> if you are on a 32 bit machine.
```sh

sudo yum install libxml2-devel-versionXXX.x86_64 (here I don't know the correct version)
sudo yum install blitz-devel.x86_64
sudo yum install fftw3-devel.x86_64
sudo yum install hdf5-devel.x86_64
sudo yum install atlas-sse3-devel.x86_64
sudo yum install lapack-devel.x86_64
sudo yum install gsl-devel.x86_64
```

It is useful to install the gcc 4.3 compilers.
```sh

sudo yum install gcc43.x86_64
sudo yum install gcc43-c++.x86_64
sudo yum install gcc43-gfortran.x86_64
```

You will want an MPI implementation if you want to run in parallel,
```sh

sudo yum install openmpi-devel.x86_64
```
The openmpi package will require that you run <tt>mpi-selector</tt> and open a new terminal to get the executables. Use the <tt>mpi-selector --list</tt> option to see what is available, then set a system-wide default.
```sh

sudo mpi-selector --system --set openmpi-1.2.7-gcc-x86_64
```

When you configure pi, you will probably need to specify the location of your BLAS and LAPACK routines,
```sh

../../pi/configure CXX=g++43 CC=gcc43 F77=gfortran43 CXXFLAGS=\
"-g -O3 -ffast-math -ftree-vectorize -march=native -fomit-frame-pointer -pipe"\
--with-blas="-L/usr/lib64/atlas -llapack -lf77blas"
```
For mpi, just add --enable-mpi.


For the python analysis utilities, you'll want to install ipython and matplotlib.
```sh

sudo yum install python-matplotlib
sudo yum install ipython
sudo yum install scipy
```
The python package [pytables](http://www.pytables.org/) for reading HDF5 files is also required for the analysis scripts, but it is not available through yum, so you'll have to download it and install it yourself.

## HPC Centers ##

### ASU Fulton: saguaro ###
For a serial build in two dimensions,
```

../../pi/configure --with-ndim=2 --enable-sprng  CXX=icpc CC=icc CXXFLAGS="-O3 -xP -ipo" \
--with-blas="-L$MKL_LIB -lmkl_lapack -lmkl_intel_lp64 -lmkl_sequential -lmkl_core" \
F77=ifort AR="xild -lib"
```
or for a parallel version,
```

../../pi/configure --with-ndim=2 --enable-sprng --enable-mpi MPICC=mpicc MPICXX=mpicxx \
CXX=icpc CC=icc F77=ifort CXXFLAGS="-O3 -xP -ipo" AR="xild -lib" \
--with-blas="-L$MKL_LIB -lmkl_lapack -lmkl_intel_lp64 -lmkl_sequential -lmkl_core"
```
Omit the <tt>--enable-sprng</tt> option if you do not have the SPRNG library.

### LONI-LSU: queenbee ###
You need to add some lines to your <tt>.soft</tt> file to include some required libraries,
```sh

#My additions (CPATH mimics -I include directories).
CPATH += /usr/local/packages/hdf5-1.8.1-intel10.1/include
+gsl-1.9-intel10.1
+sprng4-mvapich-1.1-intel-10.1
+fftw-3.1.2-intel10.1
CPATH += :/usr/local/packages/fftw-3.1.2-intel10.1/include
+intel-mkl
CPPFLAGS += -DMPICH_IGNORE_CXX_SEEK
```
For an MPI build, use,
```

../../pi/configure --with-ndim=3 --enable-mpi MPICC=mpicc MPICXX=mpicxx \
CXX=icpc CC=icc F77=ifort AR="xild -lib" CXXFLAGS="-O3 -xP -ipo" \
--with-blas="-lmkl_lapack -lmkl_intel_lp64 -lmkl_sequential -lmkl_core"
```
### NCSA: abe ###

You need to add some lines to your <tt>.soft</tt> file to include some required libraries,
```sh

#My additions (CPATH mimics -I include directories).
+libxml2-2.6.29
+libxml2
+intel-mkl
+gsl-intel
+hdf5-1.8.2
CPATH += :/usr/apps/hdf/hdf5/v182/include
LD_LIBRARY_PATH += /usr/apps/hdf/szip/lib
+fftw-3.1-intel
LD_LIBRARY_PATH += /usr/apps/math/fftw/fftw-3.1.2/intel10/lib
CPATH += :/usr/apps/math/fftw/fftw-3.1.2/intel10/include
+intel-mkl
CPPFLAGS = "${CPPFLAGS} -DMPICH_IGNORE_CXX_SEEK"
Also have blitz installed locally with --prefix=(your dir choice)

```
For an MPI build, use,
```sh

../../pi/configure --with-ndim=3 --enable-mpi MPICC=mpicc MPICXX=mpicxx CXX=icpc CC=icc \
CXXFLAGS="-O3 -xP -ipo" LDFLAGS="-lsz" \
--with-blas="-lmkl_lapack -lmkl_intel_lp64 -lmkl_sequential -lmkl_core" F77=ifort AR="xild -lib"
```
### TACC: Ranger ###

### Cornell CNF: nanolab ###
The svn client wasn't working for me, so I built one in my ~/packages/bin directory. You need to specify the most recent C++ and Fortran compilers by including the following in your .bash\_profile,
```sh

# Version 10 compilers
source /opt/intel/cc/10.1.017/bin/iccvars.sh
source /opt/intel/fc/10.1.017/bin/ifortvars.shsource /opt/intel/idb/10.1.017/bin/idbvars.sh
source /opt/intel/mkl/10.0.4.023/tools/environment/mklvars32.sh
```
Also, make sure that <tt>/usr/lam-7.4.1_intelv10/bin</tt> is in your path to get the correct MPI compilers.

You need to build blitz (again, in my ~/packages directory). For a serial **_pi_** build,
```sh

../../pi/configure --with-ndim=3 CXX=icpc CC=icc CXXFLAGS="-O3 -ipo" \
--with-blas="-Wl,-rpath,$MKLROOT/lib/32 -L/opt$MKLROOT/lib/32 -lmkl_intel \
-lmkl_sequential -lmkl_core -lpthread -lm" F77=ifort AR="xild -lib"
```

```sh

../../pi/configure --with-ndim=3 CXX=icpc CC=icc CXXFLAGS="-O3 -ipo" \
--with-blas="-Wl,-rpath,$MKLROOT/lib/32 -L$MKLROOT/lib/32 -lmkl_intel \
-lmkl_sequential -lmkl_core -lpthread -lm" F77=ifort AR="xild -lib" \
--enable-mpi MPICXX=mpic++ MPICC=mpicc MPIF77=mpif77
```