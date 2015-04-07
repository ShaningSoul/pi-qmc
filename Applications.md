# Featured Application #

## Coherent exciton state in bilayer graphene ##

<img src='http://pi-qmc.googlecode.com/svn/wiki/images/GrapheneBilayer.png' alt='Illustration of bilayer graphene device for studying coherent exciton condensates.' width='244' align='right' height='182' />

In this work, we use path integral quantum Monte Carlo to study condensation of electron-hole pairs in bilayer graphene. Within the assumptions of our model, we find a transition temperature well above room temperature. The article describes some background on the bilayer graphene systems and explains how path integrals reveal excitonic pairing and excitonic superfluidity.

Essentially, this problem involves the simulation of an excitonic condensate with fixed-node
path integral Monte Carlo. Simulation of an idealized exicitonic condensate in three-dimensions
was the topic of John Shumway's 1999 Ph. D. dissertation (<a href='http://shumway.physics.asu.edu/papers/shumwaydis.pdf'>Quantum Monte<br>
Carlo Simulations of Electrons and Holes, pdf download, 792 KB</a>).

This research is part of the <a href='http://nri.src.org'>SRC-NRI South West Academy of Nanoelectronics (SWAN)</a>.

<ul>
<li>M. J. Gilbert and J. Shumway,<br>
“<em>Probing quantum coherent states in bilayer graphene,</em>”<br>
prepared for a special issue of the Journal of Computational Electronics<br>
(<a href='http://shumway.physics.asu.edu/papers/graphene-jce.pdf'>pdf download, 10 pp. 3.3 MB</a>).<br>
</li>
</ul>

# Applications by area #

## Quantum Dots ##

<img src='http://pi-qmc.googlecode.com/svn/wiki/images/GeSiDot.jpg' alt='Charge density around a Si/Ge quantum dot, from a path integral simulation.' width='200' align='right' height='150' />

