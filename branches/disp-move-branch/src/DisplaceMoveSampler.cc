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
//#include "PathsChooser.h"
#include <sstream>
#include <string>

DisplaceMoveSampler::DisplaceMoveSampler(int nmoving, Paths& paths, double dist, 
  ParticleChooser& particleChooser, Mover& mover, Action* action,
  const int nrepeat, const BeadFactory& beadFactory)
  : nslice(paths.getNSlice()), nmoving(nmoving), pathsBeads(beadFactory.getNewBeads(paths.getNPart(), paths.getNSlice())),
    dist(dist), pathsPermutation(paths.getPermutation()),
    movingBeads(beadFactory.getNewBeads(paths.getNPart(),paths.getNSlice())),
    mover(mover), cell(paths.getSuperCell()), action(action),
    movingIndex(new IArray(nmoving)),
    identityIndex(nmoving), pMovingIndex(nmoving),
    particleChooser(particleChooser), //permutationChooser(permutationChooser),
    paths(paths), accRejEst(0),
    nrepeat(nrepeat) {
  std :: cout << "******* "<<nslice<<std :: endl;
  for (int i=0; i<nmoving; ++i) (*movingIndex)(i)=identityIndex(i)=i;
}

DisplaceMoveSampler::~DisplaceMoveSampler() {
  delete movingBeads;
  delete movingIndex;
  delete &particleChooser;
  //delete &permutationChooser;
}

void DisplaceMoveSampler::run() {
  // Select particles that are not permuting to move
  
  //  std :: cout << "******* Printing  permutations"<<std :: endl;
  //std :: cout <<pathsPermutation<<std :: endl;
  for (int irepeat=0; irepeat<nrepeat; ++irepeat) {
    particleChooser.chooseParticles();
    
    int imovingNonPerm = 0;         
    for (int i=0; i<nmoving; ++i) {
      std :: cout <<"loop "<<i <<std :: endl;
      int j = particleChooser[i];
      int jperm = pathsPermutation[j];
      std :: cout <<"check perm "<<j <<" ?  "<<jperm<<std :: endl;
      if (j==jperm){
	(*movingIndex)(imovingNonPerm)=j;
	imovingNonPerm++;
      }
    }
     std :: cout <<"After loop "<<imovingNonPerm <<std :: endl;

     std :: cout <<"After loop "<<(*movingIndex).size() <<std :: endl;
    // Copy old coordinate to the moving coordinate
     for (int islice=0; islice<nslice; ++islice) {
       pathsBeads->copySlice(*movingIndex,islice,*movingBeads,identityIndex,islice);
     }
    if (tryMove(imovingNonPerm+1)) continue;
  }
}

bool DisplaceMoveSampler::tryMove(int imovingNonPerm) {
  accRejEst->tryingMove(imovingNonPerm);
 
  double l = mover.makeMove(*this);
  // Evaluate the change in action.
  double deltaAction=(action==0)?0:action->getActionDifference(*this, imovingNonPerm);
  double acceptProb=exp(-deltaAction);
  //std::cout << acceptProb << ", " << deltaAction << ",  " << imovingNonPerm << ", "
  //          << action  << ", " << std::endl;
  if (RandomNumGenerator::getRand()>acceptProb) return false;
  accRejEst->moveAccepted(imovingNonPerm);
  
  // Move accepted.
  action->acceptLastMove();
  // Put moved beads in paths beads.
  for (int islice=0; islice<nslice; ++islice) {
    movingBeads->copySlice(identityIndex,islice,
			   *pathsBeads,*movingIndex,islice);
  }
  paths.putBeads(0,*movingBeads,pathsPermutation);// seems repetitive of last step but may needed for parallism
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
