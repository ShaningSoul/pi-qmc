// $Id: MultiLevelSampler.cc 22 2009-5-18 Saad khairallah$
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
#include "DisplaceMoveSampler.h"
#include "stats/AccRejEstimator.h"
#include "Beads.h"
#include "BeadFactory.h"
#include "Action.h"
#include "Mover.h"
#include "RandomNumGenerator.h"
#include "Paths.h"
#include "Permutation.h"
#include "PermutationChooser.h"
#include "ParticleChooser.h"
#include "SimpleParticleChooser.h"
#include "RandomPermutationChooser.h"
#include <sstream>
#include <string>

DisplaceMoveSampler::DisplaceMoveSampler(int nmoving, Paths& paths, double dist, 
  ParticleChooser& particleChooser, Mover& mover, Action* action,
					 const int nrepeat, const BeadFactory& beadFactory,const MPIManager* mpi)
  : nmoving(nmoving), dist(dist), mover(mover), cell(paths.getSuperCell()), action(action),
    movingIndex(new IArray(nmoving)), identityIndex(nmoving), pathsPermutation(paths.getPermutation()),
    particleChooser(particleChooser), paths(paths), accRejEst(0), nrepeat(nrepeat), mpi(mpi) {
#ifdef ENABLE_MPI
  int iworker = (mpi)?mpi->getWorkerID():0;
  int nworker =(mpi)? mpi->getNWorker():1;
  int nsl=paths.getNSlice();
  nslice=nsl/nworker;
#else
  nslice=paths.getNSlice();
#endif
  pathsBeads=beadFactory.getNewBeads(paths.getNPart(), nslice);
  movingBeads=beadFactory.getNewBeads(paths.getNPart(),nslice);
for (int i=0; i<nmoving; ++i) (*movingIndex)(i)=identityIndex(i)=i;
}

DisplaceMoveSampler::~DisplaceMoveSampler() {
  delete movingBeads;
  delete movingIndex;
  delete &particleChooser;
  
  delete &pathsPermutation;
}

void DisplaceMoveSampler::run() {
  // Select particles that are not permuting to move
  iFirstSlice = paths.getLowestSampleSlice(0,false);
  paths.getBeads(iFirstSlice,*pathsBeads);
 
#ifdef ENABLE_MPI
      int nworker =  mpi->getNWorker();
      particleChooser.chooseParticles();
      if (mpi && nworker > 1) mpi->getWorkerComm().Bcast(&particleChooser, nmoving, MPI::INT, 0); 
#endif

  for (int irepeat=0; irepeat<nrepeat; ++irepeat) {
    //particleChooser.chooseParticles();
   int imovingNonPerm = 0;         
    for (int i=0; i<nmoving; ++i) {
      int j = particleChooser[i];
      int jperm = pathsPermutation[j];
     
      if (j==jperm){
	(*movingIndex)(imovingNonPerm)=j;
	imovingNonPerm++;
      }
    } 

    // Copy old coordinate to the moving coordinate
    for (int islice=0; islice<nslice; ++islice) { 
      pathsBeads->copySlice(*movingIndex,islice,*movingBeads,identityIndex,islice);
    }
    if (tryMove(imovingNonPerm)) continue;
  }
}

bool DisplaceMoveSampler::tryMove(int imovingNonPerm) {
  accRejEst->tryingMove(0);
  double l = mover.makeMove(*this);

  // Evaluate the change in action.
  double deltaAction=(action==0)?0:action->getActionDifference(*this, imovingNonPerm);

#ifdef ENABLE_MPI
  if (mpi && (mpi->getNWorker())>1) {
    double netDeltaAction=0;
    mpi->getWorkerComm().Reduce(&deltaAction,&netDeltaAction,1,MPI::DOUBLE,MPI::SUM,0);
    double acceptProb=exp(-deltaAction);
    bool acceptReject = RandomNumGenerator::getRand()>acceptProb;
    mpi->getWorkerComm().Bcast(&acceptReject,1,MPI::LOGICAL,0); 
    if (acceptReject) return false;
  }else
    {
      double acceptProb=exp(-deltaAction);
      if (RandomNumGenerator::getRand()>acceptProb) return false;
    }
#else
  double acceptProb=exp(-deltaAction);
  if (RandomNumGenerator::getRand()>acceptProb) return false;
#endif
  
  accRejEst->moveAccepted(0);
  
  // Move accepted.
  action->acceptLastMove();
  
  // Put moved beads in paths beads.
  for (int islice=0; islice<nslice; ++islice) {
    movingBeads->copySlice(identityIndex,islice,
			   *pathsBeads,*movingIndex,islice);
  }
  paths.putBeads(iFirstSlice,*pathsBeads,pathsPermutation);
  return true;
}

void DisplaceMoveSampler::setAction(Action* act, const int level) {action=act;}

AccRejEstimator* 
DisplaceMoveSampler::getAccRejEstimator(const std::string& name) {
  std::ostringstream longName;
  longName << name << ": moving " << nmoving
           << " " << particleChooser.getName();
  return accRejEst=new AccRejEstimator(longName.str(),1);
}
