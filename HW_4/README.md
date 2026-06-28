# HomeWork 04 - Fourier Transform

This folder contains my complete answer for **HomeWork 04 - Fourier Transform**.  The homework is based on the FFT tutorial project:

```text
https://github.com/alexninogh/myfft_tutorial
```

The questions are in:

```text
https://github.com/alexninogh/myfft_tutorial/blob/main/LESSONS.md
```

The guided reference solution is in:

```text
https://github.com/alexninogh/myfft_tutorial/blob/main/SOLUTIONS.md
```

Only one Markdown file is used in this submission: this `README.md`.  It contains both the instructions for running the project and the written answers to the homework questions.

---

## File structure

```text
HW_4_README_only/
├── README.md
└── code_modification/
    ├── sampling_demo_option_B.patch
    └── modified_short_record_case.c
```

The file `sampling_demo_option_B.patch` shows the code modification for Part 7.  The file `modified_short_record_case.c` contains the modified code block with bilingual English/Chinese comments.

---

## How to run the original tutorial project

Clone the original repository:

```bash
git clone https://github.com/alexninogh/myfft_tutorial.git
cd myfft_tutorial
```

Build the C programs:

```bash
make
```

Generate CSV output files:

```bash
make run
```

Generate plots using `gnuplot`:

```bash
make plot
```

or generate plots using Python:

```bash
make plot-python
```

Clean generated files:

```bash
make clean
```

---

## Part 1: Before running anything

### 1. What does an FFT tell you about a signal?

An FFT, or Fast Fourier Transform, tells us how much of each frequency is present in a sampled signal.  It converts information from the time domain into the frequency domain.  If a signal contains sinusoidal components at different frequencies, the FFT spectrum should show peaks near those frequencies.

The FFT does not analyze the original continuous signal directly.  It analyzes the discrete sequence of samples given to the algorithm.  Therefore, the result depends on the sampling rate, the total observation time, and whether the samples are uniformly spaced.

### 2. Difference between sampling rate, Nyquist frequency, and frequency resolution

The **sampling rate** is the number of samples collected per second:

\[
f_s = \frac{1}{\Delta t},
\]

where \(\Delta t\) is the time interval between consecutive samples.

The **Nyquist frequency** is half of the sampling rate:

\[
f_N = \frac{f_s}{2}.
\]

It is the highest frequency that can be represented without aliasing in a uniformly sampled signal.

The **frequency resolution** is the spacing between neighboring FFT frequency bins:

\[
\Delta f = \frac{f_s}{N} = \frac{1}{T},
\]

where \(N\) is the number of samples and \(T = N\Delta t\) is the total acquisition time.  A smaller \(\Delta f\) means nearby frequencies are easier to distinguish.

### 3. Why is a uniformly spaced time grid important for the FFT routines used in this project?

The FFT assumes that the input samples are equally spaced in time.  If the time intervals are not uniform, then the standard FFT frequency bins no longer correspond correctly to the sampled data.  For non-uniformly sampled data, one should resample the data onto a uniform grid or use a method designed for non-uniform data.

---

## Part 2: Build and run

After running:

```bash
make run
```

the programs generate CSV files in the `output/` directory.

### 1. Which CSV files are created in `output/`?

The expected output files are:

```text
output/good_sampling_signal.csv
output/good_sampling_spectrum.csv
output/undersampled_signal.csv
output/undersampled_spectrum.csv
output/short_record_signal.csv
output/short_record_spectrum.csv
output/coupled_oscillators_time.csv
output/coupled_oscillators_spectrum.csv
```

### 2. Which program studies pure sampling issues?

The program that studies pure sampling issues is:

```text
src/sampling_demo.c
```

It contains examples about good sampling, undersampling, aliasing, and limited frequency resolution.

### 3. Which program studies a mechanics problem?

The mechanics example is studied by:

```text
src/coupled_oscillators_fft.c
```

This program simulates two coupled oscillators and then analyzes the displacement signal with an FFT.

### 4. What two frequencies are reported in the well-sampled case?

In the well-sampled case, the signal contains two components:

```text
50 Hz and 120 Hz
```

Since the sampling frequency is high enough, both components are below the Nyquist frequency and can be recovered correctly.

---

## Part 3: Sampling and aliasing

This part focuses on `src/sampling_demo.c`.

### 1. Given information

In the `undersampled` case, the sampling frequency is:

\[
f_s = 128\,\mathrm{Hz}.
\]

The original signal contains a \(120\,\mathrm{Hz}\) component.

### 2. Nyquist frequency

The Nyquist frequency is:

\[
f_N = \frac{f_s}{2} = \frac{128}{2} = 64\,\mathrm{Hz}.
\]

