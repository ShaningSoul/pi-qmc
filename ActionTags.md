# Introduction #

## EFieldAction ##

```

<EFieldAction strength="50000 V/cm" dir="z"  width="10 nm" />
```

dir = "x" "y" or "z" , width is region centred at 0 where finite field is generated.


## FixedNodeAction ##
```

<FixedNodeAction species="eup" model="SHONodes" temperature="1 Ha"/>
```

Attributes:
  * <tt>model</tt>
  * "FreeNodes" (default)
  * "SHONodes"
  * "WireNodes"
  * "GSSNode" or "GroundStateSNode"
  * <tt>species</tt>
  * double <tt>temperature</tt>
  * bool <tt>noNodalAction</tt>
  * bool <tt>useUpdates</tt>

## SHODotAction ##
```

<SHODotAction thickness="10 nm" v0="100 meV" k="0"/>
```


## TwoQDAction ##
```

<TwoQDAction omega="12 meV" d="50 nm" alpha="1.0"/>
```


## SHOAction ##

Action for a Simple Harmonic Oscillator (SHO) potential, using the exact density matrix (For a derivation, see Feynman, <em>Statistical Mecahnics</em> (1972).)

```

<SHOAction omega="30 meV" ndim="3" species="hole"/>
```

Attributes:
**omega - sets hbar\*omega of the SHO Action.**

**ndim - defines the number of dimensions for the action, if left out the action will default to no. dimensions in the simulation.**

