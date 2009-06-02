//$Id: AugmentedNodes.cc 52 2009-05-13 18:51:51Z john.shumwayjr $
/*  Copyright (C) 2009 John B. Shumway, Jr.

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
#include <blitz/tinyvec-et.h>
#include "AugmentedNodes.h"
#include "PeriodicGaussian.h"
#include "SimulationInfo.h"
#include "Species.h"
#include "SuperCell.h"
#include "Beads.h"
#include "DoubleMLSampler.h"
#include <cstdlib>

#define DGETRF_F77 F77_FUNC(dgetrf,DGETRF)
extern "C" void DGETRF_F77(const int*, const int*, double*, const int*,
                           const int*, int*);
#define DGETRI_F77 F77_FUNC(dgetri,DGETRI)
extern "C" void DGETRI_F77(const int*, double*, const int*, const int*,
                           double*, const int*, int*);
#define ASSNDX_F77 F77_FUNC(assndx,ASSNDX)
extern "C" void ASSNDX_F77(const int *mode, double *a, const int *n, 
  const int *m, const int *ida, int *k, double *sum, int *iw, const int *idw);

AugmentedNodes::AugmentedNodes(const SimulationInfo &simInfo,
  const Species &species, const Species &species2,
  const double temperature, const double radius, const double weight,
  const int maxlevel, const bool useUpdates, const int maxMovers)
  : NodeModel("_"+species.name),
    tau(simInfo.getTau()),mass(species.mass),mass2(species2.mass),
    npart(species.count), ifirst(species.ifirst), 
    npart2(species2.count), kfirst(species2.ifirst), 
    fpnorm(pow(4*PI*mass*temperature, -0.5*NDIM)),
    alpha(1.0/radius), 
    anorm(weight*pow(alpha,NDIM)/PI*(NDIM==2?2:1)),
    matrix((int)(pow(2,maxlevel)+0.1)+1),
    ipiv(npart),lwork(npart*npart),work(lwork),
    cell(*simInfo.getSuperCell()),
    pg(NDIM), pgp(NDIM), pgm(NDIM),
    notMySpecies(false),
    gradArray1(npart), gradArray2(npart), 
    temp1(simInfo.getNPart()), temp2(simInfo.getNPart()),
    uarray(npart,npart,ColMajor()), 
    kindex((int)(pow(2,maxlevel)+0.1)+1,npart), kwork(npart*6), nerror(0),
    kindex2(npart2),kmat(npart2,npart2,ColMajor()) {
  for (unsigned int i=0; i<matrix.size(); ++i)  {
    matrix[i] = new Matrix(npart,npart,ColMajor());
  }
  std::cout << "AugmentedNodes with temperature = "
            << temperature << std::endl;
  double tempp=temperature/(1.0+EPSILON); //Larger beta (plus).
  double tempm=temperature/(1.0-EPSILON); //Smaller beta (minus).
  if (temperature!=simInfo.getTemperature()) {
    tempp=tempm=temperature;
  }
  for (int idim=0; idim<NDIM; ++idim) {
    pg[idim]=new PeriodicGaussian(mass*temperature,cell.a[idim],
                            (int)(100*cell.a[idim]*sqrt(mass*temperature)));
    pgm[idim]=new PeriodicGaussian(mass*tempm,cell.a[idim],
                             (int)(100*cell.a[idim]*sqrt(mass*tempm)));
    pgp[idim]=new PeriodicGaussian(mass*tempp,cell.a[idim],
                             (int)(100*cell.a[idim]*sqrt(mass*tempp)));
  }
  if (useUpdates) {
    updateObj = new MatrixUpdate(maxMovers,maxlevel,npart,matrix,*this);
  }
}

AugmentedNodes::~AugmentedNodes() {
  for (int idim=0; idim<NDIM; ++idim) {
    delete pg[idim]; delete pgm[idim]; delete pgp[idim];
  }
  delete updateObj;
}

double AugmentedNodes::evaluate(const VArray &r1, const VArray &r2, 
                          const int islice) {
  Matrix& mat(*matrix[islice]);
  mat=0;
  // To avoid double sum over k, find first find closest kindex.
  kmat=0;
  for (int ipart=0; ipart<npart2; ++ipart) {
    for (int jpart=0; jpart<npart2; ++jpart) {
      Vec delta(r1(jpart+kfirst)-r2(ipart+kfirst));
      cell.pbc(delta);
      kmat(jpart,ipart)=dot(delta,delta);
    }
  }
  const int MODE=1;
  double sum;
  ASSNDX_F77(&MODE,kmat.data(),&npart2,&npart2,&npart2,kindex2.data(),
             &sum,kwork.data(),&npart2);
  kindex2 -= 1;
  double rcut2=16./(alpha*alpha);
  double shift=exp(-4.);
  // Now compute determinant.
  for(int jpart=0; jpart<npart; ++jpart) {
    for(int ipart=0; ipart<npart; ++ipart) {
      Vec delta(r1(jpart+ifirst)-r2(ipart+ifirst));
      cell.pbc(delta);
      double ear2=1;
      for (int i=0; i<NDIM; ++i) ear2*=(*pg[i])(fabs(delta[i]));
      mat(ipart,jpart)=fpnorm*ear2;
      for (int kpart=0; kpart<npart2; ++kpart) {
        Vec delta1(r1(jpart+ifirst)-r1(kpart+kfirst));
        cell.pbc(delta1);
        double d1 = dot(delta1,delta1);
        if (d1>rcut2) continue;
        Vec delta2(r2(ipart+ifirst)-r2(kindex2(kpart)+kfirst));
        cell.pbc(delta2);
        double d2 = dot(delta2,delta2);
        if (d2>rcut2) continue;
        mat(ipart,jpart) += anorm*((exp(-alpha*sqrt(d1))-shift)
                                  *(exp(-alpha*sqrt(d2))-shift));
      }
      uarray(ipart,jpart)=-log(fabs(mat(ipart,jpart))+1e-100);
    }
  }
  // Find dominant contribution to determinant (distroys uarray).
  double usum=0;
  ASSNDX_F77(&MODE,uarray.data(),&npart,&npart,&npart,&kindex(islice,0),
             &usum,kwork.data(),&npart);
  for(int ipart=0; ipart<npart; ++ipart) kindex(islice,ipart)-=1;
  // Note: u(ipart,jpart=kindex(islice,ipart)) makes maximum contribution
  // or lowest total action.
  // Next calculate determinant and inverse of slater matrix.
  int info=0;//LU decomposition
  DGETRF_F77(&npart,&npart,mat.data(),&npart,ipiv.data(),&info);
  if (info!=0) {
    std::cout << "BAD RETURN FROM ZGETRF!!!!" << std::endl;
    nerror++;
    if (nerror>1000) {
      std::cout << "too many errors!!!!" << std::endl;
      std::exit(-1);
    }
  }
  double det = 1;
  for (int i=0; i<npart; ++i) {
    det*= mat(i,i); 
    det *= (i+1==ipiv(i))?1:-1;
  }
  DGETRI_F77(&npart,mat.data(),&npart,ipiv.data(),work.data(),&lwork,&info);
  if (info!=0) {
    std::cout << "BAD RETURN FROM ZGETRI!!!!" << std::endl;
    nerror++;
    if (nerror>1000) {
      std::cout << "too many errors!!!!" << std::endl;
      std::exit(-1);
    }
  }
  return det;
}

void AugmentedNodes::evaluateDotDistance(const VArray &r1, const VArray &r2,
         const int islice, Array &d1, Array &d2) {
  std::vector<PeriodicGaussian*> pgSave(NDIM);
  for (int i=0; i<NDIM; ++i) pgSave[i]=pg[i];
  double tauSave=tau;
  // Calculate gradient with finite differences.
  // First calculate distance at smaller beta.
  tau = tauSave*(1.-EPSILON);
  for (int i=0; i<NDIM; ++i) pg[i]=pgm[i];
  evaluate(r1, r2, islice);
  evaluateDistance(r1, r2, islice, temp1, temp2);
  // Next calculate distance at larger beta.
  tau = tauSave*(1.+EPSILON);
  for (int i=0; i<NDIM; ++i) pg[i]=pgp[i];
  evaluate(r1, r2, islice);
  evaluateDistance(r1, r2, islice, d1, d2);
  // Now use finite differences.
  double denom=1./(2*tau*EPSILON);
  d1 = denom*(d1-temp1);
  d2 = denom*(d2-temp2);
  // Restore pg and tau to proper temperature.
  for (int i=0; i<NDIM; ++i) pg[i]=pgSave[i];
  tau=tauSave;
}

void AugmentedNodes::evaluateDistance(const VArray& r1, const VArray& r2,
                              const int islice, Array& d1, Array& d2) {
  Matrix& mat(*matrix[islice]);
  // To avoid double sum over k, find first find closest kindex.
  for (int ipart=0; ipart<npart2; ++ipart) {
    for (int jpart=0; jpart<npart2; ++jpart) {
      Vec delta(r1(jpart+kfirst)-r2(ipart+kfirst));
      cell.pbc(delta);
      kmat(jpart,ipart)=dot(delta,delta);
    }
  }
  const int MODE=1;
  double sum;
  ASSNDX_F77(&MODE,kmat.data(),&npart2,&npart2,&npart2,kindex2.data(),
             &sum,kwork.data(),&npart2);
  kindex2 -= 1;
  double rcut2=16./(alpha*alpha);
  double shift=exp(-4.);
  // Calculate log gradients to estimate distance.
  d1=200; d2=200; // Initialize distances to a very large value.
  for (int jpart=0; jpart<npart; ++jpart) {
    Vec logGrad=0.0, fgrad=0.0;
    for (int ipart=0; ipart<npart; ++ipart) {
      Vec delta=r1(jpart+ifirst)-r2(ipart+ifirst);
      cell.pbc(delta);
      Vec grad;
      double value=1;
      for (int i=0; i<NDIM; ++i) {
        grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
        if (delta[i]<0) grad[i]=-grad[i];
      }
      for (int i=0; i<NDIM; ++i) value*=(*pg[i])(fabs(delta[i]));
      grad *= value;
      // Add contribution from orbital.
      for (int kpart=0; kpart<npart2; ++kpart) {
        Vec delta1(r1(jpart+ifirst)-r1(kpart+kfirst));
        cell.pbc(delta1);
        double d1 = dot(delta1,delta1);
        if (d1>rcut2) continue;
        Vec delta2(r2(ipart+ifirst)-r2(kindex2(kpart)+kfirst));
        cell.pbc(delta2);
        double d2 = dot(delta2,delta2);
        if (d2>rcut2) continue;
        grad += -alpha*delta1/sqrt(d1) 
                      *anorm*(exp(-alpha*sqrt(d1))
                            *(exp(-alpha*sqrt(d2))-shift));
        value += anorm*((exp(-alpha*sqrt(d1))-shift)
                       *(exp(-alpha*sqrt(d2))-shift));
      }
      if (ipart==kindex(islice,jpart)) fgrad=grad/value;
      logGrad+=mat(jpart,ipart)*grad;
    }
    gradArray1(jpart)=logGrad-fgrad;
    d1(jpart+ifirst)=sqrt(2*mass/
                      ((dot(gradArray1(jpart),gradArray1(jpart))+1e-15)*tau));
  }
  for (int ipart=0; ipart<npart; ++ipart) {
    Vec logGrad=0.0, fgrad=0.0;
    for(int jpart=0; jpart<npart; ++jpart) {
      Vec delta=r2(ipart+ifirst)-r1(jpart+ifirst);
      cell.pbc(delta);
      Vec grad;
      double value=1;
      for (int i=0; i<NDIM; ++i) {
        grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
        if (delta[i]<0) grad[i]=-grad[i];
      }
      for (int i=0; i<NDIM; ++i) value*=(*pg[i])(fabs(delta[i]));
      grad *= value;
      // Add contribution from orbital.
      for (int kpart=0; kpart<npart2; ++kpart) {
        Vec delta1(r1(jpart+ifirst)-r1(kpart+kfirst));
        cell.pbc(delta1);
        double d1 = dot(delta1,delta1);
        if (d1>rcut2) continue;
        Vec delta2(r2(ipart+ifirst)-r2(kindex2(kpart)+kfirst));
        cell.pbc(delta2);
        double d2 = dot(delta2,delta2);
        if (d2>rcut2) continue;
        grad += -alpha*delta2/sqrt(d2) 
                      *anorm*((exp(-alpha*sqrt(d1))-shift)
                              *exp(-alpha*sqrt(d2)));
        value += anorm*((exp(-alpha*sqrt(d1))-shift)
                       *(exp(-alpha*sqrt(d2))-shift));
      }
      if (ipart==kindex(islice,jpart)) fgrad=grad/value;
      logGrad+=mat(jpart,ipart)*grad;
    }
    gradArray2(ipart)=logGrad-fgrad;
    d2(ipart+ifirst)=sqrt(2*mass/
                      ((dot(gradArray2(ipart),gradArray2(ipart))+1e-15)*tau));
  }
}

void AugmentedNodes::evaluateGradLogDist(const VArray &r1, const VArray &r2,
       const int islice, VMatrix &gradd1, VMatrix &gradd2, 
       const Array& dist1, const Array& dist2) {
  gradd1=0.; gradd2=0.; 
  const Matrix& mat(*matrix[islice]);
  // ipart is the index of the particle in the gradient.
  // jpart is the index of the particle in the distance.
  for (int ipart=0; ipart<npart; ++ipart) {
    for (int jpart=0; jpart<npart; ++jpart) {
      // First treat d1 terms.
      if (ipart==jpart) {
        Mat d2ii;
        d2ii = 0.0;
        for(int kpart=0; kpart<npart; ++kpart) {
          Vec delta=r1(ipart+ifirst)-r2(kpart+ifirst);
          cell.pbc(delta);
          Mat d2iik;
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
              if (i==j) {
                d2iik(i,j)=(*pg[i]).d2(fabs(delta[i]))
                          /(*pg[i])(fabs(delta[i]));
              } else {
                d2iik(i,j)=(*pg[i]).grad(fabs(delta[i]))
                          /(*pg[i])(fabs(delta[i])) 
                          *(*pg[j]).grad(fabs(delta[j]))
                          /(*pg[j])(fabs(delta[j]));
                if (delta[i]*delta[j]<0) d2iik(i,j)=-d2iik(i,j);
              }
            }
          }
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
              d2ii(i,j) += mat(ipart,kpart)*d2iik(i,j);
            }
          }
        }
        for (int i=0; i<NDIM; ++i) {
          for (int j=0; j<NDIM; ++j) {
            gradd1(ipart+ifirst,jpart+ifirst)(i)=gradArray1(jpart)(j)*d2ii(j,i);
          }
        }
      } else { // ipart!=jpart
        // Two derivatives act on different columns, so you get a 2x2 det.
        Vec g00(0.0),g01(0.0),g10(0.0),g11(0.0);
        for(int kpart=0; kpart<npart; ++kpart) {
          // First derivative acts on particle ipart.
          Vec delta=r1(ipart+ifirst)-r2(kpart+ifirst);
          cell.pbc(delta);
          Vec grad;
          for (int i=0; i<NDIM; ++i) {
            grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
            if (delta[i]<0) grad[i]=-grad[i];
          }
          for (int i=0; i<NDIM; ++i) grad*=(*pg[i])(fabs(delta[i]));
          g00 += mat(ipart,kpart)*grad;
          g10 += mat(jpart,kpart)*grad;
          // Next derivative acts on particle jpart.
          delta=r1(jpart+ifirst)-r2(kpart+ifirst);
          cell.pbc(delta);
          for (int i=0; i<NDIM; ++i) {
            grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
            if (delta[i]<0) grad[i]=-grad[i];
          }
          for (int i=0; i<NDIM; ++i) grad*=(*pg[i])(fabs(delta[i]));
          g01 += mat(ipart,kpart)*grad;
          g11 += mat(jpart,kpart)*grad;
        }
        for (int i=0; i<NDIM; ++i) {
          for (int j=0; j<NDIM; ++j) {
            gradd1(ipart+ifirst,jpart+ifirst)(i)=gradArray1(jpart)(j)
                                 *(g00(i)*g11(j)-g10(i)*g01(j));
          }
        }
      }
      // Then treat d2 terms.
      // Derivative on row is a sum of derivatives on each column.
      for(int lpart=0; lpart<npart; ++lpart) {
        if (ipart==lpart) {
          // Only one term is non-zero. NEED TO CHECK THESE DERIVATIVES
          Vec delta=r1(ipart+ifirst)-r2(jpart+ifirst);
          cell.pbc(delta);
          Mat d2ii;
          d2ii = 0.0;
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
              if (i==j) {
                d2ii(i,j)=(*pg[i]).d2(fabs(delta[i]))
                         /(*pg[i])(fabs(delta[i]));
              } else {
                d2ii(i,j)=(*pg[i]).grad(fabs(delta[i]))
                         /(*pg[i])(fabs(delta[i])) 
                         *(*pg[j]).grad(fabs(delta[j]))
                         /(*pg[j])(fabs(delta[j]));
                if (delta[i]*delta[j]>=0) d2ii(i,j)=-d2ii(i,j);
              }
            }
          }
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
               d2ii(i,j) *= mat(ipart,jpart);
            }
          }
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
              gradd2(ipart+ifirst,jpart+ifirst)(i)=gradArray2(jpart)(j)*d2ii(j,i);
            }
          }
        } else { // ipart!=lpart
          // Two derivatives act on different columns, so you get a 2x2 det.
          Vec g00(0.0),g01(0.0),g10(0.0),g11(0.0);
          for(int kpart=0; kpart<npart; ++kpart) {
            // First derivative acts on particle ipart.
            Vec delta=r1(ipart+ifirst)-r2(kpart+ifirst);
            cell.pbc(delta);
            Vec grad;
            for (int i=0; i<NDIM; ++i) {
              grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
              if (delta[i]<0) grad[i]=-grad[i];
            }
            for (int i=0; i<NDIM; ++i) grad*=(*pg[i])(fabs(delta[i]));
            g00 += mat(ipart,kpart)*grad;
            g10 += mat(lpart,kpart)*grad;
          }
          // Next derivative acts on particle jpart.
          Vec delta=r2(jpart+ifirst)-r1(lpart+ifirst);
          cell.pbc(delta);
          Vec grad(0.0);
          for (int i=0; i<NDIM; ++i) {
            grad[i]=(*pg[i]).grad(fabs(delta[i]))/(*pg[i])(fabs(delta[i]));
            if (delta[i]<0) grad[i]=-grad[i];
          }
          for (int i=0; i<NDIM; ++i) grad*=(*pg[i])(fabs(delta[i]));
          g01 = mat(ipart,jpart)*grad;
          g11 = mat(lpart,jpart)*grad;
          for (int i=0; i<NDIM; ++i) {
            for (int j=0; j<NDIM; ++j) {
              gradd2(ipart+ifirst,jpart+ifirst)(i)=gradArray2(jpart)(j)
                                   *(g00(i)*g11(j)-g10(i)*g01(j));
            }
          }
        }
      }
      // Put in prefactors.
      gradd1(ipart+ifirst,jpart+ifirst)
        *= -(tau/mass)*dist1(jpart+ifirst)*dist1(jpart+ifirst);
      gradd2(ipart+ifirst,jpart+ifirst)
        *= -(tau/mass)*dist2(jpart+ifirst)*dist2(jpart+ifirst);
      // Then add linear (j independent) contribution.
      gradd1(ipart+ifirst,jpart+ifirst)+=gradArray1(ipart);
      gradd2(ipart+ifirst,jpart+ifirst)+=gradArray1(ipart);
    }
  }
}

const double AugmentedNodes::EPSILON=1e-6;

AugmentedNodes::MatrixUpdate::MatrixUpdate(int maxMovers, int maxlevel, 
    int npart, std::vector<Matrix*> &matrix, const AugmentedNodes &fpNodes)
  : fpNodes(fpNodes), maxMovers(maxMovers), npart(npart),
    newMatrix((int)(pow(2,maxlevel)+0.1)+1),
    phi((int)(pow(2,maxlevel)+0.1)+1),
    bvec((int)(pow(2,maxlevel)+0.1)+1),
    smallDet(maxMovers,maxMovers),
    matrix(matrix),
    ipiv(maxMovers),lwork(maxMovers*maxMovers),work(lwork),
    isNewMatrixUpdated(false),index1(maxMovers),mindex1(maxMovers),nMoving(0) {
  for (unsigned int i=0; i<matrix.size(); ++i)  {
    newMatrix[i] = new Matrix(npart,npart,ColMajor());
    phi[i] = new Matrix(npart,maxMovers,ColMajor());
    bvec[i] = new Matrix(npart,maxMovers,ColMajor());
  }
}

double AugmentedNodes::MatrixUpdate::evaluateChange(
    const DoubleMLSampler &sampler, int islice) {
  isNewMatrixUpdated=false;
  const Beads<NDIM>& sectionBeads2=sampler.getSectionBeads(2);
  const Beads<NDIM>& movingBeads1=sampler.getMovingBeads(1);
  // Get info on moving paths of this species.
  const IArray &movingIndex(sampler.getMovingIndex(1));
  nMoving=0;
  for (int i=0; i<movingIndex.size(); ++i) {
    if (movingIndex(i) >= fpNodes.ifirst &&
        movingIndex(i) < fpNodes.npart+fpNodes.ifirst) {
      index1(nMoving) = movingIndex(i);
      mindex1(nMoving) = i;
      ++nMoving;
    }
  }
  // Compute new slater matrix elements for moving particles.
  for (int jmoving=0; jmoving<nMoving; ++jmoving) {
    for (int ipart=0; ipart<npart; ++ipart) {
      Vec delta(movingBeads1(mindex1(jmoving),islice)
               -sectionBeads2(ipart+fpNodes.ifirst,islice));
      fpNodes.cell.pbc(delta);
      double ear2=1;
      for (int i=0; i<NDIM; ++i) ear2*=(*fpNodes.pg[i])(fabs(delta[i]));
      (*phi[islice])(ipart,jmoving)=ear2;
    }
    for (int imoving=0; imoving<nMoving; ++imoving) {
      int ipart=index1(imoving)-fpNodes.ifirst;
      (*bvec[islice])(ipart,jmoving)=0;
      for (int k=0; k<npart; ++k) {
        (*bvec[islice])(ipart,jmoving)
          += (*phi[islice])(k,jmoving)*(*matrix[islice])(ipart,k);
      }
    }
  }
  // Compute change in the slater determinant.
  for (int jmoving=0; jmoving<nMoving; ++jmoving) {
    for (int imoving=0; imoving<nMoving; ++imoving) {
      int ipart=index1(imoving)-fpNodes.ifirst;
      smallDet(imoving,jmoving)=(*bvec[islice])(ipart,jmoving);
    }
  }
  // Calculate determinant and inverse.
  int info=0;//LU decomposition
  DGETRF_F77(&nMoving,&nMoving,smallDet.data(),&maxMovers,ipiv.data(),&info);
  if (info!=0) std::cout << "BAD RETURN FROM ZGETRF!!!!" << std::endl;
  double det = 1;
  for (int i=0; i<nMoving; ++i) {
    det *= smallDet(i,i); 
    det *= (i+1==ipiv(i))?1:-1;
  }
  return det;
}

void AugmentedNodes::MatrixUpdate::evaluateNewInverse(const int islice) {
  *newMatrix[islice]=*matrix[islice];
  for (int jmoving=0; jmoving<nMoving; ++jmoving) {
    int jpart=index1(jmoving)-fpNodes.ifirst;
    double bjjinv=0;
    for (int k=0; k<npart; ++k) {
      bjjinv += (*phi[islice])(k,jmoving)*(*newMatrix[islice])(jpart,k);
    }
    bjjinv=1./bjjinv;
    for (int k=0; k<npart; ++k) {
      (*newMatrix[islice])(jpart,k) *= bjjinv;
    } 
    for (int ipart=0; ipart<npart; ++ipart) {
      if (ipart==jpart) continue;
      double bij=0;
      for (int k=0; k<npart; ++k) {
        bij += (*phi[islice])(k,jmoving)*(*newMatrix[islice])(ipart,k);
      }
      for (int k=0; k<npart; ++k) {
        (*newMatrix[islice])(ipart,k) -= (*newMatrix[islice])(jpart,k)*bij;
      }
    }
  }
  isNewMatrixUpdated=true;
}

void AugmentedNodes::MatrixUpdate::evaluateNewDistance(
    const VArray& r1, const VArray& r2,
    const int islice, Array &d1, Array &d2) {
  // Calculate log gradients to estimate distance.
  d1=200; d2=200; // Initialize distances to a very large value.
  const Matrix& mat(*matrix[islice]);
  for (int jpart=0; jpart<npart; ++jpart) {
    Vec logGrad=0.0, fgrad=0.0;
    for (int ipart=0; ipart<npart; ++ipart) {
      Vec delta=r1(jpart+fpNodes.ifirst)-r2(ipart+fpNodes.ifirst);
      fpNodes.cell.pbc(delta);
      Vec grad;
      for (int i=0; i<NDIM; ++i) {
        grad[i]=(*fpNodes.pg[i]).grad(fabs(delta[i]))/(*fpNodes.pg[i])(fabs(delta[i]));
        if (delta[i]<0) grad[i]=-grad[i];
      }
      if (ipart==fpNodes.kindex(islice,jpart)) fgrad=grad;
      for (int i=0; i<NDIM; ++i) grad*=(*fpNodes.pg[i])(fabs(delta[i]));
      logGrad+=mat(jpart,ipart)*grad;
    }
    fpNodes.gradArray1(jpart)=logGrad-fgrad;
    d1(jpart+fpNodes.ifirst)=sqrt(2*fpNodes.mass
      /((dot(logGrad,logGrad)+1e-15)*fpNodes.tau));
  }
  for (int ipart=0; ipart<npart; ++ipart) {
    Vec logGrad=0.0, fgrad=0.0;
    for(int jpart=0; jpart<npart; ++jpart) {
      Vec delta=r2(ipart+fpNodes.ifirst)-r1(jpart+fpNodes.ifirst);
      fpNodes.cell.pbc(delta);
      Vec grad;
      for (int i=0; i<NDIM; ++i) {
        grad[i]=(*fpNodes.pg[i]).grad(fabs(delta[i]))/(*fpNodes.pg[i])(fabs(delta[i]));
        if (delta[i]<0) grad[i]=-grad[i];
      }
      if (ipart==fpNodes.kindex(islice,jpart)) fgrad=grad;
      for (int i=0; i<NDIM; ++i) grad*=(*fpNodes.pg[i])(fabs(delta[i]));
      logGrad+=mat(jpart,ipart)*grad;
    }
    fpNodes.gradArray2(ipart)=logGrad-fgrad;
    d2(ipart+fpNodes.ifirst)=sqrt(2*fpNodes.mass
      /((dot(logGrad,logGrad)+1e-15)*fpNodes.tau));
  }
}

void AugmentedNodes::MatrixUpdate::acceptLastMove(int nslice) {
  if (isNewMatrixUpdated) {
    for (int islice=1; islice<nslice-1; ++islice) {
      Matrix* ptr=matrix[islice];    
      matrix[islice]=newMatrix[islice];
      newMatrix[islice]=ptr;
    }
  } else {
    for (int islice=1; islice<nslice-1; ++islice) {
      for (int jmoving=0; jmoving<nMoving; ++jmoving) {
        int jpart=index1(jmoving)-fpNodes.ifirst;
        double bjjinv=0;
        for (int k=0; k<npart; ++k) {
          bjjinv += (*phi[islice])(k,jmoving)*(*matrix[islice])(jpart,k);
        }
        bjjinv=1./bjjinv;
        for (int k=0; k<npart; ++k) {
          (*matrix[islice])(jpart,k) *= bjjinv;
        } 
        for (int ipart=0; ipart<npart; ++ipart) {
          if (ipart==jpart) continue;
          double bij=0;
          for (int k=0; k<npart; ++k) {
            bij += (*phi[islice])(k,jmoving)*(*matrix[islice])(ipart,k);
          }
          for (int k=0; k<npart; ++k) {
            (*matrix[islice])(ipart,k) -= (*matrix[islice])(jpart,k)*bij;
          }
        }
      }
    }
  }
}

const double AugmentedNodes::PI = acos(-1.0);
