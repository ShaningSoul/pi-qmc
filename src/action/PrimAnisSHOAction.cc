// $Id$
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
#include "PrimAnisSHOAction.h"
#include <cstdlib>
#include <blitz/tinyvec.h>
#include <blitz/tinyvec-et.h>
#include "sampler/SectionSamplerInterface.h"
#include "Beads.h"
#include "util/SuperCell.h"
#include "Paths.h"
#include "SimulationInfo.h"
#include "Species.h"

/*
This is a SHO potential, but it allows different values of omega
for each direction. It also contains the species tag.
a=0.5*m*omegax*omegax
b=0.5*m*omegay*omegay
c=0.5*m*omegaz*omegaz
where omega[x/y/z] are defined in the XML input

Changelog
-Removed loop over NDIMS
-defined x,y,z and a,b,c for anisotropic SHO
-virial forces added / works
-added species option from SHOAction

Peter G McDonald 2009, Heriot Watt University, Scotland. pgm4@hw.ac.uk
*/

PrimAnisSHOAction::PrimAnisSHOAction(const double a, const double b, const double c, 
  const SimulationInfo &simInfo, int ndim, const Species &species)
  : tau(simInfo.getTau()), a(a), b(b), c(c), ndim(ndim),
  ifirst(species.ifirst), npart(species.count){
}

double PrimAnisSHOAction::getActionDifference(const SectionSamplerInterface& sampler,
                                         const int level) {
  const Beads<NDIM>& sectionBeads=sampler.getSectionBeads();
  const Beads<NDIM>& movingBeads=sampler.getMovingBeads();
  const SuperCell& cell=sampler.getSuperCell();
  const int nStride=(int)pow(2,level);
  const int nSlice=sectionBeads.getNSlice();
  const IArray& index=sampler.getMovingIndex(); 
  const int nMoving=index.size();
  double deltaAction=0;
  double ktstride = tau*nStride;
  double x=0;
  double y=0;      
  double z=0;

  for (int islice=nStride; islice<nSlice-nStride; islice+=nStride) {
    for (int iMoving=0; iMoving<nMoving; ++iMoving) {
      const int i=index(iMoving);
		if (i<ifirst || i>=ifirst+npart) continue;
      // Add action for moving beads.
      Vec delta=movingBeads(iMoving,islice);
      cell.pbc(delta);
	
	 x=delta[0];
       y=delta[1];
		z=delta[2];
		deltaAction+=(a*x*x + b*y*y +c*z*z)*ktstride;
      // Subtract action for old beads.
      delta=sectionBeads(i,islice);
      cell.pbc(delta);
		x=0;
		y=0;
		z=0;
  x=delta[0];
  y=delta[1];
  z=delta[2];
		deltaAction-=(a*x*x + b*y*y +c*z*z)*ktstride;
    }
  }
  return deltaAction;
}

double PrimAnisSHOAction::getTotalAction(const Paths& paths, 
    const int level) const {
  return 0;
}

void PrimAnisSHOAction::getBeadAction(const Paths& paths, int ipart, int islice,
     double& u, double& utau, double& ulambda, Vec &fm, Vec &fp) const {
  Vec delta=paths(ipart,islice);
fm=0; fp=0; ulambda=0;
	double x=0;
	double y=0;
	double z=0;
	 if (ipart<ifirst || ipart>=ifirst+npart) return;
	x=delta[0];
	y=delta[1];
	z=delta[2];
  utau=a*x*x + b*y*y + c*z*z;
u=utau*tau;
fm=-(a*x + b*y + c*z)*tau;
fp=-(a*x + b*y + c*z)*tau;
}
