**_pi_** — open-source path integral Monte Carlo program

This is a quantum simulation program from the
[Shumway Research Group](http://shumway.physics.asu.edu/), which focuses on applications to nanoscience and technology.
A live version of the code is available as the application [“pimc” on the NCN Purdue nanoHUB](http://www.nanohub.org/tools/pimc/).
The current source code is available from
[project pi-qmc on Google code](http://code.google.com/p/pi-qmc).
The program is object-oriented with [Doxygen markup](http://shumway.physics.asu.edu/codes/doc/pi/index.html) and features flexible XML input format, as described in the [[pimc.xml Reference Guide]].
<br />
<img src='http://pi-qmc.googlecode.com/svn/wiki/images/Promo-qpc-v040.png' alt='Charge density of 100 interacting electrons around a quantum point contact at 750 mK.' align='right' width='300' />

<font color='red'>
<b>UNDER CONSTRUCTION: We are currently porting this wiki over from <a href='http://pimc.wiki.asu.edu'>http://pimc.wiki.asu.edu</a> (19-May-2009)</b>
</font>

## Introduction to the PIMC method ##
A good starting place is David Ceperley's review article on path integrals for superfluid helium,
<ul>
<li>D. M. Ceperley, <a href='http://prola.aps.org/abstract/RMP/v67/i2/p279_1'><i>“Path integrals in the theory of condensed helium,”</i> Rev. Mod. Phys. <b>67</b>, 279–355 (1995)</a>.</li>
</ul>
The path integral approach to quantum statistical physics is described in many books, including
<ul>
<li><a href='http://books.google.com/books?id=4YDfQAYrkjkC'>Feynman's <i>Statistical Physics</i></a> and</li>
<li><a href='http://books.google.com/books?id=VwG0AAAACAAJ'> A. Zee's <i>Quantum Field Theory in a Nutshell</i></a>.</li>
</ul>


## Tutorials ##

### Built in demos ###
You can run demos in the <tt>pi</tt> with automatically generated scripts. To get started, run <tt>pi --demo</tt>. This shows you a list of available demos.
To generate an input file for a simple harmonic oscillator, type <tt>pi --demo=SHO</tt>. This generates an input file <tt>sho.xml</tt> which you can run with <tt>pi sho.xml</tt>.

Currently available demos include:
<dl>
<dt>SHO</dt><dd>A simple harmonic oscillator</dd>
<dt>H_atom</dt><dd>A hydrogen atom</dd>
<dt>HeliumAtomDemo</dt><dd>A helium atom</dd>
<dt>spin</dt><dd>A single spin in a magnetic field</dd>
<dt>QuantumWireDemo</dt><dd>10 electrons in a quasi-1D GaAs (2D) wire</dd>
</dl>

### **_pi_** on nanoHUB ###
We are currently porting our <tt>pi</tt> path integral code to run interactively on the [Purdue nanoHUB (app-pimc)](http://www.nanohub.org/tools/pimc/). More information coming soon.
  * General purpose PIMC interface ([app-pimc](http://www.nanohub.org/tools/pimc/)).
  * Demo on correlated electrons and excitons in quantum dots.

#### Spin Coupled Quantum Dots ####
<img src='http://pi-qmc.googlecode.com/svn/wiki/images/NanoHUBspin.png' />
http://www.nanohub.org/tools/spincoupleddots

#### Expert mode PIMC tool ####
<img src='http://pi-qmc.googlecode.com/svn/wiki/images/NanoHUBpimc.png' />
http://www.nanohub.org/tools/pimc

### Scientific Tutorials ###

  * InGaAs Dot Tutorial.
  * Trapped Bose Condensate Tutorial (<sup>87</sup>Rb condensate).
  * Debye screening
  * Atomic polarizability
  * Thermodynamics of phonons

## Research projects by topic ##

  * Quantum dots
  * Quantum wires
  * Molecular polarizability
  * Graphene
  * H/He plasmas
  * Lattice vibrations

## Tests and benchmarks ##