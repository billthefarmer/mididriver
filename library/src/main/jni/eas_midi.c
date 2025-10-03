/*----------------------------------------------------------------------------
 *
 * File:
 * eas_midi.c
 *
 * Contents and purpose:
 * Contains EAS library public interface
 *
 * Copyright Sonic Network Inc. 2004

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
 *   $Revision: 842 $
 *   $Date: 2007-08-23 14:32:31 -0700 (Thu, 23 Aug 2007) $
 *----------------------------------------------------------------------------
*/

#include "eas_synthcfg.h"
#include "eas.h"
#include "eas_config.h"
#include "eas_host.h"
#include "eas_report.h"
#include "eas_data.h"
#include "eas_parser.h"
#include "eas_pcm.h"
#include "eas_midi.h"
#include "eas_mixer.h"
#include "eas_build.h"
#include "eas_vm_protos.h"
#include "eas_math.h"

#ifdef JET_INTERFACE
#include "jet_data.h"
#endif

#ifdef DLS_SYNTHESIZER
#include "eas_mdls.h"
#endif

static EAS_RESULT MIDIStream_State (S_EAS_DATA *pEASData, EAS_VOID_PTR pInstData, EAS_I32 *pState);
static EAS_RESULT MIDIStream_GetData (S_EAS_DATA *pEASData, EAS_VOID_PTR pInstData, EAS_I32 param, EAS_I32 *pValue);

/*---------------------------------------------------------------------------- 
  * 
  * EAS_MIDIStream_Parser 
  * 
  * This structure contains the functional interface for the MIDI Stream parser 
  *---------------------------------------------------------------------------- 
 */ 
const static S_FILE_PARSER_INTERFACE EAS_MIDIStream_Parser = 
{ 
    NULL,
    NULL,
    NULL,
    NULL,
    MIDIStream_State,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MIDIStream_GetData,
    NULL 
};

/*----------------------------------------------------------------------------
 * MIDIStream_State()
 *----------------------------------------------------------------------------
 * Purpose:
 * Returns the current state of the stream
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * handle           - pointer to file handle
 * pState           - pointer to variable to store state
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
/*lint -esym(715, pEASData) common decoder interface - pEASData not used */
static EAS_RESULT MIDIStream_State (S_EAS_DATA *pEASData, EAS_VOID_PTR pInstData, EAS_I32 *pState)
{
    *pState = EAS_STATE_READY;
    return EAS_SUCCESS;
}

/*----------------------------------------------------------------------------
 * MIDIStream_GetData()
 *----------------------------------------------------------------------------
 * Purpose:
 * Get specified data
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * handle           - pointer to file handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
/*lint -esym(715, pEASData) common decoder interface - pEASData not used */
static EAS_RESULT MIDIStream_GetData (S_EAS_DATA *pEASData, EAS_VOID_PTR pInstData, EAS_I32 param, EAS_I32 *pValue)
{
    S_INTERACTIVE_MIDI *pData;

    pData = (S_INTERACTIVE_MIDI*) pInstData;
    switch (param)
    {
        case PARSER_DATA_SYNTH_HANDLE:
            *pValue = (EAS_I32) pData->pSynth;
            break;

        default:
            return EAS_ERROR_INVALID_PARAMETER;
    }
    return EAS_SUCCESS;
}

/*----------------------------------------------------------------------------
 * EAS_AllocateStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Allocates a stream handle
 *
 * Inputs:
 *
 * Outputs:
 *
 *----------------------------------------------------------------------------
*/
static EAS_INT EAS_AllocateStream (EAS_DATA_HANDLE pEASData)
{
    EAS_INT streamNum;

    /* check for static allocation, only one stream allowed */
    if (pEASData->staticMemoryModel)
    {
        if (pEASData->streams[0].handle != NULL)
        {
            { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "Attempt to open multiple streams in static model\n"); */ }
            return -1;
        }
        return 0;
    }

    /* dynamic model */
    for (streamNum = 0; streamNum < MAX_NUMBER_STREAMS; streamNum++)
        if (pEASData->streams[streamNum].handle == NULL)
            break;
    if (streamNum == MAX_NUMBER_STREAMS)
    {
        { /* dpp: EAS_ReportEx(_EAS_SEVERITY_ERROR, "Exceeded maximum number of open streams\n"); */ }
        return -1;
    }
    return streamNum;
}

/*----------------------------------------------------------------------------
 * EAS_InitStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Initialize a stream
 *
 * Inputs:
 *
 * Outputs:
 *
 *----------------------------------------------------------------------------
*/
static void EAS_InitStream (S_EAS_STREAM *pStream, EAS_VOID_PTR pParserModule, EAS_VOID_PTR streamHandle)
{
    pStream->pParserModule = pParserModule;
    pStream->handle = streamHandle;
    pStream->time = 0;
    pStream->frameLength = AUDIO_FRAME_LENGTH;
    pStream->repeatCount = 0;
    pStream->volume = DEFAULT_STREAM_VOLUME;
    pStream->streamFlags = 0;
}

