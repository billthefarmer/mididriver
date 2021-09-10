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

#include <jni.h>
#include <pthread.h>

#include <android/log.h>

// for oboe native audio
#include <oboe/Oboe.h>

// for EAS midi
#include "eas.h"
#include "eas_reverb.h"

#include "org_billthefarmer_mididriver_MidiDriver.h"
#include "midi.h"

#define LOG_TAG "MidiDriver"

#define LOG_D(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LOG_E(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#define LOG_I(tag, ...) __android_log_print(ANDROID_LOG_INFO, tag, __VA_ARGS__)

// determines how many EAS buffers to fill a host buffer
#define NUM_BUFFERS 4

// mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// oboe stream
std::shared_ptr<oboe::AudioStream> oboeStream;

// EAS data
static EAS_DATA_HANDLE pEASData;
const S_EAS_LIB_CONFIG *pLibConfig;
static EAS_I32 bufferSize;
static EAS_HANDLE midiHandle;

// Functions
oboe::Result initOboe();
oboe::Result closeOboe();

// oboe callback
class OboeCallback : public oboe::AudioStreamDataCallback
{
public:
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream,
                                          void *audioData, int32_t numFrames)
    {
        EAS_RESULT result;
        EAS_I32 numGenerated;
        EAS_I32 count = 0;

        // We requested AudioFormat::I16. So if the stream opens
        // we know we got the I16 format.
        auto *outputData = static_cast<int16_t *>(audioData);

        while (count < bufferSize)
        {
            // lock
            pthread_mutex_lock(&mutex);

            result = EAS_Render(pEASData, outputData + count,
                                pLibConfig->mixBufferSize, &numGenerated);
            // unlock
            pthread_mutex_unlock(&mutex);

            assert(result == EAS_SUCCESS);

            count += numGenerated * pLibConfig->numChannels;
        }

        return oboe::DataCallbackResult::Continue;
    }

    void onErrorAfterClose (oboe::AudioStream *audioStream , oboe::Result  error)
    {
        if(error ==  oboe::Result::ErrorDisconnected)
        {
            initOboe();
        }
    }
};

// oboe callback
OboeCallback oboeCallback;

// build oboe
oboe::Result buildOboe()
{
    oboe::AudioStreamBuilder builder;

    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setFormat(oboe::AudioFormat::I16);
    builder.setFramesPerCallback(bufferSize / pLibConfig->numChannels);
    builder.setChannelCount(pLibConfig->numChannels);
    builder.setSampleRate(pLibConfig->sampleRate);
    builder.setDataCallback(&oboeCallback);

    return builder.openStream(oboeStream);
}

oboe::Result initOboe()
{
    oboe::Result oboeResult;

    if ((oboeResult = buildOboe()) != oboe::Result::OK)
    {
        LOG_E(LOG_TAG, "Failed to create oboe stream. Error: %s",
              oboe::convertToText(oboeResult));

        return oboeResult;
    }

    if ((oboeResult = oboeStream->requestStart()) != oboe::Result::OK)
    {
        closeOboe();

        LOG_E(LOG_TAG, "Failed to start oboe stream. Error: %s",
              oboe::convertToText(oboeResult));

        return oboeResult;
    }

    return oboe::Result::OK;
}

// close oboe
oboe::Result closeOboe()
{
    oboeStream->requestStop();
    return oboeStream->close();
}

// init EAS midi
EAS_RESULT initEAS()
{
    EAS_RESULT result;

    // get the library configuration
    pLibConfig = EAS_Config();
    if (pLibConfig == NULL || pLibConfig->libVersion != LIB_VERSION)
        return EAS_FAILURE;

    // calculate buffer size
    bufferSize = pLibConfig->mixBufferSize * pLibConfig->numChannels * NUM_BUFFERS;

    // init library
    if ((result = EAS_Init(&pEASData)) != EAS_SUCCESS)
        return result;

    // select reverb preset and enable
    EAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_PRESET,
                     EAS_PARAM_REVERB_CHAMBER);
    EAS_SetParameter(pEASData, EAS_MODULE_REVERB, EAS_PARAM_REVERB_BYPASS,
                     EAS_FALSE);

    // open midi stream
    if ((result = EAS_OpenMIDIStream(pEASData, &midiHandle, NULL)) != EAS_SUCCESS)
        return result;

    return EAS_SUCCESS;
}

