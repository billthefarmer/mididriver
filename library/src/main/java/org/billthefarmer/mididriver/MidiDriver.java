////////////////////////////////////////////////////////////////////////////////
//
//  MidiDriver - An Android Midi Driver.
//
//  Copyright (C) 2013	Bill Farmer
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

/**
 * MidiDriver class
 */
public class MidiDriver
{
    private OnMidiStartListener listener;

    /**
     * Class constructor
     */
    public MidiDriver()
    {
    }

    /**
     * Start midi driver
     */
    public void start()
    {
        if (init() != true)
            return;

        // Call listener

        if (listener != null)
            listener.onMidiStart();
    }

    /**
     * Queue midi event or events
     *
     * @param byte array of midi events
     */
    public void queueEvent(byte[] event)
    {
        write(event);
    }

    /**
     * Stop midi driver
     */
    public void stop()
    {
        shutdown();
    }

    /**
     * Set midi driver start listener
     *
     * @param OnMidiStartListener
     */
    public void setOnMidiStartListener(OnMidiStartListener l)
    {
        listener = l;
    }

    /**
     * Midi start listener interface
     */
    public interface OnMidiStartListener
    {
        public abstract void onMidiStart();
    }

    // Native midi methods

    /**
     * Initialise native code
     *
     * @return true for success
     */
    private native boolean init();

    /**
     * Returm part of EAS config
     *
     * @return Int array of part of EAS config
     *   config[0] = pLibConfig->maxVoices;
     *   config[1] = pLibConfig->numChannels;
     *   config[2] = pLibConfig->sampleRate;
     *   config[3] = pLibConfig->mixBufferSize;
     */
    public  native int[]   config();

    /**
     * Write midi event or events
     *
     * @param byte array of midi events
     */
    public  native boolean write(byte a[]);

    /**
     * Set master volume
     * @param volume master volume for EAS synthesizer (between 0 and 100)
     * @return true for success
     */
    public  native boolean setVolume(int volume);

    /**
     * Shut down native code
     *
     * @return true for success
     */
    private native boolean shutdown();

    // Load midi library
    static
    {
        System.loadLibrary("midi");
    }
}
