// $Id: SHODemo.h,v 1.1 2007/05/15 19:43:36 jshumwa Exp $
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
#ifndef __SHODemo_h_
#define __SHODemo_h_

#include "Demo.h"
#include <iostream>

/// Class for simple harmonic oscillator demo.
/// @version $Revision: 1.1 $
/// @author John Shumway.
class SHODemo : public Demo {
public:
  /// Method to generate demo.
  virtual void generate() const;
};
#endif
