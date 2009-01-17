//$Id: GroundStateSNode.cc,v 1.7 2007/10/03 12:53:56 jshumwa Exp $
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
#include "config.h"
#endif

#include <blitz/tinyvec-et.h>
#include "GroundStateSNode.h"
#include "Species.h"

GroundStateSNode::GroundStateSNode(const Species& s, const int icenter, 
                                   const double tau) 
  : ifirst(s.ifirst), npart(s.count), icenter(icenter), dunit(0.5*tau/s.mass) {
  std::cout << "GroundState S-Node with center = " << icenter << std::endl;
}

GroundStateSNode::~GroundStateSNode() {
}

double GroundStateSNode::evaluate(const VArray &r1, const VArray &r2, 
                                  const int islice) {
  double f=sqrt(dot(r1(ifirst+1)-r1(icenter),r1(ifirst+1)-r1(icenter))) 
          -sqrt(dot(r1(ifirst)-r1(icenter),r1(ifirst)-r1(icenter)));
  f*=sqrt(dot(r2(ifirst+1)-r2(icenter),r2(ifirst+1)-r2(icenter))) 
    -sqrt(dot(r2(ifirst)-r2(icenter),r2(ifirst)-r2(icenter)));
  return f;
}

void GroundStateSNode::evaluateDistance(const VArray &r1, const VArray &r2,
                             const int islice, Array& d1, Array& d2) {
  //double x=fabs(sqrt(dot(r1(ifirst+1)-r1(icenter),r1(ifirst+1)-r1(icenter)))
  //             -sqrt(dot(r1(ifirst)-r1(icenter),r1(ifirst)-r1(icenter))));
  //double y=fabs(sqrt(dot(r2(ifirst+1)-r2(icenter),r2(ifirst+1)-r2(icenter)))
  //             -sqrt(dot(r2(ifirst)-r2(icenter),r2(ifirst)-r2(icenter))));
  //return (x<y)?x/dunit/sqrt(2):y/dunit/sqrt(2);
}

void GroundStateSNode::evaluateGradLogDist(const VArray &r1, const VArray &r2,
          const int islice, VMatrix &gradd1, VMatrix& gradd2,
          const Array &dist1, const Array& dist2) {
/*  force=0;
  double x=fabs(sqrt(dot(r1(ifirst+1)-r1(icenter),r1(ifirst+1)-r1(icenter)))
               -sqrt(dot(r1(ifirst)-r1(icenter),r1(ifirst)-r1(icenter))));
  double y=fabs(sqrt(dot(r2(ifirst+1)-r2(icenter),r2(ifirst+1)-r2(icenter)))
               -sqrt(dot(r2(ifirst)-r2(icenter),r2(ifirst)-r2(icenter))));
  if (x<y) {
    force(ifirst)=r1(ifirst)-r1(icenter);
    force(ifirst+1)=r1(ifirst+1)-r1(icenter);
    double a=sqrt(dot(force(ifirst),force(ifirst)));
    double b=sqrt(dot(force(ifirst+1),force(ifirst+1)));
    force(ifirst)/=a*(a>b)?1:-1;
    force(ifirst+1)/=b*(b>a)?1:-1;
    force(icenter)=-(force(ifirst)+force(ifirst+1));
  } */
}
