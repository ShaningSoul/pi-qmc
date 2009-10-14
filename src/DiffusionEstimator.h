/*  Copyright (C) 2008 John B. Shumway, Jr.

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
#ifndef __DiffusionEstimator_h_
#define __DiffusionEstimator_h_
#include "stats/BlitzArrayBlkdEst.h"
#include "LinkSummable.h"
#include "Paths.h"
#include <blitz/array.h>
#include "Species.h"
class Paths;
class SimulationInfo;
class MPIManager;
class SuperCell;

class DiffusionEstimator : public BlitzArrayBlkdEst<1>, public LinkSummable {
public:
  typedef blitz::Array<int,1> IArray;
  typedef blitz::Array<double,1> Array;
  typedef blitz::Array<Vec,1> VArray;
  /// Constructor.
  DiffusionEstimator(const SimulationInfo& simInfo,  const std::string& name,
		     const Species &s1,const bool classicalPoint, MPIManager *mpi);
  /// Virtual destructor.
  virtual ~DiffusionEstimator();
  /// Clear value of the estimator.
  virtual void reset() {}
  /// Evaluate for Paths configuration.
   virtual void evaluate(const Paths& paths);
  /// Add contribution from a link.
  virtual void handleLink(const blitz::TinyVector<double,NDIM>& start,
                          const blitz::TinyVector<double,NDIM>& end,
                          const int ipart, const int islice, const Paths&);
  virtual void averageOverClones(const MPIManager* mpi);

private:
  ///
  int ifirst, nipart;
  const int npart;
  MPIManager *mpi;
  bool flag;
  int mcStep;
  ////
  const SuperCell* cell;
  const double tau;
  const double lambda;
  VArray orig;
  const bool classicalPoint;
};

#endif
