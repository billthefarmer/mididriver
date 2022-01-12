# ![Logo](app/src/main/res/drawable-hdpi/ic_launcher.png) Midi Driver ![.github/workflows/build.yml](https://github.com/billthefarmer/mididriver/workflows/.github/workflows/build.yml/badge.svg) [![GitHub All Releases](https://img.shields.io/github/downloads/billthefarmer/mididriver/total)](https://github.com/billthefarmer/mididriver/releases) [![JitPack](https://jitpack.io/v/billthefarmer/mididriver.svg)](https://jitpack.io/#billthefarmer/mididriver)

Android midi driver using Sonivox EAS library. The library is
available in
[Releases](https://github.com/billthefarmer/mididriver/releases).

![](https://github.com/billthefarmer/billthefarmer.github.io/raw/master/images/miditest.png)

Licensed under Apache license.

Includes Sonivox EAS library rebuilt without spurious references to
libutils and libcutils. The ARM versions have been build with the
assembler source files.

## Load DLS soundfont files
I have attempted to add code to load DLS soundfont files, which I have
not been able to get working.  The code is in the [DLS
branch](https://github.com/billthefarmer/mididriver/tree/DLS) and the
relevent issue is [Costum soundfont is
possible?](https://github.com/billthefarmer/mididriver/issues/21).

If anyone would like to take this up and attempt to get it working
that would be much appreciated. I know Google know how it works
because they have fixed a bug in it...
```
    Author: Eric Laurent <elaurent@google.com>
    Date:   Thu May 14 09:10:40 2015 -0700

    DLS parser: fix wave pool size check.

    Bug: 21132860.
```

## Bugs
Google has fixed the 64 bit reverb bug:
[#15](https://github.com/billthefarmer/mididriver/issues/15) -
[Fix math functions for 64-bit](https://github.com/android/platform_external_sonivox/commit/d216e303f65f5e36ffba92d342a8c61f753c98d7). The
driver should now work correctly on all 64 bit devices.

The Sonivox library is now built as a static library and linked with
the midi library. This should resolve problems with various android
versions.

## Use
Add the driver to your build.gradle with
```gradle
allprojects {
    repositories {
        maven { url "https://jitpack.io" }
    }
}
```
and:
```gradle
dependencies {
    implementation 'com.github.billthefarmer:mididriver:v1.24'
}
```
### Constructor
The constructor has been made private and `getInstance()` methods
added so it is not possible to have more than one instance in an
app. See the demo app for an example.

### Methods
```java
    static MidiDriver getInstance() // Get instance of driver

    static MidiDriver getInstance(OnMidiStartListener l)

    void start() // Start the driver. Call from onResume().

    void stop()  // Stop the driver. Call from onPause();

    void addOnMidiStartListener(OnMidiStartListener l)

    void queueEvent(byte[]) // Send a midi message. This method now just
                            // calls write()
```
#### Listener
```java
    void onMidiStart() // This is called when the driver has started so
                       // that program change messages, etc, may be sent.
                       // This is now somewhat redundant but has been left in.
```
### Native Methods
```java
    boolean init()  // Return true on success, or false on failure.
	
    int[] config()  // Return a four element array of ints with part of
                    // the EAS config:

        config[0] = pLibConfig->maxVoices;
        config[1] = pLibConfig->numChannels;
        config[2] = pLibConfig->sampleRate;
        config[3] = pLibConfig->mixBufferSize;

    boolean write(byte buffer[]) // Writes midi data to the Sonivox
                                 // synthesizer. The length of the array
                                 // should be the exact length of the
                                 // message or messages. Returns true
                                 // on success, false on
                                 // failure.

    boolean setVolume(int volume) // Set master volume for EAS
                                  // synthesizer (between 0 and 100).
                                  // Returns true on success, false on
                                  // failure.
    boolean setReverb(int preset) // Set Reverb effect preset for EAS
                                  // synthesizer (value from
                                  // ReverbConstants). Returns true
                                  // on success, false on failure.

    boolean shutdown() // Shut down the synthesizer. Returns true on
                       // success, false on failure.
```
### C/C++ Native Functions
```c++
#include "midi.h"

    jboolean midi_init()  // Return true on success, or false on failure.
    jboolean midi_write(EAS_U8 *bytes, jint length)
                                 // Writes midi data to the Sonivox
                                 // synthesizer. The length of the array
                                 // should be the exact length of the
                                 // message or messages. Returns true
                                 // on success, false on
                                 // failure.
    jboolean midi_setVolume(jint volume)
                                  // Set master volume for EAS
                                  // synthesizer (between 0 and 100).
                                  // Returns true on success, false on
                                  // failure.
    jboolean midi_setReverb(jint preset)
                                  // Set Reverb effect preset for EAS
                                  // synthesizer. Preset could be:
                                  //   -1: turn reverb off
                                  //    0: large hall
                                  //    1: hall
                                  //    2: chamber
                                  //    3: room.
                                  // Returns true on success, false on
                                  // failure.
    jboolean midi_shutdown() // Shut down the synthesizer. Returns true on
                             // success, false on failure.
```
### Native library locations
The location of the native `libmidi.so` libraries for building native
code may be found by doing a search in the app build folder. This
location may change with different versions of the android build
tools.
```shell
$ find ./build -name libmidi.so
./build/intermediates/merged_native_libs/debug/out/lib/arm64-v8a/libmidi.so
./build/intermediates/merged_native_libs/debug/out/lib/armeabi-v7a/libmidi.so
./build/intermediates/merged_native_libs/debug/out/lib/x86/libmidi.so
./build/intermediates/merged_native_libs/debug/out/lib/x86_64/libmidi.so
./build/intermediates/merged_native_libs/release/out/lib/arm64-v8a/libmidi.so
./build/intermediates/merged_native_libs/release/out/lib/armeabi-v7a/libmidi.so
./build/intermediates/merged_native_libs/release/out/lib/x86/libmidi.so
./build/intermediates/merged_native_libs/release/out/lib/x86_64/libmidi.so
./build/intermediates/stripped_native_libs/debug/out/lib/arm64-v8a/libmidi.so
./build/intermediates/stripped_native_libs/debug/out/lib/armeabi-v7a/libmidi.so
./build/intermediates/stripped_native_libs/debug/out/lib/x86/libmidi.so
./build/intermediates/stripped_native_libs/debug/out/lib/x86_64/libmidi.so
./build/intermediates/stripped_native_libs/release/out/lib/arm64-v8a/libmidi.so
./build/intermediates/stripped_native_libs/release/out/lib/armeabi-v7a/libmidi.so
./build/intermediates/stripped_native_libs/release/out/lib/x86/libmidi.so
./build/intermediates/stripped_native_libs/release/out/lib/x86_64/libmidi.so
```
