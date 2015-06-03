////////////////////////////////////////////////////////////////////////////////
//
//  MidiDriver - An Android Midi Driver.
//
//  Copyright (C) 2013  Bill Farmer
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
//  Bill Farmer  william j farmer [at] yahoo [dot] co [dot] uk.
//
///////////////////////////////////////////////////////////////////////////////

#include <jni.h>

#include "org_billthefarmer_mididriver_MidiDriver.h"

// for EAS midi
#include "eas.h"
#include "eas_reverb.h"

#include <dlfcn.h>
#include <android/log.h>

// EAS function pointers
EAS_PUBLIC const S_EAS_LIB_CONFIG *(*pEAS_Config) (void);
EAS_PUBLIC EAS_RESULT (*pEAS_Init) (EAS_DATA_HANDLE *ppEASData);
EAS_PUBLIC EAS_RESULT (*pEAS_SetParameter) (EAS_DATA_HANDLE pEASData, EAS_I32 module, EAS_I32 param, EAS_I32 value);
EAS_PUBLIC EAS_RESULT (*pEAS_OpenMIDIStream) (EAS_DATA_HANDLE pEASData, EAS_HANDLE *pStreamHandle,
                                              EAS_HANDLE streamHandle);
EAS_PUBLIC EAS_RESULT (*pEAS_Shutdown) (EAS_DATA_HANDLE pEASData);
EAS_PUBLIC EAS_RESULT (*pEAS_Render) (EAS_DATA_HANDLE pEASData, EAS_PCM *pOut, EAS_I32 numRequested,
                                      EAS_I32 *pNumGenerated);
EAS_PUBLIC EAS_RESULT (*pEAS_WriteMIDIStream)(EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_U8 *pBuffer,
                                              EAS_I32 count);
EAS_PUBLIC EAS_RESULT (*pEAS_CloseMIDIStream) (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle);

// determines how many EAS buffers to fill a host buffer
#define NUM_BUFFERS 4

// EAS data
static EAS_DATA_HANDLE pEASData;
const S_EAS_LIB_CONFIG *pLibConfig;
static EAS_PCM *buffer;
static EAS_I32 bufferSize;
static EAS_HANDLE midiHandle;

// init EAS midi
jint
Java_org_billthefarmer_mididriver_MidiDriver_init(JNIEnv *env,
                                                  jobject obj)
{
    EAS_RESULT result;

    // get the library configuration
    pLibConfig = pEAS_Config();
    if (pLibConfig == NULL || pLibConfig->libVersion != LIB_VERSION)
        return 0;

    // calculate buffer size
    bufferSize = pLibConfig->mixBufferSize * pLibConfig->numChannels *
        NUM_BUFFERS;

    // init library
    if ((result = pEAS_Init(&pEASData)) != EAS_SUCCESS)
        return 0;

    // select reverb preset and enable
    pEAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_PRESET,
                      EAS_PARAM_REVERB_CHAMBER);
    pEAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS,
                      EAS_FALSE);

    // open midi stream
    if (result = pEAS_OpenMIDIStream(pEASData, &midiHandle, NULL) !=
        EAS_SUCCESS)
        {
            pEAS_Shutdown(pEASData);
            return 0;
        }

    return bufferSize;
}

// midi config
jintArray
Java_org_billthefarmer_mididriver_MidiDriver_config(JNIEnv *env,
                                                    jobject obj)
{
    jboolean isCopy;

    if (pLibConfig == NULL)
        return NULL;

    jintArray configArray = env->NewIntArray(4);

    jint *config = env->GetIntArrayElements(configArray, &isCopy);

    config[0] = pLibConfig->maxVoices;
    config[1] = pLibConfig->numChannels;
    config[2] = pLibConfig->sampleRate;
    config[3] = pLibConfig->mixBufferSize;

    env->ReleaseIntArrayElements(configArray, config, 0);

    return configArray;
}

// midi render
jint
Java_org_billthefarmer_mididriver_MidiDriver_render(JNIEnv *env,
                                                    jobject obj,
                                                    jshortArray shortArray)
{
    jboolean isCopy;
    EAS_RESULT result;
    EAS_I32 numGenerated;
    EAS_I32 count;
    jsize size;

    // jbyte* GetByteArrayElements(jbyteArray array, jboolean* isCopy)
    // void ReleaseByteArrayElements(jbyteArray array, jbyte* elems,

    // void* GetPrimitiveArrayCritical(JNIEnv*, jarray, jboolean*);
    // void ReleasePrimitiveArrayCritical(JNIEnv*, jarray, void*, jint);

    if (pEASData == NULL)
        return 0;

    buffer =
        (EAS_PCM *)env->GetShortArrayElements(shortArray, &isCopy);

    size = env->GetArrayLength(shortArray);

    count = 0;
    while (count < size)
        {
            result = pEAS_Render(pEASData, buffer + count,
                                 pLibConfig->mixBufferSize, &numGenerated);
            if (result != EAS_SUCCESS)
                break;

            count += numGenerated * pLibConfig->numChannels;
        }

    env->ReleaseShortArrayElements(shortArray, buffer, 0);

    return count;
}

