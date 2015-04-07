## Getting started: ##
You can use the built-in demo feature to get started,
```
pi3D --demo=He_atom
```
This generates the following input file (heatom.xml). You may want to rename this file pimc.xml:
```
<?xml version="1.0"?>
<Simulation>
  <SuperCell a="25 A" x="1" y="1" z="1"/>
  <Species name="eup" count="1" mass="1 m_e" charge="-1"/>
  <Species name="edn" count="1" mass="1 m_e" charge="-1"/>
  <Species name="He" count="1" mass="4.0004248 amu" charge="+2"
           isStatic="true"/>
  <Temperature value="2400 K" nslice="12500"/>
  <Action>
    <SpringAction/>
    <CoulombAction norder="2" rmin="0." rmax="10." ngridPoints="2000"
                   dumpFiles="true"/>
    <SphereAction radius="5." species="He"/>
  </Action>
  <Estimators>
    <ThermalEnergyEstimator/>
    <VirialEnergyEstimator nwindow="500"/>
    <CoulombEnergyEstimator/>
    <PairCFEstimator name="rho_up(r)" species1="eup" species2="He">
      <Radial nbin="100" max="5"/>
    </PairCFEstimator>
    <PairCFEstimator name="rho_dn(r)" species1="edn" species2="He">
      <Radial nbin="100" max="5"/>
    </PairCFEstimator>
  </Estimators>
  <PIMC>
    <RandomGenerator/>
    <SetCubicLattice nx="2" ny="2" nz="2" a="1."/>
    <!-- Thermalize -->
    <Loop nrepeat="10000">
      <ChooseSection nlevel="7">
         <Sample npart="1" mover="Free" species="eup"/>
         <Sample npart="1" mover="Free" species="edn"/>
      </ChooseSection>
      <ChooseSection nlevel="5">
         <Sample npart="1" mover="Free" species="eup"/>
         <Sample npart="1" mover="Free" species="edn"/>
      </ChooseSection>
    </Loop>
    <!-- Sample data -->
    <Loop nrepeat="100">
      <Loop nrepeat="50">
        <Loop nrepeat="3">
          <ChooseSection nlevel="9">
            <Sample npart="1" mover="Free" species="eup" nrepeat="5"/>
            <Sample npart="1" mover="Free" species="edn" nrepeat="5"/>
          </ChooseSection>
        </Loop>
        <Loop nrepeat="25">
          <ChooseSection nlevel="7">
            <Sample npart="1" mover="Free" species="eup" nrepeat="2"/>
            <Sample npart="1" mover="Free" species="edn" nrepeat="2"/>
          </ChooseSection>
        </Loop>
        <Loop nrepeat="71">
          <ChooseSection nlevel="5">
            <Sample npart="1" mover="Free" species="eup"/>
            <Sample npart="1" mover="Free" species="edn"/>
          </ChooseSection>
        </Loop>
        <Loop nrepeat="257">
          <ChooseSection nlevel="3">
            <Sample npart="1" mover="Free" species="eup"/>
            <Sample npart="1" mover="Free" species="edn"/>
          </ChooseSection>
        </Loop>
        <Measure estimator="all"/>
      </Loop>
      <Collect estimator="all"/>
      <WritePaths file="paths.out"/>
    </Loop>
  </PIMC>
</Simulation>
```

## Plotting the radial electron distribution ##
The following python script will plot the radial electron distribution.
```
#!/usr/bin/env python

import tables
import numpy
import pylab

h5file = tables.openFile("pimc.h5","r")
gr_up = h5file.getNode("/estimators","rho_up(r)").read()
gr_up_err = h5file.getNode("/estimators","rho_up(r)_err").read()
gr_dn = h5file.getNode("/estimators","rho_dn(r)").read()
gr_dn_err = h5file.getNode("/estimators","rho_dn(r)_err").read()
h5file.close()

nbin=gr_up.shape[0]
deltar = 5.0/nbin
rarray = numpy.array(range(nbin))*deltar + 0.5*deltar

pylab.clf()
pylab.errorbar(rarray,gr_up/deltar,gr_up_err/deltar)
pylab.errorbar(rarray,gr_dn/deltar,gr_dn_err/deltar)
pylab.xlabel(r"r ($a_0$)")
pylab.ylabel(r"$\rho(r)$ [$a_0^{-1}$]")

pylab.savefig("rhor.pdf")
```


