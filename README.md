Midi Driver
===========

Android midi driver using Sonivox EAS library. The app and library are
available [here](https://github.com/billthefarmer/mididriver/releases).

I have added an event queue as per the comment by akdmia. I have only
tested this with this app, not with an extended event stream.  The app
just has a couple of buttons that play a couple of piano notes when
touched. I've added two more buttons that play and stop a midi file
using the MediaPlayer to check that there is no interaction
problem. Added a listener for sending initial midi messages when the
midi driver has started. This project may be compiled as a library by
changing the setting in project.properties to android.library=true.
The MainActivity.java source file will be moved to a temporary folder
so it is not compiled into in the library jar file.

To use this driver you need to:

  * Get and install the Android NDK toolkit.
  * Get the documentation and include files from
    https://github.com/android/platform_external_sonivox.
  * Get a copy of the libsonivox.so library from an Android device or
    an emulator (in system/lib) and put it in the ~/jni/libs/<arch>
    folder.
  * Adjust the ~jni/Application.mk to build the architecture(s) you
    want.

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

	void queueEvent(byte[]) Send a midi message

### Native Methods

	int init()  Return buffer size in shorts, or 0 on failure.
	
	int[] config()  Return a four element array of ints with part of
	the EAS onfig:

      config[0] = pLibConfig->maxVoices;
      config[1] = pLibConfig->numChannels;
      config[2] = pLibConfig->sampleRate;
      config[3] = pLibConfig->mixBufferSize;

	int render(short buffer[]) Renders a buffer's worth of audio from
	the Sonivox synthesizer into the buffer provided. The buffer
	should be an array of shorts of the size returned by init(). Returns
	0 on failure, number of shorts rendered on success.

	boolean write(byte buffer[])  Writes midi data to the Sonivox
	synthesizer. The length of the array should be the exact length of
	the message or messages. Returns true on success, false on
	failure.

	boolean shutdown()  Shut down the synthesizer. Returns true on
	success, false on failure.