/*----------------------------------------------------------------------------
 * EAS_GetStreamParameter
 *----------------------------------------------------------------------------
 * Sets the specified parameter in the stream. Allows access to
 * customizable settings within the individual file parsers.
 *----------------------------------------------------------------------------
 * pEASData         - pointer to EAS persistent data object
 * pStream          - stream handle
 * param            - enumerated parameter (see eas_parser.h)
 * pValue           - pointer to variable to receive current setting
 *----------------------------------------------------------------------------
*/
EAS_RESULT EAS_GetStreamParameter (S_EAS_DATA *pEASData, EAS_HANDLE pStream, EAS_I32 param, EAS_I32 *pValue);

/*----------------------------------------------------------------------------
 * VMIncRefCount()
 *----------------------------------------------------------------------------
 * Increment reference count for virtual synth
 *----------------------------------------------------------------------------
*/
void VMIncRefCount (S_SYNTH *pSynth)
{
    pSynth->refCount++;
}

/*----------------------------------------------------------------------------
 * EAS_OpenMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Opens a raw MIDI stream allowing the host to route MIDI cable data directly to the synthesizer
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * pHandle          - pointer to variable to hold file or stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_OpenMIDIStream (EAS_DATA_HANDLE pEASData, EAS_HANDLE *ppStream, EAS_HANDLE streamHandle)
{
    EAS_RESULT result;
    S_INTERACTIVE_MIDI *pMIDIStream;
    EAS_INT streamNum;

    /* initialize some pointers */
    *ppStream = NULL;

    /* allocate a stream */
    if ((streamNum = EAS_AllocateStream(pEASData)) < 0)
        return EAS_ERROR_MAX_STREAMS_OPEN;

    /* check Configuration Module for S_EAS_DATA allocation */
    if (pEASData->staticMemoryModel)
        pMIDIStream = EAS_CMEnumData(EAS_CM_MIDI_STREAM_DATA);
    else
        pMIDIStream = EAS_HWMalloc(pEASData->hwInstData, sizeof(S_INTERACTIVE_MIDI));

    /* allocate dynamic memory */
    if (!pMIDIStream)
    {
        { /* dpp: EAS_ReportEx(_EAS_SEVERITY_FATAL, "Failed to allocate MIDI stream data\n"); */ }
        return EAS_ERROR_MALLOC_FAILED;
    }

    /* zero the memory to insure complete initialization */
    EAS_HWMemSet(pMIDIStream, 0, sizeof(S_INTERACTIVE_MIDI));
    EAS_InitStream(&pEASData->streams[streamNum], (EAS_VOID_PTR) &EAS_MIDIStream_Parser, pMIDIStream);

    /* instantiate a new synthesizer */
    if (streamHandle == NULL)
    {
        result = VMInitMIDI(pEASData, &pMIDIStream->pSynth);
    }

    /* use an existing synthesizer */
    else
    {
        EAS_I32 value;
        result = EAS_GetStreamParameter(pEASData, streamHandle, PARSER_DATA_SYNTH_HANDLE, &value);
        pMIDIStream->pSynth = (S_SYNTH*) value;
        VMIncRefCount(pMIDIStream->pSynth);
    }
    if (result != EAS_SUCCESS)
    {
        EAS_CloseMIDIStream(pEASData, &pEASData->streams[streamNum]);
        return result;
    }

    /* initialize the MIDI stream data */
    EAS_InitMIDIStream(&pMIDIStream->stream);

    *ppStream = (EAS_HANDLE) &pEASData->streams[streamNum];
    return EAS_SUCCESS;
}

/*----------------------------------------------------------------------------
 * EAS_WriteMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Send data to the MIDI stream device
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * handle           - stream handle
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
EAS_PUBLIC EAS_RESULT EAS_WriteMIDIStream (EAS_DATA_HANDLE pEASData, EAS_HANDLE pStream, EAS_U8 *pBuffer, EAS_I32 count)
{
    S_INTERACTIVE_MIDI *pMIDIStream;
    EAS_RESULT result;

    pMIDIStream = (S_INTERACTIVE_MIDI*) pStream->handle;

    if (count <= 0)
        return EAS_ERROR_PARAMETER_RANGE;

    /* send the entire buffer */
    while (count--)
    {
        if ((result = EAS_ParseMIDIStream(pEASData, pMIDIStream->pSynth, &pMIDIStream->stream, *pBuffer++, eParserModePlay)) != EAS_SUCCESS)
            return result;
    }
    return EAS_SUCCESS;
}

/*----------------------------------------------------------------------------
 * EAS_CloseMIDIStream()
 *----------------------------------------------------------------------------
 * Purpose:
 * Closes a raw MIDI stream
 *
 * Inputs:
 * pEASData         - pointer to overall EAS data structure
 * handle           - stream handle
 *
 * Outputs:
 *
 *
 * Side Effects:
 *
 *----------------------------------------------------------------------------
*/
EAS_PUBLIC EAS_RESULT EAS_CloseMIDIStream (EAS_DATA_HANDLE pEASData, EAS_HANDLE pStream)
{
    S_INTERACTIVE_MIDI *pMIDIStream;

    pMIDIStream = (S_INTERACTIVE_MIDI*) pStream->handle;

    /* close synth */
    if (pMIDIStream->pSynth != NULL)
    {
        VMMIDIShutdown(pEASData, pMIDIStream->pSynth);
        pMIDIStream->pSynth = NULL;
    }

    /* release allocated memory */
    if (!pEASData->staticMemoryModel)
        EAS_HWFree(((S_EAS_DATA*) pEASData)->hwInstData, pMIDIStream);

    pStream->handle = NULL;
    return EAS_SUCCESS;
}
