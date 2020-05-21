## tresf

Implementation (with some enhancements) of the "3f synthesis" process, created by Miller Puckette and used by Phillip Manoury in some pieces like *Tensio* (see: http://www.philippemanoury.com/?p=6506, section 3.2).

The object receives lists of 3 frequency values (in Hz) to create up to 64 resulting lists of `frequency`, `amplitude` (dB, from 0 to 100), and `damping` (also, from 0 to 100). The value of frequencies is generated according to the partitioning of multipliers of the original frequencies and their sum. A probability factor (from 0.03 to 1) allows controlling the behavior of frequency filtering, affecting the harmonicity/inharmonicity if the parameters are used in an addictive synthesis, for instance (see `tresf-example.pd`).

One can also control the minimum and maximum values of the generated frequencies (`lowf` and `hif` methods), the maximum amplitude in dB (`maxamp` method), and the factor by which the amplitude decays, in dB, for each band of 1kHz.

Binaries for macOS, Windows, and Linux are provided here: https://github.com/zepadovani/tresf/releases. Please inform me if they are not working in some specific architecture/system (they should run on Pd Vanilla 0.50-2).
