# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("sonivox")

set(CMAKE_C_STANDARD 11)

# Library that will be called directly from JAVA
add_library (sonivox SHARED
  midi.cpp
  eas_midi.c)


set(host_DIR ${CMAKE_CURRENT_SOURCE_DIR}/host_src)
set(lib_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib_src)
set(log_DIR ${CMAKE_CURRENT_SOURCE_DIR}/log)
set(media_DIR ${CMAKE_CURRENT_SOURCE_DIR}/media)

target_sources (sonivox PRIVATE
  ${lib_DIR}/eas_data.c
  ${lib_DIR}/eas_dlssynth.c
  ${lib_DIR}/eas_flog.c
  ${lib_DIR}/eas_math.c
  ${lib_DIR}/eas_mdls.c
  ${lib_DIR}/eas_midi.c
  ${lib_DIR}/eas_mididata.c
  ${lib_DIR}/eas_mixbuf.c
  ${lib_DIR}/eas_mixer.c
  ${lib_DIR}/eas_pan.c
  ${lib_DIR}/eas_pcm.c
  ${lib_DIR}/eas_pcmdata.c
  ${lib_DIR}/eas_public.c
  ${lib_DIR}/eas_reverb.c
  ${lib_DIR}/eas_reverbdata.c
  ${lib_DIR}/eas_smf.c
  ${lib_DIR}/eas_smfdata.c
  ${lib_DIR}/eas_voicemgt.c
  ${lib_DIR}/eas_wtengine.c
  ${lib_DIR}/eas_wtsynth.c
  ${lib_DIR}/wt_22khz.c
  ${host_DIR}/eas_config.c
  ${host_DIR}/eas_hostmm.c
  ${host_DIR}/eas_report.c
  ${media_DIR}/MediaPlayerInterface.h)

  # not using these modules
  # ${lib_DIR}/eas_chorus.c
  # ${lib_DIR}/eas_chorusdata.c
  # ${lib_DIR}/eas_imelody.c
  # ${lib_DIR}/eas_imelodydata.c
  # ${lib_DIR}/eas_ima_tables.c
  # ${lib_DIR}/eas_imaadpcm.c
  # ${lib_DIR}/eas_ota.c
  # ${lib_DIR}/eas_otadata.c
  # ${lib_DIR}/eas_rtttl.c
  # ${lib_DIR}/eas_rtttldata.c
  # ${lib_DIR}/eas_xmf.c
  # ${lib_DIR}/eas_xmfdata.c
  # ${host_DIR}/eas_main.c
  # ${host_DIR}/eas_wave.c
  # ${lib_DIR}/eas_wavefile.c
  # ${lib_DIR}/eas_wavefiledata.c
  # ${lib_DIR}/jet.c)

add_definitions (
  -O2
  -D UNIFIED_DEBUG_MESSAGES
  -D EAS_WT_SYNTH
  -D NUM_OUTPUT_CHANNELS=2
  -D _SAMPLE_RATE_22050
  -D MAX_SYNTH_VOICES=64
  -D _16_BIT_SAMPLES
  -D _FILTER_ENABLED
  -D DLS_SYNTHESIZER
  -D _REVERB_ENABLED
  -D false=0
  -DANDROID_ARM_MODE=arm
  -Wno-unused-parameter
  -Werror)

  # not using these options
  # -D _IMELODY_PARSER
  # -D _RTTTL_PARSER
  # -D _OTA_PARSER
  # -D _XMF_PARSER
  # -D _WAVE_PARSER
  # -D _IMA_DECODER (needed for IMA-ADPCM wave files)
  # -D _CHORUS_ENABLED

# Specify directories which the compiler should look for headers
target_include_directories (sonivox PUBLIC
  .include
  ${lib_DIR}
  ${host_DIR})

target_include_directories (sonivox PUBLIC
  ${CMAKE_SOURCE_DIR}/cutils
  ${log_DIR}
  ${CMAKE_SOURCE_DIR}/media)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library ( # Sets the name of the path variable.
  log-lib

  # Specifies the name of the NDK library that
  # you want CMake to locate.
  log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries ( # Specifies the target library.
  sonivox
  # Links the target library to the log library
  # included in the NDK.
  ${log-lib})


# Find the Oboe package
find_package (oboe REQUIRED CONFIG)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries ( # Specifies the target library.
  sonivox
  oboe::oboe
  )

# Equivalent to LOCAL_ARM_MODE := arm in Android.mk
set (CMAKE_ANDROID_ARM_MODE ON)


if (CMAKE_ANDROID_ARCH STREQUAL arm)
  enable_language(C ASM)

  target_sources (sonivox PRIVATE
    ${lib_DIR}/ARM-E_filter_gnu.s
    ${lib_DIR}/ARM-E_mastergain_gnu.s)

    # not using these modules
    # lib_src/ARM-E_interpolate_loop_gnu.s
    # lib_src/ARM-E_interpolate_noloop_gnu.s
    # lib_src/ARM-E_voice_gain_gnu.s

  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}-x assembler-with-cpp ")
  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}-D STEREO_OUTPUT ")
  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}-D FILTER_ENABLED ")
  set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}-D SAMPLES_16_BIT ")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D NATIVE_EAS_KERNEL ")

endif()
