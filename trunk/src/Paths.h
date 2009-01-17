// $Id: Paths.h,v 1.14 2008/01/08 18:38:20 jshumwa Exp $
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
#ifndef __Paths_h_
#define __Paths_h_

class SuperCell;
template <int TDIM> class Beads;
class Permutation;
class LinkSummable;
#include <blitz/array.h>

/// Class for Paths, including Beads, connectivity (including any Permutation),
/// the time step and SuperCell.
/// @version $Revision: 1.14 $
/// @author John Shumway
class Paths {
public:
  /// Constants and typedefs.
  typedef blitz::TinyVector<double,NDIM> Vec;
  typedef blitz::Array<Vec,1> VArray;
  /// Constructor.
  Paths(const int npart, const int nslice, const double tau,
    const SuperCell& cell);
  /// Destructor.
  virtual ~Paths() {}
  /// Loop over links, calling a LinkSummable object.
  virtual void sumOverLinks(LinkSummable&) const=0;
  /// Get the number of particles.
  int getNPart() const {return npart;}
  /// Get the number of slices.
  int getNSlice() const {return nslice;}
  /// Get the temperature for a slice.
  double getTau() const {return tau;}
  /// Get the supercell.
  const SuperCell& getSuperCell() const {return cell;}
  /// Get a reference to a bead.
  virtual Vec& operator()(const int ipart, const int islice)=0;
  /// Get a const reference to a bead.
  virtual const Vec& operator()(const int ipart, const int islice) const=0;
  /// Get a reference to a bead by offset.
  virtual Vec& operator()(const int ipart, const int islice, const int istep)=0;
  /// Get a const reference to a bead by offset.
  virtual const Vec&
    operator()(const int ipart, const int islice, const int istep) const=0;
  /// Get a relative displacement a bead by offset.
  virtual Vec
    delta(const int ipart, const int islice, const int istep) const=0;
  /// Get beads.
  virtual void getBeads(int ifirstSlice, Beads<NDIM>& ) const=0;
  /// Get auxialiary bead.
  virtual const void* getAuxBead(const int ipart, const int islice, 
                                 const int iaux) const=0;
  /// Get auxialiary bead.
  virtual void* getAuxBead(const int ipart, const int islice, 
                                 const int iaux)=0;
  /// Get a slice.
  virtual void getSlice(int islice, VArray& ) const=0;
  /// Put beads.
  virtual void putBeads(int ifirstSlice,
                        const Beads<NDIM>&, const Permutation&) const=0;
  virtual void putDoubleBeads(
            int ifirstSlice1,Beads<NDIM>&, Permutation&,
            int ifirstSlice2,Beads<NDIM>&, Permutation&) const=0;
  /// Get the global permuation.
  virtual const Permutation& getPermutation() const=0;
  virtual int getLowestSampleSlice(const int n, bool d) const=0;
  virtual int getHighestSampleSlice(const int n, bool d) const=0;
  virtual int getHighestStoredSlice(const int n, bool d) const=0;
  virtual void shift(const int ishift)=0;
  virtual void setBuffers() {}
  virtual bool isDouble() const {return false;}
  virtual bool isProcessorSlice(const int islice) const=0;
  virtual void clearPermutation()=0;
protected:
  /// Number of particles.
  const int npart;
  /// Number of slices.
  const int nslice;
  /// The temperature of one timestep.
  const double tau;
  /// The supercell.
  const SuperCell& cell;
};
#endif