Our work developing the **_pi_** code began with applications to self-assembled quantum dots.
In addition to the path integral Monte Carlo algorithm, we have build a set of tools (http://code.google.com/p/qdot-tools) for accurately
modeling the confinement potentials of self-assembled nanostructures, starting from a
given size, shape, and composition profile.


In a project with experimentalists Dr. Sutharsan Ketharanath and Prof. Jeff Drucker, we studied the problem of how many electrons sit in an n-doped Ge/Si quantum dot. The paper, “Electron charging in epitaxial Ge quantum dots on Si(001),” <a href='http://link.aip.org/link/?JAPIAU/105/044312/1'>J. Appl Phys. 105, 044312 (2009)</a>, describes Sutha's experiments, where he measured C-V curves while depleting charge from Ge huts and domes in his gated quantum dot sample. Sourabh provided quantum dot modeling that shows that domes have much wider and deeper confinement potentials than smaller pyramidal dots.

<ul>
<li>
P. G. McDonald, J. Shumway, and I. Galbraith,<br>
“<em>Lateral spatial switching of excitons using vertical electric<br>
fields in semiconductor quantum rings,</em>”<br>
<a href='http://apl.aip.org/resource/1/applab/v97/i17/p173101_s1'>Appl.<br>
Phys. Lett., <b>97</b>, 173101 (2010)</a>
(<a href='http://apl.aip.org/resource/1/applab/v97/i17'>cover image</a>).<br>
(<a href='http://arxiv.org/abs/1007.3975'>arXiv:1007.3975</a>)<br>
</li>
<li>
Sutharsan Ketharanathan, Sourabh Sinha, John Shumway, and Jeff Drucker,<br>
“<em>Electron charging in<br>
epitaxial Ge quantum dots on Si(001),</em>”<br>
<a href='http://link.aip.org/link/?JAPIAU/105/044312/1'>
J. Appl Phys. <b>105</b>, 044312 (2009)</a>
</li>
<li>Jesper Pedersen, Lei Zhang,<br>
Matthew J. Gilbert, and J. Shumway,<br>
<em>“Path integral study of the role of correlation in exchange coupling<br>
of spins in double quantum dots and optical lattices,”</em>
<a href='http://stacks.iop.org/0953-8984/22/145301'>J. Phys.: Condens. Matter<br>
<b>22</b>, 145301 (2010)</a>.<br>
(<a href='http://arxiv.org/abs/0809.0038'>arXiv:0809.0038</a>).<br>
Note: <a href='http://www.nanohub.org/tools/spincoupleddots'>the nanoHUB tool<br>
“Spin Coupled Quantum Dots”</a> lets you try these simulations<br>
yourself.</li>
<li>
M. Harowitz, Daejin Shin, and J. Shumway, “<em>Path-Integral<br>
Quantum Monte Carlo Techniques for Self-Assembled Quantum<br>
Dots,</em>”<br>
<a href='http://www.springerlink.com/content/k3233844185l2k55'>J. of Low Temp.<br>
Phys. <strong>140</strong>, 211-226<br>
(2005)</a>.<br>
</li>
</ul>

## Nanoelectronics ##

<img src='http://pi-qmc.googlecode.com/svn/wiki/images/Promo-qpc-v040.png' alt='Charge density of 100 interacting electrons around a quantum point contact at 750 mK.' width='218' align='right' height='182' />

As part of our work with <a href='http://nri.src.org'>NRI-SRC SWAN</a> center,
we have developed new techniques for studying quantum conductance with
path integral quantum Monte Carlo.
We collect imaginary-time current current correlation functions from our fixed-node
simulations. The Kubo formula relates the spontaneous quantum and thermal fluctuations
of current to the linear conductance of a wire or device.

For studying wires, this approach has profound importance. The phenomena of
one-dimensional wires is usually treated in the Luttinger model, which linearizes
the dispersion relation and has parameters that often must be fit to experiment.
In contrast, the path integral Monte Carlo simulations start from a three-dimensional
effective mass model, and the transport phenomena emerge. This gives the method
significant predictive power, and allows us to study systems outside the domain
of the Luttinger model. For systems where the Luttinger model holds, we can
predict the Luttinger parameters directly from device geometry. Further we can
simulate systems where temperature or finite size effects invalidate the
Luttinger model, and even study the crossover from one dimension to two or three dimensions.

<ul>
<li>
“<em>Current fluctuations and quantized conductance,</em>” Applied Physics Seminar,  Department of Physics and Astronomy, University of Kansas, October 5th, 2009,<br>
(<a href='http://shumway.physics.asu.edu/papers/ku_sem08.pdf'>pdf download, 10.6 MB</a>).</li>
</ul>


## Chemistry/ab inito ##

<img src='http://pi-qmc.googlecode.com/svn/wiki/images/HDimer.jpg' alt='Charge density of 100 interacting electrons around a quantum point contact at 750 mK.' width='200' align='right' height='150' />

We have a long-term goal of realizing path integral Monte Carlo as a practical tool for ab initio
materials simulations and quantum chemistry.
We currently have collaborations with Lawrence Livermore National Laboratory to study materials
under extreme conditions.

One issue we have addressed in recent years is to determine the kind of information
that may be extracted from an ab initio simulation.
We have been particularly interested in linear-reponse theory.
We have shown that the vibrational frequency of a molecule may be extracted
from the autocorrelation of the bond-length in imaginary time. Further,
the spontaneous fluctuations of the dipole can provide highly accurate estimates
of the static polarizability, and—using higher order correlations—third and
fourth order hyperpolarizabilty.

We have a long-term goal of merging our studies of quantum wires with ab initio
simulations of electrical transport.

<ul>
<li>Daejin Shin, Ming-Chak Ho, and J. Shumway, ”<em>Ab inito<br>
path integral techniques for molecules,</em>” submitted to J. Chem. Phys.<br>
(<a href='http://arxiv.org/abs/quant-ph/0611105'>quant-ph/0611105</a>). </li>
<li>J. Shumway, “<em>All-Electron Path Integral Monte Carlo<br>
Simulations of Small Atoms and Molecules</em>,” pp. 181-195 in <em>
<a href='http://www.springeronline.com/materials/book/978-3-540-26564-1'>Computer<br>
Simulations<br>
Studies in Condensed Matter Physics XVII</a></em>, edited by<br>
D. P. Landau, S. P. Lewis, and<br>
H. B. Schütter, (Springer Verlag, Heidelberg, Berlin, 2006)<br>
(<a href='http://shumway.physics.asu.edu/papers/csscmp.pdf'>pdf download 15 pp. 1.65 MB</a>).</li>
</ul>

## Ultracold trapped gases ##

<img src='http://pi-qmc.googlecode.com/svn/wiki/images/Rb87TwoSite.png' alt='Probability density and typical worldlines for four Rb-87 atoms trapped on a two-site optical lattice.' width='243' align='right' height='216' />

We have been working on problems in ultra-cold atomic gases trapped on optical
lattices, in conjunction with the group of Prof. Erich Mueller at Cornell
University.

The image to the right shows a simulation of four <sup>87</sup>Rb atoms in a two-site optical lattice at a temperature of 5 nK. The top panel shows the integrated charge density, and the lower panel shows typical paths. Instantons (quick crossing of the paths between sites) capture the tunneling of Rubidium atoms between sites.