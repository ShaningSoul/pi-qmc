// $Id: DensityCurrentEstimator.h 338 2010-11-30 18:56:16Z john.shumwayjr $
/*  Copyright (C) 2004-2006 John B. Shumway, Jr.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#ifndef __DensityCurrentEstimator_h_
#define __DensityCurrentEstimator_h_
#include "stats/BlitzArrayBlkdEst.h"
#include "LinkSummable.h"
#include "Paths.h"
#include <fftw3.h>
#include <cstdlib>
#include <blitz/array.h>
#include <blitz/tinyvec-et.h>
#include <vector>
class Distance;
class Paths;
class SimulationInfo;
class MPIManager;
/// Density-current estimator estimator for an inhomogeneous system.
///
/// @f[
/// \chi_{nj}(x,\tau) = -\langle n(x,\tau) j_x(0,0) \rangle_beta
/// @f]
class DensityCurrentEstimator : public BlitzArrayBlkdEst<NDIM+1>, public LinkSummable {
public:
  typedef blitz::Array<std::complex<double>,NDIM+1> CArrayN;
  typedef blitz::Array<std::complex<double>,2> CArray2;
  typedef blitz::Array<std::complex<double>,1> CArray1;
  typedef blitz::Array<double,1> Array;
  typedef blitz::Array<float,2> FArray2;
  typedef blitz::TinyVector<double,NDIM> Vec;
  typedef blitz::TinyVector<int,NDIM> IVec;
  typedef blitz::TinyVector<int,NDIM+1> IVecN;
  typedef std::vector<Distance*> DistArray;

  /// Constructor.
  DensityCurrentEstimator(const SimulationInfo& simInfo,
    const std::string& name, const Vec &min, 
    const Vec &max, const IVec &nbin, const IVecN &nbinN, const DistArray &dist,
    const int nstride, MPIManager *mpi);
  /// Virtual destructor.
  virtual ~DensityCurrentEstimator();
  /// Initialize the calculation.
  virtual void initCalc(const int nslice, const int firstSlice);
  /// Add contribution from a link.
  virtual void handleLink(const blitz::TinyVector<double,NDIM>& start,
                          const blitz::TinyVector<double,NDIM>& end,
                          const int ipart, const int islice, const Paths&);
  /// Finalize the calculation.
  virtual void endCalc(const int nslice);
  /// Clear value of the estimator.
  virtual void reset() {}
  /// Evaluate for Paths configuration.
  virtual void evaluate(const Paths& paths) {paths.sumOverLinks(*this);}
private:
  ///
  const Vec min;
  const Vec deltaInv;
  const IVec nbin;
  int npart, nstride, nfreq, nsliceEff;
//  int nslice;
  const double tau, ntot, ax;
  const DistArray dist;
  CArrayN tempn;
  CArray2 tempj;
  CArray2* tempn_;
  FArray2* value_;
  Array q;
  fftw_plan fwdn, fwdj;
  MPIManager *mpi;
};

#endif
