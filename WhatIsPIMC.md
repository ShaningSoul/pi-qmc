# Introduction #

Path integral Monte Carlo (PIMC) simulates particles (often electrons and ions) by directly sampling the canonical partition function. In the path integral formulation of quantum statistical mechanics developed by Richard Feynman, particles get represented by closed imaginary-time trajectories of length ℏ_/k_<sub>B</sub>T_. PIMC simulations are able to compute total energies, correlation functions, charge distribution, and linear response functions for thermal equilibrium. As in many quantum Monte Carlo methods, PIMC has efficient scaling with system size, often order N or N_<sup>2</sup>.

Our application, _**pi**_, is well suited for modeling conduction electrons in quantum dots, quantum wires, and quantum wells. We have also tested it for ab initio calculations, but at this point only hydrogen and helium atoms work well.

David Ceperley and his research group have done groundbreaking work on PIMC development and applications to superfluid helium, hot dense plasmas, and electron gases.