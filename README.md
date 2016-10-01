# Midi Driver [![Build Status](https://travis-ci.org/billthefarmer/mididriver.svg?branch=master)](https://travis-ci.org/billthefarmer/mididriver)

Android midi driver using Sonivox EAS library. The app and library are
available [here](https://github.com/billthefarmer/mididriver/releases).

Licensed under GPL or Apache license - your choice.

Note that this driver will generate a wrning dialog on Android 7
Nougat and above. The libsonivox library is no longer directly
accessible. I have included copies of the library in the driver, but
this ganerates problems due to use of libutils and libcutils by
libsonivox.

Do not use or build 64 bit versions of this native library as Google
have included broken 64 bit versions of libsonivox.so in devices and
at least one emulator. Do not include other 64 bit native libraries in
your app as this appears not to work. See
[#15](https://github.com/billthefarmer/mididriver/issues/15),
[#14](https://github.com/billthefarmer/mididriver/issues/14),
[#11](https://github.com/billthefarmer/mididriver/issues/11).

[smarek](https://github.com/smarek) has made a
[fork](https://github.com/smarek/mididriver) of the latest master,
merging some of the features from the jhindin branch, and laid out for
Gradle. I have merged this into the
[smarek](https://github.com/billthefarmer/mididriver/tree/smarek)
branch.

The
[OpenSLES](https://github.com/billthefarmer/mididriver/tree/OpenSLES)
branch contains a purely native version of the driver using
OpenSLES. This is an attempt to resolve the
[choppy sound issue](https://github.com/billthefarmer/mididriver/issues/12)
reported by [smarek](https://github.com/smarek). Now merged to master.

I have added a
[jhindin](https://github.com/billthefarmer/mididriver/tree/jhindin)
branch to this repository which is a merge of
[jhindin/mididriver](https://github.com/jhindin/mididriver). Do not
use this branch as it is not up to date. He has made these changes:

  1. The source file tree has been rearranged to fit Android
     Studio/Gradle integration structure
  2. Linking with SoniVox library deferred to the launch time - in
     this way, there is no need to pull sonivox libraries from all
     potential platforms just to build the system

I have removed the event queue as per the comment by akdmia because it
is no longer necessary. Contention is handled by using a mutex in the
native code. I have only tested this with this app, not with an
extended event stream.  The app just has a couple of buttons that play
a couple of piano notes when touched. I've added two more buttons that
play and stop a midi file using the MediaPlayer to check that there is
no interaction problem. Added a listener for sending initial midi
messages when the midi driver has started. Removed the handler for the
listener as the callback is not on a driver thread. This project may
be compiled as a library by changing the setting in project.properties
to android.library=true.  The MainActivity.java source file will be
moved to a temporary folder so it is not compiled into in the library
jar file. I have made the changes in
[jhindin/mididriver](https://github.com/jhindin/mididriver) without
changing the project structure. Copies of the libsonivox.so library
are no longer required.

To use this driver you need to:

  * Get and install the Android NDK toolkit.
  * Get the documentation and include files from
    [android/platform_external_sonivox](https://github.com/android/platform_external_sonivox).
  * Adjust the ~jni/Application.mk to build the architecture(s) you
    want.
  * Don't build 64 bit versions as Google have included broken 64 bit
    versions of libsonivox.so in devices and at least one emulator.

Or you can just copy the libmidi.so file from here, but it has

    Java_org_billthefarmer_mididriver_MidiDriver

embedded in the native method names.

The windows NDK toolkit doesn't appear to use anything other than
standard windows shell commands and it's own tools, so you don't need
Cygwin, despite what the docs say.

### Methods

	void start()  Start the driver. Call from onResume().

	void stop()  Stop the driver. Call from onPause();

	void addOnMidiStartListener(OnMidiStartListener l);

	void queueEvent(byte[]) Send a midi message. This method now just
    calls write()

#### Listener

	void onMidiStart() This is called when the driver has started so
    that program change messages, etc may be sent. This is now
    somewhat redundant but has been left in.

### Native Methods

	boolean init()  Return true on success, or false on failure.
	
	int[] config()  Return a four element array of ints with part of
	the EAS onfig:

      config[0] = pLibConfig->maxVoices;
      config[1] = pLibConfig->numChannels;
      config[2] = pLibConfig->sampleRate;
      config[3] = pLibConfig->mixBufferSize;

	boolean write(byte buffer[])  Writes midi data to the Sonivox
	synthesizer. The length of the array should be the exact length of
	the message or messages. Returns true on success, false on
	failure.

	boolean shutdown()  Shut down the synthesizer. Returns true on
	success, false on failure.
