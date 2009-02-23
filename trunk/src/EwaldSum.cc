// $Id$
/*  Copyright (C) 2004-2007 John B. Shumway, Jr.

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
#include "EwaldSum.h"
#include <cmath>
#include "SimulationInfo.h"
#include "Species.h"
#include "SuperCell.h"
#include "Paths.h"
#include "Beads.h"
#include "MultiLevelSampler.h"

EwaldSum::EwaldSum(const SuperCell& cell, const int npart,
                   const double rcut, const double kcut)
  : cell(cell), npart(npart), rcut(rcut), kcut(kcut),
    kappa(3.0/rcut), q(npart)
#if NDIM==3
    ,ikmax((int)(cell.a[0]*kcut/(2*PI)),(int)(cell.a[1]*kcut/(2*PI)),
          (int)(cell.a[2]*kcut/(2*PI))),
    deltak(2*PI*cell.b[0],2*PI*cell.b[1],2*PI*cell.b[2]),
    eikx(blitz::shape(npart,ikmax[0]+1)),
    eiky(blitz::shape(0,-ikmax[1]), blitz::shape(npart,2*ikmax[1]+1)),
    eikz(blitz::shape(0,-ikmax[2]), blitz::shape(npart,2*ikmax[2]+1)),
    twoPiOverV(2*PI/(cell.a[0]*cell.a[1]*cell.a[2]))
#endif
{
#if NDIM==3
  q=1.0;
  evalSelfEnergy();
  // Calculate self-energy contribution.
  //for (int jpart=0; jpart<npart; ++jpart) selfEnergy+=q(jpart)*q(jpart);
  //selfEnergy*=kappa/sqrt(PI);
  // Calculate k-vectors.
  totk=0;
  std::cout << ikmax;
  for (int kx=0; kx<=ikmax[0]; ++kx) {
    double kx2=kx*kx*deltak[0]*deltak[0];
    for (int ky=((kx==0) ? 0 : -ikmax[1]); ky<=ikmax[1]; ++ky) {
      double ky2=ky*ky*deltak[1]*deltak[1];
      for (int kz=((kx==0 && ky==0)? 0 : -ikmax[2]); kz<=ikmax[2]; ++kz) {
        double k2=kx2+ky2+kz*kz*deltak[2]*deltak[2];
        if (k2<kcut*kcut && k2!=0) ++totk;
      }
    }
  }
  std::cout << "Ewald: totk=" << totk << std::endl;
  expoverk2.resize(totk);
  int ikvec=0;
  for (int kx=0; kx<=ikmax[0]; ++kx) {
    double kx2=kx*kx*deltak[0]*deltak[0];
    for (int ky=((kx==0) ? 0 : -ikmax[1]); ky<=ikmax[1]; ++ky) {
      double ky2=ky*ky*deltak[1]*deltak[1];
      for (int kz=((kx==0 && ky==0)? 0 : -ikmax[2]); kz<=ikmax[2]; ++kz) {
        double k2=kx2+ky2+kz*kz*deltak[2]*deltak[2];
        if (k2<kcut*kcut && k2!=0) {
          expoverk2(ikvec++)=exp(-k2/(4*kappa*kappa))/k2;
        }
      }
    }
  }
#endif
}

EwaldSum::~EwaldSum() {
}

void EwaldSum::evalSelfEnergy() {
  selfEnergy = -kappa/sqrt(PI)*sum(q*q);
}

double EwaldSum::evalShortRange(const double r) const {
 return -erf(kappa*r)/r;
}

double EwaldSum::evalLongRange(const VArray1& r) const {
  // Set up the exponential tables
#if NDIM==3
  for (int ipart=0; ipart<npart; ++ipart) {
    const Complex I(0,1);
    if (ikmax[0]>0) {
      eikx(ipart,0)=eiky(ipart,0)=eikz(ipart,0)=1.0;
      eikx(ipart,1)=exp(I*deltak[0]*r(ipart)[0]);
    }
    if (ikmax[1]>0) {
      eiky(ipart,1)=exp(I*deltak[1]*r(ipart)[1]);
      eiky(ipart,-1)=conj(eiky(ipart,1));
    }
    if (ikmax[2]>0) {
      eikz(ipart,1)=exp(I*deltak[2]*r(ipart)[2]);
      eikz(ipart,-1)=conj(eikz(ipart,1));
    } 
    for (int kx=2; kx<=ikmax[0]; ++kx) {
      eikx(ipart,kx)=eikx(ipart,kx-1)*eikx(ipart,1);
    }
    for (int ky=2; ky<=ikmax[1]; ++ky) {
      eiky(ipart, ky)=eiky(ipart, ky-1)*eiky(ipart, 1);
      eiky(ipart,-ky)=eiky(ipart,-ky+1)*eiky(ipart,-1);
    }
    for (int kz=2; kz<=ikmax[2]; ++kz) {
      eikz(ipart, kz)=eikz(ipart, kz-1)*eikz(ipart, 1);
      eikz(ipart,-kz)=eikz(ipart,-kz+1)*eikz(ipart,-1);
    }
  }
  // Sum long range action over all k-vectors.
#endif
  double sum=0;
#if NDIM==3
  int ikvec=0;
  for (int kx=0; kx<=ikmax[0]; ++kx) {
    double factor=(kx==0)?1.0:2.0;
    double kx2=kx*kx*deltak[0]*deltak[0];
    for (int ky=((kx==0) ? 0 : -ikmax[1]); ky<=ikmax[1]; ++ky) {
      double ky2=ky*ky*deltak[1]*deltak[1];
      for (int kz=((kx==0 && ky==0)? 0 : -ikmax[2]); kz<=ikmax[2]; ++kz) {
        double k2=kx2+ky2+kz*kz*deltak[2]*deltak[2];
        if (k2<kcut*kcut && k2>=kcut*kcut*1e-9) {
          Complex csum=0.;
          csum*=0.;
          for (int jpart=0; jpart<npart; ++jpart) {
            csum+=q(jpart)*eikx(jpart,kx)*eiky(jpart,ky)*eikz(jpart,kz);
          }
          //sum+=factor*expoverk2(ikvec++)*norm(csum);
          sum+=factor*expoverk2(ikvec++)*
               abs(csum)*abs(csum);
//               (real(csum)*real(csum)+imag(csum)*imag(csum));
//std::cout << "sum=" << sum << std::endl;
//std::cout << "csum=" << csum << std::endl;
        }
      }
    }
  }
#endif
  return sum*twoPiOverV + selfEnergy;
}

const double EwaldSum::PI=3.14159265358979;