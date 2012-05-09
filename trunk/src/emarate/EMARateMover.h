#ifndef __EMARateMover_h_
#define __EMARateMover_h_
#include <cstdlib>
#include <blitz/array.h>
#include <vector>
#include "sampler/Mover.h"
#include "sampler/PermutationChooser.h"
#include "sampler/ParticleChooser.h"
#include "Beads.h"
class SimulationInfo;
class PeriodicGaussian;

class EMARateMover : public Mover,
                     public PermutationChooser,
                     public ParticleChooser {
public:
    typedef blitz::Array<int,1> IArray;
    typedef blitz::Array<double,1> Array;
    typedef blitz::Array<PeriodicGaussian*,3> PGArray;
    typedef blitz::TinyVector<double,NDIM> Vec;
    EMARateMover(double tau, double mass1, double mass2,
            int maxlevel, double C);
    virtual ~EMARateMover();
    /// Move the samplers moving beads for a given level, returning
    /// the probability for the old move divided by the probability for the
    /// new move.
    virtual double makeMove(MultiLevelSampler&, const int level);
    virtual double makeDelayedMove(MultiLevelSampler&, const int level) ;
    virtual double getForwardProb() {return forwardProb;}
    virtual void chooseParticles() {}

    void chooseDiagonalOrRadiating(const Beads<NDIM> &movingBeads,
            int nSlice, const SuperCell&, int nStride);
    double calculateDiagonalProbability(const Beads<NDIM>& movingBeads,
            int nSlice, const SuperCell&, int nStride);
    double calculateRadiatingProbability(const Beads<NDIM> &movingBeads,
            int nSlice, const SuperCell&, int nStride);
private:
    double tau;
    double lambda1;
    double lambda2;
    double forwardProb;
    bool isSamplingRadiating;
    double earlierTransitions;
    const double C;
};
#endif