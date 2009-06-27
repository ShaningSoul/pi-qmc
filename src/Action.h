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
#ifndef __Action_h_
#define __Action_h_
class MultiLevelSampler;
class DisplaceMoveSampler;
class SectionChooser;
class Paths;
#include <blitz/tinyvec.h>
#include <typeinfo>

/// Virtual base class for calculating the action.
/// Action objects can compute differences in action from
/// a MultiLevelSampler, the total action, and the action and
/// its derivatives at a bead.
/// @version $Revision$
/// @author John Shumway.
class Action {
public:
  /// Typdefs and constants.
  typedef blitz::TinyVector<double,NDIM> Vec;
  /// Virtual destructor.
  virtual ~Action() {}
  /// Calculate the difference in action.
  virtual double getActionDifference(const MultiLevelSampler&, int level)=0;
  virtual double getActionDifference(const DisplaceMoveSampler&, int nmoving)=0;
 
  /// Calculate the total action.
  virtual double getTotalAction(const Paths&, const int level) const=0;
  /// Calculate action and derivatives at a bead (defaults to no
  /// contribution).
  virtual void getBeadAction(const Paths&, int ipart, int islice,
          double& u, double& utau, double& ulambda, Vec& fm, Vec& fp) const=0;
  /// Initialize for a sampling section.
  virtual void initialize(const SectionChooser&) {};
  virtual void initialize(const DisplaceMoveSampler &) {};

  /// Accept last move.
  virtual void acceptLastMove() {};
  /// Returns pointer to Action of type t, otherwise returns null pointer.
  virtual const Action* getAction(const std::type_info &type) const { 
    return (typeid(*this)==type) ? this : 0;
  };
};
#endif
