//$Id$
/*  Copyright (C) 2009,2010 John B. Shumway, Jr.

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
#ifndef __AugmentedNodes_h_
#define __AugmentedNodes_h_

#include "NodeModel.h"
#include <vector>
#include <blitz/array.h>
#include <blitz/tinyvec-et.h>
#include <blitz/tinymat.h>
class PeriodicGaussian;
class SimulationInfo;
class Species;
class SuperCell;

/*! Augmented free particle nodes.
We define the node model as a slater determinant of density matricies
of single particles,
@f[\rho_T(R,R')=\operatorname{det}|\rho(r_j,r_i)|.@f]
The single particle density matrices are taken to be a mixture
of free particle density matricies plus orbital density matrices,
@f[\rho(r_j,r_i)=n_0 \frac{1}{(2\pi m k \tau)^{N/2}}
\exp\left(-\frac{m|r_j-r_i|^2}{2\tau}\right)
+ w_i\sum_k \psi(r_j)\psi^*(r_i),@f]
where the @f$w_k@f$ are the weights of the atomic orbitals.
For periodic boundary conditions, we use must use a PeriodicGaussian in 
the free particle density matrix; we assume the atomic orbitals are
smaller than the periodic box.

We estimate the distance to the node as the inverse log gradient,
@f$d_i\approx\rho_T/|\nabla_{R_i}\rho_T|@f$.

The gradient of the distance is given by first and second derivatives
of the trial density matrix,
@f[
\frac{\nabla_i d}{d}=
\frac{\nabla_i \rho}{\rho}
-d^2 \sum_j
\frac{\overleftarrow{\vec{\nabla}}_{i,j}\rho}{\rho}\cdot
\frac{\vec{\nabla}_j\rho}{\rho}
-d^2 \sum_j
\frac{\overleftarrow{\vec{\nabla}}'_{i,j}\rho}{\rho}\cdot
\frac{\vec{\nabla}'_j\rho}{\rho}.
@f]
where the indicies refer to particles.

If this works we will want to rewrite for efficiency!!!
@author John Shumway */
class AugmentedNodes : public NodeModel {
public:
  typedef blitz::TinyMatrix<double,NDIM,NDIM> Mat;
  typedef blitz::Array<double,2> Matrix;
  typedef blitz::Array<Vec,2> VMatrix;
  typedef blitz::Array<double,1> Array;
  typedef blitz::Array<int,1> IArray;
  typedef blitz::Array<int,2> IArray2;
  typedef blitz::Array<double,2> Array2;
  typedef blitz::Array<Vec,2> VArray2;
  typedef blitz::ColumnMajorArray<2> ColMajor;
  /// Class for matrix updates.
  class MatrixUpdate : public NodeModel::MatrixUpdate {
  public:
    MatrixUpdate(int maxMovers, int maxlevel, int npart,
       std::vector<Matrix*>& matrix, const AugmentedNodes &fpnodes);
    virtual double evaluateChange(const DoubleMLSampler&, int islice);
    virtual void evaluateNewInverse(const int islice);
    virtual void evaluateNewDistance(const VArray &r1, const VArray &r2,
      const int islice, Array &d1, Array &d2);
    virtual void acceptLastMove(int nslice);
  private:
    const AugmentedNodes& fpNodes;
    const int maxMovers;
    const int npart;
    /// Temporary storage for inverse slater matricies.
    mutable std::vector<Matrix*> newMatrix;
    /// Temporary storage for new slater matrix columns.
    mutable std::vector<Matrix*> phi;
    /// Temporary storage for new slater matrix products.
    mutable std::vector<Matrix*> bvec;
    /// Temporary storage for small (moving) slater determinant.
    mutable Matrix smallDet;
    /// Reference to the inverse slater matrices.
    std::vector<Matrix*>& matrix;
    /// Matrix diagonalization arrays.
    mutable IArray ipiv;
    const int lwork;
    mutable Array work;
    /// Flag for updating inverse.
    bool isNewMatrixUpdated;
    /// Reference to moving particles.
    IArray index1,mindex1;
    int nMoving;
  };
  /// Classes for atomic orbital density matricies.
  /// These are optimized to work for vectors of identical orbitals.
  class AtomicOrbitalDM {
  public:
    AtomicOrbitalDM(int ifirst, int npart, int nfermion, double weight);
    const double weight;
    const int ifirst;
    const int npart;
    /// Vector displacement of particle i from orbital center j in section 1.
    mutable VArray2 dist1;
    /// Vector displacement of particle i from orbital center j in section 2.
    mutable VArray2 dist2;
    /// Get the array of vector distances for section 1.
    VArray2& getD1Array() const {return dist1;}
    /// Get the array of vector distances for section 2.
    VArray2& getD2Array() const {return dist2;}
    /// Structure for value and gradient.
    struct ValAndGrad {
      ValAndGrad() : val(0.), grad1(0.), grad2(0.) {}
      double val; Vec grad1, grad2;
      ValAndGrad operator+=(ValAndGrad& s) {
        val += s.val; grad1 += s.grad1; grad2 += s.grad2; return *this;
      }
    };
    /// Calculate matrix values using vector distance arrays.
    /// On exit, mat(i,j) has matrix elements for particles ri and rj'.
    virtual void evaluateValue(Matrix&, double scale) const {;}
    /// Calculate matrix values using vector distance arrays.
    /// On exit work and distance arrays are setup for ValAndGrad operator().
    virtual void evaluateValueAndGrad() const {;}
    /// Return value and gradients of matrix element between ri  and rj'.
    /// Must call evaluateValueAndGrad() first to initialize work arrays.
    virtual ValAndGrad operator()(int i, int j) const {
      ValAndGrad temp; return temp;}
  };
  /// Normalized density matrix for 1s orbital.
  /// @f[\rho(r,r') = C^2 e^{-Z(r+r')},@f] where @f$C=\sqrt{Z^3/\pi}@f$.
  class Atomic1sDM : public AtomicOrbitalDM {
  public:
    Atomic1sDM(double Z, int ifirst, int npart, int nfermion, double weight);
    const int nfermion;
    /// Coefficient before the exponential, @f$ C = \sqrt{Z^3/\pi} @f$.
    const double coef;
    /// Storage for @f$ \psi_k(r_i) @f$.
    mutable Array2 work1;
    /// Storage for @f$ \psi^*_k(r_i) @f$.
    mutable Array2 work2;
    virtual void evaluateValue(Matrix&, double scale) const;
    virtual void evaluateValueAndGrad() const;
    virtual ValAndGrad operator()(int i, int j) const;
    const double Z;
  };