// shutdown EAS midi
void shutdownEAS()
{

    if (midiHandle != NULL)
    {
        EAS_CloseMIDIStream(pEASData, midiHandle);
        midiHandle = NULL;
    }

    if (pEASData != NULL)
    {
        EAS_Shutdown(pEASData);
        pEASData = NULL;
    }
}

// init mididriver
jboolean midi_init()
{
    EAS_RESULT result;
    oboe::Result oboeResult;

    if ((result = initEAS()) != EAS_SUCCESS)
    {
        shutdownEAS();

        LOG_E(LOG_TAG, "Init EAS failed: %ld", result);

        return JNI_FALSE;
    }

    // LOG_D(LOG_TAG, "Init EAS success, buffer: %ld", bufferSize);

    if ((oboeResult = initOboe()) != oboe::Result::OK)
    {
        shutdownEAS();

        return JNI_FALSE;
    }

    return JNI_TRUE;
}

jboolean
Java_org_billthefarmer_mididriver_MidiDriver_init(JNIEnv *env,
                                                  jobject obj)
{
    return midi_init();
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

// midi write
jboolean midi_write(EAS_U8 *bytes, jint length)
{
    EAS_RESULT result;

    if (pEASData == NULL || midiHandle == NULL)
        return JNI_FALSE;

    // lock
    pthread_mutex_lock(&mutex);

    result = EAS_WriteMIDIStream(pEASData, midiHandle, bytes, length);

    // unlock
    pthread_mutex_unlock(&mutex);

    if (result != EAS_SUCCESS)
        return JNI_FALSE;

    return JNI_TRUE;
}

jboolean
Java_org_billthefarmer_mididriver_MidiDriver_write(JNIEnv *env,
                                                   jobject obj,
                                                   jbyteArray byteArray)
{
    EAS_RESULT result;
    jboolean isCopy;
    jint length;
    EAS_U8 *bytes;

    bytes = (EAS_U8 *) env->GetByteArrayElements(byteArray, &isCopy);
    length = env->GetArrayLength(byteArray);

    result = midi_write(bytes, length);

    env->ReleaseByteArrayElements(byteArray, (jbyte *) bytes, 0);

    return result;
}

// set EAS master volume
jboolean midi_setVolume(jint volume)
{
    EAS_RESULT result;

    if (pEASData == NULL || midiHandle == NULL)
        return JNI_FALSE;

    result = EAS_SetVolume(pEASData, NULL, (EAS_I32) volume);

    if (result != EAS_SUCCESS)
        return JNI_FALSE;

    return JNI_TRUE;
}

jboolean
Java_org_billthefarmer_mididriver_MidiDriver_setVolume(JNIEnv *env,
                                                       jobject obj,
                                                       jint volume)
{
    return midi_setVolume(volume);
}

// Set EAS reverb
jboolean midi_setReverb(jint preset)
{
    EAS_RESULT result;

    if (preset >= 0)
    {
        result = EAS_SetParameter(pEASData, EAS_MODULE_REVERB,
                                  EAS_PARAM_REVERB_PRESET, preset);
        if (result != EAS_SUCCESS)
        {
            LOG_E(LOG_TAG, "Set EAS reverb preset failed: %ld", result);
            return JNI_FALSE;
        }

        result = EAS_SetParameter(pEASData, EAS_MODULE_REVERB,
                                  EAS_PARAM_REVERB_BYPASS, EAS_FALSE);
        if (result != EAS_SUCCESS)
        {
            LOG_E(LOG_TAG, "Enable EAS reverb failed: %ld", result);
            return JNI_FALSE;
        }
    }

    else
    {
        result = EAS_SetParameter(pEASData, EAS_MODULE_REVERB,
                                  EAS_PARAM_REVERB_BYPASS, EAS_TRUE);
        if (result != EAS_SUCCESS)
        {
            LOG_E(LOG_TAG, "Disable EAS reverb failed: %ld", result);
            return JNI_FALSE;
        }
    }

    return JNI_TRUE;
}

jboolean
Java_org_billthefarmer_mididriver_MidiDriver_setReverb(JNIEnv *env,
                                                       jobject obj,
                                                       jint preset)
{
    return midi_setReverb(preset);
}

// shutdown EAS midi
jboolean midi_shutdown()
{
    closeOboe();
    shutdownEAS();

    return JNI_TRUE;
}

jboolean
Java_org_billthefarmer_mididriver_MidiDriver_shutdown(JNIEnv *env,
                                                      jobject obj)
{
    return midi_shutdown();
}
