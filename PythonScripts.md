# Introduction #

We often use python to analyze the output from **_pi_**.


# pitools Package #

The source distribution has a subdirectory <tt>python</tt> that contains some utilities for reading and analyzing the <tt>pimc.h5</tt> files. To use pitools in your scripts, it is helpful to include pitools in your <tt>PYTHON_PATH</tt>,
```sh

export PYTHONPATH = $HOME/codes/pi/python/pitools
```
where you should replace <tt>$HOME/codes/pi</tt> with location of the **_pi_** source directory on your computer.
Then you can include pitools in python with the following commands,
```py

import pitools
from pitools import Unit
```
The second line is convenient so you can just refer to the static <tt>Unit</tt> object by name.

## File Object ##

Constructed with the function
```py

filemount=pitools.openFile("pimc.h5")
```
filemountis the file where data are mounted in. The filename defaults to <tt>pimc.h5</tt>, so you can omit that argument if you are running the script on
a <tt>pimc.h5</tt> file in the current working directory.

A File object has the following methods,
  * getNSlice()
  * getNPart()
  * getTemperature(unit=Unit.Ha)
  * getSuperCell(unit=Unit.a0)
  * getScalar(name,unit=None)
  * getResponseFunction(name)
  * getDensity(name,unit=Unit.a0)
  * getPairCF(name)

## FileGroup Object ##

To analyze multiple <tt>pimc.h5</tt> files, you can use a FileGroup.
You can create a FileGroup using an explicit list of files or by giving a wildcard pattern.
```py

# Open three files by giving explicit filenames.
filegroup = openFileGroup(["pimc.h5","newdir/pimc.h5","oldpimc.h5"])
# Open all files matching the pattern (here, all pimc.h5 files in immediate subdirectories).
filegroup = openFileGroup(pattern="*/pimc.h5")
```

A FileGroup object has the following methods, which return arrays of values,
  * getNSlices()
  * getNPart()
  * getTemperature(unit=Unit.Ha)
  * getScalar(name,unit=None)

## Scalar Object ##
A Scalar object has the following method,
  * getAverage()
More methods for Scalar objects are planned, to reproduce the features of the java dataspork utility.

## Density Object ##
A Density object has the following fields,
  * data
  * error
  * origin
  * scale
  * extent

## PairCF Object ##
A PairCF object has the following fields,
  * data
  * error

## ResponseFunction Object ##
A ResponseFunction object has the following fields,
  * data
  * error
  * nfreq
  * omega1
  * omega

# Hints for useful python scripts #

Start your script with the lines
```
#!/usr/bin/env python
# -*- coding: utf-8 -*-
```
The first line is standard to tell the shell to execute the script with the python program in your path.
The second line lets you use UTF-8 characters (such as α,β,γ or ℏ or math symbols like ±) in output strings
in your code.

To make publication quality figures, set the frame size to 3 3/8 inches (or whatever size your journal uses),
use TeX to render labels, and set the font point size to appropriate values (usually 6 to 10 points).

```
pylab.gcf().set_size_inches(3.375,2.5)
pylab.rc('text', usetex=True)
pylab.rc('font', family='serif', size=9)
```

To make subplots, use <tt>pylab.axes</tt> to specify the region in the unit square to lay out your plot.
You can use xticks with empty label names to plot the ticks
without the numbers.
```
pylab.axes([0.17,0.67,0.82,0.31])
pylab.errorbar(npart,en,den)
pylab.axis(ymin=0,xmin=0.5,xmax=5.5)
pylab.xticks([1,2,3,4,5],[""]`*`5)
pylab.ylabel(r"$E$ [$E_R$]")
```