  /// Unnormalized density matrix for 2s and 2p orbital.
  /// To avoid negative regions, this does not include the radial
  /// node in the 2s orbital, and there is enough 2s contribution
  /// to avoid an angular node (provided @f$0<p_{\text{angle}}@f<1$).
  /// @f[\rho(r,r') = C^2 rr' e^{-Z(r+r')/2}
  ///                  (1+\hat{\mathbf{r}}\cdot\hat{\mathbf{r}}'),@f]
  /// where @f$C=\sqrt{Z^5/32\pi}@f$.
  /// and @f$0<p_{\text{angle}}@f<1$ controls the strength
  /// angular dependence (@f$p_{\text{angle}}=0@f$ for s-only).
  class Atomic2spDM : public AtomicOrbitalDM {
  public:
    Atomic2spDM(double Z, int ifirst, int npart, int nfermion, 
                double pweight, double weight);
    const int nfermion;
    /// Coefficient before the exponential, @f$ C = \sqrt{Z^5/32\pi} @f$.
    const double coef;
    /// Storage for @f$ \psi_k(r_i) @f$.
    mutable Array2 work1;
    /// Storage for @f$ \psi^*_k(r_i) @f$.
    mutable Array2 work2;
    /// Storage for unit vector r_ik.
    mutable VArray2 work3;
    /// Storage for unit vector r'_ik.
    mutable VArray2 work4;
    virtual void evaluateValue(Matrix&, double scale) const;
    virtual void evaluateValueAndGrad() const;
    virtual ValAndGrad operator()(int i, int j) const;
    const double pweight;
    const double Z;
  };

  /** Constructor. */
  AugmentedNodes(const SimulationInfo&, const Species&,
    double temperature, int maxlevel, bool useUpdates, int maxMovers, 
    const std::vector<const AtomicOrbitalDM*>&, bool useHungarian);
  /** Virtual destructor. */
  virtual ~AugmentedNodes();
  /** Evaluate the density matrix function, returning the value. */
  virtual DetWithFlag evaluate(const VArray &r1, const VArray &r2, 
                               const int islice, bool scaleMagnitude);
  // Evaluate distance to the node in units of @f$ \sqrt{\tau/2m}@f$.
  // Assumes that evaluate has already been called on the slice.
  virtual void evaluateDistance(const VArray &r1, const VArray &r2,
                                const int islice, Array &d1, Array &d2);
  // Evaluate the time-derivative of the distance to the 
  // node in units of @f$ \sqrt{\tau/2m}@f$.
  virtual void evaluateDotDistance(const VArray &r1, const VArray &r2,
                                   const int islice, Array &d1, Array &d2);
  // Evaluate gradient of log of the distance to the node
  // in units of @f$ \sqrt{\tau/2m}@f$.
  virtual void evaluateGradLogDist(const VArray &r1, const VArray &r2,
           const int islice, VMatrix &gradd1, VMatrix &gradd2,
                             const Array &d1, const Array &d2);
  // Returns true if action depends on other particle coordinates.
  virtual bool dependsOnOtherParticles() {return true;}
private:
  /// The time step.
  double tau;
  /// The mass.
  const double mass;
  /// Number of particles of this type of fermion.
  const int npart;
  /// Index of first particle of this type of fermion.
  const int ifirst;
  /// Number of slices.
  int nslice;
  /// The inverse slater matricies.
  mutable std::vector<Matrix*> matrix;
  /// Gradient of the slater determinant.
  mutable VMatrix gradmat;
  /// Matrix diagonalization arrays.
  mutable IArray ipiv;
  const int lwork;
  mutable Array work;
  /// The SuperCell.
  SuperCell& cell;
  /// A periodic gaussian.
  std::vector<PeriodicGaussian*> pg, pgp, pgm;
  /// Flag for checking if this species is being moved.
  bool notMySpecies;
  /// Storage for first derivatives needed for forces.
  mutable VArray gradArray1, gradArray2;
  /// Step for finite difference calculation of time derivative.
  static const double EPSILON;
  /// Storage for calculating derivatives.
  Array temp1, temp2;
  /// Cache storage for calculating distances.
  Array2 cache;
  /// Cache storage for calculating distances.
  VArray2 cacheV;
  /// Storage for calculating dominant contribution to determinant.
  Matrix uarray;
  IArray2 kindex;
  IArray kwork;
  /// Flag for number of bad returns from LAPACK calls.
  int nerror;
  /// Flag to use Hungarian method to remove largest contribution to det.
  const bool useHungarian;
  /// Scale factor to avoid overflow or underflow.
  double scale;
  /// Constant.
  static const double PI;
  /// Density coefficient in front of free particle density matrix.
  const double density;
  const std::vector<const AtomicOrbitalDM*> orbitals;
};
#endif
