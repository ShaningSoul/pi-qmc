// $Id: GridPotential.cc,v 1.14 2008/12/09 04:34:59 jshumwa Exp $
/*  Copyright (C) 2004-2008 John B. Shumway, Jr.

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
#include "GridPotential.h"
#include <blitz/tinyvec-et.h>
#include "MultiLevelSampler.h"
#include "SimulationInfo.h"
#include "Beads.h"
#include "Paths.h"
#include "SuperCell.h"
#include <hdf5.h>
#include <fstream>

GridPotential::GridPotential(const SimulationInfo& simInfo,
                             const std::string& filename)
  : tau(simInfo.getTau()), 
    vindex(simInfo.getNPart(),(Array3*)0) {
  int nn;
  // Read the bandoffsets from grid.h5.
  hid_t fileID = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=8))
  hid_t groupID = H5Gopen2(fileID, (filename=="emagrids.h5"?"boffset":"/"),
                           H5P_DEFAULT);
#else
  hid_t groupID = H5Gopen(fileID, (filename=="emagrids.h5"?"boffset":"/"));
#endif
#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=8))
  hid_t dataSetID = H5Dopen2(groupID, "vh", H5P_DEFAULT);
#else
  hid_t dataSetID = H5Dopen(groupID, "vh");
#endif
  hid_t dataSpaceID = H5Dget_space(dataSetID);
  hsize_t dims[3];
  H5Sget_simple_extent_dims(dataSpaceID, dims, NULL);
  H5Sclose(dataSpaceID);
  n1=dims[0]; n2=dims[1]; n3=dims[2]; nn=n1*n2*n3;
  vhgrid.resize(n1,n2,n3);
  H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
          vhgrid.data()); 
  vegrid.resize(n1,n2,n3);
  H5Dclose(dataSetID);
#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=8))
  dataSetID = H5Dopen(groupID, "ve", H5P_DEFAULT);
#else
  dataSetID = H5Dopen(groupID, "ve");
#endif
  H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
          vegrid.data()); 
  H5Dclose(dataSetID);
  // Read the grid spacing.
  double a=0;
#if (H5_VERS_MAJOR>1)||((H5_VERS_MAJOR==1)&&(H5_VERS_MINOR>=8))
  dataSetID = H5Dopen2(groupID, "a", H5P_DEFAULT);
#else
  dataSetID = H5Dopen(groupID, "a");
#endif
  H5Dread(dataSetID, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&a);
  H5Dclose(dataSetID);
  b=1.0/a;
  H5Gclose(groupID);
  H5Fclose(fileID);
  // Convert grids from eV to Ha.
  const double eVtoHa=1./27.211396;
  if (filename=="boffset.h5") {
    vhgrid *= eVtoHa;
    vegrid *= eVtoHa;
  } 
  // Flip sign of hole grid if read from emagrid.h5
  if (filename=="emagrids.h5") vhgrid *= -1; 
  // Setup the vindex.
  for (int i=0; i<simInfo.getNPart(); ++i) {
    std::string name=simInfo.getPartSpecies(i).name;
    if (name.substr(0,1)=="h") vindex[i]=&vhgrid;
    else if (name.substr(0,1)=="e") vindex[i]=&vegrid;
  }
}

double GridPotential::getActionDifference(const MultiLevelSampler& sampler,
                                         const int level) {
  const Beads<NDIM>& sectionBeads=sampler.getSectionBeads();
  const Beads<NDIM>& movingBeads=sampler.getMovingBeads();
  const SuperCell& cell=sampler.getSuperCell();
  const int nStride=(int)pow(2,level);
  const int nSlice=sectionBeads.getNSlice();
  const IArray& index=sampler.getMovingIndex(); 
  const int nMoving=index.size();
  double deltaAction=0;
  // Read values off of the grid.
  //for (int islice=nStride; islice<nSlice-nStride; islice+=nStride) {
  for (int islice=0; islice<nSlice-nStride; islice+=nStride) {
    for (int iMoving=0; iMoving<nMoving; ++iMoving) {
      const int i=index(iMoving);
      // Add action for moving beads. (Evaluate v at midpoint)
      Vec r=movingBeads(iMoving,islice);
      cell.pbc(r);
      Vec delta=movingBeads(iMoving,islice+nStride);
      delta-=r; cell.pbc(delta);delta*=0.5; r+=delta;
      cell.pbc(r);
      deltaAction+=v(r,i)*tau*nStride;
      // Subtract action for old beads.
      r=sectionBeads(i,islice);
      cell.pbc(r);
      delta=sectionBeads(i,islice+nStride);
      delta-=r; cell.pbc(delta);delta*=0.5; r+=delta;
      cell.pbc(r);
      deltaAction-=v(r,i)*tau*nStride;
    }
  }
  return deltaAction;
}

double GridPotential::v(Vec r, const int i) const {
  r*=b;
  int i1=(int)floor(r[0]), i2=(int)floor(r[1]), i3=(int)floor(r[2]);
  double x=r[0]-i1, y=r[1]-i2, z=r[2]-i3;
  i1+=n1/2; i2+=n2/2; i3+=n3/2;
  if (i1<0) {i1=0; x=0;}; if (i2<0) {i2=0; y=0;}; if (i3<0) {i3=0; z=0;};
  if (i1>n1-2) {i1=n1-2; x=1;};
  if (i2>n2-2) {i2=n2-2; y=1;};
  if (i3>n3-2) {i3=n3-2; z=1;};
  const Array3& v(*vindex[i]);
  double V =(1-z)*( (1-y)*( (1-x)*v(i1,i2,  i3  ) + x*v(i1+1,i2,  i3  ) ) 
                     + y *( (1-x)*v(i1,i2+1,i3  ) + x*v(i1+1,i2+1,i3  ) ))
             + z *( (1-y)*( (1-x)*v(i1,i2,  i3+1) + x*v(i1+1,i2,  i3+1) ) 
                     + y *( (1-x)*v(i1,i2+1,i3+1) + x*v(i1+1,i2+1,i3+1)));
  return V;
}

double GridPotential::getTotalAction(const Paths& paths, int level) const {
  return 0;
}


void GridPotential::getBeadAction(const Paths& paths, int ipart, int islice,
    double& u, double& utau, double& ulambda, Vec& fm, Vec& fp) const {
  const Array3& v(*vindex[ipart]);
  Vec r=paths(ipart,islice);
  r*=b;
  int i1=(int)floor(r[0]), i2=(int)floor(r[1]), i3=(int)floor(r[2]);
  double x=r[0]-i1, y=r[1]-i2, z=r[2]-i3;
  i1+=n1/2; i2+=n2/2; i3+=n3/2; 
  if (i1<0) {i1=0; x=0;} if (i2<0) {i2=0; y=0;} if (i3<0) {i3=0; z=0;}
  if (i1>n1-2) {i1=n1-2; x=1;}
  if (i2>n2-2) {i2=n2-2; y=1;}
  if (i3>n3-2) {i3=n3-2; z=1;}
  utau =(1-z)*( (1-y)*( (1-x)*v(i1,i2,i3)+x*v(i1+1,i2,i3) ) 
                 + y *( (1-x)*v(i1,i2+1,i3)+x*v(i1+1,i2+1,i3) ) )
         + z *( (1-y)*( (1-x)*v(i1,i2,i3+1)+x*v(i1+1,i2,i3+1) ) 
                 + y *( (1-x)*v(i1,i2+1,i3+1)+x*v(i1+1,i2+1,i3+1)));
  u = utau*tau;
  ulambda=0;
  fm[0] =(1-z)*( (1-y)*( (1-x)*v(i1,i2,i3)+x*v(i1+1,i2,i3) )
                  + y *( (1-x)*v(i1,i2+1,i3)+x*v(i1+1,i2+1,i3) ) )
          + z *( (1-y)*( (1-x)*v(i1,i2,i3+1)+x*v(i1+1,i2,i3+1) ) 
                  + y *( (1-x)*v(i1,i2+1,i3+1)+x*v(i1+1,i2+1,i3+1)));
  fm[1] =(1-z)*( (1-y)*( (1-x)*v(i1,i2,i3)+x*v(i1+1,i2,i3) ) 
                  + y *( (1-x)*v(i1,i2+1,i3)+x*v(i1+1,i2+1,i3) ) )
          + z *( (1-y)*( (1-x)*v(i1,i2,i3+1)+x*v(i1+1,i2,i3+1) ) 
                  + y *( (1-x)*v(i1,i2+1,i3+1)+x*v(i1+1,i2+1,i3+1)));
  fm[2] =(1-z)*( (1-y)*( (1-x)*v(i1,i2,i3)+x*v(i1+1,i2,i3) ) 
                  + y *( (1-x)*v(i1,i2+1,i3)+x*v(i1+1,i2+1,i3) ) )
          + z *( (1-y)*( (1-x)*v(i1,i2,i3+1)+x*v(i1+1,i2,i3+1) ) 
                  + y *( (1-x)*v(i1,i2+1,i3+1)+x*v(i1+1,i2+1,i3+1)));
  fm*=0.5;
  fp=fm;
}