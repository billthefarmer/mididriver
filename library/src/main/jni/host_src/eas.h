/*----------------------------------------------------------------------------
 *
 * File:
 * eas.h
 *
 * Contents and purpose:
 * The public interface header for the EAS synthesizer.
 *
 * This header only contains declarations that are specific
 * to this implementation.
 *
 * DO NOT MODIFY THIS FILE!
 *
 * Copyright Sonic Network Inc. 2005, 2006

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *----------------------------------------------------------------------------
 * Revision Control:
 *   $Revision: 852 $
 *   $Date: 2007-09-04 11:43:49 -0700 (Tue, 04 Sep 2007) $
 *----------------------------------------------------------------------------
*/

#ifndef _EAS_H
#define _EAS_H

#include "eas_types.h"

/* for C++ linkage */
#ifdef __cplusplus
extern "C" {
#endif

/* library version macro */
#define MAKE_LIB_VERSION(a,b,c,d) (((((((EAS_U32) a <<8) | (EAS_U32) b) << 8) | (EAS_U32) c) << 8) | (EAS_U32) d)
#define LIB_VERSION MAKE_LIB_VERSION(3, 6, 10, 14)

typedef struct
{
    EAS_U32     libVersion;
    EAS_BOOL    checkedVersion;
    EAS_I32     maxVoices;
    EAS_I32     numChannels;
    EAS_I32     sampleRate;
    EAS_I32     mixBufferSize;
    EAS_BOOL    filterEnabled;
    EAS_U32     buildTimeStamp;
    EAS_CHAR    *buildGUID;
} S_EAS_LIB_CONFIG;

/* enumerated effects module numbers for configuration */
typedef enum
{
    EAS_MODULE_ENHANCER = 0,
    EAS_MODULE_COMPRESSOR,
    EAS_MODULE_REVERB,
    EAS_MODULE_CHORUS,
    EAS_MODULE_WIDENER,
    EAS_MODULE_GRAPHIC_EQ,
    EAS_MODULE_WOW,
    EAS_MODULE_MAXIMIZER,
    EAS_MODULE_TONECONTROLEQ,
    NUM_EFFECTS_MODULES
} E_FX_MODULES;

/* enumerated optional module numbers for configuration */
typedef enum
{
    EAS_MODULE_MMAPI_TONE_CONTROL = 0,
    EAS_MODULE_METRICS
} E_OPT_MODULES;
#define NUM_OPTIONAL_MODULES    2

/* enumerated audio decoders for configuration */
typedef enum
{
    EAS_DECODER_PCM = 0,
    EAS_DECODER_SMAF_ADPCM,
    EAS_DECODER_IMA_ADPCM,
    EAS_DECODER_7BIT_SMAF_ADPCM,
    EAS_DECODER_NOT_SUPPORTED
} E_DECODER_MODULES;
#define NUM_DECODER_MODULES     4

/* defines for EAS_PEOpenStream flags parameter */
#define PCM_FLAGS_STEREO        0x00000100  /* stream is stereo */
#define PCM_FLAGS_8_BIT         0x00000001  /* 8-bit format */
#define PCM_FLAGS_UNSIGNED      0x00000010  /* unsigned format */
#define PCM_FLAGS_STREAMING     0x80000000  /* streaming mode */

/* maximum volume setting */
#define EAS_MAX_VOLUME          100

/*----------------------------------------------------------------------------
 * EAS_Init()
 *----------------------------------------------------------------------------
 * Purpose:
 * Initialize the synthesizer library
 *
 * Inputs:
 *  polyphony       - number of voices to play (dynamic memory model only)
 *  ppLibData       - pointer to data handle variable for this instance
 *
 * Outputs:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Init (EAS_DATA_HANDLE *ppEASData);

/*----------------------------------------------------------------------------
 * EAS_Config()
 *----------------------------------------------------------------------------
 * Purpose:
 * Returns a pointer to a structure containing the configuration options
 * in this library build.
 *
 * Inputs:
 *
 * Outputs:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC const S_EAS_LIB_CONFIG *EAS_Config (void);

/*----------------------------------------------------------------------------
 * EAS_Shutdown()
 *----------------------------------------------------------------------------
 * Purpose:
 * Shuts down the library. Deallocates any memory associated with the
 * synthesizer (dynamic memory model only)
 *
 * Inputs:
 *  pEASData        - handle to data for this instance
 *
 * Outputs:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Shutdown (EAS_DATA_HANDLE pEASData);

/*----------------------------------------------------------------------------
 * EAS_Render()
 *----------------------------------------------------------------------------
 * Purpose:
 * Parse the Midi data and render PCM audio data.
 *
 * Inputs:
 *  pEASData        - buffer for internal EAS data
 *  pOut            - output buffer pointer
 *  nNumRequested   - requested num samples to generate
 *  pnNumGenerated  - actual number of samples generated
 *
 * Outputs:
 *  EAS_SUCCESS if PCM data was successfully rendered
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Render (EAS_DATA_HANDLE pEASData, EAS_PCM *pOut, EAS_I32 numRequested, EAS_I32 *pNumGenerated);

/*----------------------------------------------------------------------------
 * EAS_SetTransposition)
 *----------------------------------------------------------------------------
 * Purpose:
 * Sets the key tranposition for the synthesizer. Transposes all
 * melodic instruments by the specified amount. Range is limited
 * to +/-12 semitones.
 *
 * Inputs:
 *  pEASData        - handle to data for this instance
 *  streamHandle    - handle to stream
 *  transposition   - +/-12 semitones
 *
 * Outputs:
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_SetTransposition (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 transposition);
#define MAX_TRANSPOSE       12


/*----------------------------------------------------------------------------
 * EAS_SetVolume()
 *----------------------------------------------------------------------------
 * Purpose:
 * Set the master volume for the mixer. The default volume setting is
 * 90 (-10 dB). The volume range is 0 to 100 in 1dB increments.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * volume           - the desired master volume
 *
 * Outputs:
 *
 *
 * Side Effects:
 * overrides any previously set master volume from sysex
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_SetVolume (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 volume);

/*----------------------------------------------------------------------------
 * EAS_OpenFile()
 *----------------------------------------------------------------------------
 * Purpose:
 * Opens a file for audio playback.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * locator          - pointer to filename or other locating information
 * pStreamHandle    - pointer to stream handle variable
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_OpenFile (EAS_DATA_HANDLE pEASData, EAS_FILE_LOCATOR locator, EAS_HANDLE *pStreamHandle);

#ifdef MMAPI_SUPPORT
/*----------------------------------------------------------------------------
 * EAS_MMAPIToneControl()
 *----------------------------------------------------------------------------
 * Purpose:
 * Opens a ToneControl file for audio playback.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * locator          - pointer to filename or other locating information
 * pStreamHandle    - pointer to stream handle variable
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_MMAPIToneControl (EAS_DATA_HANDLE pEASData, EAS_FILE_LOCATOR locator, EAS_HANDLE *pStreamHandle);

/*----------------------------------------------------------------------------
 * EAS_GetWaveFmtChunk
 *----------------------------------------------------------------------------
 * Helper function to retrieve WAVE file fmt chunk for MMAPI
 *----------------------------------------------------------------------------
 * pEASData         - pointer to EAS persistent data object
 * streamHandle     - stream handle
 * pFmtChunk        - pointer to pointer to FMT chunk data
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_GetWaveFmtChunk (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_VOID_PTR *ppFmtChunk);
#endif

/*----------------------------------------------------------------------------
 * EAS_GetFileType
 *----------------------------------------------------------------------------
 * Returns the file type (see eas_types.h for enumerations)
 *----------------------------------------------------------------------------
 * pEASData         - pointer to EAS persistent data object
 * streamHandle     - stream handle
 * pFileType        - pointer to variable to receive file type
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_GetFileType (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 *pFileType);

/*----------------------------------------------------------------------------
 * EAS_ParseMetaData()
 *----------------------------------------------------------------------------
 * Purpose:
 *
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 * playLength       - pointer to variable to store the play length (in msecs)
 *
 * Outputs:
 *
 *
 * Side Effects:
 *                  - resets the parser to the start of the file
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_ParseMetaData (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 *pPlayLength);

/*----------------------------------------------------------------------------
 * EAS_Prepare()
 *----------------------------------------------------------------------------
 * Purpose:
 * Prepares the synthesizer to play the file or stream. Parses the first
 * frame of data from the file and arms the synthesizer.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Prepare (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_State()
 *----------------------------------------------------------------------------
 * Purpose:
 * Returns the state of an audio file or stream.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_State (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_STATE *pState);

/*----------------------------------------------------------------------------
 * EAS_RegisterMetaDataCallback()
 *----------------------------------------------------------------------------
 * Purpose:
 * Registers a metadata callback function for parsed metadata. To
 * de-register the callback, call this function again with parameter
 * cbFunc set to NULL.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 * cbFunc           - pointer to host callback function
 * metaDataBuffer   - pointer to metadata buffer
 * metaDataBufSize  - maximum size of the metadata buffer
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_RegisterMetaDataCallback (
    EAS_DATA_HANDLE pEASData,
    EAS_HANDLE streamHandle,
    EAS_METADATA_CBFUNC cbFunc,
    char *metaDataBuffer,
    EAS_I32 metaDataBufSize,
    EAS_VOID_PTR pUserData);

/*----------------------------------------------------------------------------
 * EAS_GetNoteCount ()
 *----------------------------------------------------------------------------
 * Returns the total number of notes played in this stream
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 * pNoteCount       - pointer to variable to receive note count
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_GetNoteCount (EAS_DATA_HANDLE pEASData, EAS_HANDLE pStream, EAS_I32 *pNoteCount);

/*----------------------------------------------------------------------------
 * EAS_CloseFile()
 *----------------------------------------------------------------------------
 * Purpose:
 * Closes an audio file or stream. Playback should have either paused or
 * completed (EAS_State returns EAS_PAUSED or EAS_STOPPED).
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_CloseFile (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_OpenMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Opens a raw MIDI stream allowing the host to route MIDI cable data directly to the synthesizer
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * pStreamHandle    - pointer to variable to hold file or stream handle
 * streamHandle     - open stream or NULL for new synthesizer instance
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_OpenMIDIStream (EAS_DATA_HANDLE pEASData, EAS_HANDLE *pStreamHandle, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_WriteMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Send data to the MIDI stream device
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - stream handle
 * pBuffer          - pointer to buffer
 * count            - number of bytes to write
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_WriteMIDIStream(EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_U8 *pBuffer, EAS_I32 count);

/*----------------------------------------------------------------------------
 * EAS_CloseMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Closes a raw MIDI stream
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_CloseMIDIStream (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_Locate()
 *----------------------------------------------------------------------------
 * Purpose:
 * Locate into the file associated with the handle.
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file handle
 * milliseconds     - playback offset from start of file in milliseconds
 *
 * Outputs:
 *
 *
 * Side Effects:
 * the actual offset will be quantized to the closest update period, typically
 * a resolution of 5.9ms. Notes that are started prior to this time will not
 * sound. Any notes currently playing will be shut off.
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Locate (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 milliseconds, EAS_BOOL offset);

/*----------------------------------------------------------------------------
 * EAS_GetLocation()
 *----------------------------------------------------------------------------
 * Purpose:
 * Returns the current playback offset
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * streamHandle     - file handle
 *
 * Outputs:
 * The offset in milliseconds from the start of the current sequence, quantized
 * to the nearest update period. Actual resolution is typically 5.9 ms.
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_GetLocation (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_I32 *pTime);

/*----------------------------------------------------------------------------
 * EAS_Pause()
 *----------------------------------------------------------------------------
 * Purpose:
 * Pauses the playback of the data associated with this handle. The audio
 * is gracefully ramped down to prevent clicks and pops. It may take several
 * buffers of audio before the audio is muted.
 *
 * Inputs:
 * psEASData        - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Pause (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_Resume()
 *----------------------------------------------------------------------------
 * Purpose:
 * Resumes the playback of the data associated with this handle. The audio
 * is gracefully ramped up to prevent clicks and pops.
 *
 * Inputs:
 * psEASData        - pointer to overall EAS data structure
 * streamHandle     - file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_Resume (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

/*----------------------------------------------------------------------------
 * EAS_SetParameter()
 *----------------------------------------------------------------------------
 * Purpose:
 * Set the parameter of a module. See E_MODULES for a list of modules
 * and the header files of the modules for a list of parameters.
 *
 * Inputs:
 * psEASData        - pointer to overall EAS data structure
 * handle           - file or stream handle
 * module           - enumerated module number
 * param            - enumerated parameter number
 * value            - new parameter value
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_SetParameter (EAS_DATA_HANDLE pEASData, EAS_I32 module, EAS_I32 param, EAS_I32 value);

#ifdef _METRICS_ENABLED
/*----------------------------------------------------------------------------
 * EAS_MetricsReport()
 *----------------------------------------------------------------------------
 * Purpose:
 * Displays the current metrics through the EAS_Report interface.
 *
 * Inputs:
 * pEASData             - instance data handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_MetricsReport (EAS_DATA_HANDLE pEASData);

/*----------------------------------------------------------------------------
 * EAS_MetricsReset()
 *----------------------------------------------------------------------------
 * Purpose:
 * Displays the current metrics through the EAS_Report interface.
 *
 * Inputs:
 * pEASData             - instance data handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_MetricsReset (EAS_DATA_HANDLE pEASData);
#endif

/*----------------------------------------------------------------------------
 * EAS_SearchFile
 *----------------------------------------------------------------------------
 * Search file for specific sequence starting at current file
 * position. Returns offset to start of sequence.
 *
 * Inputs:
 * pEASData         - pointer to EAS persistent data object
 * fileHandle       - file handle
 * searchString     - pointer to search sequence
 * len              - length of search sequence
 * pOffset          - pointer to variable to store offset to sequence
 *
 * Returns EAS_EOF if end-of-file is reached
 *----------------------------------------------------------------------------
*/
EAS_RESULT EAS_SearchFile (EAS_DATA_HANDLE pEASData, EAS_FILE_HANDLE fileHandle, const EAS_U8 *searchString, EAS_I32 len, EAS_I32 *pOffset);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* #ifndef _EAS_H */
