# ZePolA
A Parametric Equalizer with Interactive Poles and Zeros Control for Digital Signal Processing Education

![screenshot](https://gist.githubusercontent.com/ChromaticIsobar/2478ada1946b8af2f5f49840de30de5e/raw/97c026249ea2a2fcb71cfae6ab41e8344bdafb9e/screenshot.png)

ZePolA is a parameteric equalizer that can be operated by specifying the positions of poles and zeros on the Gaussian plane. You can experiment with your own filter by either:
 - specifying the numeric values of the parameters of the filter elements
 - interacting with the filter element representations in the Gaussian plane
 - designing a classical filter via the filter designer

## Parameter panel
- **Radius** Element distance from the origin in the Gaussian Plane
- **Angle** Element angle in the Gaussian plane (normalized between 0 and 1)
- **Hz** Element angle as a frequency in hertz
- **Type** Element type (zero or pole)
- **Active** Turns the element on/off
- **Gain** Input gain in decibel
- **Out** Moves zeros outside the unit circle (the magnitude is the inverse of the radius parameter)
- **1x** First-order filter element (real axis only)

### Gaussian Plane
Interact with the circles (zeros) and crosses (poles) on the plane to modify the element parameters
 - *Right-click* Activate/deactivate elements
 - *Drag* Adjust radius and angle
 - *Double-click* Swap between zero and pole
 - *Scroll* Adjust input gain

### Shortcuts
- **All On** Turn all elements on
- **All Off** Turn all elements off
- **Phases x2** Multiply all phase values by 2
- **Phases ÷2** Divide all phase values by 2
- **Swap Ps/Zs** Swaps all zeros with poles and vice-versa

## Plots
The plots panel shows the filter's DTFT magnitude and phase
 - **LIN/DB** Toggle between linear amplitude or decibel
 - **LIN/LOG** Toggle between linear or logarithmic frequencies

## Filter Design
Parameters:
- **Type** Butterworth, Chebyshev Type-I, Chebyshev Type-II, or Elliptic
- **Shape** Low-pass, or High-pass
- **Order** Filter order (number of elements)
- **Cutoff frequency** Transition frequency between pass-band and stop-band
- **Passband ripple** Ripple amplitude in the passband, in decibel
- **Stopband ripple** Ripple amplitude in the stopband, in decibel

Buttons:
- **Update** Apply the settings to the filter elements
- **Auto** Automatically update on parameter change

## Top Menu
- **Undo** Revert changes
- **Redo** Restore changes
- **Reset** Restore default parameters
- **Save** Write parameters to an XML file
- **Load** Read parameters from an XML file
- **Export** Save filter coefficients to a CSV file
- **Man/Auto Gain** Toggles the auto-gain feature 

# License
Copyright (c) 2025 Laboratorio di Informatica Musicale

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