### 3. Why can a 120 Hz component not be reconstructed faithfully here?

The \(120\,\mathrm{Hz}\) component is above the Nyquist frequency:

\[
120\,\mathrm{Hz} > 64\,\mathrm{Hz}.
\]

Therefore, the sampled data cannot represent this component correctly.  Instead of appearing at \(120\,\mathrm{Hz}\), it is folded back into the visible frequency range and appears as an alias.

### 4. Aliased peak near 8 Hz

The program reports an aliased peak near:

```text
8 Hz
```

This agrees with the aliasing formula:

\[
f_{\mathrm{alias}} = |f - n f_s|.
\]

For \(f = 120\,\mathrm{Hz}\), \(f_s = 128\,\mathrm{Hz}\), and \(n = 1\):

\[
f_{\mathrm{alias}} = |120 - 128| = 8\,\mathrm{Hz}.
\]

### 5. Physical and mathematical explanation

Physically, sampling means observing the signal only at discrete time points.  If the signal oscillates too fast compared with the sampling frequency, many oscillations occur between two consecutive samples.  The sampled points may then look like they came from a much slower signal.

Mathematically, two continuous sinusoidal signals whose frequencies differ by integer multiples of the sampling frequency can produce the same sampled sequence.  Therefore, a high-frequency component above Nyquist can appear as a false lower-frequency component in the FFT spectrum.

---

## Part 4: Frequency resolution

This part focuses on the `short_record` case.

### 1. Given information

The signal contains two close frequencies:

```text
50 Hz and 55 Hz
```

The original case uses:

```text
N = 64 samples
fs = 512 Hz
```

### 2. Total record length

The total record length is:

\[
T = \frac{N}{f_s} = \frac{64}{512} = 0.125\,\mathrm{s}.
\]

### 3. FFT bin spacing

The frequency spacing is:

\[
\Delta f = \frac{f_s}{N} = \frac{512}{64} = 8\,\mathrm{Hz}.
\]

Equivalently:

\[
\Delta f = \frac{1}{T} = \frac{1}{0.125} = 8\,\mathrm{Hz}.
\]

### 4. Why does this make the two frequencies difficult to separate?

The two true frequencies differ by only:

\[
55 - 50 = 5\,\mathrm{Hz}.
\]

However, the FFT bin spacing is:

\[
\Delta f = 8\,\mathrm{Hz}.
\]

Since the frequency separation is smaller than the bin spacing, the two peaks are not cleanly resolved.  Their spectral contributions overlap, and the FFT cannot clearly separate them.

### 5. What should be changed to improve the separation?

The first thing to change should be the total acquisition time.  Increasing the total time \(T\) decreases the frequency spacing because:

\[
\Delta f = \frac{1}{T}.
\]

Changing the plotting tool or the output file format may change how the data are displayed, but it does not improve the actual frequency resolution of the FFT.

---

## Part 5: Coupled oscillators

This part focuses on `src/coupled_oscillators_fft.c`.

### 1. Equations of motion

The code represents two identical masses attached to walls and coupled by a spring.  The equations of motion are:

\[
m\ddot{x}_1 = -(k+k_c)x_1 + k_c x_2,
\]

\[
m\ddot{x}_2 = k_c x_1 - (k+k_c)x_2,
\]

where:

- \(m\) is the mass,
- \(k\) is the wall spring constant,
- \(k_c\) is the coupling spring constant,
- \(x_1\) and \(x_2\) are the displacements of the two masses.

### 2. What physical system do these equations represent?

They represent two coupled harmonic oscillators.  Each mass is connected to a wall by a spring, and the two masses are connected to each other by another spring.

### 3. What are the two normal modes?

The two normal modes are:

1. **In-phase mode**: both masses move in the same direction at the same time.  In this mode, the coupling spring is not stretched very much.
2. **Out-of-phase mode**: the two masses move in opposite directions.  In this mode, the coupling spring is stretched and compressed more strongly.

The corresponding theoretical frequencies are:

\[
f_{\mathrm{in}} = \frac{1}{2\pi}\sqrt{\frac{k}{m}},
\]

\[
f_{\mathrm{out}} = \frac{1}{2\pi}\sqrt{\frac{k+2k_c}{m}}.
\]

### 4. Why does the FFT of \(x_1(t)\) show more than one important frequency?

The initial condition does not excite only one pure normal mode.  Instead, it excites a combination of the in-phase and out-of-phase modes.  Since \(x_1(t)\) contains contributions from both modes, its FFT shows more than one important frequency peak.

### 5. Why compare numerical FFT peaks with theoretical normal-mode frequencies?

