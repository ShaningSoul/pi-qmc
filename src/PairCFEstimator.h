// $Id: PairCFEstimator.h,v 1.12 2008/08/05 21:54:30 jshumwa Exp $
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
#ifndef __PairCFEstimator_h_
#define __PairCFEstimator_h_
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef ENABLE_MPI
#include <mpi.h>
#endif
#include "stats/BlitzArrayBlkdEst.h"
#include "stats/MPIManager.h"
#include "LinkSummable.h"
#include "Paths.h"
#include <blitz/array.h>
#include "SimulationInfo.h"
#include "Species.h"
#include "SuperCell.h"
#include "Action.h"
#include "Paths.h"
#include <blitz/tinyvec-et.h>
#include <vector>
/** Pair correlation class.
 *  Handles general pair correlation functions for many different geometries.
 *  @version $Revision: 1.12 $
 *  @author John Shumway  */
template <int N>
class PairCFEstimator : public BlitzArrayBlkdEst<N>, public LinkSummable {
public:
  typedef blitz::Array<float,N> ArrayN;
  typedef blitz::TinyVector<double,NDIM> Vec;
  typedef blitz::TinyVector<double,N> VecN;
  typedef blitz::TinyVector<int,N> IVecN;
  /// Base class for distance functions.
  class Dist {public: 
    virtual double operator()(const Vec &r1, const Vec &r2, 
                              const SuperCell &cell)=0;
  };
  /// Distance taken from radial separation.
  class Radial : public Dist { public:
    Radial(int idir=-1) : mask(1.0) {if (idir!=-1) mask(idir)=0;}
    virtual double operator()(const Vec &r1, const Vec &r2, 
                              const SuperCell &cell) {
      Vec delta=r1-r2; cell.pbc(delta);
      double radius2=0;
      for (int i=0; i<NDIM; ++i) radius2 += delta(i)*delta(i)*mask(i);
      return sqrt(radius2);
    }
    Vec mask;
  };
  /// Distance taken from cartesian position of particle 1.
  class Cart1 : public Dist { public:
    Cart1(int idim) : idim(idim){};
    int idim;
    virtual double operator()(const Vec &r1, const Vec &r2, 
                              const SuperCell &cell) {return r1[idim];};
  };
  /// Distance taken from cartesian position of particle 2.
  class Cart2 : public Dist { public:
    Cart2(int idim) : idim(idim){};
    int idim;
    virtual double operator()(const Vec &r1, const Vec &r2, 
                              const SuperCell &cell) {return r2[idim];};
  };
  /// Distance taken from cartesian separation.
  class Cart : public Dist { public:
    Cart(int idim) : idim(idim){};
    int idim;
    virtual double operator()(const Vec &r1, const Vec &r2, 
                              const SuperCell &cell) {
      Vec delta=r1-r2; cell.pbc(delta);
      return delta[idim];
    }
  };
  typedef std::vector<Dist*> DistN;
  /// Constructor.
  PairCFEstimator(const SimulationInfo& simInfo, const std::string& name,
                  const Species &s1, const Species &s2, const VecN &min, 
                  const VecN &max, const IVecN &nbin, const DistN &dist,
                  MPIManager *mpi) 
    : BlitzArrayBlkdEst<N>(name,nbin,true), 
      min(min), deltaInv(nbin/(max-min)), nbin(nbin), dist(dist),
      cell(*simInfo.getSuperCell()), temp(nbin),
      ifirst(s1.ifirst), jfirst(s2.ifirst), nipart(s1.count), njpart(s2.count),
      mpi(mpi) {
    BlitzArrayBlkdEst<N>::norm=0;
  }
  /// Virtual destructor.
  virtual ~PairCFEstimator() {
    for (int i=0; i<N; ++i) delete dist[i];
  }
  /// Initialize the calculation.
  virtual void initCalc(const int nslice, const int firstSlice) {
    temp=0;
  }
  /// Add contribution from a link.
  virtual void handleLink(const Vec& start, const Vec& end,
         const int ipart, const int islice, const Paths &paths) {
    if (ipart>=ifirst && ipart<ifirst+nipart) {
//      for (int jpart=jfirst; jpart<(jfirst+njpart) 
//           && (jfirst!=ifirst ||jpart<ipart); ++jpart) {
      for (int jpart=jfirst; jpart<(jfirst+njpart); ++jpart) {
        if (ipart!=jpart) {
          Vec r1=start; Vec r2=paths(jpart,islice);
          IVecN ibin=0;
          for (int i=0; i<N; ++i) {
            double d=(*dist[i])(r1,r2,cell);
            ibin[i]=int((d-min[i])*deltaInv[i]);
            if (d<min[i] || ibin[i]>nbin[i]-1) break;
            if (i==N-1) ++temp(ibin);
          }
        }
      }
    }
  }
  /// Finalize the calculation.
  virtual void endCalc(const int nslice) {
    temp/=nslice;
    BlitzArrayBlkdEst<N>::norm+=1;
    // First move all data to 1st worker. 
    int workerID=(mpi)?mpi->getWorkerID():0;
#ifdef ENABLE_MPI
//    if (mpi) {
//      if (workerID==0) {
//        mpi->getWorkerComm().Reduce(&temp(0,0,0),&temp(0,0,0),
//                                    product(nbin),MPI::DOUBLE,MPI::SUM,0);
//      } else {
//        mpi->getWorkerComm().Reduce(MPI::IN_PLACE,&temp(0,0,0),
//                                    product(nbin),MPI::DOUBLE,MPI::SUM,0);
//      }
//    }
#endif
    ///Need code for multiple workers!
    if (workerID==0) {
      BlitzArrayBlkdEst<N>::value+=temp;
    }
  }
  /// Clear value of estimator.
  virtual void reset() {}
  /// Evaluate for Paths configuration.
  virtual void evaluate(const Paths& paths) {paths.sumOverLinks(*this);}
private:
  VecN min;
  VecN deltaInv;
  IVecN nbin;
  DistN dist;
  SuperCell cell;
  ArrayN temp;
  int ifirst, jfirst, nipart, njpart;
  MPIManager *mpi;
};
#endif