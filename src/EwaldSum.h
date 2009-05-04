// $Id$
/*  Copyright (C) 2008-2009 John B. Shumway, Jr.

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
#ifndef __EwaldSum
#define __EwaldSum
class MultiLevelSampler;
class SectionChooser;
class Paths;
class SuperCell;
#include <vector>
#include <blitz/array.h>
#include <blitz/tinyvec.h>
#include <complex>

/** Class for creating and evaluating Ewald sums.
The most common Ewald summation technique splits a sum of @f$1/r@f$ 
potentials into short-range real space and long-range k-space terms,
@f[
\newcommand{\bfr}{\mathbf{r}}
\newcommand{\bfk}{\mathbf{k}}
\sum_{i<j} \frac{q_iq_j}{|\bfr_{ij}|}
=\sum_{i<j}\frac{q_iq_j\operatorname{erfc}(\kappa|\bfr_{ij}|)}{|\bfr_{ij}|}
+\frac{2\pi}{V}\sum_{\bfk\ne 0}\frac{e^{-\frac{|\bfk|^2}{4\kappa^2}}}{|\bfk|^2}
  \left|\sum_j q_j e^{i\bfk\cdot\bfr_j}\right|^2
-\frac{\kappa}{\sqrt\pi}\sum_j q_j^2
-\frac{2\pi}{3V}\left|\sum_j q_j\bfr_j\right|^2
@f]
Thus the following quantity is zero,
@f[
\newcommand{\bfr}{\mathbf{r}}
\newcommand{\bfk}{\mathbf{k}}
0=-\sum_{i<j}\frac{q_iq_j\operatorname{erf}(\kappa|\bfr_{ij}|)}{|\bfr_{ij}|}
+\frac{2\pi}{V}\sum_{\bfk\ne 0}\frac{e^{-\frac{|\bfk|^2}{4\kappa^2}}}{|\bfk|^2}
  \left|\sum_j q_j e^{i\bfk\cdot\bfr_j}\right|^2
-\frac{\kappa}{\sqrt\pi}\sum_j q_j^2
-\frac{2\pi}{3V}\left|\sum_j q_j\bfr_j\right|^2,
@f]
For a charged system such as the electron gas, there is an additional 
constant term,
@f[
-\frac{\pi}{2\kappa^2 V}\left|\sum_j q_j \right|^2.
@f]
@version $Revision$
@author John Shumway. */
class EwaldSum {
public:
  typedef blitz::TinyVector<double,NDIM> Vec;
  typedef blitz::TinyVector<int,NDIM> IVec;
  typedef blitz::Array<int,1> IArray;
  typedef blitz::Array<double,1> Array1;
  typedef blitz::Array<Vec,1> VArray1;
  typedef std::complex<double> Complex;
  typedef blitz::Array<Complex,2> CArray2;
  /// Constructor calcuates the k-vectors for a given rcut and kcut.
  EwaldSum(const SuperCell&, const int npart, 
           const double rcut, const double kcut);
  /// Virtual destructor.
  virtual ~EwaldSum();
  /// Evaluate the short range function for a radial distance.
  virtual double evalVShort(const double r) const=0;
  /// Evaluate the long range function for a k-vector magnitude.
  virtual double evalVLong(const double k2) const=0;
  /// Evaluate the long range sum.
  double evalLongRange(const VArray1& r) const;
  /// Evaluate the self energy.
  virtual void evalSelfEnergy()=0;
  /// Get a reference to the charge array.
  Array1& getQArray() {return q;}
  /// Set the long range table using the evalVLong virtual method.
  void setLongRangeArray();
protected:
  /// The SuperCell.
  const SuperCell &cell;
  /// The number of particles.
  const int npart;
  /// Real space cutoff.
  const double rcut;
  /// K-space cutoff.
  const double kcut;
  /// Self energy;
  double selfEnergy;
  /// The particle charges.
  mutable Array1 q;
  /// The particle positions.
  mutable VArray1 pos;
  /// Constants.
  static const double PI;
  /// Integer limits of k-space sums.
  IVec ikmax;
  /// Reciprical space lattice spacing.
  Vec deltak;
  /// Number of k-vectors.
  int totk;
  /// Stored values of k-space potential.
  Array1 vk;
  /// Arrays to tabulate @f$e^{ik_xx},e^{ik_yy},e^{ik_zz}@f$
  mutable CArray2 eikx, eiky, eikz;
  /// Calculate the long range part.
  double calcLongRangeUtau(VArray1& r) const;
  /// The the prefactor on the k-space sum.
  double kPrefactor;
};
#endif
