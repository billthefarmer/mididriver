Midi Driver
===========

Android midi driver using Sonivox EAS library

The app just has a couple of buttons that play a couple of piano notes when touched.

To use this driver you need to:

  * Get and install the Android NDK toolkit.
  * Get the documentation and include files from https://github.com/android/platform_external_sonivox.
  * Get a copy of the libsonivox.so library from an Android device or an emulator (in system/lib) and put it with the other libraries in the platforms/android-14/arch-arm/usr/lib folder of the NDK.
  * Add it to your ~/jni/Android.mk file along with any other libraries.

Or you can just copy the libmidi.so file from here, but it has

    Java_org_billthefarmer_mididriver_MidiDriver

embedded in the native method names.

The NDK toolkit doesn't appear to use anything other than standard windows shell commands and it's own tools, so you don't need Cygwin, despite what the docs say.

# Native Methods

	int init()  Return buffer size in shorts, or 0 on failure.
	
	int[] config() Return a four element array of ints with part of
	the the EAS onfig:

      config[0] = pLibConfig->maxVoices;
      config[1] = pLibConfig->numChannels;
      config[2] = pLibConfig->sampleRate;
      config[3] = pLibConfig->mixBufferSize;

	int render(short buffer[]) Renders a buffer's worth of audio from
	the Sonivox synthesizer. The buffer provided should be an array of
	shorts of the size returned by init().

	boolean write(byte buffer[]) Writes midi data to the Sonivox
	Synthesizer. The length of the array should be the exact length of
	the message or messages. It seems to ignore three byte program
	change messages, for example. Returns true on success, false on
	faulure.

	boolean shutdown() Shut down the synthesizer. Returns true on
	success, false on faulure.
