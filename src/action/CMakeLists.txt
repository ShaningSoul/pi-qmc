set (sources
    ActionChoice.cc
    CaoBerneAction.cc
    CompositeAction.cc
    CompositeDoubleAction.cc
    CoulombAction.cc
    DotGeomAction.cc
    DoubleActionChoice.cc
    EFieldAction.cc
    EwaldAction.cc
    EwaldImagePairAction.cc
    FixedPhaseAction.cc
    GateAction.cc
    GaussianAction.cc
    GaussianDotAction.cc
    GrapheneAction.cc
    GridPotential.cc
    HyperbolicAction.cc
    ImagePairAction.cc
    JelliumSlab.cc
    OpticalLatticeAction.cc
    PairAction.cc
    PairIntegrator.cc
    PairPotential.cc
    PrimAnisSHOAction.cc
    PrimColloidalAction.cc
    PrimCosineAction.cc
    PrimSHOAction.cc
    PrimImageAction.cc
    PrimShellAction.cc
    PrimTorusAction.cc
    PrimativePairAction.cc
    QPCAction.cc
    RingGateAction.cc
    SHOAction.cc
    SHODotAction.cc
    SmoothedGridPotential.cc
    SphereAction.cc
    SpringAction.cc
    SpringTensorAction.cc
    StillWebAction.cc
    TimpQPC.cc
    TwoQDAction.cc
    WellImageAction.cc
)

add_library(action STATIC ${sources})

add_subdirectory(test)
