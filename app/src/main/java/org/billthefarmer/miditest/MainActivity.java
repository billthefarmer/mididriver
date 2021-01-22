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

package org.billthefarmer.miditest;

import android.app.Activity;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;

import org.billthefarmer.mididriver.MidiDriver;
import org.billthefarmer.mididriver.MidiConstants;
import org.billthefarmer.mididriver.GeneralMidiConstants;
import org.billthefarmer.mididriver.ReverbConstants;

import java.util.Locale;

public class MainActivity extends Activity
    implements View.OnTouchListener, View.OnClickListener,
               CompoundButton.OnCheckedChangeListener,
               MidiDriver.OnMidiStartListener
{
    private TextView text;

    protected MidiDriver midi;
    protected MediaPlayer player;

    // On create
    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Create midi driver
        midi = new MidiDriver();

        // Set on touch listener
        View v = findViewById(R.id.c);
        if (v != null)
            v.setOnTouchListener(this);

        v = findViewById(R.id.g);
        if (v != null)
            v.setOnTouchListener(this);

        v = findViewById(R.id.ants);
        if (v != null)
            v.setOnClickListener(this);

        v = findViewById(R.id.nants);
        if (v != null)
            v.setOnClickListener(this);

        v = findViewById(R.id.reverb);
        if (v != null)
            ((CompoundButton) v).setOnCheckedChangeListener(this);

        text = findViewById(R.id.status);

        // Set on midi start listener
        if (midi != null)
            midi.setOnMidiStartListener(this);
    }

    // On resume
    @Override
    protected void onResume()
    {
        super.onResume();

        // Start midi

        if (midi != null)
            midi.start();
    }

    // On pause
    @Override
    protected void onPause()
    {
        super.onPause();

        // Stop midi
        if (midi != null)
            midi.stop();

        // Stop player
        if (player != null)
            player.stop();
    }

    // On touch
    @Override
    public boolean onTouch(View v, MotionEvent event)
    {
        int action = event.getAction();
        int id = v.getId();

        switch (action) {
            // Down
        case MotionEvent.ACTION_DOWN:
            switch (id) {
            case R.id.c:
                sendMidi(MidiConstants.NOTE_ON, 48, 63);
                sendMidi(MidiConstants.NOTE_ON, 52, 63);
                sendMidi(MidiConstants.NOTE_ON, 55, 63);
                break;

            case R.id.g:
                sendMidi(MidiConstants.NOTE_ON, 55, 63);
                sendMidi(MidiConstants.NOTE_ON, 59, 63);
                sendMidi(MidiConstants.NOTE_ON, 62, 63);
                break;

            default:
                return false;
            }

            v.performClick();
            break;

            // Up
        case MotionEvent.ACTION_UP:
            switch (id) {
            case R.id.c:
                sendMidi(MidiConstants.NOTE_OFF, 48, 0);
                sendMidi(MidiConstants.NOTE_OFF, 52, 0);
                sendMidi(MidiConstants.NOTE_OFF, 55, 0);
                break;

            case R.id.g:
                sendMidi(MidiConstants.NOTE_OFF, 55, 0);
                sendMidi(MidiConstants.NOTE_OFF, 59, 0);
                sendMidi(MidiConstants.NOTE_OFF, 62, 0);
                break;

            default:
                return false;
            }
            break;

        default:
            return false;
        }

        return false;
    }

    // On click
    @Override
    public void onClick(View v)
    {
        int id = v.getId();

        switch (id) {
        case R.id.ants:
            if (player != null) {
                player.stop();
                player.release();
            }

            player = MediaPlayer.create(this, R.raw.ants);
            player.start();
            break;

        case R.id.nants:
            if (player != null)
                player.stop();
            break;
        }
    }

    // onCheckedChanged
    @Override
    public void onCheckedChanged(CompoundButton buttonView, boolean isChecked)
    {
        if (isChecked)
            midi.setReverb(ReverbConstants.CHAMBER);

        else
            midi.setReverb(ReverbConstants.OFF);
    }

    // Listener for sending initial midi messages when the Sonivox
    // synthesizer has been started, such as program change.
    @Override
    public void onMidiStart()
    {
        // Program change - harpsichord
        sendMidi(MidiConstants.PROGRAM_CHANGE,
                 GeneralMidiConstants.HARPSICHORD);

        // Get the config
        int config[] = midi.config();

        String format = getString(R.string.format);
        String info = String.format(Locale.getDefault(), format, config[0],
                                    config[1], config[2], config[3]);

        if (text != null)
            text.setText(info);
    }

    // Send a midi message, 2 bytes
    protected void sendMidi(int m, int n)
    {
        byte msg[] = new byte[2];

        msg[0] = (byte) m;
        msg[1] = (byte) n;

        midi.write(msg);
    }

    // Send a midi message, 3 bytes
    protected void sendMidi(int m, int n, int v)
    {
        byte msg[] = new byte[3];

        msg[0] = (byte) m;
        msg[1] = (byte) n;
        msg[2] = (byte) v;

        midi.write(msg);
    }
}
