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
#include <stdio.h>
#include <math.h>
#endif
#include "RingGateAction.h"
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>
#include "MultiLevelSampler.h"
#include "Beads.h"
#include "SuperCell.h"
#include "Paths.h"
#include "SimulationInfo.h"
#include "Species.h"

/* 
This is a potential for a 2d ring mimicking the gate potential in experiments.
The potential is Vg * (tanh(s(theta+theta0)) - tanh(s(theta-theta0))).
Vg is the potential of the top gate in energy unit.
s is a parameter.
theta0 is defined w.r.t x-axis and cannot be zero.
The potential can be expanded:

Vg * (2 * tanh(s * theta) - 2 * tanh(s * theta)^2 * tanh(s * theta0))/(1 - tanh(s * theta)^2 * tanh(s * theta0)^2)

*/

RingGateAction::RingGateAction(const SimulationInfo &simInfo, const double Vg, const double s, const double theta0, const Species &species)
  : tau(simInfo.getTau()), Vg(Vg), s(s), theta0(theta0), ifirst(species.ifirst), npart(species.count){
}

double RingGateAction::getActionDifference(const MultiLevelSampler& sampler, const int level) {
  const Beads<NDIM>& sectionBeads=sampler.getSectionBeads();
  const Beads<NDIM>& movingBeads=sampler.getMovingBeads();
  const SuperCell& cell=sampler.getSuperCell();
  const int nStride=(int)pow(2,level);
  const int nSlice=sectionBeads.getNSlice();
  const IArray& index=sampler.getMovingIndex(); 
  const int nMoving=index.size();
  double deltaAction=0;
  double ktstride = tau*nStride;
  double st0 = tanh(s * theta0);
  double st02 = pow(st0,2);
  double x = 0;
  double y = 0;
  double theta = 0;
  double st = 0;
  for (int islice=nStride; islice<nSlice-nStride; islice+=nStride) {
    for (int iMoving=0; iMoving<nMoving; ++iMoving) {
      const int i=index(iMoving);
      if (i<ifirst || i>=ifirst+npart) continue;
      // Add action for moving beads.
      Vec delta=movingBeads(iMoving,islice);
      cell.pbc(delta);
      x = delta[0];
      y = delta[1];
      if (x == 0. && y == 0.) deltaAction+=0;
      else {
        theta = atan2(y,x);
        st = tanh(s*theta);
        deltaAction+=Vg * (2 * st0 - 2 * st * st * st0)/(1 - st * st * st02) * ktstride;
      }
      // Subtract action for old beads.
      delta = sectionBeads(i,islice);
      cell.pbc(delta);
      x = delta[0];
      y = delta[1];
      if (x == 0. && y == 0.) deltaAction-=0;
      else {
        theta = atan2(y,x);
        st = tanh(s*theta);
        deltaAction-=Vg * (2 * st0 - 2 * st * st * st0)/(1 - st * st * st02) * ktstride;
      }
    }
  }
  return deltaAction;
}

double RingGateAction::getTotalAction(const Paths& paths, const int level) const {
  return 0;
}

void RingGateAction::getBeadAction(const Paths& paths, int ipart, int islice, double& u, double& utau, double& ulambda, Vec &fm, Vec &fp) const {
  Vec delta = paths(ipart,islice);
  fm = 0; fp = 0; ulambda = 0;
  if (ipart < ifirst || ipart >= ifirst + npart) return;
  double x = delta[0];
  double y = delta[1];
  double theta = 0;
  double st0 = tanh(s * theta0);
  double st02 = pow(st0, 2);
  double st = 0;
  if (x == 0. && y == 0.) utau=0;
  else {
    theta = atan2(y,x);
    st = tanh(s*theta);
    utau=Vg * (2 * st0 - 2 * st * st * st0)/(1 - st * st * st02);
  }
  u = utau * tau;
}