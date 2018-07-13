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
//  Joseph Hindin hindin [at] jhindin [dot] com
//
///////////////////////////////////////////////////////////////////////////////

package org.billthefarmer.mididriver;

/**
 * Midi event type constants
 */
public class MidiConstants
{
    public static final byte MIDI_TIME_CODE          = (byte)0xF1;
    public static final byte SONG_POSITION_POBYTEER  = (byte)0xF2;
    public static final byte SONG_SELECT             = (byte)0xF3;
    public static final byte TUNE_REQUEST            = (byte)0xF6;
    public static final byte END_OF_EXCLUSIVE        = (byte)0xF7;
    public static final byte TIMING_CLOCK            = (byte)0xF8;
    public static final byte START                   = (byte)0xFA;
    public static final byte CONTINUE                = (byte)0xFB;
    public static final byte STOP                    = (byte)0xFC;
    public static final byte ACTIVE_SENSING          = (byte)0xFE;
    public static final byte SYSTEM_RESET            = (byte)0xFF;
    public static final byte NOTE_OFF                = (byte)0x80;
    public static final byte NOTE_ON                 = (byte)0x90;
    public static final byte POLY_PRESSURE           = (byte)0xA0;
    public static final byte CONTROL_CHANGE          = (byte)0xB0;
    public static final byte PROGRAM_CHANGE          = (byte)0xC0;
    public static final byte CHANNEL_PRESSURE        = (byte)0xD0;
    public static final byte PITCH_BEND              = (byte)0xE0;
}
