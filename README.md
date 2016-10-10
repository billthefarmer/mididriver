# Midi Driver [![Build Status](https://travis-ci.org/billthefarmer/mididriver.svg?branch=master)](https://travis-ci.org/billthefarmer/mididriver)

Android midi driver using Sonivox EAS library. The app and library are
available [here](https://github.com/billthefarmer/mididriver/releases).

Licensed under GPL or Apache license - your choice.

Includes Sonivox EAS library rebuilt without spurious references to
libutils and libcutils. The ARM versions have been build without the
assembler source files as the NDK refuses to assemble one of them.

Google have fixed the 64 bit reverb bug:
[#15](https://github.com/billthefarmer/mididriver/issues/15) -
[Fix math functions for 64-bit](https://github.com/android/platform_external_sonivox/commit/d216e303f65f5e36ffba92d342a8c61f753c98d7). The
driver now works correctly on 64 bit devices.

[smarek](https://github.com/smarek) has made a
[fork](https://github.com/smarek/mididriver) of the latest master,
merging some of the features from the jhindin branch, and laid out for
Gradle. I have merged this into the
[smarek](https://github.com/billthefarmer/mididriver/tree/smarek)
branch.

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