This comparison checks whether the numerical simulation is physically and numerically consistent.  If the FFT peaks are close to the theoretical frequencies, then the ODE model, numerical integration, sampling, and FFT analysis are all behaving correctly.  If the peaks are very different from theory, there may be an error in the equations, parameters, integration, sampling, or FFT normalization.

---

## Part 6: Plot inspection

After generating plots with one of the following commands:

```bash
make plot
```

or:

```bash
make plot-python
```

we can inspect the generated figures.

### 1. Which plot makes aliasing easiest to see?

The sampling spectrum plot, especially the `undersampled` spectrum, makes aliasing easiest to see.  The original \(120\,\mathrm{Hz}\) component appears as a false peak near \(8\,\mathrm{Hz}\).

### 2. Which plot makes limited frequency resolution easiest to see?

The `short_record` spectrum makes limited frequency resolution easiest to see.  The two close frequencies, \(50\,\mathrm{Hz}\) and \(55\,\mathrm{Hz}\), are difficult to separate because the record is too short.

### 3. Do the coupled-oscillator FFT peaks line up with the theoretical reference frequencies?

Yes.  The numerical FFT peaks should line up closely with the theoretical normal-mode frequencies.  They may not be exactly identical because the FFT only evaluates discrete frequency bins.

### 4. What numerical reasons could explain small differences?

Small differences can come from:

- finite observation time,
- discrete FFT bin spacing,
- numerical ODE solver tolerance,
- spectral leakage,
- the fact that the true theoretical frequency may fall between two FFT bins.

---

## Part 7: Small code modification

I chose **Option B: Change the observation time**.

### Modification

In `src/sampling_demo.c`, the original `short_record` case is:

```c
{
    "short_record",
    "The signal contains 50 Hz and 55 Hz, but the record is too short, so the FFT cannot separate nearby frequencies cleanly.",
    close_frequencies_signal,
    512.0,
    64U,
},
```

I modified the number of samples from `64U` to `512U`:

```c
{
    "short_record",
    "The signal contains 50 Hz and 55 Hz. This modified case uses a longer record, so the FFT can separate the nearby frequencies more clearly.",
    close_frequencies_signal,
    512.0,
    512U,
},
```

The sampling frequency stays the same:

\[
f_s = 512\,\mathrm{Hz}.
\]

Only the number of samples is increased.

### Prediction before rerunning

Original case:

\[
N = 64, \quad f_s = 512\,\mathrm{Hz}.
\]

Therefore:

\[
T = \frac{64}{512} = 0.125\,\mathrm{s},
\]

and:

\[
\Delta f = \frac{512}{64} = 8\,\mathrm{Hz}.
\]

Modified case:

\[
N = 512, \quad f_s = 512\,\mathrm{Hz}.
\]

Therefore:

\[
T = \frac{512}{512} = 1.0\,\mathrm{s},
\]

and:

\[
\Delta f = \frac{512}{512} = 1\,\mathrm{Hz}.
\]

Since the two frequencies are \(50\,\mathrm{Hz}\) and \(55\,\mathrm{Hz}\), their separation is \(5\,\mathrm{Hz}\).  With the modified frequency resolution \(\Delta f = 1\,\mathrm{Hz}\), the two frequencies should become much easier to distinguish.

### Expected result after rerunning

After rerunning:

```bash
make clean
make run
make plot-python
```

the `short_record_spectrum.csv` and the corresponding plot should show two more distinguishable peaks near:

```text
50 Hz and 55 Hz
```

The result should be better than the original short-record case because the frequency-bin spacing is now smaller than the distance between the two frequencies.

### Patch for the modification

A patch file is included in:

```text
code_modification/sampling_demo_option_B.patch
```

To apply it manually, edit `src/sampling_demo.c` and replace `64U` by `512U` in the `short_record` case.  Alternatively, apply the patch from the root of the original tutorial repository.

---

## Part 8: Reflection

From this project I learned that an FFT result depends not only on the signal itself, but also on the way the signal is sampled.  If the sampling frequency is too low, high-frequency components can appear as false low-frequency peaks because of aliasing.  If the observation time is too short, nearby frequencies may not be resolved even if they are below the Nyquist frequency.  I also learned that a good scientific-computing project should have a clear structure, with separate source files, reusable helper functions, a Makefile, output directories, and plotting scripts.  One possible improvement would be to add a noisy signal or a damped oscillator example, because that would make the tutorial closer to real experimental data.

---

## Optional extension idea

A useful extension would be to add a damped oscillator:

\[
m\ddot{x} + \gamma \dot{x} + kx = 0.
\]

Compared with an undamped oscillator, the FFT should show a broadened peak rather than a perfectly sharp peak.  Increasing the damping coefficient \(\gamma\) should make the peak wider and reduce the duration of oscillations in the time-domain signal.
