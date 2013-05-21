////////////////////////////////////////////////////////////////////////////////
//
//  MidiDriver - An Android Midi Driver.
//
//  Copyright (C) 2013	Bill Farmer
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
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

#include <jni.h>

// for EAS midi
#include "eas.h"
#include "eas_reverb.h"

// determines how many EAS buffers to fill a host buffer
#define NUM_BUFFERS 4

// EAS data
static EAS_DATA_HANDLE pEASData;
const S_EAS_LIB_CONFIG *pLibConfig;
static EAS_PCM *buffer;
static EAS_RESULT result;
static EAS_I32 bufferSize;
static EAS_HANDLE midiHandle;

// init EAS midi
jint
Java_org_billthefarmer_mididriver_MidiDriver_init(JNIEnv *env,
						  jobject clazz)
{
    // get the library configuration
    pLibConfig = EAS_Config();
    if (pLibConfig == NULL || pLibConfig->libVersion != LIB_VERSION)
	return 0;

    // calculate buffer size
    bufferSize = pLibConfig->mixBufferSize * pLibConfig->numChannels *
	NUM_BUFFERS;

    // init library
    if ((result = EAS_Init(&pEASData)) != EAS_SUCCESS)
        return 0;

    // select reverb preset and enable
    EAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_PRESET,
		     EAS_PARAM_REVERB_CHAMBER);
    EAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS,
		     EAS_FALSE);

    // open midi stream
    if (result = EAS_OpenMIDIStream(pEASData, &midiHandle, NULL) !=
	EAS_SUCCESS)
    {
	EAS_Shutdown(pEASData);
	return 0;
    }

    return bufferSize;
}

// midi config
jintArray
Java_org_billthefarmer_mididriver_MidiDriver_config(JNIEnv *env,
						    jobject clazz)
{
    jboolean isCopy;

    jintArray configArray = (*env)->NewIntArray(env, 4);

    jint *config = (*env)->GetIntArrayElements(env, configArray, &isCopy);

    config[0] = pLibConfig->maxVoices;
    config[1] = pLibConfig->numChannels;
    config[2] = pLibConfig->sampleRate;
    config[3] = pLibConfig->mixBufferSize;

    (*env)->ReleaseIntArrayElements(env, configArray, config, 0);

    return configArray;
}

// midi render
jint
Java_org_billthefarmer_mididriver_MidiDriver_render(JNIEnv *env,
						    jobject clazz,
						    jshortArray shortArray)
{
    jboolean isCopy;
    EAS_I32 numGenerated;
    EAS_I32 count;
    jsize size;

    // jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy)
    // void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,

    // void* GetPrimitiveArrayCritical(JNIEnv*, jarray, jboolean*);
    // void ReleasePrimitiveArrayCritical(JNIEnv*, jarray, void*, jint);

    buffer =
	(EAS_PCM *)(*env)->GetShortArrayElements(env, shortArray, &isCopy);

    size = (*env)->GetArrayLength(env, shortArray);

    count = 0;
    while (count < size)
    {
    	result = EAS_Render(pEASData, buffer + count,
    			    pLibConfig->mixBufferSize, &numGenerated);
    	if (result != EAS_SUCCESS)
    	    break;

    	count += numGenerated * pLibConfig->numChannels;
    }

    (*env)->ReleaseShortArrayElements(env, shortArray, buffer, 0);

    return count;
}

// midi write
jboolean
Java_org_billthefarmer_mididriver_MidiDriver_write(JNIEnv *env,
						   jobject clazz,
						   jbyteArray byteArray)
{
    jboolean isCopy;
    jint length;
    EAS_U8 *buf;

    buf = (EAS_U8 *)(*env)->GetByteArrayElements(env, byteArray, &isCopy);
    length = (*env)->GetArrayLength(env, byteArray);

    result = EAS_WriteMIDIStream(pEASData, midiHandle, buf, length);

    (*env)->ReleaseByteArrayElements(env, byteArray, buf, 0);

    if (result != EAS_SUCCESS)
	return JNI_FALSE;

    return JNI_TRUE;
}

// shutdown EAS midi
jboolean
Java_org_billthefarmer_mididriver_MidiDriver_shutdown(JNIEnv *env,
						      jobject clazz)
{

    EAS_CloseMIDIStream(pEASData, midiHandle);
    EAS_Shutdown(pEASData);

    return JNI_TRUE;
}
