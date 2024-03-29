// $Id$
/*  Copyright (C) 2010 John B. Shumway, Jr.

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
#include "ModelSampler.h"
#include "stats/AccRejEstimator.h"
#include "stats/MPIManager.h"
#include "action/Action.h"
#include "action/ActionChoice.h"
#include "util/RandomNumGenerator.h"
#include "Paths.h"
#include "EnumeratedModelState.h"
#include <sstream>
#include <string>

ModelSampler::ModelSampler(Paths& paths, Action* action, 
  ActionChoiceBase* actionChoice, int target, const MPIManager* mpi)
  : paths(paths), action(action), actionChoice(actionChoice),
    modelState(dynamic_cast<EnumeratedModelState&>
                 (actionChoice->getModelState())),
    nmodel(modelState.getModelCount()), accRejEst(0),  target(target),
    mpi(mpi) {
}

ModelSampler::~ModelSampler() {
}

void ModelSampler::run() {
  tryMove();

}


bool ModelSampler::tryMove() {
  int workerID = (mpi) ? mpi->getWorkerID() : 0;

  if (workerID==0) accRejEst->tryingMove(0);

  int imodel = modelState.getModelState();

  modelState.setModelState(imodel);
  modelState.broadcastToMPIWorkers(mpi);

  // We select the next model so that the end points always
  // try a valid model. This is efficient for two models, but
  // efficiency is around 60% for several models.
  // Probably want to revisit this later.
  int jmodel = imodel+2*((int)floor(RandomNumGenerator::getRand()
      +(nmodel-1.-imodel)/(nmodel-1.)))-1;
#ifdef ENABLE_MPI
  int nworker = (mpi) ? mpi->getNWorker():1;
  if (mpi && nworker>1) {
    mpi->getWorkerComm().Bcast(&jmodel,1,MPI::INT,0);
  }
#endif 

  if (jmodel<0 or jmodel>=nmodel) return false;

  // If we are targeting a model, reject moves that take us away from there.
  if (target >= 0 && std::abs(jmodel-target) > std::abs(imodel-target)) {
    return false;
  }

  double tranProb = (jmodel>imodel)
                    ? (nmodel-1.-imodel)/jmodel :imodel/(nmodel-1.-jmodel);

  // Evaluate the change in action.
  double deltaAction = actionChoice->getActionChoiceDifference(paths,jmodel);
#ifdef ENABLE_MPI
  double totalDeltaAction = 0;
  mpi->getWorkerComm().Reduce(&deltaAction,&totalDeltaAction,
                              1,MPI::DOUBLE,MPI::SUM,0);
  deltaAction = totalDeltaAction;
#endif 

  double acceptProb=exp(-deltaAction)/tranProb;

  bool reject = RandomNumGenerator::getRand()>acceptProb;
#ifdef ENABLE_MPI
    if (nworker > 1) {
      mpi->getWorkerComm().Bcast(&reject, sizeof(bool), MPI::CHAR, 0); 
    }
#endif 
  if (reject) return false;

  modelState.setModelState(jmodel);
  modelState.broadcastToMPIWorkers(mpi);

  if (workerID==0) accRejEst->moveAccepted(0);
  return true;
}

AccRejEstimator* 
ModelSampler::getAccRejEstimator(const std::string& name) {
  return accRejEst=new AccRejEstimator(name.c_str(),1);
}
