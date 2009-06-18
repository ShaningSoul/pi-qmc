// $Id: UniformMover.cc 22 2009-03-06 13:52:07Z john.shumwayjr $
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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef ENABLE_MPI
#include <mpi.h>
#endif
#include "stats/MPIManager.h"
#include "UniformMover.h"
#include "Beads.h"
#include "DisplaceMoveSampler.h"
#include "RandomNumGenerator.h"
#include <blitz/tinyvec.h>
#include "SuperCell.h"
#include "SimulationInfo.h"
#include <cmath>

UniformMover::UniformMover(const MPIManager* mpi)
  : mpi(mpi) {
}

UniformMover::~UniformMover() {
 }

double UniformMover::makeMove(DisplaceMoveSampler& sampler) {
  typedef blitz::TinyVector<double,NDIM> Vec;
  const Beads<NDIM>& pathsBeads=sampler.getPathsBeads();
  Beads<NDIM>& movingBeads=sampler.getMovingBeads();
  const SuperCell& cell=sampler.getSuperCell();
  const double dist = sampler.getDist();
  const int nSlice=pathsBeads.getNSlice();
  const int ifirstSlice = 0; 
   IArray& index=sampler.getMovingIndex(); 
  const int nMoving=index.size();
  blitz::Array<double,1> uniRand(nMoving*NDIM); uniRand=0.0;
  
#ifdef ENABLE_MPI
  int workerID=(mpi)?mpi->getWorkerID():0;
  if (workerID ==0) RandomNumGenerator::makeRand(uniRand);  
  if (mpi && (mpi->getNWorker())>1) {
    mpi->getWorkerComm().Bcast(&uniRand(0),nMoving*NDIM,MPI::DOUBLE,0);
  }
#else
  RandomNumGenerator::makeRand(uniRand);  
#endif
  
  for (int iMoving=0; iMoving<nMoving; ++iMoving) {
#ifdef ENABLE_MPI
    std :: cout <<"The random numbers on cloneID"<<mpi->getCloneID()<< ", WorkerID "<<mpi->getWorkerID()<<" "<<    uniRand(NDIM*iMoving)<<std :: endl;
    std :: cout <<"The random numbers on cloneID"<<mpi->getCloneID()<< ", WorkerID "<<mpi->getWorkerID()<<" "<<    uniRand(NDIM*iMoving+1)<<std :: endl;
    std :: cout <<"The random numbers on cloneID"<<mpi->getCloneID()<< ", WorkerID "<<mpi->getWorkerID()<<" "<<    uniRand(NDIM*iMoving+2)<<std :: endl;
#endif
    const int i=index(iMoving);// not needed
    for (int islice=ifirstSlice; islice<nSlice; islice++) {
      
      // Calculate the new position.
      movingBeads(iMoving,islice)[0] += dist*(uniRand(NDIM*iMoving)-0.5);
      movingBeads(iMoving,islice)[1] += dist*(uniRand(NDIM*iMoving+1)-0.5);
      movingBeads(iMoving,islice)[2] += dist*(uniRand(NDIM*iMoving+2)-0.5);
      
      cell.pbc(movingBeads(iMoving,islice));
    }
  }
  
  return 0; 
}
