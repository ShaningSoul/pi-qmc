//$Id: DiamagneticEstimator.h 22 2009-03-06 13:52:07Z john.shumwayjr $
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

#ifndef __DiamagneticEstimator_h_
#define __DiamagneticEstimator_h_
#include "stats/ScalarEstimator.h"
#include "LinkSummable.h"
#include "Paths.h"
#include <cstdlib>
#include <blitz/array.h>
#include <iostream>
class Paths;
class SimulationInfo;
/** Testing estimator for diamagnetic sus.
 * See Pollock and Runge, "Study of diamagnetic response,"
 * J Chem Phys Vol 96, pp. ????? (1992).
 * Eq. (7) (above equation) & Eq. 26 (which includes time-step error)
 * @bug Needs to treat particles correct at periodic boundaries.
 **/ 
class DiamagneticEstimator : public ScalarEstimator, public LinkSummable {
public:
  typedef blitz::Array<double,1> Array;
  typedef blitz::TinyVector<double,NDIM> Vec;
  /// Constructor.
  DiamagneticEstimator(const SimulationInfo& simInfo, double temperature,
                       const std::string& unitName, double scale);
  /// Virtual destructor.
  virtual ~DiamagneticEstimator() {}
  /// Initialize the calculation.
  virtual void initCalc(const int nslice, const int firstSlice);
  /// Add contribution from a link.
  virtual void handleLink(const Vec& start, const Vec& end,
                          const int ipart, const int islice, const Paths&);
  /// Finalize the calculation.
  virtual void endCalc(const int nslice);
  /// Get value of coulomb energy estimator.
  virtual double calcValue() {return value/norm;}
  /// Clear value of dipole energy estimator.
  virtual void reset() {value=norm=0;}
  /// Evaluate for Paths configuration.
  virtual void evaluate(const Paths& paths) {paths.sumOverLinks(*this);}
private:
  /// The diamagnetic susceptibility.
  double value;
  /// The area of the path.
  double area;
  /// The higher order term to average, Eq. (24).
  double term2;
  /// The normalization.
  double norm;
  /// temperature, for calculating susceptibility.
  const double temperature;
  /// charges..
  Array q;
  /// Mass coefficents.
  Array coef;
  /// Constant term in Eq. (24).
  double tauTerm;
  /// The speed of light.
  static const double C;
};

#endif
