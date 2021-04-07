# Soundart Chameleon emulator

[SoundArt Chameleon](https://www.chameleon.synth.net/english/index.shtml) was a 19" rack device for musicians as well as for programmers. The device featured a Motorola DSP 56303 and was freely programmable. Soundart provided an SDK to let everyone who was interested program the device to run custom effects or synthesizers.

### Emulation?

This project is a wrapper for the [Soundart Chameleon SDK](https://www.chameleon.synth.net/english/developers/downloads.shtml). It does **not** include the SDK! What this project provides is the following:

It wraps all Chameleon SDK functions, such as the front panel, MIDI, rtems and DSP and uses my [Motorola DSP 56300 emulator](https://github.com/Lyve1981/dsp56300/) to run the DSP code and it compiles everything into a VST plugin, ready to be run in a DAW of your choice.

Strictly speaking, this is not an emulator, but only a wrapper, as existing code must be recompiled. But the result is a Soundart Chameleon as VST plugin without the need for the hardware. Knobs and push buttons are exposed as VST parameters so it can be adjusted.

### Why?

You're right, an emulation that does need the original code is not really helpful. But I needed something to test my DSP emulation with. The Chameleon SDK is very well documented and contains simple examples for developers to start creating their own FX or Instruments. For me, these simple examples are perfect to validate, test & improve my DSP emulation. Once I have a simple example up and running, I continue to run the next one. Goal for now is to have a VST version of the [MonoWave II](https://www.chameleon.synth.net/english/skins/monowave2/), its code is included in the SDK.

### State

 - At the moment, the *dspthru* example executes correctly and in realtime as polling and as interrupt variant. Gain can be controlled with the volume knob, which is a VST parameter, as all other knobs of the Chameleon are, too.
 - MIDI is not implemented yet and projects using it won't compile
 - There is no UI editor yet, VST parameters need to be automated or controller in another way, depending on your DAW you might have a simple editor for all parameters. The panel output is logged to console.

### How to use?
Although the build system needs them, I did **not** include the Chameleon SDK or the VST2 SDK on purpose to not violate any possible copyrights. The build system used is cmake, the code is C++ 14.

After cloning this github project (don't forget the DSP emulator subrepo), you need to add the following:

 - Copy the Chameleon SDK to *tools/* so that you end up with a folder structure of *tools/Chameleon.sdk/...*
 - Copy the VST2 SDK to *source/vstsdk2.4.2/* so that you end up with a folder structure of *source/vstsdk2.4.2/public.sdk/...*
 - cmake . -B temp/cmake_out
 - cmake --build temp/cmake_out

I did *not* include the Chameleon SDK or the VST2 SDK on purpose to not violate any copyrights.