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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef ENABLE_MPI
#include <mpi.h>
#endif
#include "DiffusionEstimator.h"
#include "SimulationInfo.h"
#include "Paths.h"
#include "DiffusionEstimator.h"
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>
#include "stats/MPIManager.h"
#include <fstream>
#include "Species.h"
//#include "RandomNumGenerator.h"
#include "SuperCell.h"
//sak
#include <iostream>
DiffusionEstimator::DiffusionEstimator(const SimulationInfo& simInfo, const std::string& name,
		       const Species &s1,const bool classicalPoint,  MPIManager *mpi)
  : BlitzArrayBlkdEst<1>(name, IVecN(simInfo.getNSlice()/2), true), 
    npart(simInfo.getNPart()),  ifirst(s1.ifirst), nipart(s1.count),
    mpi(mpi), lambda(0.5/s1.mass), tau(simInfo.getTau()),cell(simInfo.getSuperCell()),orig(nipart),classicalPoint(classicalPoint){
  value = 0.;
  if (classicalPoint) {
  norm = 1;
  } else {
    norm =0;
  }
  flag=true;
  mcStep=1;// used ifclassicalpoint needed. Ultimately the size of value should be set in input file.
}

DiffusionEstimator::~DiffusionEstimator() {
}

void DiffusionEstimator::evaluate(const Paths &paths) {

  if (classicalPoint && flag) {
    int iFirstSlice = paths.getLowestSampleSlice(0,false);
    for (int ipart=0; ipart < nipart; ipart++){
      orig(ipart) = paths(ipart+ifirst,1+iFirstSlice); 
    }
    flag = false;
  }
  
  if (classicalPoint){
    int iFirstSlice = paths.getLowestSampleSlice(0,false);
      if (mcStep == value.size()){
	mcStep=1;
	norm += 1.;
      }
    for (int ipart=ifirst; ipart < ifirst+nipart; ipart++){
      Vec delta=  orig(ipart-ifirst)- paths(ipart, 1+iFirstSlice);
      cell->pbc(delta);
      double tmp =0;
      for (int idim=0;idim<NDIM;++idim) {
	tmp+=delta[idim]*delta[idim];
      }
      tmp *=1.0/(2*NDIM*lambda*tau*mcStep*nipart);      
      value(mcStep-1) += tmp;
    }
    mcStep++;
  }

  if (!classicalPoint) {
    int iFirstSlice = paths.getLowestSampleSlice(0,false);
    for (int ipart=0; ipart < nipart; ipart++){
      orig(ipart) = paths(ipart+ifirst,1+iFirstSlice); 
    }
 
#ifdef ENABLE_MPI
    if (mpi && (mpi->getNWorker())>1) {
      mpi->getWorkerComm().Bcast(&orig(0),nipart*NDIM,MPI::DOUBLE,0);
    }
#endif
  }
  paths.sumOverLinks(*this);
}


void DiffusionEstimator::handleLink(const Vec& start, const Vec& end,
          const int ipart, const int islice, const Paths& paths) {
  if (ipart>=ifirst && ipart<ifirst+nipart && islice!=0 && islice <=value.size()/2) { 
     if (!classicalPoint) {
      Vec delta=  orig(ipart-ifirst)- paths(ipart,islice);
      cell->pbc(delta);
      double tmp =0;
      for (int idim=0;idim<NDIM;++idim) {
	tmp+=delta[idim]*delta[idim];
      }
  
      tmp *=1.0/(2*NDIM*lambda*tau*islice*nipart);
      value(islice-1) += tmp;
      norm += 1.;
    }
  }
}


void DiffusionEstimator::averageOverClones(const MPIManager* mpi) {
#ifdef ENABLE_MPI
  //reduce values among nworker
  if (mpi && (mpi->getNWorker())>1) {   
    ArrayN vbuff(n);
    mpi->getWorkerComm().Reduce(value.data(),vbuff.data(),value.size(),
				MPI::FLOAT,MPI::SUM,0);
    value=vbuff;
  }
  //reduce values of clones
  if (mpi && mpi->isCloneMain()) {
    int rank = mpi->getCloneComm().Get_rank();
    int size = mpi->getCloneComm().Get_size();
    if (size>1) {
      reset();
      if (rank==0) {
#if MPI_VERSION==2
        mpi->getCloneComm().Reduce(MPI::IN_PLACE,&norm,1,MPI::DOUBLE,
                                   MPI::SUM,0);
        mpi->getCloneComm().Reduce(MPI::IN_PLACE,value.data(),value.size(),
                                   MPI::FLOAT,MPI::SUM,0);
#else
        double nbuff;
        ArrayN vbuff(n);
        mpi->getCloneComm().Reduce(&norm,&nbuff,1,MPI::DOUBLE,
                                   MPI::SUM,0);
        mpi->getCloneComm().Reduce(value.data(),vbuff.data(),value.size(),
                                   MPI::FLOAT,MPI::SUM,0);
        norm=nbuff;
        value=vbuff;
#endif
      } else {
        mpi->getCloneComm().Reduce(&norm,NULL,1,MPI::DOUBLE,MPI::SUM,0);
        mpi->getCloneComm().Reduce(value.data(),NULL,value.size(),
                                   MPI::FLOAT,MPI::SUM,0);
      }
    }
  }
#endif
  // Next add value to accumvalue and accumvalue2.
  accumvalue += value/norm;
  if (hasErrorFlag) accumvalue2 += (value*value)/(norm*norm);
  accumnorm+=1.; 
  if (classicalPoint) {
    value=0.; norm=1;
  } else {
    value=0.; norm=0;
  }

  ++iblock; 
}

