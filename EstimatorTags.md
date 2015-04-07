# Introduction #

## Scalar estimators ##

### Thermodynamic energy estimator ###

```

<ThermalEnergyEstimator unit="Ha" shift="0 Ha" perN="1"/>
```
  * unit allows different energy units, such as eV.
  * shift is useful in semiconductors, for example, to get energy relative to CBM.
  * perN can be used to get the energy per atom, rather than total energy.

## Imaginary time correlation functions ##

A general imaginary time correlation function is

χ<sub>AB</sub>(τ) = -(1/ℏ)⟨A<sup>†</sup>(τ) B(0)⟩

PIMC reports these functions in imaginary frequency space,


χ<sub>AB</sub>(iω<sub>n</sub>) = ∫<sub>0</sub><sup>βℏ</sup> exp(iω<sub>n</sub>τ) χ<sub>AB</sub>(τ) dτ.

where the Matsubura frequencies are iω<sub>n</sub>=2πink<sub>B</sub>T/ℏ.

### ConductanceEstimator ###

```

<ConductanceEstimator nfreq="50" component="all" useCharge="false" useSpeciesTensor="false" species="eup" order="1"/>
```

Current-current correlation estimator for a homogeneous system. (For current density correlations, see ConductivityEstimator.)
Calculates the current-current correlation function,

χ<sub>J<sub>α</sub><sup>A</sup>J<sub>β</sub><sup>B</sup></sub>
= -(1/ℏ)⟨J<sub>α</sub><sup>A</sup>J<sub>β</sub><sup>B</sup>⟩,

where the J<sub>α</sub><sup>A</sup> are the particle (or charge) current
in direction  α for species A.

Results are stored in a rank 6 array,

χ<sub>JJ</sub>(i<sub>order</sub> , α, β, A, B, iω<sub>n</sub>).

Flags control the behavior:
  * `bool useCharge` uses charge currents, otherwise use particle number current.
  * Component is “x,” “y,” “z,” or “all”. If “all” it computes a tensor.
  * `species` selects a species
  * `bool speciesTensor computes a tensor of cross-species correlations
  * `int nfreq` number of frequencies to store.
  * `int order` specifies the order (1 for polarizability, 2 or 3 for hyperpolarizability).


### ConductivityEstimator ###

```

<ConductivityEstimator nbin="100" ndbin="25" nfreq="50" nstride="10" calcInduced="false"/>
```

## Density ##

```

<DensityEstimator nx="20" ny="20" nz="20" a="1 nm"/>
```

```
 <DensityEstimator name="rhoe" species="eup">
   <Radial min="0" max="10 nm" nbin="100"/>
 </DensityEstimator>
```

Up to NDIM distances can be combined to make multidimensional correlation functions. Options are
  * `Cartesian`
  * `dir = x,y,z`
  * `int min, max, nbin`
  * `Radial`
  * `dir=x,y,z`, or `none` (for cylidrical or spherical metric)
  * `int min, max, nbin`

## Pair Correlation functions ##

```
 <PairCFEstimator name="g(r)" species1="eup" species2="H">
   <Radial min="0" max="10 nm" nbin="100"/>
 </PairCFEstimator>
```

or

```
<PairCFEstimator species1="eup" species2="edn" name="cf_ee">
  <Cartesian dir="x"  min="0. nm" max="5 nm" nbin="100"/>
  <Cartesian dir="z"  min="0. nm" max="3 nm" nbin="100"/>
 </PairCFEstimator> 
```

Any number of distances can be combined to make multidimensional correlation functions. Options are
  * `Cartesian`, `Cartesian1`, `Cartesian2` (relative separation or absolute position of species 1 or 2 respectively).
  * `dir = x,y,z`
  * `int min, max, nbin`
  * `Radial`
  * `dir=x,y,z`, or `none` (for cylidrical or spherical metric)
  * `int min, max, nbin`