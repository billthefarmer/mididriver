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

import java.util.ArrayList;
import java.util.List;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;

import android.os.Handler;
import android.os.Message;

// MidiDriver

public class MidiDriver
        implements Handler.Callback {
    private static final int SAMPLE_RATE = 22050;
    private static final int BUFFER_SIZE = 4096;

    private Handler handler;
    private AudioTrack audioTrack;
    private List<byte[]> queuedEvents;
    private DriverThread driverThread;
    private OnMidiStartListener listener;

    private short buffer[];

    private boolean running;

    // Constructor

    public MidiDriver() {
        queuedEvents = new ArrayList<byte[]>();
        handler = new Handler(this);

    }

    // Start midi

    public void start() {
        driverThread = new DriverThread(this);
        driverThread.start();
    }

    // Queue event

    public void queueEvent(byte[] event) {
        synchronized (this) {
            queuedEvents.add(event);
        }
    }

    // Stop

    public void stop() {
        synchronized (this) {
            running = false;
        }
    }

    // Handle message

    public boolean handleMessage(Message msg) {
        // Call listener

        if (listener != null)
            listener.onMidiStart();

        return true;
    }

    // Set listener

    public void setOnMidiStartListener(OnMidiStartListener l) {
        listener = l;
    }

    private class DriverThread extends Thread {
        private static final String TAG = "DriverThread";

        private MidiDriver driver;

        // Constructor

        public DriverThread(MidiDriver d) {
            super(TAG);

            driver = d;
        }

        // Run

        @Override
        public void run() {
            processMidi();
        }

        synchronized  boolean  shouldRun() {
            return running || !queuedEvents.isEmpty();
        }

        // Process MidiDriver

        private void processMidi() {
            int status = 0;
            int size = 0;

            // Init midi

            if ((size = init()) == 0)
                return;

            buffer = new short[size];

            // Create audio track

            audioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, SAMPLE_RATE,
                    AudioFormat.CHANNEL_OUT_STEREO,
                    AudioFormat.ENCODING_PCM_16BIT,
                    BUFFER_SIZE, AudioTrack.MODE_STREAM);
            // Check audiotrack

            if (audioTrack == null) {
                shutdown();
                return;
            }

            // Check state

            int state = audioTrack.getState();

            if (state != AudioTrack.STATE_INITIALIZED) {
                audioTrack.release();
                shutdown();
                return;
            }

            // Send message

            handler.sendEmptyMessage(0);

            // Play track

            audioTrack.play();

            // Keep running until stopped

            running = true;
            while (shouldRun()) {
                // Write the midi events

                synchronized (driver) {
                    for (byte[] queuedEvent : queuedEvents)
                        write(queuedEvent);

                    queuedEvents.clear();
                }

                // Render the audio

                if (render(buffer) == 0)
                    break;

                // Write audio to audiotrack

                status = audioTrack.write(buffer, 0, buffer.length);

                if (status < 0)
                    break;
            }

            // Render and write the last bit of audio

            if (status > 0)
                if (render(buffer) > 0)
                    audioTrack.write(buffer, 0, buffer.length);

            // Shut down audio

            shutdown();
            audioTrack.release();
        }
    }

    // Listener interface

    public interface OnMidiStartListener {
        public abstract void onMidiStart();
    }

    // Native midi methods

    private native int init();

    public native int[] config();

    private native int render(short a[]);

    private native boolean write(byte a[]);

    private native boolean shutdown();

    // Load midi library

    static {
        System.loadLibrary("midi");
    }
}
