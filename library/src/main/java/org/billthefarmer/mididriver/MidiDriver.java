////////////////////////////////////////////////////////////////////////////////
//
//  MidiDriver - An Android Midi Driver.
//
//  Copyright (C) 2013	Bill Farmer
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
//  Bill Farmer	 william j farmer [at] yahoo [dot] co [dot] uk.
//
///////////////////////////////////////////////////////////////////////////////

package org.billthefarmer.mididriver;

/** MidiDriver class */
public final class MidiDriver {

    /** Midi start listener */
    private static OnMidiStartListener listener;

    /** Instance */
    private static MidiDriver instance;

    /** Class constructor */
    private MidiDriver() {}

    /** Get instance */
    public static MidiDriver getInstance() {
        if (instance == null) {
            instance = new MidiDriver();
        }
        return instance;
    }

    /** Get instance */
    public static MidiDriver getInstance(OnMidiStartListener l) {
        MidiDriver instance = getInstance();
        listener = l;
        return instance;
    }

    /** Start midi driver */
    public void start() {
        if (!nativeInit()) {
            return;
        }
        // Call listener
        if (listener != null) {
            listener.onMidiStart();
         }
    }

    /**
     * Queue midi event or events
     *
     * @param byte array of midi events
     */
    public void queueEvent(byte[] event) {
        write(event);
    }

    /** Stop midi driver */
    public void stop() {
        nativeShutdown();
    }

    /**
     * Set midi driver start listener
     *
     * @param OnMidiStartListener
     */
    public void setOnMidiStartListener(OnMidiStartListener l) {
        listener = l;
    }

    /** Midi start listener interface */
    public interface OnMidiStartListener {
        void onMidiStart();
    }

    /**
     * Returm part of EAS config
     *
     * @return Int array of part of EAS config config[0] = pLibConfig->maxVoices; config[1] =
     *     pLibConfig->numChannels; config[2] = pLibConfig->sampleRate; config[3] =
     *     pLibConfig->mixBufferSize;
     */
    public int[] config() {
        return nativeConfig();
    }

    /**
     * Write midi event or events
     *
     * @param byte array of midi events
     */
    public boolean write(byte[] a) {
        return nativeWrite(a, 0, a.length);
    }

    /**
     * Write midi event or events
     *
     * @param byte array of midi events
     */
    public boolean write(byte[] a, int off, int len) {
        if ((off | len) < 0 || off > a.length - len) {
            throw new ArrayIndexOutOfBoundsException();
        }
        return nativeWrite(a, off, len);
    }

    /**
     * Set master volume
     *
     * @param volume master volume for EAS synthesizer (between 0 and 100)
     * @return true for success
     */
    public boolean setVolume(int volume) {
        return nativeSetVolume(volume);
    }

    /**
     * Set EAS module parameter
     *
     * @param preset reverb preset to use (value from ReverbConstants)
     * @return true for success
     */
    public boolean setReverb(int preset) {
        return nativeSetReverb(preset);
    }

    /**
     * Shut down midi driver
     *
     * @return true for success
     */
    public boolean shutdown() {
        return nativeShutdown();
    }

    /**
     * Load DLS soundbank from memory
     *
     * @param byte array of DLS file data
     */
    public boolean loadDLS(byte[] a) {
        return nativeLoadDLS(a);
    }

    /**
     * Initialise native code
     *
     * @return true for success
     */
    private static native boolean nativeInit();

    private static native int[] nativeConfig();

    private static native boolean nativeWrite(byte[] a, int off, int len);

    private static native boolean nativeSetVolume(int volume);

    private static native boolean nativeSetReverb(int preset);

    private static native boolean nativeShutdown();

    private static native boolean nativeLoadDLS(byte[] a);

    // Load midi library
    static {
        System.loadLibrary("midi");
    }

}