## Calculating polarizability ##

Copy paths.out to paths.in, and add a ReadPaths command to pimc.xml (right after SetCubicLattice).
```
 <ReadPaths file="paths.in"/>
```

Add a polarizability estimator:
```
    <ConductanceEstimator nfreq="20"/>
```
The following script will analyze the polarizability:
```
#!/usr/bin/env python
import scipy
import numpy
import tables
import pylab
from scipy.fftpack import *
from scipy.optimize import leastsq


print "\nAnalyzing polarizability\n"

# Read data from density.h5.
#
h5file=tables.openFile("pimc.h5","r")
jjw=h5file.getNode("/estimators","conductance").read()[0,0,0,0,:]
l=h5file.getNode("/simInfo","superCell").read()[0]*0.05291772108
npart=h5file.getNode("/simInfo","npart").read()[0];
nslice=h5file.getNode("/simInfo","nslice").read()[0];
temperature=h5file.getNode("/simInfo","temperature").read()[0];
h5file.close();

nfreq = jjw.shape[0]
print "  Simulation has a temperature of %.3f Ha (%.f K)." % \
      (temperature,temperature*315774.65)
dtau = (1./temperature)/nslice
print "  nslice=%i, nfreq=%i, dtau=%.3f" % (nslice,nfreq,dtau)

omega1=2*numpy.pi*temperature
omegan=omega1*numpy.array(range(1,nfreq))
alphaiwn = -jjw[1:]/omegan**2


#############Fit to a Lorentzian.###############################

def peval(omegan,p):
  return p[0]/(omegan**2+p[1]**2)         #peval=p[0]/(x**2+p[1]**2)<--Lorentzian dist.

def residuals(p,alphaiwn,omegan):
  return alphaiwn-peval(omegan,p)

p0=[1,10*omega1]
plsq = leastsq(residuals,p0,args=(alphaiwn[:],omegan[:]),maxfev=2000)
pfit=plsq[0]
print "\n Fitting parameters: alpha_0=%f, omega_0=%f eV"\
      %(pfit[0]/pfit[1]**2,pfit[1]*27.211)
#print plsq
#pylab.plot(omegan,peval(omegan,plsq[0]))
#pylab.savefig("lorentz.png")

################################################################

print "\n  First few diagonal values of conductance are:"
print "    %f, %f, %f, %f, %f"%tuple(alphaiwn[:5].tolist())

font = {'fontname'   : 'Helvetica',
        'color'      : 'k',
        'fontweight' : 'normal',
        'fontsize'   : 24}

pylab.plot(omegan/omega1,alphaiwn,'b')
pylab.plot(omegan/omega1,peval(omegan,plsq[0]),'r')
#pylab.axis([0,20,0,1.5])
pylab.xlabel(r"$i\omega_n/i\omega_1$",font)
pylab.ylabel(r"$-\chi_{jj}(i\omega_n)/\omega_n^2$",font)
#pylab.show()
pylab.savefig("alphaiwn.pdf")
pylab.savefig("alphaiwn.png")

file = open("alphaiwn.dat","w")
for i in range(nfreq-1):
  file.write("%10.6f %10.6f\n"%(omegan[i], alphaiwn[i]))
print
print "\
The following plots have been made (in png and pdf format):\n\n\
  alphaiwn    The negative of the polarizability in imaginary frequency.\n"
```

The exact answer is α = 1.38... [reference?].

## Calculating Hyperpolarizability ##

Now add an norder="3" attribute to calculate the cubic hyperpolarizability.
```
<ConductanceEstimator nfreq="20" norder="3"/>
```
The exact answer is γ = 43.5... [reference?].