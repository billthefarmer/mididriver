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
//  Bill Farmer	 william j farmer [at] yahoo [dot] co [dot] uk.
//
///////////////////////////////////////////////////////////////////////////////

package org.billthefarmer.mididriver;

// MidiDriver

public class MidiDriver
{
    private OnMidiStartListener listener;

    // Constructor

    public MidiDriver()
    {
    }

    // Start midi

    public void start()
    {
	if (init() != true)
	    return;

	// Call listener

	if (listener != null)
	    listener.onMidiStart();
    }

    // Queue event

    public void queueEvent(byte[] event)
    {
	write(event);
    }

    // Stop

    public void stop()
    {
	shutdown();
    }

    // Set listener

    public void setOnMidiStartListener(OnMidiStartListener l)
    {
	listener = l;
    }

    // Listener interface

    public interface OnMidiStartListener
    {
	public abstract void onMidiStart();
    }

    // Native midi methods

    private native boolean init();
    public  native int[]   config();
    private native boolean write(byte a[]);
    private native boolean shutdown();

    // Load midi library

    static
    {
	System.loadLibrary("midi");
    }
}
