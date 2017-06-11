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

package org.billthefarmer.miditest;

import android.app.Activity;
import android.content.res.Resources;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.Menu;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;

import java.util.Locale;

import org.billthefarmer.mididriver.MidiDriver;

public class MainActivity extends Activity
    implements View.OnTouchListener, View.OnClickListener,
	       MidiDriver.OnMidiStartListener
{
    private TextView text;

    protected MidiDriver midi;
    protected MediaPlayer player;

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

	text = (TextView)findViewById(R.id.status);

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

	switch (action)
	{
	    // Down

	case MotionEvent.ACTION_DOWN:
	    switch (id)
	    {
	    case R.id.c:
		sendMidi(0x90, 48, 63);
		sendMidi(0x90, 52, 63);
		sendMidi(0x90, 55, 63);
		break;

	    case R.id.g:
		sendMidi(0x90, 55, 63);
		sendMidi(0x90, 59, 63);
		sendMidi(0x90, 62, 63);
		break;

	    default:
		return false;
	    }

	    v.performClick();
	    break;

	    // Up

	case MotionEvent.ACTION_UP:
	    switch (id)
	    {
	    case R.id.c:
		sendMidi(0x80, 48, 0);
		sendMidi(0x80, 52, 0);
		sendMidi(0x80, 55, 0);
		break;

	    case R.id.g:
		sendMidi(0x80, 55, 0);
		sendMidi(0x80, 59, 0);
		sendMidi(0x80, 62, 0);
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

	switch (id)
	{
	case R.id.ants:
	    if (player != null)
	    {
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

    // Listener for sending initial midi messages when the Sonivox
    // synthesizer has been started, such as program change.

    @Override
    public void onMidiStart()
    {
	// Program change - harpsicord

	sendMidi(0xc0, 6);

	// Get the config

	int config[] = midi.config();

	Resources resources = getResources();

	String format = resources.getString(R.string.format);
	String info = String.format(Locale.getDefault(), format, config[0],
				    config[1], config[2], config[3]);

	if (text != null)
	    text.setText(info);
    }

    // Send a midi message

    protected void sendMidi(int m, int p)
    {
	byte msg[] = new byte[2];

	msg[0] = (byte) m;
	msg[1] = (byte) p;

	midi.write(msg);
    }

    // Send a midi message

    protected void sendMidi(int m, int n, int v)
    {
	byte msg[] = new byte[3];

	msg[0] = (byte) m;
	msg[1] = (byte) n;
	msg[2] = (byte) v;

	midi.write(msg);
    }
}
