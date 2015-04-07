# Introduction #
This documentation describes the <tt>pimc.xml</tt> input file and the
[pitools](pitools.md) python utilities for reading the <tt>pimc.h5</tt> output file.

The pimc.xml files are xml files (start with <?xml version="1.0"?>) and have a root tag named < Simulation>. The root tag contains six elements. Order doesn't matter to the parser, but we always order them in our pimc.xml input files:

```
<?xml version="1.0"?>
<Simulation>
  <SuperCell ... />
  <Species ... />
  <Temperature .../>
  <Action>
          ...
  </Action>
  <Estimators>
     ...
  </Estimators>
  <PIMC ...>
    ...
  </PIMC>
</Simulation>
```

As you can see, the file is grouped by six tags that describe the PIMC simulation:
  * SuperCell
  * Species (this tag is repeated for each species).
  * Temperature
  * Action (contains all the [ActionTags](ActionTags.md)).
  * Estimators (contains all the [EstimatorTags](EstimatorTags.md)).
  * PIMC (specifies the parallelism and contains the algorithm tree as [PIMCTags](PIMCTags.md)).
All these tags are required.

Notes:
**The code uses [atomic units](http://www.answers.com/topic/hartree-units?cat=technology) (Ha, a**<sub>0</sub>), but has some unit conversion capabilities.

## Supercell ##
The SuperCell tag is parsed in <tt>SimInfoParser.cc</tt>.


## Species ##
The Species tags are parsed in <tt>SimInfoParser.cc</tt>.


## Temperature ##

The Temperature tag is parsed in <tt>SimInfoParser.cc</tt>.

## Actions ##

In path integrals, the action plays the same role as the Hamiltonian plays in Schrödinger's equation.
The action section of a pimc.xml file is denoted
```
<Action>
...
</Action>
```
and contains any number of [ActionTags](ActionTags.md). Most simulations contain at least SpringAction for free-particle kinetic action, but in some cases even that may be replaced.
Action tags are parsed in <tt>ActionParser.cc</tt>.

## Estimators ##

Estimators are the mathematical and algorithmic tools to extract physical information from the path integral.
The estimator section is denoted
```
<Estimators>
...
</Estimators>
```
and may contain any number of [EstimatorTags](EstimatorTags.md).
Estimator tags are parsed in <tt>EstimatorParser.cc</tt>.

## PIMC Commands ##

The PIMC commands are denoted
```
<PIMC>
...
</PIMC>
```
and may contain any number of sequential or nested [PIMCTags](PIMCTags.md). PIMC commands describe the how the paths are sampled, when measurements are performed, and when data is written to disk.
PIMC tags are parsed in <tt>PIMCParser.cc</tt>.

## Output: pimc.h5 and pitools ##

See [pitools](pitools.md) documentation.