// midi write
jboolean
Java_org_billthefarmer_mididriver_MidiDriver_write(JNIEnv *env,
                                                   jobject obj,
                                                   jbyteArray byteArray)
{
    jboolean isCopy;
    EAS_RESULT result;
    jint length;
    EAS_U8 *buf;

    if (pEASData == NULL || midiHandle == NULL)
        return JNI_FALSE;

    buf = (EAS_U8 *)env->GetByteArrayElements(byteArray, &isCopy);
    length = env->GetArrayLength(byteArray);

    result = pEAS_WriteMIDIStream(pEASData, midiHandle, buf, length);

    env->ReleaseByteArrayElements(byteArray, (jbyte *)buf, 0);

    if (result != EAS_SUCCESS)
        return JNI_FALSE;

    return JNI_TRUE;
}

// shutdown EAS midi
jboolean
Java_org_billthefarmer_mididriver_MidiDriver_shutdown(JNIEnv *env,
                                                      jobject obj)
{
    EAS_RESULT result;

    if (pEASData == NULL || midiHandle == NULL)
        return JNI_FALSE;

    if ((result = pEAS_CloseMIDIStream(pEASData, midiHandle)) != EAS_SUCCESS)
        {
            pEAS_Shutdown(pEASData);
            return JNI_FALSE;
        }

    if ((result = pEAS_Shutdown(pEASData)) != EAS_SUCCESS)
        return JNI_FALSE;

    return JNI_TRUE;
}

extern "C" {
    jint JNI_OnLoad(JavaVM* vm, void* reserved);
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    jclass   dlExceptionClass = env->FindClass("org/billthefarmer/mididriver/DynamicLinkException");
    if (dlExceptionClass == NULL) {
        __android_log_write(ANDROID_LOG_ERROR, "org.billthefarmer.mididriver", "Failed to resolve "
                            "org/billthefarmer/mididriver/DynamicLinkException");
        return -1;
    }


    void *libHandler;

    __android_log_write(ANDROID_LOG_VERBOSE, "org.billthefarmer.mididriver", "Init function called" );

    libHandler = dlopen("libsonivox.so", RTLD_LAZY);
    if (!libHandler) {
        env->ThrowNew(dlExceptionClass, "dlopen libsonivox.so failed" );
        return -1;
    } else {
        __android_log_write(ANDROID_LOG_VERBOSE, "org.billthefarmer.mididriver", "dlopen libsonivox.so passed" );
    }

    pEAS_Config =  (EAS_PUBLIC const S_EAS_LIB_CONFIG *(*) (void))dlsym(libHandler, "EAS_Config");
    if (!pEAS_Config) {
        env->ThrowNew(dlExceptionClass, "EAS_Config resolution failed");
        return -1;
    }
    
    pEAS_Init = (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE *ppEASData))dlsym(libHandler, "EAS_Init");
    if (!pEAS_Config) {
        env->ThrowNew(dlExceptionClass, "EAS_Init resolution failed");
        return -1;
    }
      
    pEAS_SetParameter = 
        (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE pEASData, EAS_I32 module, EAS_I32 param, EAS_I32 value))
        dlsym(libHandler, "EAS_SetParameter");
    if (!pEAS_SetParameter) {
        env->ThrowNew(dlExceptionClass, "EAS_SetParameter resolution failed");
        return -1;
    }
    
    pEAS_OpenMIDIStream = 
        (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE pEASData, EAS_HANDLE *pStreamHandle, EAS_HANDLE streamHandle))
        dlsym(libHandler, "EAS_OpenMIDIStream");
    if (!pEAS_OpenMIDIStream) {
        env->ThrowNew(dlExceptionClass, "EAS_OpenMIDIStream resolution failed");
        return -1;
    }

    pEAS_Shutdown = (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE pEASData))dlsym(libHandler, "EAS_Shutdown");
    if (!pEAS_Shutdown) {
        env->ThrowNew(dlExceptionClass, "EAS_Shutdown resolution failed");
        return -1;
    }

    pEAS_Render =
        (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE pEASData, EAS_PCM *pOut, EAS_I32 numRequested,
                                    EAS_I32 *pNumGenerated))
        dlsym(libHandler, "EAS_Render");
    if (!pEAS_Render) {
        env->ThrowNew(dlExceptionClass, "EAS_Render resolution failed");
        return -1;
    }
    
    pEAS_WriteMIDIStream = (EAS_PUBLIC EAS_RESULT (*)(EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle, EAS_U8 *pBuffer,
                                                      EAS_I32 count))
        dlsym(libHandler, "EAS_WriteMIDIStream");
    if (!pEAS_WriteMIDIStream) {
        env->ThrowNew(dlExceptionClass, "EAS_WriteMIDIStream resolution failed");
        return -1;
    }

    pEAS_CloseMIDIStream = (EAS_PUBLIC EAS_RESULT (*) (EAS_DATA_HANDLE pEASData, EAS_HANDLE streamHandle))
        dlsym(libHandler, "EAS_CloseMIDIStream");
    if (!pEAS_CloseMIDIStream) {
        env->ThrowNew(dlExceptionClass, "EAS_CloseMIDIStream resolution failed");
        return -1;
    }

    __android_log_write(ANDROID_LOG_VERBOSE, "org.billthefarmer.mididriver", "Init function passed" );
    return JNI_VERSION_1_6;
}
