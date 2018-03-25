// Audio playback and capture library. Public domain. See "unlicense" statement at the end of this file.
// mini_al - v0.x - 2018-xx-xx
//
// David Reid - davidreidsoftware@gmail.com

// ABOUT
// =====
// mini_al is a small library for making it easy to connect to a playback or capture device and send
// or receive data from that device.
//
// mini_al uses an asynchronous API. Every device is created with it's own thread, with audio data
// being delivered to or from the device via a callback. Synchronous APIs are not supported in the
// interest of keeping the library as simple and light-weight as possible.
//
// Supported Backends:
//   - WASAPI
//   - DirectSound
//   - WinMM
//   - ALSA
//   - PulseAudio
//   - JACK
//   - OSS
//   - OpenSL|ES / Android
//   - OpenAL
//   - SDL
//   - Null (Silence)
//   - ... and more in the future.
//     - Core Audio (OSX, iOS)
//
// Supported Formats:
//   - Unsigned 8-bit PCM
//   - Signed 16-bit PCM
//   - Signed 24-bit PCM (tightly packed)
//   - Signed 32-bit PCM
//   - IEEE 32-bit floating point PCM
//
//
// USAGE
// =====
// mini_al is a single-file library. To use it, do something like the following in one .c file.
//   #define MAL_IMPLEMENTATION
//   #include "mini_al.h"
//
// You can then #include this file in other parts of the program as you would with any other header file.
//
// The implementation of this library will try #include-ing necessary headers for some backends. If you do not have
// the development packages for any particular backend you can disable it by #define-ing the appropriate MAL_NO_*
// option before the implementation.
//
//
// Building for Windows
// --------------------
// The Windows build should compile clean on all popular compilers without the need to configure any include paths
// nor link to any libraries.
//
// Building for Linux
// ------------------
// The Linux build only requires linking to -ldl and -lpthread. You do not need any development packages for any
// of the supported backends.
//
// Building for BSD
// ----------------
// The BSD build uses OSS. Requires linking to -lpthread. Also requires linking to -lossaudio on {Open,Net}BSD, but
// not FreeBSD.
//
// Building for Android
// --------------------
// The Android build uses OpenSL|ES, and will require an appropriate API level that supports OpenSL|ES. mini_al has
// been tested against API levels 16 and 21.
//
// Building for Emscripten
// -----------------------
// The Emscripten build currently uses SDL 1.2 for it's backend which means specifying "-s USE_SDL=2" is unecessary
// as of this version. However, if in the future there is legitimate benefit or enough demand for SDL 2 to be used
// instead, you will need to specify this when compiling.
//
//
// Playback Example
// ----------------
//   mal_uint32 on_send_samples(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
//   {
//       // This callback is set at initialization time and will be called when a playback device needs more
//       // data. You need to write as many frames as you can to pSamples (but no more than frameCount) and
//       // then return the number of frames you wrote.
//       //
//       // The user data (pDevice->pUserData) is set by mal_device_init().
//       return (mal_uint32)mal_decoder_read((mal_decoder*)pDevice->pUserData, frameCount, pSamples);
//   }
//
//   ...
//
//   mal_device_config config = mal_device_config_init_playback(decoder.outputFormat, decoder.outputChannels, decoder.outputSampleRate, on_send_frames_to_device);
//
//   mal_device device;
//   mal_result result = mal_device_init(NULL, mal_device_type_playback, NULL, &config, &decoder /*pUserData*/, &device);
//   if (result != MAL_SUCCESS) {
//       return -1;
//   }
//
//   mal_device_start(&device);     // The device is sleeping by default so you'll need to start it manually.
//
//   ...
//
//   mal_device_uninit(&device);    // This will stop the device so no need to do that manually.
//
//
//
// NOTES
// =====
// - This library uses an asynchronous API for delivering and requesting audio data. Each device will have
//   it's own worker thread which is managed by the library.
// - If mal_device_init() is called with a device that's not aligned to the platform's natural alignment
//   boundary (4 bytes on 32-bit, 8 bytes on 64-bit), it will _not_ be thread-safe. The reason for this
//   is that it depends on members of mal_device being correctly aligned for atomic assignments.
// - Sample data is always little-endian and interleaved. For example, mal_format_s16 means signed 16-bit
//   integer samples, interleaved. Let me know if you need non-interleaved and I'll look into it.
//
//
//
// BACKEND NUANCES
// ===============
//
// DirectSound
// -----------
// - DirectSound currently supports a maximum of 4 periods.
//
// Android
// -------
// - To capture audio on Android, remember to add the RECORD_AUDIO permission to your manifest:
//     <uses-permission android:name="android.permission.RECORD_AUDIO" />
// - Only a single mal_context can be active at any given time. This is due to a limitation with OpenSL|ES.
//
// UWP
// ---
// - UWP is only supported when compiling as C++.
// - UWP only supports default playback and capture devices.
// - UWP requires the Microphone capability to be enabled in the application's manifest (Package.appxmanifest):
//       <Package ...>
//           ...
//           <Capabilities>
//               <DeviceCapability Name="microphone" />
//           </Capabilities>
//       </Package>
//
// PulseAudio
// ----------
// - Each device has it's own dedicated main loop.
//
// JACK
// ----
// - It's possible for mal_device.bufferSizeInFrames to change during run time.
//
//
// OPTIONS
// =======
// #define these options before including this file.
//
// #define MAL_NO_WASAPI
//   Disables the WASAPI backend.
//
// #define MAL_NO_DSOUND
//   Disables the DirectSound backend.
//
// #define MAL_NO_WINMM
//   Disables the WinMM backend.
//
// #define MAL_NO_ALSA
//   Disables the ALSA backend.
//
// #define MAL_NO_PULSEAUDIO
//   Disables the PulseAudio backend.
//
// #define MAL_NO_JACK
//   Disables the JACK backend.
//
// #define MAL_NO_OSS
//   Disables the OSS backend.
//
// #define MAL_NO_OPENSL
//   Disables the OpenSL|ES backend.
//
// #define MAL_NO_OPENAL
//   Disables the OpenAL backend.
//
// #define MAL_NO_SDL
//   Disables the SDL backend.
//
// #define MAL_NO_NULL
//   Disables the null backend.
//
// #define MAL_DEFAULT_BUFFER_SIZE_IN_MILLISECONDS
//   When a buffer size of 0 is specified when a device is initialized, it will default to a size with
//   this number of milliseconds worth of data. Note that some backends may adjust this setting if that
//   particular backend has unusual latency characteristics.
//
// #define MAL_DEFAULT_PERIODS
//   When a period count of 0 is specified when a device is initialized, it will default to this.
//
// #define MAL_NO_DECODING
//   Disables the decoding APIs.
//
// #define MAL_NO_STDIO
//   Disables file IO APIs

#ifndef mini_al_h
#define mini_al_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4201)   // nonstandard extension used: nameless struct/union
#endif

// Platform/backend detection.
#ifdef _WIN32
    #define MAL_WIN32
    #if (!defined(WINAPI_FAMILY) || WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP)
        #define MAL_WIN32_DESKTOP
    #endif
#else
    #define MAL_POSIX
    #include <pthread.h>    // Unfortunate #include, but needed for pthread_t, pthread_mutex_t and pthread_cond_t types.

    #define MAL_UNIX
    #ifdef __linux__
        #define MAL_LINUX
    #endif
    #ifdef __APPLE__
        #define MAL_APPLE
    #endif
    #ifdef __ANDROID__
        #define MAL_ANDROID
    #endif
    #ifdef __EMSCRIPTEN__
        #define MAL_EMSCRIPTEN
    #endif
#endif

// Some backends are only supported on certain platforms.
#if defined(MAL_WIN32)
    #define MAL_SUPPORT_WASAPI
    #if defined(MAL_WIN32_DESKTOP)  // DirectSound and WinMM backends are only supported on desktop's.
        #define MAL_SUPPORT_DSOUND
        #define MAL_SUPPORT_WINMM
        #define MAL_SUPPORT_JACK    // JACK is technically supported on Windows, but I don't know how many people use it in practice...
    #endif
#endif
#if defined(MAL_UNIX)
    #if defined(MAL_LINUX)
        #if !defined(MAL_ANDROID)   // ALSA is not supported on Android.
            #define MAL_SUPPORT_ALSA
        #endif
    #endif
    #if !defined(MAL_ANDROID) && !defined(MAL_EMSCRIPTEN)
        #define MAL_SUPPORT_PULSEAUDIO
        #define MAL_SUPPORT_JACK
    #endif
    #if defined(MAL_APPLE)
        #define MAL_SUPPORT_COREAUDIO
    #endif
    #if defined(MAL_ANDROID)
        #define MAL_SUPPORT_OPENSL
    #endif
    #if !defined(MAL_LINUX) && !defined(MAL_APPLE) && !defined(MAL_ANDROID) && !defined(MAL_EMSCRIPTEN)
        #define MAL_SUPPORT_OSS
    #endif
#endif

#define MAL_SUPPORT_SDL     // All platforms support SDL.

// Explicitly disable OpenAL and Null backends for Emscripten because they both use a background thread which is not properly supported right now.
#if !defined(MAL_EMSCRIPTEN)
#define MAL_SUPPORT_OPENAL
#define MAL_SUPPORT_NULL    // All platforms support the null backend.
#endif


#if !defined(MAL_NO_WASAPI) && defined(MAL_SUPPORT_WASAPI)
    #define MAL_ENABLE_WASAPI
#endif
#if !defined(MAL_NO_DSOUND) && defined(MAL_SUPPORT_DSOUND)
    #define MAL_ENABLE_DSOUND
#endif
#if !defined(MAL_NO_WINMM) && defined(MAL_SUPPORT_WINMM)
    #define MAL_ENABLE_WINMM
#endif
#if !defined(MAL_NO_ALSA) && defined(MAL_SUPPORT_ALSA)
    #define MAL_ENABLE_ALSA
#endif
#if !defined(MAL_NO_PULSEAUDIO) && defined(MAL_SUPPORT_PULSEAUDIO)
    #define MAL_ENABLE_PULSEAUDIO
#endif
#if !defined(MAL_NO_JACK) && defined(MAL_SUPPORT_JACK)
    #define MAL_ENABLE_JACK
#endif
#if !defined(MAL_NO_COREAUDIO) && defined(MAL_SUPPORT_COREAUDIO)
    #define MAL_ENABLE_COREAUDIO
#endif
#if !defined(MAL_NO_OSS) && defined(MAL_SUPPORT_OSS)
    #define MAL_ENABLE_OSS
#endif
#if !defined(MAL_NO_OPENSL) && defined(MAL_SUPPORT_OPENSL)
    #define MAL_ENABLE_OPENSL
#endif
#if !defined(MAL_NO_OPENAL) && defined(MAL_SUPPORT_OPENAL)
    #define MAL_ENABLE_OPENAL
#endif
#if !defined(MAL_NO_SDL) && defined(MAL_SUPPORT_SDL)
    #define MAL_ENABLE_SDL
#endif
#if !defined(MAL_NO_NULL) && defined(MAL_SUPPORT_NULL)
    #define MAL_ENABLE_NULL
#endif

#include <stddef.h> // For size_t.

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef   signed char    mal_int8;
typedef unsigned char    mal_uint8;
typedef   signed short   mal_int16;
typedef unsigned short   mal_uint16;
typedef   signed int     mal_int32;
typedef unsigned int     mal_uint32;
typedef   signed __int64 mal_int64;
typedef unsigned __int64 mal_uint64;
#else
#include <stdint.h>
typedef int8_t           mal_int8;
typedef uint8_t          mal_uint8;
typedef int16_t          mal_int16;
typedef uint16_t         mal_uint16;
typedef int32_t          mal_int32;
typedef uint32_t         mal_uint32;
typedef int64_t          mal_int64;
typedef uint64_t         mal_uint64;
#endif
typedef mal_uint8        mal_bool8;
typedef mal_uint32       mal_bool32;
#define MAL_TRUE         1
#define MAL_FALSE        0

typedef void* mal_handle;
typedef void* mal_ptr;
typedef void (* mal_proc)();

typedef struct mal_context mal_context;
typedef struct mal_device mal_device;

// Thread priorties should be ordered such that the default priority of the worker thread is 0.
typedef enum
{
    mal_thread_priority_idle     = -5,
    mal_thread_priority_lowest   = -4,
    mal_thread_priority_low      = -3,
    mal_thread_priority_normal   = -2,
    mal_thread_priority_high     = -1,
    mal_thread_priority_highest  =  0,
    mal_thread_priority_realtime =  1,
    mal_thread_priority_default  =  0
} mal_thread_priority;

typedef struct
{
    mal_context* pContext;

    union
    {
#ifdef MAL_WIN32
        struct
        {
            /*HANDLE*/ mal_handle hThread;
        } win32;
#endif
#ifdef MAL_POSIX
        struct
        {
            pthread_t thread;
        } posix;
#endif

        int _unused;
    };
} mal_thread;

typedef struct
{
    mal_context* pContext;

    union
    {
#ifdef MAL_WIN32
        struct
        {
            /*HANDLE*/ mal_handle hMutex;
        } win32;
#endif
#ifdef MAL_POSIX
        struct
        {
            pthread_mutex_t mutex;
        } posix;
#endif

        int _unused;
    };
} mal_mutex;

typedef struct
{
    mal_context* pContext;

    union
    {
#ifdef MAL_WIN32
        struct
        {
            /*HANDLE*/ mal_handle hEvent;
        } win32;
#endif
#ifdef MAL_POSIX
        struct
        {
            pthread_mutex_t mutex;
            pthread_cond_t condition;
            mal_uint32 value;
        } posix;
#endif

        int _unused;
    };
} mal_event;

#if defined(_MSC_VER) && !defined(_WCHAR_T_DEFINED)
typedef mal_uint16 wchar_t;
#endif

// Define NULL for some compilers.
#ifndef NULL
#define NULL 0
#endif

#define MAL_MAX_PERIODS_DSOUND                          4
#define MAL_MAX_PERIODS_OPENAL                          4

// Standard sample rates.
#define MAL_SAMPLE_RATE_8000                            8000
#define MAL_SAMPLE_RATE_11025                           11025
#define MAL_SAMPLE_RATE_16000                           16000
#define MAL_SAMPLE_RATE_22050                           22050
#define MAL_SAMPLE_RATE_24000                           24000
#define MAL_SAMPLE_RATE_32000                           32000
#define MAL_SAMPLE_RATE_44100                           44100
#define MAL_SAMPLE_RATE_48000                           48000
#define MAL_SAMPLE_RATE_88200                           88200
#define MAL_SAMPLE_RATE_96000                           96000
#define MAL_SAMPLE_RATE_176400                          176400
#define MAL_SAMPLE_RATE_192000                          192000
#define MAL_SAMPLE_RATE_352800                          352800
#define MAL_SAMPLE_RATE_384000                          384000

#define MAL_MIN_PCM_SAMPLE_SIZE_IN_BYTES                1   // For simplicity, mini_al does not support PCM samples that are not byte aligned.
#define MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES                8
#define MAL_MIN_CHANNELS                                1
#define MAL_MAX_CHANNELS                                32
#define MAL_MIN_SAMPLE_RATE                             MAL_SAMPLE_RATE_8000
#define MAL_MAX_SAMPLE_RATE                             MAL_SAMPLE_RATE_384000

typedef mal_uint8 mal_channel;
#define MAL_CHANNEL_NONE                                0
#define MAL_CHANNEL_MONO                                1
#define MAL_CHANNEL_FRONT_LEFT                          2
#define MAL_CHANNEL_FRONT_RIGHT                         3
#define MAL_CHANNEL_FRONT_CENTER                        4
#define MAL_CHANNEL_LFE                                 5
#define MAL_CHANNEL_BACK_LEFT                           6
#define MAL_CHANNEL_BACK_RIGHT                          7
#define MAL_CHANNEL_FRONT_LEFT_CENTER                   8
#define MAL_CHANNEL_FRONT_RIGHT_CENTER                  9
#define MAL_CHANNEL_BACK_CENTER                         10
#define MAL_CHANNEL_SIDE_LEFT                           11
#define MAL_CHANNEL_SIDE_RIGHT                          12
#define MAL_CHANNEL_TOP_CENTER                          13
#define MAL_CHANNEL_TOP_FRONT_LEFT                      14
#define MAL_CHANNEL_TOP_FRONT_CENTER                    15
#define MAL_CHANNEL_TOP_FRONT_RIGHT                     16
#define MAL_CHANNEL_TOP_BACK_LEFT                       17
#define MAL_CHANNEL_TOP_BACK_CENTER                     18
#define MAL_CHANNEL_TOP_BACK_RIGHT                      19
#define MAL_CHANNEL_AUX_0                               20
#define MAL_CHANNEL_AUX_1                               21
#define MAL_CHANNEL_AUX_2                               22
#define MAL_CHANNEL_AUX_3                               23
#define MAL_CHANNEL_AUX_4                               24
#define MAL_CHANNEL_AUX_5                               25
#define MAL_CHANNEL_AUX_6                               26
#define MAL_CHANNEL_AUX_7                               27
#define MAL_CHANNEL_AUX_8                               28
#define MAL_CHANNEL_AUX_9                               29
#define MAL_CHANNEL_AUX_10                              30
#define MAL_CHANNEL_AUX_11                              31
#define MAL_CHANNEL_AUX_12                              32
#define MAL_CHANNEL_AUX_13                              33
#define MAL_CHANNEL_AUX_14                              34
#define MAL_CHANNEL_AUX_15                              35
#define MAL_CHANNEL_AUX_16                              36
#define MAL_CHANNEL_AUX_17                              37
#define MAL_CHANNEL_AUX_18                              38
#define MAL_CHANNEL_AUX_19                              39
#define MAL_CHANNEL_AUX_20                              40
#define MAL_CHANNEL_AUX_21                              41
#define MAL_CHANNEL_AUX_22                              42
#define MAL_CHANNEL_AUX_23                              43
#define MAL_CHANNEL_AUX_24                              44
#define MAL_CHANNEL_AUX_25                              45
#define MAL_CHANNEL_AUX_26                              46
#define MAL_CHANNEL_AUX_27                              47
#define MAL_CHANNEL_AUX_28                              48
#define MAL_CHANNEL_AUX_29                              49
#define MAL_CHANNEL_AUX_30                              50
#define MAL_CHANNEL_AUX_31                              51
#define MAL_CHANNEL_LEFT                                MAL_CHANNEL_FRONT_LEFT
#define MAL_CHANNEL_RIGHT                               MAL_CHANNEL_FRONT_RIGHT

typedef int mal_result;
#define MAL_SUCCESS                                      0
#define MAL_ERROR                                       -1      // A generic error.
#define MAL_INVALID_ARGS                                -2
#define MAL_OUT_OF_MEMORY                               -3
#define MAL_FORMAT_NOT_SUPPORTED                        -4
#define MAL_NO_BACKEND                                  -5
#define MAL_NO_DEVICE                                   -6
#define MAL_API_NOT_FOUND                               -7
#define MAL_DEVICE_BUSY                                 -8
#define MAL_DEVICE_NOT_INITIALIZED                      -9
#define MAL_DEVICE_ALREADY_STARTED                      -10
#define MAL_DEVICE_ALREADY_STARTING                     -11
#define MAL_DEVICE_ALREADY_STOPPED                      -12
#define MAL_DEVICE_ALREADY_STOPPING                     -13
#define MAL_FAILED_TO_MAP_DEVICE_BUFFER                 -14
#define MAL_FAILED_TO_UNMAP_DEVICE_BUFFER               -15
#define MAL_FAILED_TO_INIT_BACKEND                      -16
#define MAL_FAILED_TO_READ_DATA_FROM_CLIENT             -17
#define MAL_FAILED_TO_READ_DATA_FROM_DEVICE             -18
#define MAL_FAILED_TO_SEND_DATA_TO_CLIENT               -19
#define MAL_FAILED_TO_SEND_DATA_TO_DEVICE               -20
#define MAL_FAILED_TO_OPEN_BACKEND_DEVICE               -21
#define MAL_FAILED_TO_START_BACKEND_DEVICE              -22
#define MAL_FAILED_TO_STOP_BACKEND_DEVICE               -23
#define MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE          -24
#define MAL_FAILED_TO_CREATE_MUTEX                      -25
#define MAL_FAILED_TO_CREATE_EVENT                      -26
#define MAL_FAILED_TO_CREATE_THREAD                     -27
#define MAL_INVALID_DEVICE_CONFIG                       -28
#define MAL_ACCESS_DENIED                               -29

typedef void       (* mal_log_proc) (mal_context* pContext, mal_device* pDevice, const char* message);
typedef void       (* mal_recv_proc)(mal_device* pDevice, mal_uint32 frameCount, const void* pSamples);
typedef mal_uint32 (* mal_send_proc)(mal_device* pDevice, mal_uint32 frameCount, void* pSamples);
typedef void       (* mal_stop_proc)(mal_device* pDevice);

typedef enum
{
    mal_backend_null,
    mal_backend_wasapi,
    mal_backend_dsound,
    mal_backend_winmm,
    mal_backend_alsa,
    mal_backend_pulseaudio,
    mal_backend_jack,
    mal_backend_oss,
    mal_backend_opensl,
    mal_backend_openal,
    mal_backend_sdl
} mal_backend;

typedef enum
{
    mal_device_type_playback,
    mal_device_type_capture
} mal_device_type;

typedef enum
{
    mal_share_mode_shared = 0,
    mal_share_mode_exclusive,
} mal_share_mode;

typedef enum
{
    mal_stream_format_pcm = 0,
} mal_stream_format;

typedef enum
{
    mal_stream_layout_interleaved = 0,
    mal_stream_layout_deinterleaved
} mal_stream_layout;

typedef enum
{
    mal_dither_mode_none = 0,
    //mal_dither_mode_triangle
} mal_dither_mode;

typedef enum
{
    // I like to keep these explicitly defined because they're used as a key into a lookup table. When items are
    // added to this, make sure there are no gaps and that they're added to the lookup table in mal_get_sample_size_in_bytes().
    mal_format_unknown = 0,     // Mainly used for indicating an error, but also used as the default for the output format for decoders.
    mal_format_u8      = 1,
    mal_format_s16     = 2,     // Seems to be the most widely supported format.
    mal_format_s24     = 3,     // Tightly packed. 3 bytes per sample.
    mal_format_s32     = 4,
    mal_format_f32     = 5,
} mal_format;

typedef enum
{
    mal_channel_mix_mode_basic,     // Drop excess channels; zeroed out extra channels.
    mal_channel_mix_mode_blend,     // Blend channels based on locality.
} mal_channel_mix_mode;

typedef enum
{
    mal_standard_channel_map_microsoft,
    mal_standard_channel_map_alsa,
    mal_standard_channel_map_rfc3551,   // Based off AIFF.
    mal_standard_channel_map_flac,
    mal_standard_channel_map_vorbis,
    mal_standard_channel_map_default = mal_standard_channel_map_microsoft
} mal_standard_channel_map;

typedef union
{
#ifdef MAL_SUPPORT_WASAPI
    wchar_t wasapi[64];             // WASAPI uses a wchar_t string for identification.
#endif
#ifdef MAL_SUPPORT_DSOUND
    mal_uint8 dsound[16];           // DirectSound uses a GUID for identification.
#endif
#ifdef MAL_SUPPORT_WINMM
    /*UINT_PTR*/ mal_uint32 winmm;  // When creating a device, WinMM expects a Win32 UINT_PTR for device identification. In practice it's actually just a UINT.
#endif
#ifdef MAL_SUPPORT_ALSA
    char alsa[256];                 // ALSA uses a name string for identification.
#endif
#ifdef MAL_SUPPORT_PULSEAUDIO
    char pulse[256];                // PulseAudio uses a name string for identification.
#endif
#ifdef MAL_SUPPORT_JACK
    int jack;                       // JACK always uses default devices.
#endif
#ifdef MAL_SUPPORT_COREAUDIO
    // TODO: Implement me.
#endif
#ifdef MAL_SUPPORT_OSS
    char oss[64];                   // "dev/dsp0", etc. "dev/dsp" for the default device.
#endif
#ifdef MAL_SUPPORT_OPENSL
    mal_uint32 opensl;              // OpenSL|ES uses a 32-bit unsigned integer for identification.
#endif
#ifdef MAL_SUPPORT_OPENAL
    char openal[256];               // OpenAL seems to use human-readable device names as the ID.
#endif
#ifdef MAL_SUPPORT_SDL
    int sdl;                        // SDL devices are identified with an index.
#endif
#ifdef MAL_SUPPORT_NULL
    int nullbackend;                // The null backend uses an integer for device IDs.
#endif
} mal_device_id;

typedef struct
{
    // Basic info. This is the only information guaranteed to be filled in during device enumeration.
    mal_device_id id;
    char name[256];

    // Detailed info. As much of this is filled as possible with mal_context_get_device_info().
    // TODO: Implement me.
} mal_device_info;

typedef struct
{
    mal_int64 counter;
} mal_timer;



typedef struct mal_format_converter mal_format_converter;
typedef mal_uint32 (* mal_format_converter_read_proc)          (mal_format_converter* pConverter, mal_uint32 frameCount, void* pFramesOut, void* pUserData);
typedef mal_uint32 (* mal_format_converter_read_separated_proc)(mal_format_converter* pConverter, mal_uint32 frameCount, void** ppSamplesOut, void* pUserData);

typedef struct
{
    mal_format formatIn;
    mal_format formatOut;
    mal_uint32 channels;
    mal_stream_format streamFormatIn;
    mal_stream_format streamFormatOut;
    mal_dither_mode ditherMode;
} mal_format_converter_config;

struct mal_format_converter
{
    mal_format_converter_config config;
    mal_format_converter_read_proc onRead;
    mal_format_converter_read_separated_proc onReadSeparated;
    void* pUserData;
    void (* onConvertPCM)(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode);
    void (* onInterleavePCM)(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels);
    void (* onDeinterleavePCM)(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels);
};



typedef struct mal_src mal_src;
typedef mal_uint32 (* mal_src_read_proc)(mal_src* pSRC, mal_uint32 frameCount, void* pFramesOut, void* pUserData); // Returns the number of frames that were read.

typedef enum
{
    mal_src_algorithm_none,
    mal_src_algorithm_linear
} mal_src_algorithm;

#define MAL_SRC_CACHE_SIZE_IN_FRAMES    512
typedef struct
{
    mal_src* pSRC;
    float pCachedFrames[MAL_MAX_CHANNELS * MAL_SRC_CACHE_SIZE_IN_FRAMES];
    mal_uint32 cachedFrameCount;
    mal_uint32 iNextFrame;
} mal_src_cache;

typedef struct
{
    mal_uint32 sampleRateIn;
    mal_uint32 sampleRateOut;
    mal_format formatIn;
    mal_format formatOut;
    mal_uint32 channels;
    mal_src_algorithm algorithm;
    mal_uint32 cacheSizeInFrames;  // The number of frames to read from the client at a time.
} mal_src_config;

struct mal_src
{
    mal_src_config config;
    mal_src_read_proc onRead;
    void* pUserData;
    float bin[256];
    mal_src_cache cache;    // <-- For simplifying and optimizing client -> memory reading.

    union
    {
        struct
        {
            float alpha;
            mal_bool32 isPrevFramesLoaded : 1;
            mal_bool32 isNextFramesLoaded : 1;
        } linear;
    };
};

typedef struct mal_dsp mal_dsp;
typedef mal_uint32 (* mal_dsp_read_proc)(mal_dsp* pDSP, mal_uint32 frameCount, void* pSamplesOut, void* pUserData);

typedef struct
{
    mal_format  formatIn;
    mal_uint32  channelsIn;
    mal_uint32  sampleRateIn;
    mal_channel channelMapIn[MAL_MAX_CHANNELS];
    mal_format  formatOut;
    mal_uint32  channelsOut;
    mal_uint32  sampleRateOut;
    mal_channel channelMapOut[MAL_MAX_CHANNELS];
    mal_uint32  cacheSizeInFrames;  // Applications should set this to 0 for now.
} mal_dsp_config;

struct mal_dsp
{
    mal_dsp_config config;
    mal_dsp_read_proc onRead;
    void* pUserDataForOnRead;
    mal_format_converter formatConverter;
    mal_src src;                            // For sample rate conversion.
    mal_channel channelMapInPostMix[MAL_MAX_CHANNELS];   // <-- When mixing, new channels may need to be created. This represents the channel map after mixing.
    mal_channel channelShuffleTable[MAL_MAX_CHANNELS];
    mal_bool32 isChannelMappingRequired : 1;
    mal_bool32 isSRCRequired : 1;
    mal_bool32 isPassthrough : 1;           // <-- Will be set to true when the DSP pipeline is an optimized passthrough.
};


typedef struct
{
    mal_format format;
    mal_uint32 channels;
    mal_uint32 sampleRate;
    mal_channel channelMap[MAL_MAX_CHANNELS];
    mal_uint32 bufferSizeInFrames;
    mal_uint32 periods;
    mal_share_mode shareMode;
    mal_recv_proc onRecvCallback;
    mal_send_proc onSendCallback;
    mal_stop_proc onStopCallback;

    struct
    {
        mal_bool32 noMMap;  // Disables MMap mode.
    } alsa;

    struct
    {
        const char* pStreamName;
    } pulse;
} mal_device_config;

typedef struct
{
    mal_log_proc onLog;
    mal_thread_priority threadPriority;

    struct
    {
        mal_bool32 useVerboseDeviceEnumeration;
    } alsa;

    struct
    {
        const char* pApplicationName;
        const char* pServerName;
        mal_bool32 tryAutoSpawn; // Enables autospawning of the PulseAudio daemon if necessary.
    } pulse;

    struct
    {
        const char* pClientName;
        mal_bool32 tryStartServer;
    } jack;
} mal_context_config;

typedef mal_bool32 (* mal_enum_devices_callback_proc)(mal_context* pContext, mal_device_type type, const mal_device_info* pInfo, void* pUserData);

struct mal_context
{
    mal_backend backend;                    // DirectSound, ALSA, etc.
    mal_context_config config;
    mal_mutex deviceEnumLock;               // Used to make mal_context_get_devices() thread safe.
    mal_mutex deviceInfoLock;               // Used to make mal_context_get_device_info() thread safe.
    mal_uint32 deviceInfoCapacity;          // Total capacity of pDeviceInfos.
    mal_uint32 playbackDeviceInfoCount;
    mal_uint32 captureDeviceInfoCount;
    mal_device_info* pDeviceInfos;          // Playback devices first, then capture.

    mal_bool32 (* onDeviceIDEqual)(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1);
    mal_result (* onEnumDevices  )(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData);    // Return false from the callback to stop enumeration.
    mal_result (* onGetDeviceInfo)(mal_context* pContext, mal_device_type type, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo);

    union
    {
#ifdef MAL_SUPPORT_WASAPI
        struct
        {
            int _unused;
        } wasapi;
#endif
#ifdef MAL_SUPPORT_DSOUND
        struct
        {
            /*HMODULE*/ mal_handle hDSoundDLL;
            mal_proc DirectSoundCreate;
            mal_proc DirectSoundEnumerateA;
            mal_proc DirectSoundCaptureCreate;
            mal_proc DirectSoundCaptureEnumerateA;
        } dsound;
#endif
#ifdef MAL_SUPPORT_WINMM
        struct
        {
            /*HMODULE*/ mal_handle hWinMM;
            mal_proc waveOutGetNumDevs;
            mal_proc waveOutGetDevCapsA;
            mal_proc waveOutOpen;
            mal_proc waveOutClose;
            mal_proc waveOutPrepareHeader;
            mal_proc waveOutUnprepareHeader;
            mal_proc waveOutWrite;
            mal_proc waveOutReset;
            mal_proc waveInGetNumDevs;
            mal_proc waveInGetDevCapsA;
            mal_proc waveInOpen;
            mal_proc waveInClose;
            mal_proc waveInPrepareHeader;
            mal_proc waveInUnprepareHeader;
            mal_proc waveInAddBuffer;
            mal_proc waveInStart;
            mal_proc waveInReset;
        } winmm;
#endif
#ifdef MAL_SUPPORT_ALSA
        struct
        {
            mal_handle asoundSO;
            mal_proc snd_pcm_open;
            mal_proc snd_pcm_close;
            mal_proc snd_pcm_hw_params_sizeof;
            mal_proc snd_pcm_hw_params_any;
            mal_proc snd_pcm_hw_params_set_format;
            mal_proc snd_pcm_hw_params_set_format_first;
            mal_proc snd_pcm_hw_params_get_format_mask;
            mal_proc snd_pcm_hw_params_set_channels_near;
            mal_proc snd_pcm_hw_params_set_rate_resample;
            mal_proc snd_pcm_hw_params_set_rate_near;
            mal_proc snd_pcm_hw_params_set_buffer_size_near;
            mal_proc snd_pcm_hw_params_set_periods_near;
            mal_proc snd_pcm_hw_params_set_access;
            mal_proc snd_pcm_hw_params_get_format;
            mal_proc snd_pcm_hw_params_get_channels;
            mal_proc snd_pcm_hw_params_get_rate;
            mal_proc snd_pcm_hw_params_get_buffer_size;
            mal_proc snd_pcm_hw_params_get_periods;
            mal_proc snd_pcm_hw_params_get_access;
            mal_proc snd_pcm_hw_params;
            mal_proc snd_pcm_sw_params_sizeof;
            mal_proc snd_pcm_sw_params_current;
            mal_proc snd_pcm_sw_params_set_avail_min;
            mal_proc snd_pcm_sw_params_set_start_threshold;
            mal_proc snd_pcm_sw_params;
            mal_proc snd_pcm_format_mask_sizeof;
            mal_proc snd_pcm_format_mask_test;
            mal_proc snd_pcm_get_chmap;
            mal_proc snd_pcm_prepare;
            mal_proc snd_pcm_start;
            mal_proc snd_pcm_drop;
            mal_proc snd_device_name_hint;
            mal_proc snd_device_name_get_hint;
            mal_proc snd_card_get_index;
            mal_proc snd_device_name_free_hint;
            mal_proc snd_pcm_mmap_begin;
            mal_proc snd_pcm_mmap_commit;
            mal_proc snd_pcm_recover;
            mal_proc snd_pcm_readi;
            mal_proc snd_pcm_writei;
            mal_proc snd_pcm_avail;
            mal_proc snd_pcm_avail_update;
            mal_proc snd_pcm_wait;
            mal_proc snd_pcm_info;
            mal_proc snd_pcm_info_sizeof;
            mal_proc snd_pcm_info_get_name;
            mal_proc snd_config_update_free_global;

            mal_mutex internalDeviceEnumLock;
        } alsa;
#endif
#ifdef MAL_SUPPORT_PULSEAUDIO
        struct
        {
            mal_handle pulseSO;
            mal_proc pa_mainloop_new;
            mal_proc pa_mainloop_free;
            mal_proc pa_mainloop_get_api;
            mal_proc pa_mainloop_iterate;
            mal_proc pa_mainloop_wakeup;
            mal_proc pa_context_new;
            mal_proc pa_context_unref;
            mal_proc pa_context_connect;
            mal_proc pa_context_disconnect;
            mal_proc pa_context_set_state_callback;
            mal_proc pa_context_get_state;
            mal_proc pa_context_get_sink_info_list;
            mal_proc pa_context_get_source_info_list;
            mal_proc pa_context_get_sink_info_by_name;
            mal_proc pa_context_get_source_info_by_name;
            mal_proc pa_operation_unref;
            mal_proc pa_operation_get_state;
            mal_proc pa_channel_map_init_extend;
            mal_proc pa_channel_map_valid;
            mal_proc pa_channel_map_compatible;
            mal_proc pa_stream_new;
            mal_proc pa_stream_unref;
            mal_proc pa_stream_connect_playback;
            mal_proc pa_stream_connect_record;
            mal_proc pa_stream_disconnect;
            mal_proc pa_stream_get_state;
            mal_proc pa_stream_get_sample_spec;
            mal_proc pa_stream_get_channel_map;
            mal_proc pa_stream_get_buffer_attr;
            mal_proc pa_stream_get_device_name;
            mal_proc pa_stream_set_write_callback;
            mal_proc pa_stream_set_read_callback;
            mal_proc pa_stream_flush;
            mal_proc pa_stream_drain;
            mal_proc pa_stream_cork;
            mal_proc pa_stream_trigger;
            mal_proc pa_stream_begin_write;
            mal_proc pa_stream_write;
            mal_proc pa_stream_peek;
            mal_proc pa_stream_drop;
        } pulse;
#endif
#ifdef MAL_SUPPORT_JACK
        struct
        {
            mal_handle jackSO;
            mal_proc jack_client_open;
            mal_proc jack_client_close;
            mal_proc jack_client_name_size;
            mal_proc jack_set_process_callback;
            mal_proc jack_set_buffer_size_callback;
            mal_proc jack_on_shutdown;
            mal_proc jack_get_sample_rate;
            mal_proc jack_get_buffer_size;
            mal_proc jack_get_ports;
            mal_proc jack_activate;
            mal_proc jack_deactivate;
            mal_proc jack_connect;
            mal_proc jack_port_register;
            mal_proc jack_port_name;
            mal_proc jack_port_get_buffer;
            mal_proc jack_free;
        } jack;
#endif
#ifdef MAL_SUPPORT_COREAUDIO
        struct
        {
            int _unused;
        } coreaudio;
#endif
#ifdef MAL_SUPPORT_OSS
        struct
        {
            int versionMajor;
            int versionMinor;
        } oss;
#endif
#ifdef MAL_SUPPORT_OPENSL
        struct
        {
            int _unused;
        } opensl;
#endif
#ifdef MAL_SUPPORT_OPENAL
        struct
        {
            /*HMODULE*/ mal_handle hOpenAL;     // OpenAL32.dll, etc.
            mal_proc alcCreateContext;
            mal_proc alcMakeContextCurrent;
            mal_proc alcProcessContext;
            mal_proc alcSuspendContext;
            mal_proc alcDestroyContext;
            mal_proc alcGetCurrentContext;
            mal_proc alcGetContextsDevice;
            mal_proc alcOpenDevice;
            mal_proc alcCloseDevice;
            mal_proc alcGetError;
            mal_proc alcIsExtensionPresent;
            mal_proc alcGetProcAddress;
            mal_proc alcGetEnumValue;
            mal_proc alcGetString;
            mal_proc alcGetIntegerv;
            mal_proc alcCaptureOpenDevice;
            mal_proc alcCaptureCloseDevice;
            mal_proc alcCaptureStart;
            mal_proc alcCaptureStop;
            mal_proc alcCaptureSamples;
            mal_proc alEnable;
            mal_proc alDisable;
            mal_proc alIsEnabled;
            mal_proc alGetString;
            mal_proc alGetBooleanv;
            mal_proc alGetIntegerv;
            mal_proc alGetFloatv;
            mal_proc alGetDoublev;
            mal_proc alGetBoolean;
            mal_proc alGetInteger;
            mal_proc alGetFloat;
            mal_proc alGetDouble;
            mal_proc alGetError;
            mal_proc alIsExtensionPresent;
            mal_proc alGetProcAddress;
            mal_proc alGetEnumValue;
            mal_proc alGenSources;
            mal_proc alDeleteSources;
            mal_proc alIsSource;
            mal_proc alSourcef;
            mal_proc alSource3f;
            mal_proc alSourcefv;
            mal_proc alSourcei;
            mal_proc alSource3i;
            mal_proc alSourceiv;
            mal_proc alGetSourcef;
            mal_proc alGetSource3f;
            mal_proc alGetSourcefv;
            mal_proc alGetSourcei;
            mal_proc alGetSource3i;
            mal_proc alGetSourceiv;
            mal_proc alSourcePlayv;
            mal_proc alSourceStopv;
            mal_proc alSourceRewindv;
            mal_proc alSourcePausev;
            mal_proc alSourcePlay;
            mal_proc alSourceStop;
            mal_proc alSourceRewind;
            mal_proc alSourcePause;
            mal_proc alSourceQueueBuffers;
            mal_proc alSourceUnqueueBuffers;
            mal_proc alGenBuffers;
            mal_proc alDeleteBuffers;
            mal_proc alIsBuffer;
            mal_proc alBufferData;
            mal_proc alBufferf;
            mal_proc alBuffer3f;
            mal_proc alBufferfv;
            mal_proc alBufferi;
            mal_proc alBuffer3i;
            mal_proc alBufferiv;
            mal_proc alGetBufferf;
            mal_proc alGetBuffer3f;
            mal_proc alGetBufferfv;
            mal_proc alGetBufferi;
            mal_proc alGetBuffer3i;
            mal_proc alGetBufferiv;

            mal_bool32 isEnumerationSupported : 1;
            mal_bool32 isFloat32Supported     : 1;
            mal_bool32 isMCFormatsSupported   : 1;
        } openal;
#endif
#ifdef MAL_SUPPORT_SDL
        struct
        {
            mal_handle hSDL;    // SDL
            mal_proc SDL_InitSubSystem;
            mal_proc SDL_QuitSubSystem;
            mal_proc SDL_CloseAudio;
            mal_proc SDL_OpenAudio;
            mal_proc SDL_PauseAudio;
            mal_proc SDL_GetNumAudioDevices;
            mal_proc SDL_GetAudioDeviceName;
            mal_proc SDL_CloseAudioDevice;
            mal_proc SDL_OpenAudioDevice;
            mal_proc SDL_PauseAudioDevice;

            mal_bool32 usingSDL1;
        } sdl;
#endif
#ifdef MAL_SUPPORT_NULL
        struct
        {
            int _unused;
        } null_backend;
#endif
    };

    union
    {
#ifdef MAL_WIN32
        struct
        {
            /*HMODULE*/ mal_handle hOle32DLL;
            mal_proc CoInitializeEx;
            mal_proc CoUninitialize;
            mal_proc CoCreateInstance;
            mal_proc CoTaskMemFree;
            mal_proc PropVariantClear;
            mal_proc StringFromGUID2;

            /*HMODULE*/ mal_handle hUser32DLL;
            mal_proc GetForegroundWindow;
            mal_proc GetDesktopWindow;
        } win32;
#endif
#ifdef MAL_POSIX
        struct
        {
            mal_handle pthreadSO;
            mal_proc pthread_create;
            mal_proc pthread_join;
            mal_proc pthread_mutex_init;
            mal_proc pthread_mutex_destroy;
            mal_proc pthread_mutex_lock;
            mal_proc pthread_mutex_unlock;
            mal_proc pthread_cond_init;
            mal_proc pthread_cond_destroy;
            mal_proc pthread_cond_wait;
            mal_proc pthread_cond_signal;
            mal_proc pthread_attr_init;
            mal_proc pthread_attr_destroy;
            mal_proc pthread_attr_setschedpolicy;
            mal_proc pthread_attr_getschedparam;
            mal_proc pthread_attr_setschedparam;
        } posix;
#endif
        int _unused;
    };
};

struct mal_device
{
    mal_context* pContext;
    mal_device_type type;
    mal_format format;
    mal_uint32 channels;
    mal_uint32 sampleRate;
    mal_channel channelMap[MAL_MAX_CHANNELS];
    mal_uint32 bufferSizeInFrames;
    mal_uint32 periods;
    mal_uint32 state;
    mal_recv_proc onRecv;
    mal_send_proc onSend;
    mal_stop_proc onStop;
    void* pUserData;        // Application defined data.
    char name[256];
    mal_mutex lock;
    mal_event wakeupEvent;
    mal_event startEvent;
    mal_event stopEvent;
    mal_thread thread;
    mal_result workResult;  // This is set by the worker thread after it's finished doing a job.
    mal_bool32 usingDefaultFormat     : 1;
    mal_bool32 usingDefaultChannels   : 1;
    mal_bool32 usingDefaultSampleRate : 1;
    mal_bool32 usingDefaultChannelMap : 1;
    mal_bool32 usingDefaultBufferSize : 1;
    mal_bool32 usingDefaultPeriods    : 1;
    mal_bool32 exclusiveMode          : 1;
    mal_bool32 isOwnerOfContext       : 1;  // When set to true, uninitializing the device will also uninitialize the context. Set to true when NULL is passed into mal_device_init().
    mal_format internalFormat;
    mal_uint32 internalChannels;
    mal_uint32 internalSampleRate;
    mal_channel internalChannelMap[MAL_MAX_CHANNELS];
    mal_dsp dsp;                    // Samples run through this to convert samples to a format suitable for use by the backend.
    mal_uint32 _dspFrameCount;      // Internal use only. Used when running the device -> DSP -> client pipeline. See mal_device__on_read_from_device().
    const mal_uint8* _dspFrames;    // ^^^ AS ABOVE ^^^

    union
    {
#ifdef MAL_SUPPORT_WASAPI
        struct
        {
            /*IAudioClient**/ mal_ptr pAudioClient;
            /*IAudioRenderClient**/ mal_ptr pRenderClient;
            /*IAudioCaptureClient**/ mal_ptr pCaptureClient;
            /*HANDLE*/ mal_handle hEvent;
            /*HANDLE*/ mal_handle hStopEvent;
            mal_bool32 breakFromMainLoop;
        } wasapi;
#endif
#ifdef MAL_SUPPORT_DSOUND
        struct
        {
            /*LPDIRECTSOUND*/ mal_ptr pPlayback;
            /*LPDIRECTSOUNDBUFFER*/ mal_ptr pPlaybackPrimaryBuffer;
            /*LPDIRECTSOUNDBUFFER*/ mal_ptr pPlaybackBuffer;
            /*LPDIRECTSOUNDCAPTURE*/ mal_ptr pCapture;
            /*LPDIRECTSOUNDCAPTUREBUFFER*/ mal_ptr pCaptureBuffer;
            /*LPDIRECTSOUNDNOTIFY*/ mal_ptr pNotify;
            /*HANDLE*/ mal_handle pNotifyEvents[MAL_MAX_PERIODS_DSOUND];  // One event handle for each period.
            /*HANDLE*/ mal_handle hStopEvent;
            mal_uint32 lastProcessedFrame;      // This is circular.
            mal_bool32 breakFromMainLoop;
        } dsound;
#endif
#ifdef MAL_SUPPORT_WINMM
        struct
        {
            /*HWAVEOUT, HWAVEIN*/ mal_handle hDevice;
            /*HANDLE*/ mal_handle hEvent;
            mal_uint32 fragmentSizeInFrames;
            mal_uint32 fragmentSizeInBytes;
            mal_uint32 iNextHeader;             // [0,periods). Used as an index into pWAVEHDR.
            /*WAVEHDR**/ mal_uint8* pWAVEHDR;   // One instantiation for each period.
            mal_uint8* pIntermediaryBuffer;
            mal_uint8* _pHeapData;              // Used internally and is used for the heap allocated data for the intermediary buffer and the WAVEHDR structures.
            mal_bool32 breakFromMainLoop;
        } winmm;
#endif
#ifdef MAL_SUPPORT_ALSA
        struct
        {
            /*snd_pcm_t**/ mal_ptr pPCM;
            mal_bool32 isUsingMMap       : 1;
            mal_bool32 breakFromMainLoop : 1;
            void* pIntermediaryBuffer;
        } alsa;
#endif
#ifdef MAL_SUPPORT_PULSEAUDIO
        struct
        {
            /*pa_mainloop**/ mal_ptr pMainLoop;
            /*pa_mainloop_api**/ mal_ptr pAPI;
            /*pa_context**/ mal_ptr pPulseContext;
            /*pa_stream**/ mal_ptr pStream;
            /*pa_context_state*/ mal_uint32 pulseContextState;
            mal_uint32 fragmentSizeInBytes;
            mal_bool32 breakFromMainLoop : 1;
        } pulse;
#endif
#ifdef MAL_SUPPORT_JACK
        struct
        {
            /*jack_client_t**/ mal_ptr pClient;
            /*jack_port_t**/ mal_ptr pPorts[MAL_MAX_CHANNELS];
            float* pIntermediaryBuffer; // Typed as a float because JACK is always floating point.
        } jack;
#endif
#ifdef MAL_SUPPORT_COREAUDIO
        struct
        {
            int _unused;
        } coreaudio;
#endif
#ifdef MAL_SUPPORT_OSS
        struct
        {
            int fd;
            mal_uint32 fragmentSizeInFrames;
            mal_bool32 breakFromMainLoop;
            void* pIntermediaryBuffer;
        } oss;
#endif
#ifdef MAL_SUPPORT_OPENSL
        struct
        {
            /*SLObjectItf*/ mal_ptr pOutputMixObj;
            /*SLOutputMixItf*/ mal_ptr pOutputMix;
            /*SLObjectItf*/ mal_ptr pAudioPlayerObj;
            /*SLPlayItf*/ mal_ptr pAudioPlayer;
            /*SLObjectItf*/ mal_ptr pAudioRecorderObj;
            /*SLRecordItf*/ mal_ptr pAudioRecorder;
            /*SLAndroidSimpleBufferQueueItf*/ mal_ptr pBufferQueue;
            mal_uint32 periodSizeInFrames;
            mal_uint32 currentBufferIndex;
            mal_uint8* pBuffer;                 // This is malloc()'d and is used for storing audio data. Typed as mal_uint8 for easy offsetting.
        } opensl;
#endif
#ifdef MAL_SUPPORT_OPENAL
        struct
        {
            /*ALCcontext**/ mal_ptr pContextALC;
            /*ALCdevice**/ mal_ptr pDeviceALC;
            /*ALuint*/ mal_uint32 sourceAL;
            /*ALuint*/ mal_uint32 buffersAL[MAL_MAX_PERIODS_OPENAL];
            /*ALenum*/ mal_uint32 formatAL;
            mal_uint32 subBufferSizeInFrames;   // This is the size of each of the OpenAL buffers (buffersAL).
            mal_uint8* pIntermediaryBuffer;     // This is malloc()'d and is used as the destination for reading from the client. Typed as mal_uint8 for easy offsetting.
            mal_uint32 iNextBuffer;             // The next buffer to unenqueue and then re-enqueue as new data is read.
            mal_bool32 breakFromMainLoop;
        } openal;
#endif
#ifdef MAL_SUPPORT_SDL
        struct
        {
            mal_uint32 deviceID;
        } sdl;
#endif
#ifdef MAL_SUPPORT_NULL
        struct
        {
            mal_timer timer;
            mal_uint32 lastProcessedFrame;      // This is circular.
            mal_bool32 breakFromMainLoop;
            mal_uint8* pBuffer;                 // This is malloc()'d and is used as the destination for reading from the client. Typed as mal_uint8 for easy offsetting.
        } null_device;
#endif
    };
};
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

// Initializes a context.
//
// The context is used for selecting and initializing the relevant backends.
//
// Note that the location of the context cannot change throughout it's lifetime. Consider allocating
// the mal_context object with malloc() if this is an issue. The reason for this is that a pointer
// to the context is stored in the mal_device structure.
//
// <backends> is used to allow the application to prioritize backends depending on it's specific
// requirements. This can be null in which case it uses the default priority, which is as follows:
//   - WASAPI
//   - DirectSound
//   - WinMM
//   - OSS
//   - PulseAudio
//   - ALSA
//   - JACK
//   - OpenSL|ES
//   - OpenAL
//   - SDL
//   - Null
//
// <pConfig> is used to configure the context. Use the onLog config to set a callback for whenever a
// log message is posted. The priority of the worker thread can be set with the threadPriority config.
//
// It is recommended that only a single context is active at any given time because it's a bulky data
// structure which performs run-time linking for the relevant backends every time it's initialized.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: UNSAFE
mal_result mal_context_init(const mal_backend backends[], mal_uint32 backendCount, const mal_context_config* pConfig, mal_context* pContext);

// Uninitializes a context.
//
// Results are undefined if you call this while any device created by this context is still active.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: UNSAFE
mal_result mal_context_uninit(mal_context* pContext);

// Enumerates over every device (both playback and capture).
//
// This is a lower-level enumeration function to the easier to use mal_context_get_devices(). Use
// mal_context_enumerate_devices() if you would rather not incur an internal heap allocation, or
// it simply suits your code better.
//
// Do _not_ assume the first enumerated device or a given type is the default device.
//
// Some backends and platforms may only support default playback and capture devices.
//
// Note that this only retrieves the ID and name/description of the device. The reason for only
// retrieving basic information is that it would otherwise require opening the backend device in
// order to probe it for more detailed information which can be inefficient. Consider using
// mal_context_get_device_info() for this, but don't call it from within the enumeration callback.
//
// In general, you should not do anything complicated from within the callback. In particular, do
// not try initializing a device from within the callback.
//
// Consider using mal_context_get_devices() for a simpler and safer API, albeit at the expense of
// an internal heap allocation.
//
// Returning false from the callback will stop enumeration. Returning true will continue enumeration.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: SAFE
//   This is guarded using a simple mutex lock.
mal_result mal_context_enumerate_devices(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData);

// Retrieves basic information about every active playback and/or capture device.
//
// You can pass in NULL for the playback or capture lists in which case they'll be ignored.
//
// It is _not_ safe to assume the first device in the list is the default device.
//
// The returned pointers will become invalid upon the next call this this function, or when the
// context is uninitialized. Do not free the returned pointers.
//
// This function follows the same enumeration rules as mal_context_enumerate_devices(). See
// documentation for mal_context_enumerate_devices() for more information.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: SAFE
//   Since each call to this function invalidates the pointers from the previous call, you
//   should not be calling this simultaneously across multiple threads. Instead, you need to
//   make a copy of the returned data with your own higher level synchronization.
mal_result mal_context_get_devices(mal_context* pContext, mal_device_info** ppPlaybackDeviceInfos, mal_uint32* pPlaybackDeviceCount, mal_device_info** ppCaptureDeviceInfos, mal_uint32* pCaptureDeviceCount);

// Retrieves information about a device with the given ID.
//
// Do _not_ call this from within the mal_context_enumerate_devices_callback().
//
// It's possible for a device to have different information and capabilities depending on wether or
// not it's opened in shared or exclusive mode. For example, in shared mode, WASAPI always uses
// floating point samples for mixing, but in exclusive mode it can be anything. Therefore, this
// function allows you to specify which share mode you want information for. Note that not all
// backends and devices support shared or exclusive mode, in which case this function will fail
// if the requested share mode is unsupported.
//
// This leaves pDeviceInfo unmodified in the result of an error.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: SAFE
//   This is guarded using a simple mutex lock.
mal_result mal_context_get_device_info(mal_context* pContext, mal_device_type type, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo);

// Initializes a device.
//
// The context can be null in which case it uses the default. This is equivalent to passing in a
// context that was initialized like so:
//
//     mal_context_init(NULL, 0, NULL, &context);
//
// Do not pass in null for the context if you are needing to open multiple devices. You can,
// however, use null when initializing the first device, and then use device.pContext for the
// initialization of other devices.
//
// The device ID (pDeviceID) can be null, in which case the default device is used. Otherwise, you
// can retrieve the ID by calling mal_context_get_devices() and using the ID from the returned data.
// Set pDeviceID to NULL to use the default device. Do _not_ rely on the first device ID returned
// by mal_context_enumerate_devices() or mal_context_get_devices() to be the default device.
//
// The device's configuration is controlled with pConfig. This allows you to configure the sample
// format, channel count, sample rate, etc. Before calling mal_device_init(), you will most likely
// want to initialize a mal_device_config object using mal_device_config_init(),
// mal_device_config_init_playback(), etc. You can also pass in NULL for the device config in
// which case it will use defaults, but will require you to call mal_device_set_recv_callback() or
// mal_device_set_send_callback() before starting the device.
//
// Passing in 0 to any property in pConfig will force the use of a default value. In the case of
// sample format, channel count, sample rate and channel map it will default to the values used by
// the backend's internal device. If <bufferSizeInFrames> is 0, it will default to
// MAL_DEFAULT_BUFFER_SIZE_IN_MILLISECONDS. If <periods> is set to 0 it will default to
// MAL_DEFAULT_PERIODS.
//
// When sending or receiving data to/from a device, mini_al will internally perform a format
// conversion to convert between the format specified by pConfig and the format used internally by
// the backend. If you pass in NULL for pConfig or 0 for the sample format, channel count,
// sample rate _and_ channel map, data transmission will run on an optimized pass-through fast path.
//
// The <periods> property controls how frequently the background thread is woken to check for more
// data. It's tied to the buffer size, so as an example, if your buffer size is equivalent to 10
// milliseconds and you have 2 periods, the CPU will wake up approximately every 5 milliseconds.
//
// When compiling for UWP you must ensure you call this function on the main UI thread because the
// operating system may need to present the user with a message asking for permissions. Please refer
// to the official documentation for ActivateAudioInterfaceAsync() for more information.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: UNSAFE
//   It is not safe to call this function simultaneously for different devices because some backends
//   depend on and mutate global state (such as OpenSL|ES). The same applies to calling this at the
//   same time as mal_device_uninit().
mal_result mal_device_init(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, void* pUserData, mal_device* pDevice);

// Initializes a device without a context, with extra parameters for controlling the configuration
// of the internal self-managed context.
//
// See mal_device_init() and mal_context_init().
mal_result mal_device_init_ex(const mal_backend backends[], mal_uint32 backendCount, const mal_context_config* pContextConfig, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, void* pUserData, mal_device* pDevice);

// Uninitializes a device.
//
// This will explicitly stop the device. You do not need to call mal_device_stop() beforehand, but it's
// harmless if you do.
//
// Return Value:
//   MAL_SUCCESS if successful; any other error code otherwise.
//
// Thread Safety: UNSAFE
//   As soon as this API is called the device should be considered undefined. All bets are off if you
//   try using the device at the same time as uninitializing it.
void mal_device_uninit(mal_device* pDevice);

// Sets the callback to use when the application has received data from the device.
//
// Thread Safety: SAFE
//   This API is implemented as a simple atomic assignment.
void mal_device_set_recv_callback(mal_device* pDevice, mal_recv_proc proc);

// Sets the callback to use when the application needs to send data to the device for playback.
//
// Note that the implementation of this callback must copy over as many samples as is available. The
// return value specifies how many samples were written to the output buffer. The backend will fill
// any leftover samples with silence.
//
// Thread Safety: SAFE
//   This API is implemented as a simple atomic assignment.
void mal_device_set_send_callback(mal_device* pDevice, mal_send_proc proc);

// Sets the callback to use when the device has stopped, either explicitly or as a result of an error.
//
// Thread Safety: SAFE
//   This API is implemented as a simple atomic assignment.
void mal_device_set_stop_callback(mal_device* pDevice, mal_stop_proc proc);

// Activates the device. For playback devices this begins playback. For capture devices it begins
// recording.
//
// For a playback device, this will retrieve an initial chunk of audio data from the client before
// returning. The reason for this is to ensure there is valid audio data in the buffer, which needs
// to be done _before_ the device begins playback.
//
// This API waits until the backend device has been started for real by the worker thread. It also
// waits on a mutex for thread-safety.
//
// Return Value:
//   - MAL_SUCCESS if successful; any other error code otherwise.
//   - MAL_INVALID_ARGS
//       One or more of the input arguments is invalid.
//   - MAL_DEVICE_NOT_INITIALIZED
//       The device is not currently or was never initialized.
//   - MAL_DEVICE_BUSY
//       The device is in the process of stopping. This will only happen if mal_device_start() and
//       mal_device_stop() is called simultaneous on separate threads. This will never be returned in
//       single-threaded applications.
//   - MAL_DEVICE_ALREADY_STARTING
//       The device is already in the process of starting. This will never be returned in single-threaded
//       applications.
//   - MAL_DEVICE_ALREADY_STARTED
//       The device is already started.
//   - MAL_FAILED_TO_READ_DATA_FROM_CLIENT
//       Failed to read the initial chunk of audio data from the client. This initial chunk of data is
//       required so that the device has valid audio data as soon as it starts playing. This will never
//       be returned for capture devices.
//   - MAL_FAILED_TO_START_BACKEND_DEVICE
//       There was a backend-specific error starting the device.
//
// Thread Safety: SAFE
mal_result mal_device_start(mal_device* pDevice);

// Puts the device to sleep, but does not uninitialize it. Use mal_device_start() to start it up again.
//
// This API needs to wait on the worker thread to stop the backend device properly before returning. It
// also waits on a mutex for thread-safety. In addition, some backends need to wait for the device to
// finish playback/recording of the current fragment which can take some time (usually proportionate to
// the buffer size that was specified at initialization time).
//
// Return Value:
//   - MAL_SUCCESS if successful; any other error code otherwise.
//   - MAL_INVALID_ARGS
//       One or more of the input arguments is invalid.
//   - MAL_DEVICE_NOT_INITIALIZED
//       The device is not currently or was never initialized.
//   - MAL_DEVICE_BUSY
//       The device is in the process of starting. This will only happen if mal_device_start() and
//       mal_device_stop() is called simultaneous on separate threads. This will never be returned in
//       single-threaded applications.
//   - MAL_DEVICE_ALREADY_STOPPING
//       The device is already in the process of stopping. This will never be returned in single-threaded
//       applications.
//   - MAL_DEVICE_ALREADY_STOPPED
//       The device is already stopped.
//   - MAL_FAILED_TO_STOP_BACKEND_DEVICE
//       There was a backend-specific error stopping the device.
//
// Thread Safety: SAFE
mal_result mal_device_stop(mal_device* pDevice);

// Determines whether or not the device is started.
//
// This is implemented as a simple accessor.
//
// Return Value:
//   True if the device is started, false otherwise.
//
// Thread Safety: SAFE
//   If another thread calls mal_device_start() or mal_device_stop() at this same time as this function
//   is called, there's a very small chance the return value will be out of sync.
mal_bool32 mal_device_is_started(mal_device* pDevice);

// Retrieves the size of the buffer in bytes for the given device.
//
// This API is efficient and is implemented with just a few 32-bit integer multiplications.
//
// Thread Safety: SAFE
//   This is calculated from constant values which are set at initialization time and never change.
mal_uint32 mal_device_get_buffer_size_in_bytes(mal_device* pDevice);

// Retrieves the size of a sample in bytes for the given format.
//
// This API is efficient and is implemented using a lookup table.
//
// Thread Safety: SAFE
//   This is API is pure.
mal_uint32 mal_get_sample_size_in_bytes(mal_format format);
static inline mal_uint32 mal_get_bytes_per_sample(mal_format format) { return mal_get_sample_size_in_bytes(format); }
static inline mal_uint32 mal_get_bytes_per_frame(mal_format format, mal_uint32 channels) { return mal_get_bytes_per_sample(format) * channels; }

// Helper function for initializing a mal_context_config object.
mal_context_config mal_context_config_init(mal_log_proc onLog);

// Initializes a default device config.
//
// A default configuration will configure the device such that the format, channel count, sample rate and channel map are
// the same as the backend's internal configuration. This means the application loses explicit control of these properties,
// but in return gets an optimized fast path for data transmission since mini_al will be releived of all format conversion
// duties. You will not typically want to use default configurations unless you have some specific low-latency requirements.
//
// mal_device_config_init(), mal_device_config_init_playback(), etc. will allow you to explicitly set the sample format,
// channel count, etc.
mal_device_config mal_device_config_init_default();
mal_device_config mal_device_config_init_default_capture(mal_recv_proc onRecvCallback);
mal_device_config mal_device_config_init_default_playback(mal_send_proc onSendCallback);

// Helper function for initializing a mal_device_config object.
//
// This is just a helper API, and as such the returned object can be safely modified as needed.
//
// The default channel mapping is based on the channel count, as per the table below. Note that these
// can be freely changed after this function returns if you are needing something in particular.
//
// |---------------|------------------------------|
// | Channel Count | Mapping                      |
// |---------------|------------------------------|
// | 1 (Mono)      | 0: MAL_CHANNEL_MONO          |
// |---------------|------------------------------|
// | 2 (Stereo)    | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |---------------|------------------------------|
// | 3             | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |---------------|------------------------------|
// | 4 (Surround)  | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |               | 3: MAL_CHANNEL_BACK_CENTER   |
// |---------------|------------------------------|
// | 5             | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |               | 3: MAL_CHANNEL_BACK_LEFT     |
// |               | 4: MAL_CHANNEL_BACK_RIGHT    |
// |---------------|------------------------------|
// | 6 (5.1)       | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |               | 3: MAL_CHANNEL_LFE           |
// |               | 4: MAL_CHANNEL_SIDE_LEFT     |
// |               | 5: MAL_CHANNEL_SIDE_RIGHT    |
// |---------------|------------------------------|
// | 7             | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |               | 3: MAL_CHANNEL_LFE           |
// |               | 4: MAL_CHANNEL_BACK_CENTER   |
// |               | 4: MAL_CHANNEL_SIDE_LEFT     |
// |               | 5: MAL_CHANNEL_SIDE_RIGHT    |
// |---------------|------------------------------|
// | 8 (7.1)       | 0: MAL_CHANNEL_FRONT_LEFT    |
// |               | 1: MAL_CHANNEL_FRONT_RIGHT   |
// |               | 2: MAL_CHANNEL_FRONT_CENTER  |
// |               | 3: MAL_CHANNEL_LFE           |
// |               | 4: MAL_CHANNEL_BACK_LEFT     |
// |               | 5: MAL_CHANNEL_BACK_RIGHT    |
// |               | 6: MAL_CHANNEL_SIDE_LEFT     |
// |               | 7: MAL_CHANNEL_SIDE_RIGHT    |
// |---------------|------------------------------|
// | Other         | All channels set to 0. This  |
// |               | is equivalent to the same    |
// |               | mapping as the device.       |
// |---------------|------------------------------|
//
// Thread Safety: SAFE
//
// Efficiency: HIGH
//   This just returns a stack allocated object and consists of just a few assignments.
mal_device_config mal_device_config_init_ex(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_channel channelMap[MAL_MAX_CHANNELS], mal_recv_proc onRecvCallback, mal_send_proc onSendCallback);

// A simplified version of mal_device_config_init_ex().
static inline mal_device_config mal_device_config_init(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_recv_proc onRecvCallback, mal_send_proc onSendCallback) { return mal_device_config_init_ex(format, channels, sampleRate, NULL, onRecvCallback, onSendCallback); }

// A simplified version of mal_device_config_init() for capture devices.
static inline mal_device_config mal_device_config_init_capture_ex(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_channel channelMap[MAL_MAX_CHANNELS], mal_recv_proc onRecvCallback) { return mal_device_config_init_ex(format, channels, sampleRate, channelMap, onRecvCallback, NULL); }
static inline mal_device_config mal_device_config_init_capture(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_recv_proc onRecvCallback) { return mal_device_config_init_capture_ex(format, channels, sampleRate, NULL, onRecvCallback); }

// A simplified version of mal_device_config_init() for playback devices.
static inline mal_device_config mal_device_config_init_playback_ex(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_channel channelMap[MAL_MAX_CHANNELS], mal_send_proc onSendCallback) { return mal_device_config_init_ex(format, channels, sampleRate, channelMap, NULL, onSendCallback); }
static inline mal_device_config mal_device_config_init_playback(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_send_proc onSendCallback) { return mal_device_config_init_playback_ex(format, channels, sampleRate, NULL, onSendCallback); }


// Helper for retrieving a standard channel map.
void mal_get_standard_channel_map(mal_standard_channel_map standardChannelMap, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS]);



///////////////////////////////////////////////////////////////////////////////
//
// Format Conversion.
//
///////////////////////////////////////////////////////////////////////////////

// Initializes a format converter.
mal_result mal_format_converter_init(const mal_format_converter_config* pConfig, mal_format_converter_read_proc onRead, void* pUserData, mal_format_converter* pConverter);

// Initializes a format converter when the input data is non-interleaved.
mal_result mal_format_converter_init_separated(const mal_format_converter_config* pConfig, mal_format_converter_read_separated_proc onRead, void* pUserData, mal_format_converter* pConverter);

// Reads data from the format converter as interleaved channels.
mal_uint64 mal_format_converter_read_frames(mal_format_converter* pConverter, mal_uint64 frameCount, void* pFramesOut);

// Reads data from the format converter as separated channels.
mal_uint64 mal_format_converter_read_frames_separated(mal_format_converter* pConverter, mal_uint64 frameCount, void** ppSamplesOut);


///////////////////////////////////////////////////////////////////////////////
//
// SRC
//
///////////////////////////////////////////////////////////////////////////////

// Initializes a sample rate conversion object.
mal_result mal_src_init(mal_src_config* pConfig, mal_src_read_proc onRead, void* pUserData, mal_src* pSRC);

// Dynamically adjusts the input sample rate.
mal_result mal_src_set_input_sample_rate(mal_src* pSRC, mal_uint32 sampleRateIn);

// Dynamically adjusts the output sample rate.
//
// This is useful for dynamically adjust pitch. Keep in mind, however, that this will speed up or slow down the sound. If this
// is not acceptable you will need to use your own algorithm.
mal_result mal_src_set_output_sample_rate(mal_src* pSRC, mal_uint32 sampleRateOut);

// Reads a number of frames.
//
// Returns the number of frames actually read.
mal_uint64 mal_src_read_frames(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut);

// The same mal_src_read_frames() with extra control over whether or not the internal buffers should be flushed at the end.
//
// Internally there exists a buffer that keeps track of the previous and next samples for sample rate conversion. The simple
// version of this function does _not_ flush this buffer because otherwise it causes glitches for streaming based conversion
// pipelines. The problem, however, is that sometimes you need those last few samples (such as if you're doing a bulk conversion
// of a static file). Enabling flushing will fix this for you.
mal_uint64 mal_src_read_frames_ex(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush);



///////////////////////////////////////////////////////////////////////////////
//
// DSP
//
///////////////////////////////////////////////////////////////////////////////

// Initializes a DSP object.
mal_result mal_dsp_init(mal_dsp_config* pConfig, mal_dsp_read_proc onRead, void* pUserData, mal_dsp* pDSP);

// Dynamically adjusts the input sample rate.
mal_result mal_dsp_set_input_sample_rate(mal_dsp* pDSP, mal_uint32 sampleRateOut);

// Dynamically adjusts the output sample rate.
//
// This is useful for dynamically adjust pitch. Keep in mind, however, that this will speed up or slow down the sound. If this
// is not acceptable you will need to use your own algorithm.
mal_result mal_dsp_set_output_sample_rate(mal_dsp* pDSP, mal_uint32 sampleRateOut);

// Reads a number of frames and runs them through the DSP processor.
//
// This this _not_ flush the internal buffers which means you may end up with a few less frames than you may expect. Look at
// mal_dsp_read_frames_ex() if you want to flush the buffers at the end of the read.
mal_uint64 mal_dsp_read_frames(mal_dsp* pDSP, mal_uint64 frameCount, void* pFramesOut);

// The same mal_dsp_read_frames() with extra control over whether or not the internal buffers should be flushed at the end.
//
// See documentation for mal_src_read_frames_ex() for an explanation on flushing.
mal_uint64 mal_dsp_read_frames_ex(mal_dsp* pDSP, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush);

// High-level helper for doing a full format conversion in one go. Returns the number of output frames. Call this with pOut set to NULL to
// determine the required size of the output buffer.
//
// A return value of 0 indicates an error.
//
// This function is useful for one-off bulk conversions, but if you're streaming data you should use the DSP APIs instead.
mal_uint64 mal_convert_frames(void* pOut, mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut, const void* pIn, mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_uint64 frameCountIn);

// Helper for initializing a mal_dsp_config object.
mal_dsp_config mal_dsp_config_init(mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut);
mal_dsp_config mal_dsp_config_init_ex(mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_channel channelMapIn[MAL_MAX_CHANNELS], mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut,  mal_channel channelMapOut[MAL_MAX_CHANNELS]);



///////////////////////////////////////////////////////////////////////////////
//
// Utiltities
//
///////////////////////////////////////////////////////////////////////////////

// Creates a mutex.
//
// A mutex must be created from a valid context. A mutex is initially unlocked.
mal_result mal_mutex_init(mal_context* pContext, mal_mutex* pMutex);

// Deletes a mutex.
void mal_mutex_uninit(mal_mutex* pMutex);

// Locks a mutex with an infinite timeout.
void mal_mutex_lock(mal_mutex* pMutex);

// Unlocks a mutex.
void mal_mutex_unlock(mal_mutex* pMutex);



///////////////////////////////////////////////////////////////////////////////
//
// Miscellaneous Helpers
//
///////////////////////////////////////////////////////////////////////////////

// Retrieves a friendly name for a backend.
const char* mal_get_backend_name(mal_backend backend);

// Retrieves a friendly name for a format.
const char* mal_get_format_name(mal_format format);

// Blends two frames in floating point format.
void mal_blend_f32(float* pOut, float* pInA, float* pInB, float factor, mal_uint32 channels);



///////////////////////////////////////////////////////////////////////////////
//
// Format Conversion
//
///////////////////////////////////////////////////////////////////////////////
void mal_pcm_u8_to_s16(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_u8_to_s24(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_u8_to_s32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_u8_to_f32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s16_to_u8(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s16_to_s24(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s16_to_s32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s16_to_f32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s24_to_u8(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s24_to_s16(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s24_to_s32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s24_to_f32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s32_to_u8(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s32_to_s16(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s32_to_s24(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_s32_to_f32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_f32_to_u8(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_f32_to_s16(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_f32_to_s24(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_f32_to_s32(void* pOut, const void* pIn, mal_uint64 count, mal_dither_mode ditherMode);
void mal_pcm_convert(void* pOut, mal_format formatOut, const void* pIn, mal_format formatIn, mal_uint64 sampleCount, mal_dither_mode ditherMode);



///////////////////////////////////////////////////////////////////////////////
//
// Decoding
//
///////////////////////////////////////////////////////////////////////////////
#ifndef MAL_NO_DECODING

typedef struct mal_decoder mal_decoder;

typedef enum
{
    mal_seek_origin_start,
    mal_seek_origin_current
} mal_seek_origin;

typedef size_t     (* mal_decoder_read_proc)         (mal_decoder* pDecoder, void* pBufferOut, size_t bytesToRead); // Returns the number of bytes read.
typedef mal_bool32 (* mal_decoder_seek_proc)         (mal_decoder* pDecoder, int byteOffset, mal_seek_origin origin);
typedef mal_result (* mal_decoder_seek_to_frame_proc)(mal_decoder* pDecoder, mal_uint64 frameIndex);
typedef mal_result (* mal_decoder_uninit_proc)       (mal_decoder* pDecoder);

typedef struct
{
    mal_format  outputFormat;       // Set to 0 or mal_format_unknown to use the stream's internal format.
    mal_uint32  outputChannels;     // Set to 0 to use the stream's internal channels.
    mal_uint32  outputSampleRate;   // Set to 0 to use the stream's internal channels.
    mal_channel outputChannelMap[MAL_MAX_CHANNELS];
} mal_decoder_config;

struct mal_decoder
{
    mal_decoder_read_proc onRead;
    mal_decoder_seek_proc onSeek;
    void* pUserData;
    mal_format  internalFormat;
    mal_uint32  internalChannels;
    mal_uint32  internalSampleRate;
    mal_channel internalChannelMap[MAL_MAX_CHANNELS];
    mal_format  outputFormat;
    mal_uint32  outputChannels;
    mal_uint32  outputSampleRate;
    mal_channel outputChannelMap[MAL_MAX_CHANNELS];
    mal_dsp dsp;                // <-- Format conversion is achieved by running frames through this.
    mal_decoder_seek_to_frame_proc onSeekToFrame;
    mal_decoder_uninit_proc onUninit;
    void* pInternalDecoder;     // <-- The drwav/drflac/stb_vorbis/etc. objects.
    struct
    {
        const mal_uint8* pData;
        size_t dataSize;
        size_t currentReadPos;
    } memory;   // Only used for decoders that were opened against a block of memory.
};

mal_decoder_config mal_decoder_config_init(mal_format outputFormat, mal_uint32 outputChannels, mal_uint32 outputSampleRate);

mal_result mal_decoder_init(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_wav(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_flac(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_vorbis(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_mp3(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder);

mal_result mal_decoder_init_memory(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_memory_wav(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_memory_flac(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_memory_vorbis(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_memory_mp3(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder);

#ifndef MAL_NO_STDIO
mal_result mal_decoder_init_file(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
mal_result mal_decoder_init_file_wav(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder);
#endif

mal_result mal_decoder_uninit(mal_decoder* pDecoder);

mal_uint64 mal_decoder_read(mal_decoder* pDecoder, mal_uint64 frameCount, void* pFramesOut);
mal_result mal_decoder_seek_to_frame(mal_decoder* pDecoder, mal_uint64 frameIndex);

#endif


#ifdef __cplusplus
}
#endif
#endif  //mini_al_h


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_IMPLEMENTATION
#include <assert.h>
#include <limits.h> // For INT_MAX

#ifdef MAL_WIN32
#include <windows.h>
#else
#include <stdlib.h> // For malloc()/free()
#include <string.h> // For memset()
#endif

#if defined(MAL_APPLE) && (__MAC_OS_X_VERSION_MIN_REQUIRED < 101200)
#include <mach/mach_time.h> // For mach_absolute_time()
#endif

#ifdef MAL_POSIX
#include <unistd.h>
#include <dlfcn.h>
#endif

#if !defined(MAL_64BIT) && !defined(MAL_32BIT)
#ifdef _WIN32
#ifdef _WIN64
#define MAL_64BIT
#else
#define MAL_32BIT
#endif
#endif
#endif

#if !defined(MAL_64BIT) && !defined(MAL_32BIT)
#ifdef __GNUC__
#ifdef __LP64__
#define MAL_64BIT
#else
#define MAL_32BIT
#endif
#endif
#endif

#if !defined(MAL_64BIT) && !defined(MAL_32BIT)
#include <stdint.h>
#if INTPTR_MAX == INT64_MAX
#define MAL_64BIT
#else
#define MAL_32BIT
#endif
#endif

// Unfortunately using runtime linking for pthreads causes problems. This has occurred for me when testing on FreeBSD. When
// using runtime linking, deadlocks can occur (for me it happens when loading data from fread()). It turns out that doing
// compile-time linking fixes this. I'm not sure why this happens, but this is the safest way I can think of to continue. To
// enable runtime linking, #define this before the implementation of this file. I am not officially supporting this, but I'm
// leaving it here in case it's useful for somebody, somewhere.
//#define MAL_USE_RUNTIME_LINKING_FOR_PTHREAD

// Disable run-time linking on certain backends.
#ifndef MAL_NO_RUNTIME_LINKING
    #if defined(MAL_ANDROID) || defined(MAL_EMSCRIPTEN)
        #define MAL_NO_RUNTIME_LINKING
    #endif
#endif

// Check if we have the necessary development packages for each backend at the top so we can use this to determine whether or not
// certain unused functions and variables can be excluded from the build to avoid warnings.
#ifdef MAL_ENABLE_WASAPI
    #define MAL_HAS_WASAPI      // Every compiler should support WASAPI
#endif
#ifdef MAL_ENABLE_DSOUND
    #define MAL_HAS_DSOUND      // Every compiler should support DirectSound.
#endif
#ifdef MAL_ENABLE_WINMM
    #define MAL_HAS_WINMM       // Every compiler I'm aware of supports WinMM.
#endif
#ifdef MAL_ENABLE_ALSA
    #define MAL_HAS_ALSA
    #ifdef MAL_NO_RUNTIME_LINKING
        #ifdef __has_include
            #if !__has_include(<alsa/asoundlib.h>)
                #undef MAL_HAS_ALSA
            #endif
        #endif
    #endif
#endif
#ifdef MAL_ENABLE_PULSEAUDIO
    #define MAL_HAS_PULSEAUDIO  // Development packages are unnecessary for PulseAudio.
    #ifdef MAL_NO_RUNTIME_LINKING
        #ifdef __has_include
            #if !__has_include(<pulse/pulseaudio.h>)
                #undef MAL_HAS_PULSEAUDIO
            #endif
        #endif
    #endif
#endif
#ifdef MAL_ENABLE_JACK
    #define MAL_HAS_JACK
    #ifdef MAL_NO_RUNTIME_LINKING
        #ifdef __has_include
            #if !__has_include(<jack/jack.h>)
                #undef MAL_HAS_JACK
            #endif
        #endif
    #endif
#endif
#ifdef MAL_ENABLE_COREAUDIO
    #define MAL_HAS_COREAUDIO
#endif
#ifdef MAL_ENABLE_OSS
    #define MAL_HAS_OSS         // OSS is the only supported backend for Unix and BSD, so it must be present else this library is useless.
#endif
#ifdef MAL_ENABLE_OPENSL
    #define MAL_HAS_OPENSL      // OpenSL is the only supported backend for Android. It must be present.
#endif
#ifdef MAL_ENABLE_OPENAL
    #define MAL_HAS_OPENAL
    #ifdef MAL_NO_RUNTIME_LINKING
        #ifdef __has_include
            #if !__has_include(<AL/al.h>)
                #undef MAL_HAS_OPENAL
            #endif
        #endif
    #endif
#endif
#ifdef MAL_ENABLE_SDL
    #define MAL_HAS_SDL

    // SDL headers are necessary if using compile-time linking.
    #ifdef MAL_NO_RUNTIME_LINKING
        #ifdef __has_include
            #ifdef MAL_EMSCRIPTEN
                #if !__has_include(<SDL/SDL_audio.h>)
                    #undef MAL_HAS_SDL
                #endif
            #else
                #if !__has_include(<SDL2/SDL_audio.h>)
                    #undef MAL_HAS_SDL
                #endif
            #endif
        #endif
    #endif
#endif
#ifdef MAL_ENABLE_NULL
    #define MAL_HAS_NULL    // Everything supports the null backend.
#endif


#ifdef MAL_WIN32
    #define MAL_THREADCALL WINAPI
    typedef unsigned long mal_thread_result;
#else
    #define MAL_THREADCALL
    typedef void* mal_thread_result;
#endif
typedef mal_thread_result (MAL_THREADCALL * mal_thread_entry_proc)(void* pData);

#ifdef MAL_WIN32
typedef HRESULT (WINAPI * MAL_PFN_CoInitializeEx)(LPVOID pvReserved, DWORD  dwCoInit);
typedef void    (WINAPI * MAL_PFN_CoUninitialize)();
typedef HRESULT (WINAPI * MAL_PFN_CoCreateInstance)(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID *ppv);
typedef void    (WINAPI * MAL_PFN_CoTaskMemFree)(LPVOID pv);
typedef HRESULT (WINAPI * MAL_PFN_PropVariantClear)(PROPVARIANT *pvar);
typedef int     (WINAPI * MAL_PFN_StringFromGUID2)(const GUID* const rguid, LPOLESTR lpsz, int cchMax);

typedef HWND (WINAPI * MAL_PFN_GetForegroundWindow)();
typedef HWND (WINAPI * MAL_PFN_GetDesktopWindow)();
#endif


#define MAL_STATE_UNINITIALIZED     0
#define MAL_STATE_STOPPED           1   // The device's default state after initialization.
#define MAL_STATE_STARTED           2   // The worker thread is in it's main loop waiting for the driver to request or deliver audio data.
#define MAL_STATE_STARTING          3   // Transitioning from a stopped state to started.
#define MAL_STATE_STOPPING          4   // Transitioning from a started state to stopped.


// The default format when mal_format_unknown (0) is requested when initializing a device.
#ifndef MAL_DEFAULT_FORMAT
#define MAL_DEFAULT_FORMAT                          mal_format_f32
#endif

// The default channel count to use when 0 is used when initializing a device.
#ifndef MAL_DEFAULT_CHANNELS
#define MAL_DEFAULT_CHANNELS                        2
#endif

// The default sample rate to use when 0 is used when initializing a device.
#ifndef MAL_DEFAULT_SAMPLE_RATE
#define MAL_DEFAULT_SAMPLE_RATE                     48000
#endif

// The default size of the device's buffer in milliseconds.
//
// If this is too small you may get underruns and overruns in which case you'll need to either increase
// this value or use an explicit buffer size.
#ifndef MAL_DEFAULT_BUFFER_SIZE_IN_MILLISECONDS
#define MAL_DEFAULT_BUFFER_SIZE_IN_MILLISECONDS     25
#endif

// Default periods when none is specified in mal_device_init(). More periods means more work on the CPU.
#ifndef MAL_DEFAULT_PERIODS
#define MAL_DEFAULT_PERIODS                         2
#endif

// Standard sample rates, in order of priority.
mal_uint32 g_malStandardSampleRatePriorities[] = {
    MAL_SAMPLE_RATE_48000,  // Most common
    MAL_SAMPLE_RATE_44100,

    MAL_SAMPLE_RATE_32000,  // Lows
    MAL_SAMPLE_RATE_24000,
    MAL_SAMPLE_RATE_22050,

    MAL_SAMPLE_RATE_88200,  // Highs
    MAL_SAMPLE_RATE_96000,
    MAL_SAMPLE_RATE_176400,
    MAL_SAMPLE_RATE_192000,

    MAL_SAMPLE_RATE_16000,  // Extreme lows
    MAL_SAMPLE_RATE_11025,
    MAL_SAMPLE_RATE_8000,

    MAL_SAMPLE_RATE_352800, // Extreme highs
    MAL_SAMPLE_RATE_384000
};

#define MAL_DEFAULT_PLAYBACK_DEVICE_NAME    "Default Playback Device"
#define MAL_DEFAULT_CAPTURE_DEVICE_NAME     "Default Capture Device"

///////////////////////////////////////////////////////////////////////////////
//
// Standard Library Stuff
//
///////////////////////////////////////////////////////////////////////////////
#ifndef mal_zero_memory
#ifdef MAL_WIN32
#define mal_zero_memory(p, sz) ZeroMemory((p), (sz))
#else
#define mal_zero_memory(p, sz) memset((p), 0, (sz))
#endif
#endif

#define mal_zero_object(p) mal_zero_memory((p), sizeof(*(p)))

#ifndef mal_copy_memory
#ifdef MAL_WIN32
#define mal_copy_memory(dst, src, sz) CopyMemory((dst), (src), (sz))
#else
#define mal_copy_memory(dst, src, sz) memcpy((dst), (src), (sz))
#endif
#endif

#ifndef mal_malloc
#ifdef MAL_WIN32
#define mal_malloc(sz) HeapAlloc(GetProcessHeap(), 0, (sz))
#else
#define mal_malloc(sz) malloc((sz))
#endif
#endif

#ifndef mal_realloc
#ifdef MAL_WIN32
#define mal_realloc(p, sz) (((sz) > 0) ? ((p) ? HeapReAlloc(GetProcessHeap(), 0, (p), (sz)) : HeapAlloc(GetProcessHeap(), 0, (sz))) : ((VOID*)(SIZE_T)(HeapFree(GetProcessHeap(), 0, (p)) & 0)))
#else
#define mal_realloc(p, sz) realloc((p), (sz))
#endif
#endif

#ifndef mal_free
#ifdef MAL_WIN32
#define mal_free(p) HeapFree(GetProcessHeap(), 0, (p))
#else
#define mal_free(p) free((p))
#endif
#endif

#ifndef mal_assert
#ifdef MAL_WIN32
#define mal_assert(condition) assert(condition)
#else
#define mal_assert(condition) assert(condition)
#endif
#endif

#define mal_countof(x)  (sizeof(x) / sizeof(x[0]))
#define mal_max(x, y)   (((x) > (y)) ? (x) : (y))
#define mal_min(x, y)   (((x) < (y)) ? (x) : (y))
#define mal_offset_ptr(p, offset) (((mal_uint8*)(p)) + (offset))

#define mal_buffer_frame_capacity(buffer, channels, format) (sizeof(buffer) / mal_get_sample_size_in_bytes(format) / (channels))


// Return Values:
//   0:  Success
//   22: EINVAL
//   34: ERANGE
//
// Not using symbolic constants for errors because I want to avoid #including errno.h
int mal_strcpy_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    size_t i;
    for (i = 0; i < dstSizeInBytes && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstSizeInBytes) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return 34;
}

int mal_strncpy_s(char* dst, size_t dstSizeInBytes, const char* src, size_t count)
{
    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    size_t maxcount = count;
    if (count == ((size_t)-1) || count >= dstSizeInBytes) {        // -1 = _TRUNCATE
        maxcount = dstSizeInBytes - 1;
    }

    size_t i;
    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return 34;
}

int mal_strcat_s(char* dst, size_t dstSizeInBytes, const char* src)
{
    if (dst == 0) {
        return 22;
    }
    if (dstSizeInBytes == 0) {
        return 34;
    }
    if (src == 0) {
        dst[0] = '\0';
        return 22;
    }

    char* dstorig = dst;

    while (dstSizeInBytes > 0 && dst[0] != '\0') {
        dst += 1;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        return 22;  // Unterminated.
    }


    while (dstSizeInBytes > 0 && src[0] != '\0') {
        *dst++ = *src++;
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return 34;
    }

    return 0;
}

int mal_itoa_s(int value, char* dst, size_t dstSizeInBytes, int radix)
{
    if (dst == NULL || dstSizeInBytes == 0) {
        return 22;
    }
    if (radix < 2 || radix > 36) {
        dst[0] = '\0';
        return 22;
    }

    int sign = (value < 0 && radix == 10) ? -1 : 1;     // The negative sign is only used when the base is 10.

    unsigned int valueU;
    if (value < 0) {
        valueU = -value;
    } else {
        valueU = value;
    }

    char* dstEnd = dst;
    do
    {
        int remainder = valueU % radix;
        if (remainder > 9) {
            *dstEnd = (char)((remainder - 10) + 'a');
        } else {
            *dstEnd = (char)(remainder + '0');
        }

        dstEnd += 1;
        dstSizeInBytes -= 1;
        valueU /= radix;
    } while (dstSizeInBytes > 0 && valueU > 0);

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return 22;  // Ran out of room in the output buffer.
    }

    if (sign < 0) {
        *dstEnd++ = '-';
        dstSizeInBytes -= 1;
    }

    if (dstSizeInBytes == 0) {
        dst[0] = '\0';
        return 22;  // Ran out of room in the output buffer.
    }

    *dstEnd = '\0';


    // At this point the string will be reversed.
    dstEnd -= 1;
    while (dst < dstEnd) {
        char temp = *dst;
        *dst = *dstEnd;
        *dstEnd = temp;

        dst += 1;
        dstEnd -= 1;
    }

    return 0;
}

int mal_strcmp(const char* str1, const char* str2)
{
    if (str1 == str2) return  0;

    // These checks differ from the standard implementation. It's not important, but I prefer
    // it just for sanity.
    if (str1 == NULL) return -1;
    if (str2 == NULL) return  1;

    for (;;) {
        if (str1[0] == '\0') {
            break;
        }
        if (str1[0] != str2[0]) {
            break;
        }

        str1 += 1;
        str2 += 1;
    }

    return ((unsigned char*)str1)[0] - ((unsigned char*)str2)[0];
}


// Thanks to good old Bit Twiddling Hacks for this one: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
static inline unsigned int mal_next_power_of_2(unsigned int x)
{
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;

    return x;
}

static inline unsigned int mal_prev_power_of_2(unsigned int x)
{
    return mal_next_power_of_2(x) >> 1;
}

static inline unsigned int mal_round_to_power_of_2(unsigned int x)
{
    unsigned int prev = mal_prev_power_of_2(x);
    unsigned int next = mal_next_power_of_2(x);
    if ((next - x) > (x - prev)) {
        return prev;
    } else {
        return next;
    }
}



// Clamps an f32 sample to -1..1
static inline float mal_clip_f32(float x)
{
    if (x < -1) return -1;
    if (x > +1) return +1;
    return x;
}

static inline float mal_mix_f32(float x, float y, float a)
{
    return x*(1-a) + y*a;
}


///////////////////////////////////////////////////////////////////////////////
//
// Atomics
//
///////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) && defined(_MSC_VER)
#define mal_memory_barrier()            MemoryBarrier()
#define mal_atomic_exchange_32(a, b)    InterlockedExchange((LONG*)a, (LONG)b)
#define mal_atomic_exchange_64(a, b)    InterlockedExchange64((LONGLONG*)a, (LONGLONG)b)
#define mal_atomic_increment_32(a)      InterlockedIncrement((LONG*)a)
#define mal_atomic_decrement_32(a)      InterlockedDecrement((LONG*)a)
#else
#define mal_memory_barrier()            __sync_synchronize()
#define mal_atomic_exchange_32(a, b)    (void)__sync_lock_test_and_set(a, b); __sync_synchronize()
#define mal_atomic_exchange_64(a, b)    (void)__sync_lock_test_and_set(a, b); __sync_synchronize()
#define mal_atomic_increment_32(a)      __sync_add_and_fetch(a, 1)
#define mal_atomic_decrement_32(a)      __sync_sub_and_fetch(a, 1)
#endif

#ifdef MAL_64BIT
#define mal_atomic_exchange_ptr mal_atomic_exchange_64
#endif
#ifdef MAL_32BIT
#define mal_atomic_exchange_ptr mal_atomic_exchange_32
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Timing
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_WIN32
LARGE_INTEGER g_mal_TimerFrequency = {{0}};
void mal_timer_init(mal_timer* pTimer)
{
    if (g_mal_TimerFrequency.QuadPart == 0) {
        QueryPerformanceFrequency(&g_mal_TimerFrequency);
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    pTimer->counter = (mal_uint64)counter.QuadPart;
}

double mal_timer_get_time_in_seconds(mal_timer* pTimer)
{
    LARGE_INTEGER counter;
    if (!QueryPerformanceCounter(&counter)) {
        return 0;
    }

    return (counter.QuadPart - pTimer->counter) / (double)g_mal_TimerFrequency.QuadPart;
}
#elif defined(MAL_APPLE) && (__MAC_OS_X_VERSION_MIN_REQUIRED < 101200)
uint64_t g_mal_TimerFrequency = 0;
void mal_timer_init(mal_timer* pTimer)
{
    mach_timebase_info_data_t baseTime;
    mach_timebase_info(&baseTime);
    g_mal_TimerFrequency = (baseTime.denom * 1e9) / baseTime.numer;

    pTimer->counter = mach_absolute_time();
}

double mal_timer_get_time_in_seconds(mal_timer* pTimer)
{
    uint64_t newTimeCounter = mach_absolute_time();
    uint64_t oldTimeCounter = pTimer->counter;

    return (newTimeCounter - oldTimeCounter) / g_mal_TimerFrequency;
}
#else
void mal_timer_init(mal_timer* pTimer)
{
    struct timespec newTime;
    clock_gettime(CLOCK_MONOTONIC, &newTime);

    pTimer->counter = (newTime.tv_sec * 1000000000) + newTime.tv_nsec;
}

double mal_timer_get_time_in_seconds(mal_timer* pTimer)
{
    struct timespec newTime;
    clock_gettime(CLOCK_MONOTONIC, &newTime);

    uint64_t newTimeCounter = (newTime.tv_sec * 1000000000) + newTime.tv_nsec;
    uint64_t oldTimeCounter = pTimer->counter;

    return (newTimeCounter - oldTimeCounter) / 1000000000.0;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Dynamic Linking
//
///////////////////////////////////////////////////////////////////////////////
mal_handle mal_dlopen(const char* filename)
{
#ifdef _WIN32
#ifdef MAL_WIN32_DESKTOP
    return (mal_handle)LoadLibraryA(filename);
#else
    // *sigh* It appears there is no ANSI version of LoadPackagedLibrary()...
    WCHAR filenameW[4096];
    if (MultiByteToWideChar(CP_UTF8, 0, filename, -1, filenameW, sizeof(filenameW)) == 0) {
        return NULL;
    }

    return (mal_handle)LoadPackagedLibrary(filenameW, 0);
#endif
#else
    return (mal_handle)dlopen(filename, RTLD_NOW);
#endif
}

void mal_dlclose(mal_handle handle)
{
#ifdef _WIN32
    FreeLibrary((HMODULE)handle);
#else
    dlclose((void*)handle);
#endif
}

mal_proc mal_dlsym(mal_handle handle, const char* symbol)
{
#ifdef _WIN32
    return (mal_proc)GetProcAddress((HMODULE)handle, symbol);
#else
    return (mal_proc)dlsym((void*)handle, symbol);
#endif
}


///////////////////////////////////////////////////////////////////////////////
//
// Threading
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_WIN32
int mal_thread_priority_to_win32(mal_thread_priority priority)
{
    switch (priority) {
        case mal_thread_priority_idle:     return THREAD_PRIORITY_IDLE;
        case mal_thread_priority_lowest:   return THREAD_PRIORITY_LOWEST;
        case mal_thread_priority_low:      return THREAD_PRIORITY_BELOW_NORMAL;
        case mal_thread_priority_normal:   return THREAD_PRIORITY_NORMAL;
        case mal_thread_priority_high:     return THREAD_PRIORITY_ABOVE_NORMAL;
        case mal_thread_priority_highest:  return THREAD_PRIORITY_HIGHEST;
        case mal_thread_priority_realtime: return THREAD_PRIORITY_TIME_CRITICAL;
        default: return mal_thread_priority_normal;
    }
}

mal_result mal_thread_create__win32(mal_context* pContext, mal_thread* pThread, mal_thread_entry_proc entryProc, void* pData)
{
    pThread->win32.hThread = CreateThread(NULL, 0, entryProc, pData, 0, NULL);
    if (pThread->win32.hThread == NULL) {
        return MAL_FAILED_TO_CREATE_THREAD;
    }

    SetThreadPriority((HANDLE)pThread->win32.hThread, mal_thread_priority_to_win32(pContext->config.threadPriority));

    return MAL_SUCCESS;
}

void mal_thread_wait__win32(mal_thread* pThread)
{
    WaitForSingleObject(pThread->win32.hThread, INFINITE);
}

void mal_sleep__win32(mal_uint32 milliseconds)
{
    Sleep((DWORD)milliseconds);
}


mal_result mal_mutex_init__win32(mal_context* pContext, mal_mutex* pMutex)
{
    (void)pContext;

    pMutex->win32.hMutex = CreateEventA(NULL, FALSE, TRUE, NULL);
    if (pMutex->win32.hMutex == NULL) {
        return MAL_FAILED_TO_CREATE_MUTEX;
    }

    return MAL_SUCCESS;
}

void mal_mutex_uninit__win32(mal_mutex* pMutex)
{
    CloseHandle(pMutex->win32.hMutex);
}

void mal_mutex_lock__win32(mal_mutex* pMutex)
{
    WaitForSingleObject(pMutex->win32.hMutex, INFINITE);
}

void mal_mutex_unlock__win32(mal_mutex* pMutex)
{
    SetEvent(pMutex->win32.hMutex);
}


mal_result mal_event_init__win32(mal_context* pContext, mal_event* pEvent)
{
    (void)pContext;

    pEvent->win32.hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (pEvent->win32.hEvent == NULL) {
        return MAL_FAILED_TO_CREATE_EVENT;
    }

    return MAL_SUCCESS;
}

void mal_event_uninit__win32(mal_event* pEvent)
{
    CloseHandle(pEvent->win32.hEvent);
}

mal_bool32 mal_event_wait__win32(mal_event* pEvent)
{
    return WaitForSingleObject(pEvent->win32.hEvent, INFINITE) == WAIT_OBJECT_0;
}

mal_bool32 mal_event_signal__win32(mal_event* pEvent)
{
    return SetEvent(pEvent->win32.hEvent);
}
#endif


#ifdef MAL_POSIX
#include <sched.h>

typedef int (* mal_pthread_create_proc)(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
typedef int (* mal_pthread_join_proc)(pthread_t thread, void **retval);
typedef int (* mal_pthread_mutex_init_proc)(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr);
typedef int (* mal_pthread_mutex_destroy_proc)(pthread_mutex_t *__mutex);
typedef int (* mal_pthread_mutex_lock_proc)(pthread_mutex_t *__mutex);
typedef int (* mal_pthread_mutex_unlock_proc)(pthread_mutex_t *__mutex);
typedef int (* mal_pthread_cond_init_proc)(pthread_cond_t *__restrict __cond, const pthread_condattr_t *__restrict __cond_attr);
typedef int (* mal_pthread_cond_destroy_proc)(pthread_cond_t *__cond);
typedef int (* mal_pthread_cond_signal_proc)(pthread_cond_t *__cond);
typedef int (* mal_pthread_cond_wait_proc)(pthread_cond_t *__restrict __cond, pthread_mutex_t *__restrict __mutex);
typedef int (* mal_pthread_attr_init_proc)(pthread_attr_t *attr);
typedef int (* mal_pthread_attr_destroy_proc)(pthread_attr_t *attr);
typedef int (* mal_pthread_attr_setschedpolicy_proc)(pthread_attr_t *attr, int policy);
typedef int (* mal_pthread_attr_getschedparam_proc)(const pthread_attr_t *attr, struct sched_param *param);
typedef int (* mal_pthread_attr_setschedparam_proc)(pthread_attr_t *attr, const struct sched_param *param);

mal_bool32 mal_thread_create__posix(mal_context* pContext, mal_thread* pThread, mal_thread_entry_proc entryProc, void* pData)
{
    pthread_attr_t* pAttr = NULL;

    // Try setting the thread priority. It's not critical if anything fails here.
    pthread_attr_t attr;
    if (((mal_pthread_attr_init_proc)pContext->posix.pthread_attr_init)(&attr) == 0) {
        int scheduler = -1;
        if (pContext->config.threadPriority == mal_thread_priority_idle) {
#ifdef SCHED_IDLE
            if (((mal_pthread_attr_setschedpolicy_proc)pContext->posix.pthread_attr_setschedpolicy)(&attr, SCHED_IDLE) == 0) {
                scheduler = SCHED_IDLE;
            }
#endif
        } else if (pContext->config.threadPriority == mal_thread_priority_realtime) {
#ifdef SCHED_FIFO
            if (((mal_pthread_attr_setschedpolicy_proc)pContext->posix.pthread_attr_setschedpolicy)(&attr, SCHED_FIFO) == 0) {
                scheduler = SCHED_FIFO;
            }
#endif
        } else {
            scheduler = sched_getscheduler(0);
        }

        if (scheduler != -1) {
            int priorityMin = sched_get_priority_min(scheduler);
            int priorityMax = sched_get_priority_max(scheduler);
            int priorityStep = (priorityMax - priorityMin) / 7;  // 7 = number of priorities supported by mini_al.

            struct sched_param sched;
            if (((mal_pthread_attr_getschedparam_proc)pContext->posix.pthread_attr_getschedparam)(&attr, &sched) == 0) {
                if (pContext->config.threadPriority == mal_thread_priority_idle) {
                    sched.sched_priority = priorityMin;
                } else if (pContext->config.threadPriority == mal_thread_priority_realtime) {
                    sched.sched_priority = priorityMax;
                } else {
                    sched.sched_priority += ((int)pContext->config.threadPriority + 5) * priorityStep;  // +5 because the lowest priority is -5.
                    if (sched.sched_priority < priorityMin) {
                        sched.sched_priority = priorityMin;
                    }
                    if (sched.sched_priority > priorityMax) {
                        sched.sched_priority = priorityMax;
                    }
                }

                if (((mal_pthread_attr_setschedparam_proc)pContext->posix.pthread_attr_setschedparam)(&attr, &sched) == 0) {
                    pAttr = &attr;
                }
            }
        }

        ((mal_pthread_attr_destroy_proc)pContext->posix.pthread_attr_destroy)(&attr);
    }

    int result = ((mal_pthread_create_proc)pContext->posix.pthread_create)(&pThread->posix.thread, pAttr, entryProc, pData);
    if (result != 0) {
        return MAL_FAILED_TO_CREATE_THREAD;
    }

    return MAL_SUCCESS;
}

void mal_thread_wait__posix(mal_thread* pThread)
{
    ((mal_pthread_join_proc)pThread->pContext->posix.pthread_join)(pThread->posix.thread, NULL);
}

void mal_sleep__posix(mal_uint32 milliseconds)
{
    usleep(milliseconds * 1000);    // <-- usleep is in microseconds.
}


mal_result mal_mutex_init__posix(mal_context* pContext, mal_mutex* pMutex)
{
    int result = ((mal_pthread_mutex_init_proc)pContext->posix.pthread_mutex_init)(&pMutex->posix.mutex, NULL);
    if (result != 0) {
        return MAL_FAILED_TO_CREATE_MUTEX;
    }

    return MAL_SUCCESS;
}

void mal_mutex_uninit__posix(mal_mutex* pMutex)
{
    ((mal_pthread_mutex_destroy_proc)pMutex->pContext->posix.pthread_mutex_destroy)(&pMutex->posix.mutex);
}

void mal_mutex_lock__posix(mal_mutex* pMutex)
{
    ((mal_pthread_mutex_lock_proc)pMutex->pContext->posix.pthread_mutex_lock)(&pMutex->posix.mutex);
}

void mal_mutex_unlock__posix(mal_mutex* pMutex)
{
    ((mal_pthread_mutex_unlock_proc)pMutex->pContext->posix.pthread_mutex_unlock)(&pMutex->posix.mutex);
}


mal_result mal_event_init__posix(mal_context* pContext, mal_event* pEvent)
{
    if (((mal_pthread_mutex_init_proc)pContext->posix.pthread_mutex_init)(&pEvent->posix.mutex, NULL) != 0) {
        return MAL_FAILED_TO_CREATE_MUTEX;
    }

    if (((mal_pthread_cond_init_proc)pContext->posix.pthread_cond_init)(&pEvent->posix.condition, NULL) != 0) {
        return MAL_FAILED_TO_CREATE_EVENT;
    }

    pEvent->posix.value = 0;
    return MAL_SUCCESS;
}

void mal_event_uninit__posix(mal_event* pEvent)
{
    ((mal_pthread_cond_destroy_proc)pEvent->pContext->posix.pthread_cond_destroy)(&pEvent->posix.condition);
    ((mal_pthread_mutex_destroy_proc)pEvent->pContext->posix.pthread_mutex_destroy)(&pEvent->posix.mutex);
}

mal_bool32 mal_event_wait__posix(mal_event* pEvent)
{
    ((mal_pthread_mutex_lock_proc)pEvent->pContext->posix.pthread_mutex_lock)(&pEvent->posix.mutex);
    {
        while (pEvent->posix.value == 0) {
            ((mal_pthread_cond_wait_proc)pEvent->pContext->posix.pthread_cond_wait)(&pEvent->posix.condition, &pEvent->posix.mutex);
        }

        pEvent->posix.value = 0;  // Auto-reset.
    }
    ((mal_pthread_mutex_unlock_proc)pEvent->pContext->posix.pthread_mutex_unlock)(&pEvent->posix.mutex);

    return MAL_TRUE;
}

mal_bool32 mal_event_signal__posix(mal_event* pEvent)
{
    ((mal_pthread_mutex_lock_proc)pEvent->pContext->posix.pthread_mutex_lock)(&pEvent->posix.mutex);
    {
        pEvent->posix.value = 1;
        ((mal_pthread_cond_signal_proc)pEvent->pContext->posix.pthread_cond_signal)(&pEvent->posix.condition);
    }
    ((mal_pthread_mutex_unlock_proc)pEvent->pContext->posix.pthread_mutex_unlock)(&pEvent->posix.mutex);

    return MAL_TRUE;
}
#endif

mal_result mal_thread_create(mal_context* pContext, mal_thread* pThread, mal_thread_entry_proc entryProc, void* pData)
{
    if (pContext == NULL || pThread == NULL || entryProc == NULL) return MAL_FALSE;

    pThread->pContext = pContext;

#ifdef MAL_WIN32
    return mal_thread_create__win32(pContext, pThread, entryProc, pData);
#endif
#ifdef MAL_POSIX
    return mal_thread_create__posix(pContext, pThread, entryProc, pData);
#endif
}

void mal_thread_wait(mal_thread* pThread)
{
    if (pThread == NULL) return;

#ifdef MAL_WIN32
    mal_thread_wait__win32(pThread);
#endif
#ifdef MAL_POSIX
    mal_thread_wait__posix(pThread);
#endif
}

void mal_sleep(mal_uint32 milliseconds)
{
#ifdef MAL_WIN32
    mal_sleep__win32(milliseconds);
#endif
#ifdef MAL_POSIX
    mal_sleep__posix(milliseconds);
#endif
}


mal_result mal_mutex_init(mal_context* pContext, mal_mutex* pMutex)
{
    if (pContext == NULL || pMutex == NULL) return MAL_INVALID_ARGS;

    pMutex->pContext = pContext;

#ifdef MAL_WIN32
    return mal_mutex_init__win32(pContext, pMutex);
#endif
#ifdef MAL_POSIX
    return mal_mutex_init__posix(pContext, pMutex);
#endif
}

void mal_mutex_uninit(mal_mutex* pMutex)
{
    if (pMutex == NULL || pMutex->pContext == NULL) return;

#ifdef MAL_WIN32
    mal_mutex_uninit__win32(pMutex);
#endif
#ifdef MAL_POSIX
    mal_mutex_uninit__posix(pMutex);
#endif
}

void mal_mutex_lock(mal_mutex* pMutex)
{
    if (pMutex == NULL || pMutex->pContext == NULL) return;

#ifdef MAL_WIN32
    mal_mutex_lock__win32(pMutex);
#endif
#ifdef MAL_POSIX
    mal_mutex_lock__posix(pMutex);
#endif
}

void mal_mutex_unlock(mal_mutex* pMutex)
{
    if (pMutex == NULL || pMutex->pContext == NULL) return;

#ifdef MAL_WIN32
    mal_mutex_unlock__win32(pMutex);
#endif
#ifdef MAL_POSIX
    mal_mutex_unlock__posix(pMutex);
#endif
}


mal_result mal_event_init(mal_context* pContext, mal_event* pEvent)
{
    if (pContext == NULL || pEvent == NULL) return MAL_FALSE;

    pEvent->pContext = pContext;

#ifdef MAL_WIN32
    return mal_event_init__win32(pContext, pEvent);
#endif
#ifdef MAL_POSIX
    return mal_event_init__posix(pContext, pEvent);
#endif
}

void mal_event_uninit(mal_event* pEvent)
{
    if (pEvent == NULL || pEvent->pContext == NULL) return;

#ifdef MAL_WIN32
    mal_event_uninit__win32(pEvent);
#endif
#ifdef MAL_POSIX
    mal_event_uninit__posix(pEvent);
#endif
}

mal_bool32 mal_event_wait(mal_event* pEvent)
{
    if (pEvent == NULL || pEvent->pContext == NULL) return MAL_FALSE;

#ifdef MAL_WIN32
    return mal_event_wait__win32(pEvent);
#endif
#ifdef MAL_POSIX
    return mal_event_wait__posix(pEvent);
#endif
}

mal_bool32 mal_event_signal(mal_event* pEvent)
{
    if (pEvent == NULL || pEvent->pContext == NULL) return MAL_FALSE;

#ifdef MAL_WIN32
    return mal_event_signal__win32(pEvent);
#endif
#ifdef MAL_POSIX
    return mal_event_signal__posix(pEvent);
#endif
}


mal_uint32 mal_get_best_sample_rate_within_range(mal_uint32 sampleRateMin, mal_uint32 sampleRateMax)
{
    // Normalize the range in case we were given something stupid.
    if (sampleRateMin < MAL_MIN_SAMPLE_RATE) {
        sampleRateMin = MAL_MIN_SAMPLE_RATE;
    }
    if (sampleRateMax > MAL_MAX_SAMPLE_RATE) {
        sampleRateMax = MAL_MAX_SAMPLE_RATE;
    }
    if (sampleRateMin > sampleRateMax) {
        sampleRateMin = sampleRateMax;
    }

    if (sampleRateMin == sampleRateMax) {
        return sampleRateMax;
    } else {
        for (size_t iStandardRate = 0; iStandardRate < mal_countof(g_malStandardSampleRatePriorities); ++iStandardRate) {
            mal_uint32 standardRate = g_malStandardSampleRatePriorities[iStandardRate];
            if (standardRate >= sampleRateMin && standardRate <= sampleRateMax) {
                return standardRate;
            }
        }
    }

    // Should never get here.
    mal_assert(MAL_FALSE);
    return 0;
}


// Posts a log message.
void mal_log(mal_context* pContext, mal_device* pDevice, const char* message)
{
    if (pContext == NULL) return;

    mal_log_proc onLog = pContext->config.onLog;
    if (onLog) {
        onLog(pContext, pDevice, message);
    }
}

// Posts an error. Throw a breakpoint in here if you're needing to debug. The return value is always "resultCode".
mal_result mal_context_post_error(mal_context* pContext, mal_device* pDevice, const char* message, mal_result resultCode)
{
    // Derive the context from the device if necessary.
    if (pContext == NULL) {
        if (pDevice != NULL) {
            pContext = pDevice->pContext;
        }
    }

    mal_log(pContext, pDevice, message);
    return resultCode;
}

mal_result mal_post_error(mal_device* pDevice, const char* message, mal_result resultCode)
{
    return mal_context_post_error(NULL, pDevice, message, resultCode);
}


// The callback for reading from the client -> DSP -> device.
static inline mal_uint32 mal_device__on_read_from_client(mal_dsp* pDSP, mal_uint32 frameCount, void* pFramesOut, void* pUserData)
{
    (void)pDSP;

    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_send_proc onSend = pDevice->onSend;
    if (onSend) {
        return onSend(pDevice, frameCount, pFramesOut);
    }

    return 0;
}

// The callback for reading from the device -> DSP -> client.
static inline mal_uint32 mal_device__on_read_from_device(mal_dsp* pDSP, mal_uint32 frameCount, void* pFramesOut, void* pUserData)
{
    (void)pDSP;

    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    if (pDevice->_dspFrameCount == 0) {
        return 0;   // Nothing left.
    }

    mal_uint32 framesToRead = frameCount;
    if (framesToRead > pDevice->_dspFrameCount) {
        framesToRead = pDevice->_dspFrameCount;
    }

    mal_uint32 bytesToRead = framesToRead * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);
    mal_copy_memory(pFramesOut, pDevice->_dspFrames, bytesToRead);
    pDevice->_dspFrameCount -= framesToRead;
    pDevice->_dspFrames += bytesToRead;

    return framesToRead;
}

// A helper function for reading sample data from the client. Returns the number of samples read from the client. Remaining samples
// are filled with silence.
static inline mal_uint32 mal_device__read_frames_from_client(mal_device* pDevice, mal_uint32 frameCount, void* pSamples)
{
    mal_assert(pDevice != NULL);
    mal_assert(frameCount > 0);
    mal_assert(pSamples != NULL);

    mal_uint32 framesRead     = (mal_uint32)mal_dsp_read_frames(&pDevice->dsp, frameCount, pSamples);
    mal_uint32 samplesRead    = framesRead * pDevice->internalChannels;
    mal_uint32 sampleSize     = mal_get_sample_size_in_bytes(pDevice->internalFormat);
    mal_uint32 consumedBytes  = samplesRead*sampleSize;
    mal_uint32 remainingBytes = ((frameCount * pDevice->internalChannels) - samplesRead)*sampleSize;
    mal_zero_memory((mal_uint8*)pSamples + consumedBytes, remainingBytes);

    return samplesRead;
}

// A helper for sending sample data to the client.
static inline void mal_device__send_frames_to_client(mal_device* pDevice, mal_uint32 frameCount, const void* pSamples)
{
    mal_assert(pDevice != NULL);
    mal_assert(frameCount > 0);
    mal_assert(pSamples != NULL);

    mal_recv_proc onRecv = pDevice->onRecv;
    if (onRecv) {
        pDevice->_dspFrameCount = frameCount;
        pDevice->_dspFrames = (const mal_uint8*)pSamples;

        mal_uint8 chunkBuffer[4096];
        mal_uint32 chunkFrameCount = sizeof(chunkBuffer) / mal_get_sample_size_in_bytes(pDevice->format) / pDevice->channels;

        for (;;) {
            mal_uint32 framesJustRead = (mal_uint32)mal_dsp_read_frames(&pDevice->dsp, chunkFrameCount, chunkBuffer);
            if (framesJustRead == 0) {
                break;
            }

            onRecv(pDevice, framesJustRead, chunkBuffer);

            if (framesJustRead < chunkFrameCount) {
                break;
            }
        }
    }
}

// A helper for changing the state of the device.
static inline void mal_device__set_state(mal_device* pDevice, mal_uint32 newState)
{
    mal_atomic_exchange_32(&pDevice->state, newState);
}

// A helper for getting the state of the device.
static inline mal_uint32 mal_device__get_state(mal_device* pDevice)
{
    return pDevice->state;
}


#ifdef MAL_WIN32
    GUID MAL_GUID_KSDATAFORMAT_SUBTYPE_PCM        = {0x00000001, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
    GUID MAL_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = {0x00000003, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
    //GUID MAL_GUID_KSDATAFORMAT_SUBTYPE_ALAW       = {0x00000006, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
    //GUID MAL_GUID_KSDATAFORMAT_SUBTYPE_MULAW      = {0x00000007, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}};
#endif


mal_bool32 mal_context__device_id_equal(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);

    if (pID0 == pID1) return MAL_TRUE;

    if ((pID0 == NULL && pID1 != NULL) ||
        (pID0 != NULL && pID1 == NULL)) {
        return MAL_FALSE;
    }

    if (pContext->onDeviceIDEqual) {
        return pContext->onDeviceIDEqual(pContext, pID0, pID1);
    }

    return MAL_FALSE;
}


typedef struct
{
    mal_device_type deviceType;
    const mal_device_id* pDeviceID;
    char* pName;
    size_t nameBufferSize;
    mal_bool32 foundDevice;
} mal_context__try_get_device_name_by_id__enum_callback_data;

mal_bool32 mal_context__try_get_device_name_by_id__enum_callback(mal_context* pContext, mal_device_type deviceType, const mal_device_info* pDeviceInfo, void* pUserData)
{
    mal_context__try_get_device_name_by_id__enum_callback_data* pData = (mal_context__try_get_device_name_by_id__enum_callback_data*)pUserData;
    mal_assert(pData != NULL);

    if (pData->deviceType == deviceType) {
        if (pContext->onDeviceIDEqual(pContext, pData->pDeviceID, &pDeviceInfo->id)) {
            mal_strncpy_s(pData->pName, pData->nameBufferSize, pDeviceInfo->name, (size_t)-1);
            pData->foundDevice = MAL_TRUE;
        }
    }

    return !pData->foundDevice;
}

// Generic function for retrieving the name of a device by it's ID.
//
// This function simply enumerates every device and then retrieves the name of the first device that has the same ID.
mal_result mal_context__try_get_device_name_by_id(mal_context* pContext, mal_device_type type, const mal_device_id* pDeviceID, char* pName, size_t nameBufferSize)
{
    mal_assert(pContext != NULL);
    mal_assert(pName != NULL);

    if (pDeviceID == NULL) {
        return MAL_NO_DEVICE;
    }

    mal_context__try_get_device_name_by_id__enum_callback_data data;
    data.deviceType = type;
    data.pDeviceID = pDeviceID;
    data.pName = pName;
    data.nameBufferSize = nameBufferSize;
    data.foundDevice = MAL_FALSE;
    mal_result result = mal_context_enumerate_devices(pContext, mal_context__try_get_device_name_by_id__enum_callback, &data);
    if (result != MAL_SUCCESS) {
        return result;
    }

    if (!data.foundDevice) {
        return MAL_NO_DEVICE;
    } else {
        return MAL_SUCCESS;
    }
}


///////////////////////////////////////////////////////////////////////////////
//
// Null Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_NULL

mal_bool32 mal_context_is_device_id_equal__null(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return pID0->nullbackend == pID1->nullbackend;
}

mal_result mal_context_enumerate_devices__null(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    mal_bool32 cbResult = MAL_TRUE;

    // Playback.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), "NULL Playback Device", (size_t)-1);
        cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
    }

    // Capture.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), "NULL Capture Device", (size_t)-1);
        cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__null(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    if (pDeviceID != NULL && pDeviceID->nullbackend != 0) {
        return MAL_NO_DEVICE;   // Don't know the device.
    }

    // Name / Description
    if (deviceType == mal_device_type_playback) {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), "NULL Playback Device", (size_t)-1);
    } else {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), "NULL Capture Device", (size_t)-1);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_init__null(mal_context* pContext)
{
    mal_assert(pContext != NULL);

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__null;
    pContext->onEnumDevices   = mal_context_enumerate_devices__null;
    pContext->onGetDeviceInfo = mal_context_get_device_info__null;

    // The null backend always works.
    return MAL_SUCCESS;
}

mal_result mal_context_uninit__null(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_null);

    (void)pContext;
    return MAL_SUCCESS;
}

void mal_device_uninit__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);
    mal_free(pDevice->null_device.pBuffer);
}

mal_result mal_device_init__null(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;
    (void)type;
    (void)pDeviceID;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->null_device);

    pDevice->bufferSizeInFrames = pConfig->bufferSizeInFrames;
    pDevice->periods = pConfig->periods;

    if (type == mal_device_type_playback) {
        mal_strncpy_s(pDevice->name, sizeof(pDevice->name), "NULL Playback Device", (size_t)-1);
    } else {
        mal_strncpy_s(pDevice->name, sizeof(pDevice->name), "NULL Capture Device", (size_t)-1);
    }

    pDevice->null_device.pBuffer = (mal_uint8*)mal_malloc(pDevice->bufferSizeInFrames * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format));
    if (pDevice->null_device.pBuffer == NULL) {
        return MAL_OUT_OF_MEMORY;
    }

    mal_zero_memory(pDevice->null_device.pBuffer, mal_device_get_buffer_size_in_bytes(pDevice));

    return MAL_SUCCESS;
}

mal_result mal_device__start_backend__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_timer_init(&pDevice->null_device.timer);
    pDevice->null_device.lastProcessedFrame = 0;

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);
    (void)pDevice;

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->null_device.breakFromMainLoop = MAL_TRUE;
    return MAL_SUCCESS;
}

mal_bool32 mal_device__get_current_frame__null(mal_device* pDevice, mal_uint32* pCurrentPos)
{
    mal_assert(pDevice != NULL);
    mal_assert(pCurrentPos != NULL);
    *pCurrentPos = 0;

    mal_uint64 currentFrameAbs = (mal_uint64)(mal_timer_get_time_in_seconds(&pDevice->null_device.timer) * pDevice->sampleRate) / pDevice->channels;

    *pCurrentPos = (mal_uint32)(currentFrameAbs % pDevice->bufferSizeInFrames);
    return MAL_TRUE;
}

mal_uint32 mal_device__get_available_frames__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_uint32 currentFrame;
    if (!mal_device__get_current_frame__null(pDevice, &currentFrame)) {
        return 0;
    }

    // In a playback device the last processed frame should always be ahead of the current frame. The space between
    // the last processed and current frame (moving forward, starting from the last processed frame) is the amount
    // of space available to write.
    //
    // For a recording device it's the other way around - the last processed frame is always _behind_ the current
    // frame and the space between is the available space.
    mal_uint32 totalFrameCount = pDevice->bufferSizeInFrames;
    if (pDevice->type == mal_device_type_playback) {
        mal_uint32 committedBeg = currentFrame;
        mal_uint32 committedEnd = pDevice->null_device.lastProcessedFrame;
        if (committedEnd <= committedBeg) {
            committedEnd += totalFrameCount;    // Wrap around.
        }

        mal_uint32 committedSize = (committedEnd - committedBeg);
        mal_assert(committedSize <= totalFrameCount);

        return totalFrameCount - committedSize;
    } else {
        mal_uint32 validBeg = pDevice->null_device.lastProcessedFrame;
        mal_uint32 validEnd = currentFrame;
        if (validEnd < validBeg) {
            validEnd += totalFrameCount;        // Wrap around.
        }

        mal_uint32 validSize = (validEnd - validBeg);
        mal_assert(validSize <= totalFrameCount);

        return validSize;
    }
}

mal_uint32 mal_device__wait_for_frames__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    while (!pDevice->null_device.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__get_available_frames__null(pDevice);
        if (framesAvailable > 0) {
            return framesAvailable;
        }

        mal_sleep(16);
    }

    // We'll get here if the loop was terminated. Just return whatever's available.
    return mal_device__get_available_frames__null(pDevice);
}

mal_result mal_device__main_loop__null(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->null_device.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->null_device.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__wait_for_frames__null(pDevice);
        if (framesAvailable == 0) {
            continue;
        }

        // If it's a playback device, don't bother grabbing more data if the device is being stopped.
        if (pDevice->null_device.breakFromMainLoop && pDevice->type == mal_device_type_playback) {
            return MAL_FALSE;
        }

        if (framesAvailable + pDevice->null_device.lastProcessedFrame > pDevice->bufferSizeInFrames) {
            framesAvailable = pDevice->bufferSizeInFrames - pDevice->null_device.lastProcessedFrame;
        }

        mal_uint32 sampleCount = framesAvailable * pDevice->channels;
        mal_uint32 lockOffset  = pDevice->null_device.lastProcessedFrame * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format);
        mal_uint32 lockSize    = sampleCount * mal_get_sample_size_in_bytes(pDevice->format);

        if (pDevice->type == mal_device_type_playback) {
            if (pDevice->null_device.breakFromMainLoop) {
                return MAL_FALSE;
            }

            mal_device__read_frames_from_client(pDevice, framesAvailable, pDevice->null_device.pBuffer + lockOffset);
        } else {
            mal_zero_memory(pDevice->null_device.pBuffer + lockOffset, lockSize);
            mal_device__send_frames_to_client(pDevice, framesAvailable, pDevice->null_device.pBuffer + lockOffset);
        }

        pDevice->null_device.lastProcessedFrame = (pDevice->null_device.lastProcessedFrame + framesAvailable) % pDevice->bufferSizeInFrames;
    }

    return MAL_SUCCESS;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// WIN32 COMMON
//
///////////////////////////////////////////////////////////////////////////////
#if defined(MAL_WIN32)
#include "objbase.h"
#if defined(MAL_WIN32_DESKTOP)
    #define mal_CoInitializeEx(pContext, pvReserved, dwCoInit)                          ((MAL_PFN_CoInitializeEx)pContext->win32.CoInitializeEx)(pvReserved, dwCoInit)
    #define mal_CoUninitialize(pContext)                                                ((MAL_PFN_CoUninitialize)pContext->win32.CoUninitialize)()
    #define mal_CoCreateInstance(pContext, rclsid, pUnkOuter, dwClsContext, riid, ppv)  ((MAL_PFN_CoCreateInstance)pContext->win32.CoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv)
    #define mal_CoTaskMemFree(pContext, pv)                                             ((MAL_PFN_CoTaskMemFree)pContext->win32.CoTaskMemFree)(pv)
    #define mal_PropVariantClear(pContext, pvar)                                        ((MAL_PFN_PropVariantClear)pContext->win32.PropVariantClear)(pvar)
#else
    #define mal_CoInitializeEx(pContext, pvReserved, dwCoInit)                          CoInitializeEx(pvReserved, dwCoInit)
    #define mal_CoUninitialize(pContext)                                                CoUninitialize()
    #define mal_CoCreateInstance(pContext, rclsid, pUnkOuter, dwClsContext, riid, ppv)  CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv)
    #define mal_CoTaskMemFree(pContext, pv)                                             CoTaskMemFree(pv)
    #define mal_PropVariantClear(pContext, pvar)                                        PropVariantClear(pvar)
#endif

// There's a few common headers for Win32 backends which include here for simplicity. Note that we should never
// include any files that do not come standard with modern compilers, and we may need to manually define a few
// symbols.
#include <mmreg.h>
#include <mmsystem.h>

#if !defined(MAXULONG_PTR)
typedef size_t DWORD_PTR;
#endif

#if !defined(WAVE_FORMAT_44M08)
#define WAVE_FORMAT_44M08 0x00000100
#define WAVE_FORMAT_44S08 0x00000200
#define WAVE_FORMAT_44M16 0x00000400
#define WAVE_FORMAT_44S16 0x00000800
#define WAVE_FORMAT_48M08 0x00001000
#define WAVE_FORMAT_48S08 0x00002000
#define WAVE_FORMAT_48M16 0x00004000
#define WAVE_FORMAT_48S16 0x00008000
#define WAVE_FORMAT_96M08 0x00010000
#define WAVE_FORMAT_96S08 0x00020000
#define WAVE_FORMAT_96M16 0x00040000
#define WAVE_FORMAT_96S16 0x00080000
#endif

#ifndef SPEAKER_FRONT_LEFT
#define SPEAKER_FRONT_LEFT            0x1
#define SPEAKER_FRONT_RIGHT           0x2
#define SPEAKER_FRONT_CENTER          0x4
#define SPEAKER_LOW_FREQUENCY         0x8
#define SPEAKER_BACK_LEFT             0x10
#define SPEAKER_BACK_RIGHT            0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER  0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER 0x80
#define SPEAKER_BACK_CENTER           0x100
#define SPEAKER_SIDE_LEFT             0x200
#define SPEAKER_SIDE_RIGHT            0x400
#define SPEAKER_TOP_CENTER            0x800
#define SPEAKER_TOP_FRONT_LEFT        0x1000
#define SPEAKER_TOP_FRONT_CENTER      0x2000
#define SPEAKER_TOP_FRONT_RIGHT       0x4000
#define SPEAKER_TOP_BACK_LEFT         0x8000
#define SPEAKER_TOP_BACK_CENTER       0x10000
#define SPEAKER_TOP_BACK_RIGHT        0x20000
#endif

// The SDK that comes with old versions of MSVC (VC6, for example) does not appear to define WAVEFORMATEXTENSIBLE. We
// define our own implementation in this case.
#if defined(_MSC_VER) && !defined(_WAVEFORMATEXTENSIBLE_)
typedef struct
{
    WAVEFORMATEX Format;
    union
    {
        WORD wValidBitsPerSample;
        WORD wSamplesPerBlock;
        WORD wReserved;
    } Samples;
    DWORD dwChannelMask;
    GUID SubFormat;
} WAVEFORMATEXTENSIBLE;
#endif

#ifndef WAVE_FORMAT_EXTENSIBLE
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

GUID MAL_GUID_NULL = {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

// Converts an individual Win32-style channel identifier (SPEAKER_FRONT_LEFT, etc.) to mini_al.
mal_uint8 mal_channel_id_to_mal__win32(DWORD id)
{
    switch (id)
    {
        case SPEAKER_FRONT_LEFT:            return MAL_CHANNEL_FRONT_LEFT;
        case SPEAKER_FRONT_RIGHT:           return MAL_CHANNEL_FRONT_RIGHT;
        case SPEAKER_FRONT_CENTER:          return MAL_CHANNEL_FRONT_CENTER;
        case SPEAKER_LOW_FREQUENCY:         return MAL_CHANNEL_LFE;
        case SPEAKER_BACK_LEFT:             return MAL_CHANNEL_BACK_LEFT;
        case SPEAKER_BACK_RIGHT:            return MAL_CHANNEL_BACK_RIGHT;
        case SPEAKER_FRONT_LEFT_OF_CENTER:  return MAL_CHANNEL_FRONT_LEFT_CENTER;
        case SPEAKER_FRONT_RIGHT_OF_CENTER: return MAL_CHANNEL_FRONT_RIGHT_CENTER;
        case SPEAKER_BACK_CENTER:           return MAL_CHANNEL_BACK_CENTER;
        case SPEAKER_SIDE_LEFT:             return MAL_CHANNEL_SIDE_LEFT;
        case SPEAKER_SIDE_RIGHT:            return MAL_CHANNEL_SIDE_RIGHT;
        case SPEAKER_TOP_CENTER:            return MAL_CHANNEL_TOP_CENTER;
        case SPEAKER_TOP_FRONT_LEFT:        return MAL_CHANNEL_TOP_FRONT_LEFT;
        case SPEAKER_TOP_FRONT_CENTER:      return MAL_CHANNEL_TOP_FRONT_CENTER;
        case SPEAKER_TOP_FRONT_RIGHT:       return MAL_CHANNEL_TOP_FRONT_RIGHT;
        case SPEAKER_TOP_BACK_LEFT:         return MAL_CHANNEL_TOP_BACK_LEFT;
        case SPEAKER_TOP_BACK_CENTER:       return MAL_CHANNEL_TOP_BACK_CENTER;
        case SPEAKER_TOP_BACK_RIGHT:        return MAL_CHANNEL_TOP_BACK_RIGHT;
        default: return 0;
    }
}

// Converts an individual mini_al channel identifier (MAL_CHANNEL_FRONT_LEFT, etc.) to Win32-style.
DWORD mal_channel_id_to_win32(DWORD id)
{
    switch (id)
    {
        case MAL_CHANNEL_MONO:               return SPEAKER_FRONT_CENTER;
        case MAL_CHANNEL_FRONT_LEFT:         return SPEAKER_FRONT_LEFT;
        case MAL_CHANNEL_FRONT_RIGHT:        return SPEAKER_FRONT_RIGHT;
        case MAL_CHANNEL_FRONT_CENTER:       return SPEAKER_FRONT_CENTER;
        case MAL_CHANNEL_LFE:                return SPEAKER_LOW_FREQUENCY;
        case MAL_CHANNEL_BACK_LEFT:          return SPEAKER_BACK_LEFT;
        case MAL_CHANNEL_BACK_RIGHT:         return SPEAKER_BACK_RIGHT;
        case MAL_CHANNEL_FRONT_LEFT_CENTER:  return SPEAKER_FRONT_LEFT_OF_CENTER;
        case MAL_CHANNEL_FRONT_RIGHT_CENTER: return SPEAKER_FRONT_RIGHT_OF_CENTER;
        case MAL_CHANNEL_BACK_CENTER:        return SPEAKER_BACK_CENTER;
        case MAL_CHANNEL_SIDE_LEFT:          return SPEAKER_SIDE_LEFT;
        case MAL_CHANNEL_SIDE_RIGHT:         return SPEAKER_SIDE_RIGHT;
        case MAL_CHANNEL_TOP_CENTER:         return SPEAKER_TOP_CENTER;
        case MAL_CHANNEL_TOP_FRONT_LEFT:     return SPEAKER_TOP_FRONT_LEFT;
        case MAL_CHANNEL_TOP_FRONT_CENTER:   return SPEAKER_TOP_FRONT_CENTER;
        case MAL_CHANNEL_TOP_FRONT_RIGHT:    return SPEAKER_TOP_FRONT_RIGHT;
        case MAL_CHANNEL_TOP_BACK_LEFT:      return SPEAKER_TOP_BACK_LEFT;
        case MAL_CHANNEL_TOP_BACK_CENTER:    return SPEAKER_TOP_BACK_CENTER;
        case MAL_CHANNEL_TOP_BACK_RIGHT:     return SPEAKER_TOP_BACK_RIGHT;
        default: return 0;
    }
}

// Converts a channel mapping to a Win32-style channel mask.
DWORD mal_channel_map_to_channel_mask__win32(const mal_channel channelMap[MAL_MAX_CHANNELS], mal_uint32 channels)
{
    DWORD dwChannelMask = 0;
    for (mal_uint32 iChannel = 0; iChannel < channels; ++iChannel) {
        dwChannelMask |= mal_channel_id_to_win32(channelMap[iChannel]);
    }

    return dwChannelMask;
}

// Converts a Win32-style channel mask to a mini_al channel map.
void mal_channel_mask_to_channel_map__win32(DWORD dwChannelMask, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    if (channels == 1 && dwChannelMask == 0) {
        channelMap[0] = MAL_CHANNEL_MONO;
    } else if (channels == 2 && dwChannelMask == 0) {
        channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
        channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
    } else {
        // Just iterate over each bit.
        mal_uint32 iChannel = 0;
        for (mal_uint32 iBit = 0; iBit < 32; ++iBit) {
            DWORD bitValue = (dwChannelMask & (1UL << iBit));
            if (bitValue != 0) {
                // The bit is set.
                channelMap[iChannel] = mal_channel_id_to_mal__win32(bitValue);
                iChannel += 1;
            }
        }
    }
}

#ifdef __cplusplus
#define mal_is_guid_equal(a, b) IsEqualGUID(*((const GUID*)a), *((const GUID*)b))
#else
#define mal_is_guid_equal(a, b) IsEqualGUID((const GUID*)a, (const GUID*)b)
#endif

mal_format mal_format_from_WAVEFORMATEX(WAVEFORMATEX* pWF)
{
    mal_assert(pWF != NULL);

    if (pWF->wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
        WAVEFORMATEXTENSIBLE* pWFEX = (WAVEFORMATEXTENSIBLE*)pWF;
        if (mal_is_guid_equal(&pWFEX->SubFormat, &MAL_GUID_KSDATAFORMAT_SUBTYPE_PCM)) {
            if (pWFEX->Samples.wValidBitsPerSample == 32) {
                return mal_format_s32;
            }
            if (pWFEX->Samples.wValidBitsPerSample == 24) {
                if (pWFEX->Format.wBitsPerSample == 32) {
                    //return mal_format_s24_32;
                }
                if (pWFEX->Format.wBitsPerSample == 24) {
                    return mal_format_s24;
                }
            }
            if (pWFEX->Samples.wValidBitsPerSample == 16) {
                return mal_format_s16;
            }
            if (pWFEX->Samples.wValidBitsPerSample == 8) {
                return mal_format_u8;
            }
        }
        if (mal_is_guid_equal(&pWFEX->SubFormat, &MAL_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT)) {
            if (pWFEX->Samples.wValidBitsPerSample == 32) {
                return mal_format_f32;
            }
            //if (pWFEX->Samples.wValidBitsPerSample == 64) {
            //    return mal_format_f64;
            //}
        }
    } else {
        if (pWF->wFormatTag == WAVE_FORMAT_PCM) {
            if (pWF->wBitsPerSample == 32) {
                return mal_format_s32;
            }
            if (pWF->wBitsPerSample == 24) {
                return mal_format_s24;
            }
            if (pWF->wBitsPerSample == 16) {
                return mal_format_s16;
            }
            if (pWF->wBitsPerSample == 8) {
                return mal_format_u8;
            }
        }
        if (pWF->wFormatTag == WAVE_FORMAT_IEEE_FLOAT) {
            if (pWF->wBitsPerSample == 32) {
                return mal_format_f32;
            }
            if (pWF->wBitsPerSample == 64) {
                //return mal_format_f64;
            }
        }
    }

    return mal_format_unknown;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// WASAPI Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_WASAPI
//#if defined(_MSC_VER)
//    #pragma warning(push)
//    #pragma warning(disable:4091)   // 'typedef ': ignored on left of '' when no variable is declared
//#endif
//#include <audioclient.h>
//#include <mmdeviceapi.h>
//#if defined(_MSC_VER)
//    #pragma warning(pop)
//#endif

#ifndef PROPERTYKEY_DEFINED
#define PROPERTYKEY_DEFINED
typedef struct
{
    GUID fmtid;
    DWORD pid;
} PROPERTYKEY;
#endif

// Some compilers don't define PropVariantInit(). We just do this ourselves since it's just a memset().
static inline void mal_PropVariantInit(PROPVARIANT* pProp)
{
    mal_zero_object(pProp);
}

const PROPERTYKEY MAL_PKEY_Device_FriendlyName      = {{0xA45C254E, 0xDF1C, 0x4EFD, {0x80, 0x20, 0x67, 0xD1, 0x46, 0xA8, 0x50, 0xE0}}, 14};
const PROPERTYKEY MAL_PKEY_AudioEngine_DeviceFormat = {{0xF19F064D, 0x82C,  0x4E27, {0xBC, 0x73, 0x68, 0x82, 0xA1, 0xBB, 0x8E, 0x4C}},  0};

const IID MAL_IID_IAudioClient                      = {0x1CB9AD4C, 0xDBFA, 0x4C32, {0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2}}; // 1CB9AD4C-DBFA-4C32-B178-C2F568A703B2 = __uuidof(IAudioClient)
const IID MAL_IID_IAudioRenderClient                = {0xF294ACFC, 0x3146, 0x4483, {0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2}}; // F294ACFC-3146-4483-A7BF-ADDCA7C260E2 = __uuidof(IAudioRenderClient)
const IID MAL_IID_IAudioCaptureClient               = {0xC8ADBD64, 0xE71E, 0x48A0, {0xA4, 0xDE, 0x18, 0x5C, 0x39, 0x5C, 0xD3, 0x17}}; // C8ADBD64-E71E-48A0-A4DE-185C395CD317 = __uuidof(IAudioCaptureClient)
#ifndef MAL_WIN32_DESKTOP
const IID MAL_IID_DEVINTERFACE_AUDIO_RENDER         = {0xE6327CAD, 0xDCEC, 0x4949, {0xAE, 0x8A, 0x99, 0x1E, 0x97, 0x6A, 0x79, 0xD2}}; // E6327CAD-DCEC-4949-AE8A-991E976A79D2
const IID MAL_IID_DEVINTERFACE_AUDIO_CAPTURE        = {0x2EEF81BE, 0x33FA, 0x4800, {0x96, 0x70, 0x1C, 0xD4, 0x74, 0x97, 0x2C, 0x3F}}; // 2EEF81BE-33FA-4800-9670-1CD474972C3F
#endif

const IID MAL_CLSID_MMDeviceEnumerator_Instance     = {0xBCDE0395, 0xE52F, 0x467C, {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}}; // BCDE0395-E52F-467C-8E3D-C4579291692E = __uuidof(MMDeviceEnumerator)
const IID MAL_IID_IMMDeviceEnumerator_Instance      = {0xA95664D2, 0x9614, 0x4F35, {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}}; // A95664D2-9614-4F35-A746-DE8DB63617E6 = __uuidof(IMMDeviceEnumerator)
#ifdef __cplusplus
#define MAL_CLSID_MMDeviceEnumerator MAL_CLSID_MMDeviceEnumerator_Instance
#define MAL_IID_IMMDeviceEnumerator  MAL_IID_IMMDeviceEnumerator_Instance
#else
#define MAL_CLSID_MMDeviceEnumerator &MAL_CLSID_MMDeviceEnumerator_Instance
#define MAL_IID_IMMDeviceEnumerator  &MAL_IID_IMMDeviceEnumerator_Instance
#endif

#ifdef MAL_WIN32_DESKTOP
#define MAL_MM_DEVICE_STATE_ACTIVE                          1
#define MAL_MM_DEVICE_STATE_DISABLED                        2
#define MAL_MM_DEVICE_STATE_NOTPRESENT                      4
#define MAL_MM_DEVICE_STATE_UNPLUGGED                       8

typedef struct mal_IMMDeviceEnumerator                      mal_IMMDeviceEnumerator;
typedef struct mal_IMMDeviceCollection                      mal_IMMDeviceCollection;
typedef struct mal_IMMDevice                                mal_IMMDevice;
typedef struct mal_IMMNotificationClient                    mal_IMMNotificationClient;
#else
typedef struct mal_IActivateAudioInterfaceAsyncOperation    mal_IActivateAudioInterfaceAsyncOperation;
#endif
typedef struct mal_IPropertyStore                           mal_IPropertyStore;
typedef struct mal_IAudioClient                             mal_IAudioClient;
typedef struct mal_IAudioRenderClient                       mal_IAudioRenderClient;
typedef struct mal_IAudioCaptureClient                      mal_IAudioCaptureClient;

typedef mal_int64                                           MAL_REFERENCE_TIME;

#define MAL_AUDCLNT_STREAMFLAGS_CROSSPROCESS                0x00010000
#define MAL_AUDCLNT_STREAMFLAGS_LOOPBACK                    0x00020000
#define MAL_AUDCLNT_STREAMFLAGS_EVENTCALLBACK               0x00040000
#define MAL_AUDCLNT_STREAMFLAGS_NOPERSIST                   0x00080000
#define MAL_AUDCLNT_STREAMFLAGS_RATEADJUST                  0x00100000
#define MAL_AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY         0x08000000
#define MAL_AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM              0x80000000
#define MAL_AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED          0x10000000
#define MAL_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDE               0x20000000
#define MAL_AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED    0x40000000

// We only care about a few error codes.
#define MAL_AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED               (-2004287463)
#define MAL_AUDCLNT_S_BUFFER_EMPTY                          (143196161)

typedef enum
{
    mal_eRender  = 0,
    mal_eCapture = 1,
    mal_eAll     = 2
} mal_EDataFlow;

typedef enum
{
    mal_eConsole        = 0,
    mal_eMultimedia     = 1,
    mal_eCommunications = 2
} mal_ERole;

typedef enum
{
    MAL_AUDCLNT_SHAREMODE_SHARED,
    MAL_AUDCLNT_SHAREMODE_EXCLUSIVE
} MAL_AUDCLNT_SHAREMODE;

#ifdef MAL_WIN32_DESKTOP
    // IMMDeviceEnumerator
    typedef struct
    {
        // IUnknown
        HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IMMDeviceEnumerator* pThis, const IID* const riid, void** ppObject);
        ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IMMDeviceEnumerator* pThis);
        ULONG   (STDMETHODCALLTYPE * Release)       (mal_IMMDeviceEnumerator* pThis);

        // IMMDeviceEnumerator
        HRESULT (STDMETHODCALLTYPE * EnumAudioEndpoints)                    (mal_IMMDeviceEnumerator* pThis, mal_EDataFlow dataFlow, DWORD dwStateMask, mal_IMMDeviceCollection** ppDevices);
        HRESULT (STDMETHODCALLTYPE * GetDefaultAudioEndpoint)               (mal_IMMDeviceEnumerator* pThis, mal_EDataFlow dataFlow, mal_ERole role, mal_IMMDevice** ppEndpoint);
        HRESULT (STDMETHODCALLTYPE * GetDevice)                             (mal_IMMDeviceEnumerator* pThis, LPCWSTR pID, mal_IMMDevice** ppDevice);
        HRESULT (STDMETHODCALLTYPE * RegisterEndpointNotificationCallback)  (mal_IMMDeviceEnumerator* pThis, mal_IMMNotificationClient* pClient);
        HRESULT (STDMETHODCALLTYPE * UnregisterEndpointNotificationCallback)(mal_IMMDeviceEnumerator* pThis, mal_IMMNotificationClient* pClient);
    } mal_IMMDeviceEnumeratorVtbl;
    struct mal_IMMDeviceEnumerator
    {
        mal_IMMDeviceEnumeratorVtbl* lpVtbl;
    };
    HRESULT mal_IMMDeviceEnumerator_QueryInterface(mal_IMMDeviceEnumerator* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
    ULONG   mal_IMMDeviceEnumerator_AddRef(mal_IMMDeviceEnumerator* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
    ULONG   mal_IMMDeviceEnumerator_Release(mal_IMMDeviceEnumerator* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
    HRESULT mal_IMMDeviceEnumerator_EnumAudioEndpoints(mal_IMMDeviceEnumerator* pThis, mal_EDataFlow dataFlow, DWORD dwStateMask, mal_IMMDeviceCollection** ppDevices) { return pThis->lpVtbl->EnumAudioEndpoints(pThis, dataFlow, dwStateMask, ppDevices); }
    HRESULT mal_IMMDeviceEnumerator_GetDefaultAudioEndpoint(mal_IMMDeviceEnumerator* pThis, mal_EDataFlow dataFlow, mal_ERole role, mal_IMMDevice** ppEndpoint) { return pThis->lpVtbl->GetDefaultAudioEndpoint(pThis, dataFlow, role, ppEndpoint); }
    HRESULT mal_IMMDeviceEnumerator_GetDevice(mal_IMMDeviceEnumerator* pThis, LPCWSTR pID, mal_IMMDevice** ppDevice)       { return pThis->lpVtbl->GetDevice(pThis, pID, ppDevice); }
    HRESULT mal_IMMDeviceEnumerator_RegisterEndpointNotificationCallback(mal_IMMDeviceEnumerator* pThis, mal_IMMNotificationClient* pClient) { return pThis->lpVtbl->RegisterEndpointNotificationCallback(pThis, pClient); }
    HRESULT mal_IMMDeviceEnumerator_UnregisterEndpointNotificationCallback(mal_IMMDeviceEnumerator* pThis, mal_IMMNotificationClient* pClient) { return pThis->lpVtbl->UnregisterEndpointNotificationCallback(pThis, pClient); }


    // IMMDeviceCollection
    typedef struct
    {
        // IUnknown
        HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IMMDeviceCollection* pThis, const IID* const riid, void** ppObject);
        ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IMMDeviceCollection* pThis);
        ULONG   (STDMETHODCALLTYPE * Release)       (mal_IMMDeviceCollection* pThis);

        // IMMDeviceCollection
        HRESULT (STDMETHODCALLTYPE * GetCount)(mal_IMMDeviceCollection* pThis, UINT* pDevices);
        HRESULT (STDMETHODCALLTYPE * Item)    (mal_IMMDeviceCollection* pThis, UINT nDevice, mal_IMMDevice** ppDevice);
    } mal_IMMDeviceCollectionVtbl;
    struct mal_IMMDeviceCollection
    {
        mal_IMMDeviceCollectionVtbl* lpVtbl;
    };
    HRESULT mal_IMMDeviceCollection_QueryInterface(mal_IMMDeviceCollection* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
    ULONG   mal_IMMDeviceCollection_AddRef(mal_IMMDeviceCollection* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
    ULONG   mal_IMMDeviceCollection_Release(mal_IMMDeviceCollection* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
    HRESULT mal_IMMDeviceCollection_GetCount(mal_IMMDeviceCollection* pThis, UINT* pDevices)                               { return pThis->lpVtbl->GetCount(pThis, pDevices); }
    HRESULT mal_IMMDeviceCollection_Item(mal_IMMDeviceCollection* pThis, UINT nDevice, mal_IMMDevice** ppDevice)           { return pThis->lpVtbl->Item(pThis, nDevice, ppDevice); }


    // IMMDevice
    typedef struct
    {
        // IUnknown
        HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IMMDevice* pThis, const IID* const riid, void** ppObject);
        ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IMMDevice* pThis);
        ULONG   (STDMETHODCALLTYPE * Release)       (mal_IMMDevice* pThis);

        // IMMDevice
        HRESULT (STDMETHODCALLTYPE * Activate)         (mal_IMMDevice* pThis, const IID* const iid, DWORD dwClsCtx, PROPVARIANT* pActivationParams, void** ppInterface);
        HRESULT (STDMETHODCALLTYPE * OpenPropertyStore)(mal_IMMDevice* pThis, DWORD stgmAccess, mal_IPropertyStore** ppProperties);
        HRESULT (STDMETHODCALLTYPE * GetId)            (mal_IMMDevice* pThis, LPWSTR *pID);
        HRESULT (STDMETHODCALLTYPE * GetState)         (mal_IMMDevice* pThis, DWORD *pState);
    } mal_IMMDeviceVtbl;
    struct mal_IMMDevice
    {
        mal_IMMDeviceVtbl* lpVtbl;
    };
    HRESULT mal_IMMDevice_QueryInterface(mal_IMMDevice* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
    ULONG   mal_IMMDevice_AddRef(mal_IMMDevice* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
    ULONG   mal_IMMDevice_Release(mal_IMMDevice* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
    HRESULT mal_IMMDevice_Activate(mal_IMMDevice* pThis, const IID* const iid, DWORD dwClsCtx, PROPVARIANT* pActivationParams, void** ppInterface) { return pThis->lpVtbl->Activate(pThis, iid, dwClsCtx, pActivationParams, ppInterface); }
    HRESULT mal_IMMDevice_OpenPropertyStore(mal_IMMDevice* pThis, DWORD stgmAccess, mal_IPropertyStore** ppProperties) { return pThis->lpVtbl->OpenPropertyStore(pThis, stgmAccess, ppProperties); }
    HRESULT mal_IMMDevice_GetId(mal_IMMDevice* pThis, LPWSTR *pID)                                     { return pThis->lpVtbl->GetId(pThis, pID); }
    HRESULT mal_IMMDevice_GetState(mal_IMMDevice* pThis, DWORD *pState)                                { return pThis->lpVtbl->GetState(pThis, pState); }
#else

    // IActivateAudioInterfaceAsyncOperation
    typedef struct
    {
        // IUnknown
        HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IActivateAudioInterfaceAsyncOperation* pThis, const IID* const riid, void** ppObject);
        ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IActivateAudioInterfaceAsyncOperation* pThis);
        ULONG   (STDMETHODCALLTYPE * Release)       (mal_IActivateAudioInterfaceAsyncOperation* pThis);

        // IActivateAudioInterfaceAsyncOperation
        HRESULT (STDMETHODCALLTYPE * GetActivateResult)(mal_IActivateAudioInterfaceAsyncOperation* pThis, HRESULT *pActivateResult, IUnknown** ppActivatedInterface);
    } mal_IActivateAudioInterfaceAsyncOperationVtbl;
    struct mal_IActivateAudioInterfaceAsyncOperation
    {
        mal_IActivateAudioInterfaceAsyncOperationVtbl* lpVtbl;
    };
    HRESULT mal_IActivateAudioInterfaceAsyncOperation_QueryInterface(mal_IActivateAudioInterfaceAsyncOperation* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
    ULONG   mal_IActivateAudioInterfaceAsyncOperation_AddRef(mal_IActivateAudioInterfaceAsyncOperation* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
    ULONG   mal_IActivateAudioInterfaceAsyncOperation_Release(mal_IActivateAudioInterfaceAsyncOperation* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
    HRESULT mal_IActivateAudioInterfaceAsyncOperation_GetActivateResult(mal_IActivateAudioInterfaceAsyncOperation* pThis, HRESULT *pActivateResult, IUnknown** ppActivatedInterface) { return pThis->lpVtbl->GetActivateResult(pThis, pActivateResult, ppActivatedInterface); }
#endif

// IPropertyStore
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IPropertyStore* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IPropertyStore* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IPropertyStore* pThis);

    // IPropertyStore
    HRESULT (STDMETHODCALLTYPE * GetCount)(mal_IPropertyStore* pThis, DWORD* pPropCount);
    HRESULT (STDMETHODCALLTYPE * GetAt)   (mal_IPropertyStore* pThis, DWORD propIndex, PROPERTYKEY* pPropKey);
    HRESULT (STDMETHODCALLTYPE * GetValue)(mal_IPropertyStore* pThis, const PROPERTYKEY* const pKey, PROPVARIANT* pPropVar);
    HRESULT (STDMETHODCALLTYPE * SetValue)(mal_IPropertyStore* pThis, const PROPERTYKEY* const pKey, const PROPVARIANT* const pPropVar);
    HRESULT (STDMETHODCALLTYPE * Commit)  (mal_IPropertyStore* pThis);
} mal_IPropertyStoreVtbl;
struct mal_IPropertyStore
{
    mal_IPropertyStoreVtbl* lpVtbl;
};
HRESULT mal_IPropertyStore_QueryInterface(mal_IPropertyStore* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IPropertyStore_AddRef(mal_IPropertyStore* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IPropertyStore_Release(mal_IPropertyStore* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IPropertyStore_GetCount(mal_IPropertyStore* pThis, DWORD* pPropCount)                            { return pThis->lpVtbl->GetCount(pThis, pPropCount); }
HRESULT mal_IPropertyStore_GetAt(mal_IPropertyStore* pThis, DWORD propIndex, PROPERTYKEY* pPropKey)          { return pThis->lpVtbl->GetAt(pThis, propIndex, pPropKey); }
HRESULT mal_IPropertyStore_GetValue(mal_IPropertyStore* pThis, const PROPERTYKEY* const pKey, PROPVARIANT* pPropVar) { return pThis->lpVtbl->GetValue(pThis, pKey, pPropVar); }
HRESULT mal_IPropertyStore_SetValue(mal_IPropertyStore* pThis, const PROPERTYKEY* const pKey, const PROPVARIANT* const pPropVar) { return pThis->lpVtbl->SetValue(pThis, pKey, pPropVar); }
HRESULT mal_IPropertyStore_Commit(mal_IPropertyStore* pThis)                                                 { return pThis->lpVtbl->Commit(pThis); }


// IAudioClient
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IAudioClient* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IAudioClient* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IAudioClient* pThis);

    // IAudioClient
    HRESULT (STDMETHODCALLTYPE * Initialize)       (mal_IAudioClient* pThis, MAL_AUDCLNT_SHAREMODE shareMode, DWORD streamFlags, MAL_REFERENCE_TIME bufferDuration, MAL_REFERENCE_TIME periodicity, const WAVEFORMATEX* pFormat, const GUID* pAudioSessionGuid);
    HRESULT (STDMETHODCALLTYPE * GetBufferSize)    (mal_IAudioClient* pThis, UINT32* pNumBufferFrames);
    HRESULT (STDMETHODCALLTYPE * GetStreamLatency) (mal_IAudioClient* pThis, MAL_REFERENCE_TIME* pLatency);
    HRESULT (STDMETHODCALLTYPE * GetCurrentPadding)(mal_IAudioClient* pThis, UINT32* pNumPaddingFrames);
    HRESULT (STDMETHODCALLTYPE * IsFormatSupported)(mal_IAudioClient* pThis, MAL_AUDCLNT_SHAREMODE shareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch);
    HRESULT (STDMETHODCALLTYPE * GetMixFormat)     (mal_IAudioClient* pThis, WAVEFORMATEX** ppDeviceFormat);
    HRESULT (STDMETHODCALLTYPE * GetDevicePeriod)  (mal_IAudioClient* pThis, MAL_REFERENCE_TIME* pDefaultDevicePeriod, MAL_REFERENCE_TIME* pMinimumDevicePeriod);
    HRESULT (STDMETHODCALLTYPE * Start)            (mal_IAudioClient* pThis);
    HRESULT (STDMETHODCALLTYPE * Stop)             (mal_IAudioClient* pThis);
    HRESULT (STDMETHODCALLTYPE * Reset)            (mal_IAudioClient* pThis);
    HRESULT (STDMETHODCALLTYPE * SetEventHandle)   (mal_IAudioClient* pThis, HANDLE eventHandle);
    HRESULT (STDMETHODCALLTYPE * GetService)       (mal_IAudioClient* pThis, const IID* const riid, void** pp);
} mal_IAudioClientVtbl;
struct mal_IAudioClient
{
    mal_IAudioClientVtbl* lpVtbl;
};
HRESULT mal_IAudioClient_QueryInterface(mal_IAudioClient* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IAudioClient_AddRef(mal_IAudioClient* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IAudioClient_Release(mal_IAudioClient* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IAudioClient_Initialize(mal_IAudioClient* pThis, MAL_AUDCLNT_SHAREMODE shareMode, DWORD streamFlags, MAL_REFERENCE_TIME bufferDuration, MAL_REFERENCE_TIME periodicity, const WAVEFORMATEX* pFormat, const GUID* pAudioSessionGuid) { return pThis->lpVtbl->Initialize(pThis, shareMode, streamFlags, bufferDuration, periodicity, pFormat, pAudioSessionGuid); }
HRESULT mal_IAudioClient_GetBufferSize(mal_IAudioClient* pThis, UINT32* pNumBufferFrames)                { return pThis->lpVtbl->GetBufferSize(pThis, pNumBufferFrames); }
HRESULT mal_IAudioClient_GetStreamLatency(mal_IAudioClient* pThis, MAL_REFERENCE_TIME* pLatency)             { return pThis->lpVtbl->GetStreamLatency(pThis, pLatency); }
HRESULT mal_IAudioClient_GetCurrentPadding(mal_IAudioClient* pThis, UINT32* pNumPaddingFrames)           { return pThis->lpVtbl->GetCurrentPadding(pThis, pNumPaddingFrames); }
HRESULT mal_IAudioClient_IsFormatSupported(mal_IAudioClient* pThis, MAL_AUDCLNT_SHAREMODE shareMode, const WAVEFORMATEX* pFormat, WAVEFORMATEX** ppClosestMatch) { return pThis->lpVtbl->IsFormatSupported(pThis, shareMode, pFormat, ppClosestMatch); }
HRESULT mal_IAudioClient_GetMixFormat(mal_IAudioClient* pThis, WAVEFORMATEX** ppDeviceFormat)            { return pThis->lpVtbl->GetMixFormat(pThis, ppDeviceFormat); }
HRESULT mal_IAudioClient_GetDevicePeriod(mal_IAudioClient* pThis, MAL_REFERENCE_TIME* pDefaultDevicePeriod, MAL_REFERENCE_TIME* pMinimumDevicePeriod) { return pThis->lpVtbl->GetDevicePeriod(pThis, pDefaultDevicePeriod, pMinimumDevicePeriod); }
HRESULT mal_IAudioClient_Start(mal_IAudioClient* pThis)                                                  { return pThis->lpVtbl->Start(pThis); }
HRESULT mal_IAudioClient_Stop(mal_IAudioClient* pThis)                                                   { return pThis->lpVtbl->Stop(pThis); }
HRESULT mal_IAudioClient_Reset(mal_IAudioClient* pThis)                                                  { return pThis->lpVtbl->Reset(pThis); }
HRESULT mal_IAudioClient_SetEventHandle(mal_IAudioClient* pThis, HANDLE eventHandle)                     { return pThis->lpVtbl->SetEventHandle(pThis, eventHandle); }
HRESULT mal_IAudioClient_GetService(mal_IAudioClient* pThis, const IID* const riid, void** pp)           { return pThis->lpVtbl->GetService(pThis, riid, pp); }


// IAudioRenderClient
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IAudioRenderClient* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IAudioRenderClient* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IAudioRenderClient* pThis);

    // IAudioRenderClient
    HRESULT (STDMETHODCALLTYPE * GetBuffer)    (mal_IAudioRenderClient* pThis, UINT32 numFramesRequested, BYTE** ppData);
    HRESULT (STDMETHODCALLTYPE * ReleaseBuffer)(mal_IAudioRenderClient* pThis, UINT32 numFramesWritten, DWORD dwFlags);
} mal_IAudioRenderClientVtbl;
struct mal_IAudioRenderClient
{
    mal_IAudioRenderClientVtbl* lpVtbl;
};
HRESULT mal_IAudioRenderClient_QueryInterface(mal_IAudioRenderClient* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IAudioRenderClient_AddRef(mal_IAudioRenderClient* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IAudioRenderClient_Release(mal_IAudioRenderClient* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IAudioRenderClient_GetBuffer(mal_IAudioRenderClient* pThis, UINT32 numFramesRequested, BYTE** ppData)    { return pThis->lpVtbl->GetBuffer(pThis, numFramesRequested, ppData); }
HRESULT mal_IAudioRenderClient_ReleaseBuffer(mal_IAudioRenderClient* pThis, UINT32 numFramesWritten, DWORD dwFlags)  { return pThis->lpVtbl->ReleaseBuffer(pThis, numFramesWritten, dwFlags); }


// IAudioCaptureClient
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IAudioCaptureClient* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IAudioCaptureClient* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IAudioCaptureClient* pThis);

    // IAudioRenderClient
    HRESULT (STDMETHODCALLTYPE * GetBuffer)        (mal_IAudioCaptureClient* pThis, BYTE** ppData, UINT32* pNumFramesToRead, DWORD* pFlags, UINT64* pDevicePosition, UINT64* pQPCPosition);
    HRESULT (STDMETHODCALLTYPE * ReleaseBuffer)    (mal_IAudioCaptureClient* pThis, UINT32 numFramesRead);
    HRESULT (STDMETHODCALLTYPE * GetNextPacketSize)(mal_IAudioCaptureClient* pThis, UINT32* pNumFramesInNextPacket);
} mal_IAudioCaptureClientVtbl;
struct mal_IAudioCaptureClient
{
    mal_IAudioCaptureClientVtbl* lpVtbl;
};
HRESULT mal_IAudioCaptureClient_QueryInterface(mal_IAudioCaptureClient* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IAudioCaptureClient_AddRef(mal_IAudioCaptureClient* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IAudioCaptureClient_Release(mal_IAudioCaptureClient* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IAudioCaptureClient_GetBuffer(mal_IAudioCaptureClient* pThis, BYTE** ppData, UINT32* pNumFramesToRead, DWORD* pFlags, UINT64* pDevicePosition, UINT64* pQPCPosition) { return pThis->lpVtbl->GetBuffer(pThis, ppData, pNumFramesToRead, pFlags, pDevicePosition, pQPCPosition); }
HRESULT mal_IAudioCaptureClient_ReleaseBuffer(mal_IAudioCaptureClient* pThis, UINT32 numFramesRead)                    { return pThis->lpVtbl->ReleaseBuffer(pThis, numFramesRead); }
HRESULT mal_IAudioCaptureClient_GetNextPacketSize(mal_IAudioCaptureClient* pThis, UINT32* pNumFramesInNextPacket)      { return pThis->lpVtbl->GetNextPacketSize(pThis, pNumFramesInNextPacket); }

mal_bool32 mal_context_is_device_id_equal__wasapi(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return memcmp(pID0->wasapi, pID1->wasapi, sizeof(pID0->wasapi)) == 0;
}

mal_result mal_context_get_device_info_from_MMDevice__wasapi(mal_context* pContext, mal_IMMDevice* pMMDevice, mal_share_mode shareMode, mal_bool32 onlySimpleInfo, mal_device_info* pInfo)
{
    mal_assert(pContext != NULL);
    mal_assert(pMMDevice != NULL);
    mal_assert(pInfo != NULL);

    // ID.
    LPWSTR id;
    HRESULT hr = mal_IMMDevice_GetId(pMMDevice, &id);
    if (SUCCEEDED(hr)) {
        size_t idlen = wcslen(id);
        if (idlen+1 > mal_countof(pInfo->id.wasapi)) {
            mal_CoTaskMemFree(pContext, id);
            mal_assert(MAL_FALSE);  // NOTE: If this is triggered, please report it. It means the format of the ID must haved change and is too long to fit in our fixed sized buffer.
            return MAL_ERROR;
        }

        mal_copy_memory(pInfo->id.wasapi, id, idlen * sizeof(wchar_t));
        pInfo->id.wasapi[idlen] = '\0';

        mal_CoTaskMemFree(pContext, id);
    }

    mal_IPropertyStore *pProperties;
    hr = mal_IMMDevice_OpenPropertyStore(pMMDevice, STGM_READ, &pProperties);
    if (SUCCEEDED(hr)) {
        PROPVARIANT var;

        // Description / Friendly Name
        mal_PropVariantInit(&var);
        hr = mal_IPropertyStore_GetValue(pProperties, &MAL_PKEY_Device_FriendlyName, &var);
        if (SUCCEEDED(hr)) {
            WideCharToMultiByte(CP_UTF8, 0, var.pwszVal, -1, pInfo->name, sizeof(pInfo->name), 0, FALSE);
            mal_PropVariantClear(pContext, &var);
        }

        // Format
        if (!onlySimpleInfo) {
            // TODO:
            // - Get MAL_PKEY_AudioEngine_DeviceFormat for the channel count
            // - Open the device
            // - If shared mode, call GetMixFormat()
            // - If exclusive mode, loop over most common formats (s16, s24, f32), then each of the standard sample rates.
            //   - If anything fails, don't allow exclusive mode for this device.
            (void)shareMode;
        }

        mal_IPropertyStore_Release(pProperties);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_enumerate_device_collection__wasapi(mal_context* pContext, mal_IMMDeviceCollection* pDeviceCollection, mal_device_type deviceType, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    UINT deviceCount;
    HRESULT hr = mal_IMMDeviceCollection_GetCount(pDeviceCollection, &deviceCount);
    if (FAILED(hr)) {
        return mal_context_post_error(pContext, NULL, "[WASAPI] Failed to get playback device count.", MAL_NO_DEVICE);
    }

    for (mal_uint32 iDevice = 0; iDevice < deviceCount; ++iDevice) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);

        mal_IMMDevice* pMMDevice;
        hr = mal_IMMDeviceCollection_Item(pDeviceCollection, iDevice, &pMMDevice);
        if (SUCCEEDED(hr)) {
            mal_result result = mal_context_get_device_info_from_MMDevice__wasapi(pContext, pMMDevice, mal_share_mode_shared, MAL_TRUE, &deviceInfo);   // MAL_TRUE = onlySimpleInfo.

            mal_IMMDevice_Release(pMMDevice);
            if (result == MAL_SUCCESS) {
                mal_bool32 cbResult = callback(pContext, deviceType, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    break;
                }
            }
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_enumerate_devices__wasapi(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    // Different enumeration for desktop and UWP.
#ifdef MAL_WIN32_DESKTOP
    // Desktop
    mal_IMMDeviceEnumerator* pDeviceEnumerator;
    HRESULT hr = mal_CoCreateInstance(pContext, MAL_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, MAL_IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator);
    if (FAILED(hr)) {
        return mal_context_post_error(pContext, NULL, "[WASAPI] Failed to create device enumerator.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    mal_IMMDeviceCollection* pDeviceCollection;

    // Playback.
    hr = mal_IMMDeviceEnumerator_EnumAudioEndpoints(pDeviceEnumerator, mal_eRender, MAL_MM_DEVICE_STATE_ACTIVE, &pDeviceCollection);
    if (SUCCEEDED(hr)) {
        mal_context_enumerate_device_collection__wasapi(pContext, pDeviceCollection, mal_device_type_playback, callback, pUserData);
        mal_IMMDeviceCollection_Release(pDeviceCollection);
    }

    // Capture.
    hr = mal_IMMDeviceEnumerator_EnumAudioEndpoints(pDeviceEnumerator, mal_eCapture, MAL_MM_DEVICE_STATE_ACTIVE, &pDeviceCollection);
    if (SUCCEEDED(hr)) {
        mal_context_enumerate_device_collection__wasapi(pContext, pDeviceCollection, mal_device_type_capture, callback, pUserData);
        mal_IMMDeviceCollection_Release(pDeviceCollection);
    }

    mal_IMMDeviceEnumerator_Release(pDeviceEnumerator);
#else
    // UWP
    //
    // The MMDevice API is only supported on desktop applications. For now, while I'm still figuring out how to properly enumerate
    // over devices without using MMDevice, I'm restricting devices to defaults.
    //
    // Hint: DeviceInformation::FindAllAsync() with DeviceClass.AudioCapture/AudioRender. https://blogs.windows.com/buildingapps/2014/05/15/real-time-audio-in-windows-store-and-windows-phone-apps/
    if (callback) {
        mal_bool32 cbResult = MAL_TRUE;

        // Playback.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
        }

        // Capture.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
        }
    }
#endif

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__wasapi(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_IMMDeviceEnumerator* pDeviceEnumerator;
    HRESULT hr = mal_CoCreateInstance(pContext, MAL_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, MAL_IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator);
    if (FAILED(hr)) {
        return mal_context_post_error(pContext, NULL, "[WASAPI] Failed to create IMMDeviceEnumerator.", MAL_FAILED_TO_INIT_BACKEND);
    }

    mal_IMMDevice* pMMDevice = NULL;
    if (pDeviceID == NULL) {
        hr = mal_IMMDeviceEnumerator_GetDefaultAudioEndpoint(pDeviceEnumerator, (deviceType == mal_device_type_playback) ? mal_eRender : mal_eCapture, mal_eConsole, &pMMDevice);
    } else {
        hr = mal_IMMDeviceEnumerator_GetDevice(pDeviceEnumerator, pDeviceID->wasapi, &pMMDevice);
    }

    mal_IMMDeviceEnumerator_Release(pDeviceEnumerator);
    if (FAILED(hr)) {
        return mal_context_post_error(pContext, NULL, "[WASAPI] Failed to retrieve IMMDevice.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    mal_result result = mal_context_get_device_info_from_MMDevice__wasapi(pContext, pMMDevice, shareMode, MAL_FALSE, pDeviceInfo);   // MAL_FALSE = !onlySimpleInfo.

    mal_IMMDevice_Release(pMMDevice);
    return result;
}


mal_result mal_context_init__wasapi(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    (void)pContext;

    mal_result result = MAL_SUCCESS;

#ifdef MAL_WIN32_DESKTOP
    // WASAPI is only supported in Vista SP1 and newer. The reason for SP1 and not the base version of Vista is that event-driven
    // exclusive mode does not work until SP1.
    OSVERSIONINFOEXW osvi;
    mal_zero_object(&osvi);
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = HIBYTE(_WIN32_WINNT_VISTA);
    osvi.dwMinorVersion = LOBYTE(_WIN32_WINNT_VISTA);
    osvi.wServicePackMajor = 1;
    if (VerifyVersionInfoW(&osvi, VER_MAJORVERSION | VER_MINORVERSION | VER_SERVICEPACKMAJOR, VerSetConditionMask(VerSetConditionMask(VerSetConditionMask(0, VER_MAJORVERSION, VER_GREATER_EQUAL), VER_MINORVERSION, VER_GREATER_EQUAL), VER_SERVICEPACKMAJOR, VER_GREATER_EQUAL))) {
        result = MAL_SUCCESS;
    } else {
        result = MAL_NO_BACKEND;
    }
#endif

    if (result != MAL_SUCCESS) {
        return result;
    }

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__wasapi;
    pContext->onEnumDevices   = mal_context_enumerate_devices__wasapi;
    pContext->onGetDeviceInfo = mal_context_get_device_info__wasapi;

    return result;
}

mal_result mal_context_uninit__wasapi(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_wasapi);
    (void)pContext;

    return MAL_SUCCESS;
}

void mal_device_uninit__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->wasapi.pRenderClient) {
        mal_IAudioRenderClient_Release((mal_IAudioRenderClient*)pDevice->wasapi.pRenderClient);
    }
    if (pDevice->wasapi.pCaptureClient) {
        mal_IAudioCaptureClient_Release((mal_IAudioCaptureClient*)pDevice->wasapi.pCaptureClient);
    }
    if (pDevice->wasapi.pAudioClient) {
        mal_IAudioClient_Release((mal_IAudioClient*)pDevice->wasapi.pAudioClient);
    }

    if (pDevice->wasapi.hEvent) {
        CloseHandle(pDevice->wasapi.hEvent);
    }
    if (pDevice->wasapi.hStopEvent) {
        CloseHandle(pDevice->wasapi.hStopEvent);
    }
}

// This is the part that's preventing mini_al from being compiled as C with UWP. We need to implement IActivateAudioInterfaceCompletionHandler
// in C which is quite annoying.
#ifndef MAL_WIN32_DESKTOP
    #ifdef __cplusplus
    #include <mmdeviceapi.h>
    #include <wrl\implements.h>

    class malCompletionHandler : public Microsoft::WRL::RuntimeClass< Microsoft::WRL::RuntimeClassFlags< Microsoft::WRL::ClassicCom >, Microsoft::WRL::FtmBase, IActivateAudioInterfaceCompletionHandler >
    {
    public:

        malCompletionHandler()
            : m_hEvent(NULL)
        {
        }

        mal_result Init()
        {
            m_hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
            if (m_hEvent == NULL) {
                return MAL_ERROR;
            }

            return MAL_SUCCESS;
        }

        void Uninit()
        {
            if (m_hEvent != NULL) {
                CloseHandle(m_hEvent);
            }
        }

        void Wait()
        {
            WaitForSingleObject(m_hEvent, INFINITE);
        }

        HRESULT STDMETHODCALLTYPE ActivateCompleted(IActivateAudioInterfaceAsyncOperation *activateOperation)
        {
            (void)activateOperation;
            SetEvent(m_hEvent);
            return S_OK;
        }

    private:
        HANDLE m_hEvent;  // This is created in Init(), deleted in Uninit(), waited on in Wait() and signaled in ActivateCompleted().
    };
    #else
    #error "The UWP build is currently only supported in C++."
    #endif
#endif  // !MAL_WIN32_DESKTOP

mal_result mal_device_init__wasapi(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->wasapi);

    HRESULT hr;
    mal_result result = MAL_SUCCESS;
    const char* errorMsg = "";
    MAL_AUDCLNT_SHAREMODE shareMode = MAL_AUDCLNT_SHAREMODE_SHARED;
    MAL_REFERENCE_TIME bufferDurationInMicroseconds = ((mal_uint64)pDevice->bufferSizeInFrames * 1000 * 1000) / pConfig->sampleRate;
    WAVEFORMATEXTENSIBLE* pBestFormatTemp = NULL;

#ifdef MAL_WIN32_DESKTOP
    mal_IMMDevice* pMMDevice = NULL;

    mal_IMMDeviceEnumerator* pDeviceEnumerator;
    hr = mal_CoCreateInstance(pContext, MAL_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, MAL_IID_IMMDeviceEnumerator, (void**)&pDeviceEnumerator);
    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to create IMMDeviceEnumerator.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    if (pDeviceID == NULL) {
        hr = mal_IMMDeviceEnumerator_GetDefaultAudioEndpoint(pDeviceEnumerator, (type == mal_device_type_playback) ? mal_eRender : mal_eCapture, mal_eConsole, &pMMDevice);
    } else {
        hr = mal_IMMDeviceEnumerator_GetDevice(pDeviceEnumerator, pDeviceID->wasapi, &pMMDevice);
    }

    if (FAILED(hr)) {
        mal_IMMDeviceEnumerator_Release(pDeviceEnumerator);
        errorMsg = "[WASAPI] Failed to create backend device.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    mal_IMMDeviceEnumerator_Release(pDeviceEnumerator);

    hr = mal_IMMDevice_Activate(pMMDevice, &MAL_IID_IAudioClient, CLSCTX_ALL, NULL, &pDevice->wasapi.pAudioClient);
    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to activate device.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }
#else
    mal_IActivateAudioInterfaceAsyncOperation *pAsyncOp = NULL;
    malCompletionHandler completionHandler;

    IID iid;
    if (pDeviceID != NULL) {
        mal_copy_memory(&iid, pDeviceID->wasapi, sizeof(iid));
    } else {
        if (type == mal_device_type_playback) {
            iid = MAL_IID_DEVINTERFACE_AUDIO_RENDER;
        } else {
            iid = MAL_IID_DEVINTERFACE_AUDIO_CAPTURE;
        }
    }

    LPOLESTR iidStr;
    hr = StringFromIID(iid, &iidStr);
    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to convert device IID to string for ActivateAudioInterfaceAsync(). Out of memory.", result = MAL_OUT_OF_MEMORY;
        goto done;
    }

    result = completionHandler.Init();
    if (result != MAL_SUCCESS) {
        mal_CoTaskMemFree(pContext, iidStr);

        errorMsg = "[WASAPI] Failed to create event for waiting for ActivateAudioInterfaceAsync().", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    hr = ActivateAudioInterfaceAsync(iidStr, MAL_IID_IAudioClient, NULL, (IActivateAudioInterfaceCompletionHandler*)&completionHandler, (IActivateAudioInterfaceAsyncOperation**)&pAsyncOp);
    if (FAILED(hr)) {
        completionHandler.Uninit();
        mal_CoTaskMemFree(pContext, iidStr);

        errorMsg = "[WASAPI] ActivateAudioInterfaceAsync() failed.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    mal_CoTaskMemFree(pContext, iidStr);

    // Wait for the async operation for finish.
    completionHandler.Wait();
    completionHandler.Uninit();

    HRESULT activateResult;
    IUnknown* pActivatedInterface;
    hr = mal_IActivateAudioInterfaceAsyncOperation_GetActivateResult(pAsyncOp, &activateResult, &pActivatedInterface);
    if (FAILED(hr) || FAILED(activateResult)) {
        errorMsg = "[WASAPI] Failed to activate device.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    // Here is where we grab the IAudioClient interface.
    hr = pActivatedInterface->QueryInterface(MAL_IID_IAudioClient, &pDevice->wasapi.pAudioClient);
    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to query IAudioClient interface.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }
#endif

    WAVEFORMATEXTENSIBLE wf;
    mal_zero_object(&wf);
    wf.Format.cbSize               = sizeof(wf);
    wf.Format.wFormatTag           = WAVE_FORMAT_EXTENSIBLE;
    wf.Format.nChannels            = (WORD)pDevice->channels;
    wf.Format.nSamplesPerSec       = (DWORD)pDevice->sampleRate;
    wf.Format.wBitsPerSample       = (WORD)mal_get_sample_size_in_bytes(pDevice->format)*8;
    wf.Format.nBlockAlign          = (wf.Format.nChannels * wf.Format.wBitsPerSample) / 8;
    wf.Format.nAvgBytesPerSec      = wf.Format.nBlockAlign * wf.Format.nSamplesPerSec;
    wf.Samples.wValidBitsPerSample = /*(pDevice->format == mal_format_s24_32) ? 24 :*/ wf.Format.wBitsPerSample;
    wf.dwChannelMask               = mal_channel_map_to_channel_mask__win32(pDevice->channelMap, pDevice->channels);
    if (pDevice->format == mal_format_f32) {
        wf.SubFormat = MAL_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
    } else {
        wf.SubFormat = MAL_GUID_KSDATAFORMAT_SUBTYPE_PCM;
    }


    // Here is where we try to determine the best format to use with the device. If the client if wanting exclusive mode, first try finding the best format for that. If this fails, fall back to shared mode.
    result = MAL_FORMAT_NOT_SUPPORTED;
    if (pConfig->shareMode == mal_share_mode_exclusive) {
    #ifdef MAL_WIN32_DESKTOP
        // In exclusive mode on desktop we always use the backend's native format.
        mal_IPropertyStore* pStore = NULL;
        hr = mal_IMMDevice_OpenPropertyStore(pMMDevice, STGM_READ, &pStore);
        if (SUCCEEDED(hr)) {
            PROPVARIANT prop;
            mal_PropVariantInit(&prop);
            hr = mal_IPropertyStore_GetValue(pStore, &MAL_PKEY_AudioEngine_DeviceFormat, &prop);
            if (SUCCEEDED(hr)) {
                WAVEFORMATEX* pActualFormat = (WAVEFORMATEX*)prop.blob.pBlobData;
                hr = mal_IAudioClient_IsFormatSupported((mal_IAudioClient*)pDevice->wasapi.pAudioClient, MAL_AUDCLNT_SHAREMODE_EXCLUSIVE, pActualFormat, NULL);
                if (SUCCEEDED(hr)) {
                    mal_copy_memory(&wf, pActualFormat, sizeof(WAVEFORMATEXTENSIBLE));
                }

                mal_PropVariantClear(pDevice->pContext, &prop);
            }

            mal_IPropertyStore_Release(pStore);
        }
    #else
        // With non-Desktop builds we just try using the requested format.
        hr = mal_IAudioClient_IsFormatSupported((mal_IAudioClient*)pDevice->wasapi.pAudioClient, MAL_AUDCLNT_SHAREMODE_EXCLUSIVE, (WAVEFORMATEX*)&wf, NULL);
    #endif

        if (hr == S_OK) {
            shareMode = MAL_AUDCLNT_SHAREMODE_EXCLUSIVE;
            result = MAL_SUCCESS;
        }
    }

    // Fall back to shared mode if necessary.
    if (result != MAL_SUCCESS) {
        WAVEFORMATEXTENSIBLE* pNativeFormat = NULL;
        hr = mal_IAudioClient_GetMixFormat((mal_IAudioClient*)pDevice->wasapi.pAudioClient, (WAVEFORMATEX**)&pNativeFormat);
        if (hr == S_OK) {
            if (pDevice->usingDefaultFormat) {
                wf.Format.wBitsPerSample       = pNativeFormat->Format.wBitsPerSample;
                wf.Format.nBlockAlign          = pNativeFormat->Format.nBlockAlign;
                wf.Format.nAvgBytesPerSec      = pNativeFormat->Format.nAvgBytesPerSec;
                wf.Samples.wValidBitsPerSample = pNativeFormat->Samples.wValidBitsPerSample;
                wf.SubFormat                   = pNativeFormat->SubFormat;
            }
            if (pDevice->usingDefaultChannels) {
                wf.Format.nChannels            = pNativeFormat->Format.nChannels;
            }
            if (pDevice->usingDefaultSampleRate) {
                wf.Format.nSamplesPerSec       = pNativeFormat->Format.nSamplesPerSec;
            }
            if (pDevice->usingDefaultChannelMap) {
                wf.dwChannelMask               = pNativeFormat->dwChannelMask;
            }

            mal_CoTaskMemFree(pDevice->pContext, pNativeFormat);
            pNativeFormat = NULL;
        }

        hr = mal_IAudioClient_IsFormatSupported((mal_IAudioClient*)pDevice->wasapi.pAudioClient, MAL_AUDCLNT_SHAREMODE_SHARED, (WAVEFORMATEX*)&wf, (WAVEFORMATEX**)&pBestFormatTemp);
        if (hr != S_OK && hr != S_FALSE) {
            hr = mal_IAudioClient_GetMixFormat((mal_IAudioClient*)pDevice->wasapi.pAudioClient, (WAVEFORMATEX**)&pBestFormatTemp);
            if (hr != S_OK) {
                result = MAL_FORMAT_NOT_SUPPORTED;
            } else {
                result = MAL_SUCCESS;
            }
        } else {
            result = MAL_SUCCESS;
        }

        shareMode = MAL_AUDCLNT_SHAREMODE_SHARED;
    }

    // Return an error if we still haven't found a format.
    if (result != MAL_SUCCESS) {
        errorMsg = "[WASAPI] Failed to find best device mix format.", result = MAL_FORMAT_NOT_SUPPORTED;
        goto done;
    }

    if (pBestFormatTemp != NULL) {
        mal_copy_memory(&wf, pBestFormatTemp, sizeof(wf));
        mal_CoTaskMemFree(pDevice->pContext, pBestFormatTemp);
    }


    pDevice->internalFormat = mal_format_from_WAVEFORMATEX((WAVEFORMATEX*)&wf);
    pDevice->internalChannels = wf.Format.nChannels;
    pDevice->internalSampleRate = wf.Format.nSamplesPerSec;

    // Get the internal channel map based on the channel mask.
    mal_channel_mask_to_channel_map__win32(wf.dwChannelMask, pDevice->internalChannels, pDevice->internalChannelMap);

    // Slightly different initialization for shared and exclusive modes.
    if (shareMode == MAL_AUDCLNT_SHAREMODE_SHARED) {
        // Shared.
        MAL_REFERENCE_TIME bufferDuration = bufferDurationInMicroseconds*10;
        hr = mal_IAudioClient_Initialize((mal_IAudioClient*)pDevice->wasapi.pAudioClient, shareMode, MAL_AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDuration, 0, (WAVEFORMATEX*)&wf, NULL);
        if (FAILED(hr)) {
            if (hr == E_ACCESSDENIED) {
                errorMsg = "[WASAPI] Failed to initialize device. Access denied.", result = MAL_ACCESS_DENIED;
            } else {
                errorMsg = "[WASAPI] Failed to initialize device.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
            }

            goto done;
        }
    } else {
        // Exclusive.
        MAL_REFERENCE_TIME bufferDuration = bufferDurationInMicroseconds*10;
        hr = mal_IAudioClient_Initialize((mal_IAudioClient*)pDevice->wasapi.pAudioClient, shareMode, MAL_AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDuration, bufferDuration, (WAVEFORMATEX*)&wf, NULL);
        if (hr == MAL_AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED) {
            UINT bufferSizeInFrames;
            hr = mal_IAudioClient_GetBufferSize((mal_IAudioClient*)pDevice->wasapi.pAudioClient, &bufferSizeInFrames);
            if (SUCCEEDED(hr)) {
                bufferDuration = (MAL_REFERENCE_TIME)((10000.0 * 1000 / wf.Format.nSamplesPerSec * bufferSizeInFrames) + 0.5);

                // Unfortunately we need to release and re-acquire the audio client according to MSDN. Seems silly - why not just call IAudioClient_Initialize() again?!
                mal_IAudioClient_Release((mal_IAudioClient*)pDevice->wasapi.pAudioClient);

            #ifdef MAL_WIN32_DESKTOP
                hr = mal_IMMDevice_Activate(pMMDevice, &MAL_IID_IAudioClient, CLSCTX_ALL, NULL, &pDevice->wasapi.pAudioClient);
            #else
                hr = pActivatedInterface->QueryInterface(MAL_IID_IAudioClient, &pDevice->wasapi.pAudioClient);
            #endif

                if (SUCCEEDED(hr)) {
                    hr = mal_IAudioClient_Initialize((mal_IAudioClient*)pDevice->wasapi.pAudioClient, shareMode, MAL_AUDCLNT_STREAMFLAGS_EVENTCALLBACK, bufferDuration, bufferDuration, (WAVEFORMATEX*)&wf, NULL);
                }
            }
        }

        if (FAILED(hr)) {
            errorMsg = "[WASAPI] Failed to initialize device.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
            goto done;
        }
    }

    hr = mal_IAudioClient_GetBufferSize((mal_IAudioClient*)pDevice->wasapi.pAudioClient, &pDevice->bufferSizeInFrames);
    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to get audio client's actual buffer size.", result = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
        goto done;
    }

    if (type == mal_device_type_playback) {
        hr = mal_IAudioClient_GetService((mal_IAudioClient*)pDevice->wasapi.pAudioClient, &MAL_IID_IAudioRenderClient, &pDevice->wasapi.pRenderClient);
    } else {
        hr = mal_IAudioClient_GetService((mal_IAudioClient*)pDevice->wasapi.pAudioClient, &MAL_IID_IAudioCaptureClient, &pDevice->wasapi.pCaptureClient);
    }

    if (FAILED(hr)) {
        errorMsg = "[WASAPI] Failed to get audio client service.", result = MAL_API_NOT_FOUND;
        goto done;
    }


    if (shareMode == MAL_AUDCLNT_SHAREMODE_SHARED) {
        pDevice->exclusiveMode = MAL_FALSE;
    } else /*if (shareMode == MAL_AUDCLNT_SHAREMODE_EXCLUSIVE)*/ {
        pDevice->exclusiveMode = MAL_TRUE;
    }


    // Grab the name of the device.
#ifdef MAL_WIN32_DESKTOP
    mal_IPropertyStore *pProperties;
    hr = mal_IMMDevice_OpenPropertyStore(pMMDevice, STGM_READ, &pProperties);
    if (SUCCEEDED(hr)) {
        PROPVARIANT varName;
        mal_PropVariantInit(&varName);
        hr = mal_IPropertyStore_GetValue(pProperties, &MAL_PKEY_Device_FriendlyName, &varName);
        if (SUCCEEDED(hr)) {
            WideCharToMultiByte(CP_UTF8, 0, varName.pwszVal, -1, pDevice->name, sizeof(pDevice->name), 0, FALSE);
            mal_PropVariantClear(pContext, &varName);
        }

        mal_IPropertyStore_Release(pProperties);
    }
#endif


    // We need to create and set the event for event-driven mode. This event is signalled whenever a new chunk of audio
    // data needs to be written or read from the device.
    pDevice->wasapi.hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (pDevice->wasapi.hEvent == NULL) {
        errorMsg = "[WASAPI] Failed to create main event for main loop.", result = MAL_FAILED_TO_CREATE_EVENT;
        goto done;
    }

    mal_IAudioClient_SetEventHandle((mal_IAudioClient*)pDevice->wasapi.pAudioClient, pDevice->wasapi.hEvent);


    // When the device is playing the worker thread will be waiting on a bunch of notification events. To return from
    // this wait state we need to signal a special event.
    pDevice->wasapi.hStopEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (pDevice->wasapi.hStopEvent == NULL) {
        errorMsg = "[WASAPI] Failed to create stop event for main loop break notification.", result = MAL_FAILED_TO_CREATE_EVENT;
        goto done;
    }

    result = MAL_SUCCESS;

done:
    // Clean up.
#ifdef MAL_WIN32_DESKTOP
    if (pMMDevice != NULL) {
        mal_IMMDevice_Release(pMMDevice);
    }
#else
    if (pAsyncOp != NULL) {
        mal_IActivateAudioInterfaceAsyncOperation_Release(pAsyncOp);
    }
#endif

    if (result != MAL_SUCCESS) {
        mal_device_uninit__wasapi(pDevice);
        return mal_post_error(pDevice, errorMsg, result);
    } else {
        return MAL_SUCCESS;
    }
}

mal_result mal_device__start_backend__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // Playback devices need to have an initial chunk of data loaded.
    if (pDevice->type == mal_device_type_playback) {
        BYTE* pData;
        HRESULT hr = mal_IAudioRenderClient_GetBuffer((mal_IAudioRenderClient*)pDevice->wasapi.pRenderClient, pDevice->bufferSizeInFrames, &pData);
        if (FAILED(hr)) {
            return mal_post_error(pDevice, "[WASAPI] Failed to retrieve buffer from internal playback device.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
        }

        mal_device__read_frames_from_client(pDevice, pDevice->bufferSizeInFrames, pData);

        hr = mal_IAudioRenderClient_ReleaseBuffer((mal_IAudioRenderClient*)pDevice->wasapi.pRenderClient, pDevice->bufferSizeInFrames, 0);
        if (FAILED(hr)) {
            return mal_post_error(pDevice, "[WASAPI] Failed to release internal buffer for playback device.", MAL_FAILED_TO_UNMAP_DEVICE_BUFFER);
        }
    }

    HRESULT hr = mal_IAudioClient_Start((mal_IAudioClient*)pDevice->wasapi.pAudioClient);
    if (FAILED(hr)) {
        return mal_post_error(pDevice, "[WASAPI] Failed to start internal device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    HRESULT hr = mal_IAudioClient_Stop((mal_IAudioClient*)pDevice->wasapi.pAudioClient);
    if (FAILED(hr)) {
        return mal_post_error(pDevice, "[WASAPI] Failed to stop internal device.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // The main loop will be waiting on a bunch of events via the WaitForMultipleObjects() API. One of those events
    // is a special event we use for forcing that function to return.
    pDevice->wasapi.breakFromMainLoop = MAL_TRUE;
    SetEvent(pDevice->wasapi.hStopEvent);
    return MAL_SUCCESS;
}

mal_uint32 mal_device__get_available_frames__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

#if 1
    if (pDevice->type == mal_device_type_playback) {
        UINT32 paddingFramesCount;
        HRESULT hr = mal_IAudioClient_GetCurrentPadding((mal_IAudioClient*)pDevice->wasapi.pAudioClient, &paddingFramesCount);
        if (FAILED(hr)) {
            return 0;
        }

        if (pDevice->exclusiveMode) {
            return paddingFramesCount;
        } else {
            return pDevice->bufferSizeInFrames - paddingFramesCount;
        }
    } else {
        UINT32 framesAvailable;
        HRESULT hr = mal_IAudioCaptureClient_GetNextPacketSize((mal_IAudioCaptureClient*)pDevice->wasapi.pCaptureClient, &framesAvailable);
        if (FAILED(hr)) {
            return 0;
        }

        return framesAvailable;
    }
#else
    UINT32 paddingFramesCount;
    HRESULT hr = mal_IAudioClient_GetCurrentPadding(pDevice->wasapi.pAudioClient, &paddingFramesCount);
    if (FAILED(hr)) {
        return 0;
    }

    if (pDevice->exclusiveMode) {
        return paddingFramesCount;
    } else {
        return pDevice->bufferSizeInFrames - paddingFramesCount;
    }
#endif
}

mal_uint32 mal_device__wait_for_frames__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    while (!pDevice->wasapi.breakFromMainLoop) {
        // Wait for a buffer to become available or for the stop event to be signalled.
        HANDLE hEvents[2];
        hEvents[0] = (HANDLE)pDevice->wasapi.hEvent;
        hEvents[1] = (HANDLE)pDevice->wasapi.hStopEvent;
        if (WaitForMultipleObjects(mal_countof(hEvents), hEvents, FALSE, INFINITE) == WAIT_FAILED) {
            break;
        }

        // Break from the main loop if the device isn't started anymore. Likely what's happened is the application
        // has requested that the device be stopped.
        if (!mal_device_is_started(pDevice)) {
            break;
        }

        mal_uint32 framesAvailable = mal_device__get_available_frames__wasapi(pDevice);
        if (framesAvailable > 0) {
            return framesAvailable;
        }
    }

    // We'll get here if the loop was terminated. Just return whatever's available.
    return mal_device__get_available_frames__wasapi(pDevice);
}

mal_result mal_device__main_loop__wasapi(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // Make sure the stop event is not signaled to ensure we don't end up immediately returning from WaitForMultipleObjects().
    ResetEvent(pDevice->wasapi.hStopEvent);

    pDevice->wasapi.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->wasapi.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__wait_for_frames__wasapi(pDevice);
        if (framesAvailable == 0) {
            continue;
        }

        // If it's a playback device, don't bother grabbing more data if the device is being stopped.
        if (pDevice->wasapi.breakFromMainLoop && pDevice->type == mal_device_type_playback) {
            return MAL_FALSE;
        }

        if (pDevice->type == mal_device_type_playback) {
            BYTE* pData;
            HRESULT hr = mal_IAudioRenderClient_GetBuffer((mal_IAudioRenderClient*)pDevice->wasapi.pRenderClient, framesAvailable, &pData);
            if (FAILED(hr)) {
                return mal_post_error(pDevice, "[WASAPI] Failed to retrieve internal buffer from playback device in preparation for sending new data to the device.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
            }

            mal_device__read_frames_from_client(pDevice, framesAvailable, pData);

            hr = mal_IAudioRenderClient_ReleaseBuffer((mal_IAudioRenderClient*)pDevice->wasapi.pRenderClient, framesAvailable, 0);
            if (FAILED(hr)) {
                return mal_post_error(pDevice, "[WASAPI] Failed to release internal buffer from playback device in preparation for sending new data to the device.", MAL_FAILED_TO_UNMAP_DEVICE_BUFFER);
            }
        } else {
            UINT32 framesRemaining = framesAvailable;
            while (framesRemaining > 0) {
                BYTE* pData;
                UINT32 framesToSend;
                DWORD flags;
                HRESULT hr = mal_IAudioCaptureClient_GetBuffer((mal_IAudioCaptureClient*)pDevice->wasapi.pCaptureClient, &pData, &framesToSend, &flags, NULL, NULL);
                if (FAILED(hr)) {
                    mal_post_error(pDevice, "[WASAPI] WARNING: Failed to retrieve internal buffer from capture device in preparation for sending new data to the client.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
                    break;
                }

                if (hr != MAL_AUDCLNT_S_BUFFER_EMPTY) {
                    mal_device__send_frames_to_client(pDevice, framesToSend, pData);

                    hr = mal_IAudioCaptureClient_ReleaseBuffer((mal_IAudioCaptureClient*)pDevice->wasapi.pCaptureClient, framesToSend);
                    if (FAILED(hr)) {
                        mal_post_error(pDevice, "[WASAPI] WARNING: Failed to release internal buffer from capture device in preparation for sending new data to the client.", MAL_FAILED_TO_UNMAP_DEVICE_BUFFER);
                        break;
                    }

                    if (framesRemaining >= framesToSend) {
                        framesRemaining -= framesToSend;
                    } else {
                        framesRemaining = 0;
                    }
                }
            }
        }
    }

    return MAL_SUCCESS;
}
#endif

///////////////////////////////////////////////////////////////////////////////
//
// DirectSound Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_DSOUND
//#include <dsound.h>

GUID MAL_GUID_IID_DirectSoundNotify = {0xb0210783, 0x89cd, 0x11d0, {0xaf, 0x08, 0x00, 0xa0, 0xc9, 0x25, 0xcd, 0x16}};

// mini_al only uses priority or exclusive modes.
#define MAL_DSSCL_NORMAL                 1
#define MAL_DSSCL_PRIORITY               2
#define MAL_DSSCL_EXCLUSIVE              3
#define MAL_DSSCL_WRITEPRIMARY           4

#define MAL_DSCAPS_PRIMARYMONO           0x00000001
#define MAL_DSCAPS_PRIMARYSTEREO         0x00000002
#define MAL_DSCAPS_PRIMARY8BIT           0x00000004
#define MAL_DSCAPS_PRIMARY16BIT          0x00000008
#define MAL_DSCAPS_CONTINUOUSRATE        0x00000010
#define MAL_DSCAPS_EMULDRIVER            0x00000020
#define MAL_DSCAPS_CERTIFIED             0x00000040
#define MAL_DSCAPS_SECONDARYMONO         0x00000100
#define MAL_DSCAPS_SECONDARYSTEREO       0x00000200
#define MAL_DSCAPS_SECONDARY8BIT         0x00000400
#define MAL_DSCAPS_SECONDARY16BIT        0x00000800

#define MAL_DSBCAPS_PRIMARYBUFFER        0x00000001
#define MAL_DSBCAPS_STATIC               0x00000002
#define MAL_DSBCAPS_LOCHARDWARE          0x00000004
#define MAL_DSBCAPS_LOCSOFTWARE          0x00000008
#define MAL_DSBCAPS_CTRL3D               0x00000010
#define MAL_DSBCAPS_CTRLFREQUENCY        0x00000020
#define MAL_DSBCAPS_CTRLPAN              0x00000040
#define MAL_DSBCAPS_CTRLVOLUME           0x00000080
#define MAL_DSBCAPS_CTRLPOSITIONNOTIFY   0x00000100
#define MAL_DSBCAPS_CTRLFX               0x00000200
#define MAL_DSBCAPS_STICKYFOCUS          0x00004000
#define MAL_DSBCAPS_GLOBALFOCUS          0x00008000
#define MAL_DSBCAPS_GETCURRENTPOSITION2  0x00010000
#define MAL_DSBCAPS_MUTE3DATMAXDISTANCE  0x00020000
#define MAL_DSBCAPS_LOCDEFER             0x00040000
#define MAL_DSBCAPS_TRUEPLAYPOSITION     0x00080000

#define MAL_DSBPLAY_LOOPING              0x00000001
#define MAL_DSBPLAY_LOCHARDWARE          0x00000002
#define MAL_DSBPLAY_LOCSOFTWARE          0x00000004
#define MAL_DSBPLAY_TERMINATEBY_TIME     0x00000008
#define MAL_DSBPLAY_TERMINATEBY_DISTANCE 0x00000010
#define MAL_DSBPLAY_TERMINATEBY_PRIORITY 0x00000020

#define MAL_DSCBSTART_LOOPING            0x00000001

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX* lpwfxFormat;
    GUID guid3DAlgorithm;
} MAL_DSBUFFERDESC;

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
    WAVEFORMATEX* lpwfxFormat;
    DWORD dwFXCount;
    void* lpDSCFXDesc;  // <-- mini_al doesn't use this, so set to void*.
} MAL_DSCBUFFERDESC;

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwMinSecondarySampleRate;
    DWORD dwMaxSecondarySampleRate;
    DWORD dwPrimaryBuffers;
    DWORD dwMaxHwMixingAllBuffers;
    DWORD dwMaxHwMixingStaticBuffers;
    DWORD dwMaxHwMixingStreamingBuffers;
    DWORD dwFreeHwMixingAllBuffers;
    DWORD dwFreeHwMixingStaticBuffers;
    DWORD dwFreeHwMixingStreamingBuffers;
    DWORD dwMaxHw3DAllBuffers;
    DWORD dwMaxHw3DStaticBuffers;
    DWORD dwMaxHw3DStreamingBuffers;
    DWORD dwFreeHw3DAllBuffers;
    DWORD dwFreeHw3DStaticBuffers;
    DWORD dwFreeHw3DStreamingBuffers;
    DWORD dwTotalHwMemBytes;
    DWORD dwFreeHwMemBytes;
    DWORD dwMaxContigFreeHwMemBytes;
    DWORD dwUnlockTransferRateHwBuffers;
    DWORD dwPlayCpuOverheadSwBuffers;
    DWORD dwReserved1;
    DWORD dwReserved2;
} MAL_DSCAPS;

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwUnlockTransferRate;
    DWORD dwPlayCpuOverhead;
} MAL_DSBCAPS;

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFormats;
    DWORD dwChannels;
} MAL_DSCCAPS;

typedef struct
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwBufferBytes;
    DWORD dwReserved;
} MAL_DSCBCAPS;

typedef struct
{
    DWORD  dwOffset;
    HANDLE hEventNotify;
} MAL_DSBPOSITIONNOTIFY;

typedef struct mal_IDirectSound              mal_IDirectSound;
typedef struct mal_IDirectSoundBuffer        mal_IDirectSoundBuffer;
typedef struct mal_IDirectSoundCapture       mal_IDirectSoundCapture;
typedef struct mal_IDirectSoundCaptureBuffer mal_IDirectSoundCaptureBuffer;
typedef struct mal_IDirectSoundNotify        mal_IDirectSoundNotify;


// COM objects. The way these work is that you have a vtable (a list of function pointers, kind of
// like how C++ works internally), and then you have a structure with a single member, which is a
// pointer to the vtable. The vtable is where the methods of the object are defined. Methods need
// to be in a specific order, and parent classes need to have their methods declared first.

// IDirectSound
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IDirectSound* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IDirectSound* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IDirectSound* pThis);

    // IDirectSound
    HRESULT (STDMETHODCALLTYPE * CreateSoundBuffer)   (mal_IDirectSound* pThis, const MAL_DSBUFFERDESC* pDSBufferDesc, mal_IDirectSoundBuffer** ppDSBuffer, void* pUnkOuter);
    HRESULT (STDMETHODCALLTYPE * GetCaps)             (mal_IDirectSound* pThis, MAL_DSCAPS* pDSCaps);
    HRESULT (STDMETHODCALLTYPE * DuplicateSoundBuffer)(mal_IDirectSound* pThis, mal_IDirectSoundBuffer* pDSBufferOriginal, mal_IDirectSoundBuffer** ppDSBufferDuplicate);
    HRESULT (STDMETHODCALLTYPE * SetCooperativeLevel) (mal_IDirectSound* pThis, HWND hwnd, DWORD dwLevel);
    HRESULT (STDMETHODCALLTYPE * Compact)             (mal_IDirectSound* pThis);
    HRESULT (STDMETHODCALLTYPE * GetSpeakerConfig)    (mal_IDirectSound* pThis, DWORD* pSpeakerConfig);
    HRESULT (STDMETHODCALLTYPE * SetSpeakerConfig)    (mal_IDirectSound* pThis, DWORD dwSpeakerConfig);
    HRESULT (STDMETHODCALLTYPE * Initialize)          (mal_IDirectSound* pThis, const GUID* pGuidDevice);
} mal_IDirectSoundVtbl;
struct mal_IDirectSound
{
    mal_IDirectSoundVtbl* lpVtbl;
};
HRESULT mal_IDirectSound_QueryInterface(mal_IDirectSound* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IDirectSound_AddRef(mal_IDirectSound* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IDirectSound_Release(mal_IDirectSound* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IDirectSound_CreateSoundBuffer(mal_IDirectSound* pThis, const MAL_DSBUFFERDESC* pDSBufferDesc, mal_IDirectSoundBuffer** ppDSBuffer, void* pUnkOuter) { return pThis->lpVtbl->CreateSoundBuffer(pThis, pDSBufferDesc, ppDSBuffer, pUnkOuter); }
HRESULT mal_IDirectSound_GetCaps(mal_IDirectSound* pThis, MAL_DSCAPS* pDSCaps)                           { return pThis->lpVtbl->GetCaps(pThis, pDSCaps); }
HRESULT mal_IDirectSound_DuplicateSoundBuffer(mal_IDirectSound* pThis, mal_IDirectSoundBuffer* pDSBufferOriginal, mal_IDirectSoundBuffer** ppDSBufferDuplicate) { return pThis->lpVtbl->DuplicateSoundBuffer(pThis, pDSBufferOriginal, ppDSBufferDuplicate); }
HRESULT mal_IDirectSound_SetCooperativeLevel(mal_IDirectSound* pThis, HWND hwnd, DWORD dwLevel)          { return pThis->lpVtbl->SetCooperativeLevel(pThis, hwnd, dwLevel); }
HRESULT mal_IDirectSound_Compact(mal_IDirectSound* pThis)                                                { return pThis->lpVtbl->Compact(pThis); }
HRESULT mal_IDirectSound_GetSpeakerConfig(mal_IDirectSound* pThis, DWORD* pSpeakerConfig)                { return pThis->lpVtbl->GetSpeakerConfig(pThis, pSpeakerConfig); }
HRESULT mal_IDirectSound_SetSpeakerConfig(mal_IDirectSound* pThis, DWORD dwSpeakerConfig)                { return pThis->lpVtbl->SetSpeakerConfig(pThis, dwSpeakerConfig); }
HRESULT mal_IDirectSound_Initialize(mal_IDirectSound* pThis, const GUID* pGuidDevice)                    { return pThis->lpVtbl->Initialize(pThis, pGuidDevice); }


// IDirectSoundBuffer
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IDirectSoundBuffer* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IDirectSoundBuffer* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IDirectSoundBuffer* pThis);

    // IDirectSoundBuffer
    HRESULT (STDMETHODCALLTYPE * GetCaps)           (mal_IDirectSoundBuffer* pThis, MAL_DSBCAPS* pDSBufferCaps);
    HRESULT (STDMETHODCALLTYPE * GetCurrentPosition)(mal_IDirectSoundBuffer* pThis, DWORD* pCurrentPlayCursor, DWORD* pCurrentWriteCursor);
    HRESULT (STDMETHODCALLTYPE * GetFormat)         (mal_IDirectSoundBuffer* pThis, WAVEFORMATEX* pFormat, DWORD dwSizeAllocated, DWORD* pSizeWritten);
    HRESULT (STDMETHODCALLTYPE * GetVolume)         (mal_IDirectSoundBuffer* pThis, LONG* pVolume);
    HRESULT (STDMETHODCALLTYPE * GetPan)            (mal_IDirectSoundBuffer* pThis, LONG* pPan);
    HRESULT (STDMETHODCALLTYPE * GetFrequency)      (mal_IDirectSoundBuffer* pThis, DWORD* pFrequency);
    HRESULT (STDMETHODCALLTYPE * GetStatus)         (mal_IDirectSoundBuffer* pThis, DWORD* pStatus);
    HRESULT (STDMETHODCALLTYPE * Initialize)        (mal_IDirectSoundBuffer* pThis, mal_IDirectSound* pDirectSound, const MAL_DSBUFFERDESC* pDSBufferDesc);
    HRESULT (STDMETHODCALLTYPE * Lock)              (mal_IDirectSoundBuffer* pThis, DWORD dwOffset, DWORD dwBytes, void** ppAudioPtr1, DWORD* pAudioBytes1, void** ppAudioPtr2, DWORD* pAudioBytes2, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE * Play)              (mal_IDirectSoundBuffer* pThis, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE * SetCurrentPosition)(mal_IDirectSoundBuffer* pThis, DWORD dwNewPosition);
    HRESULT (STDMETHODCALLTYPE * SetFormat)         (mal_IDirectSoundBuffer* pThis, const WAVEFORMATEX* pFormat);
    HRESULT (STDMETHODCALLTYPE * SetVolume)         (mal_IDirectSoundBuffer* pThis, LONG volume);
    HRESULT (STDMETHODCALLTYPE * SetPan)            (mal_IDirectSoundBuffer* pThis, LONG pan);
    HRESULT (STDMETHODCALLTYPE * SetFrequency)      (mal_IDirectSoundBuffer* pThis, DWORD dwFrequency);
    HRESULT (STDMETHODCALLTYPE * Stop)              (mal_IDirectSoundBuffer* pThis);
    HRESULT (STDMETHODCALLTYPE * Unlock)            (mal_IDirectSoundBuffer* pThis, void* pAudioPtr1, DWORD dwAudioBytes1, void* pAudioPtr2, DWORD dwAudioBytes2);
    HRESULT (STDMETHODCALLTYPE * Restore)           (mal_IDirectSoundBuffer* pThis);
} mal_IDirectSoundBufferVtbl;
struct mal_IDirectSoundBuffer
{
    mal_IDirectSoundBufferVtbl* lpVtbl;
};
HRESULT mal_IDirectSoundBuffer_QueryInterface(mal_IDirectSoundBuffer* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IDirectSoundBuffer_AddRef(mal_IDirectSoundBuffer* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IDirectSoundBuffer_Release(mal_IDirectSoundBuffer* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IDirectSoundBuffer_GetCaps(mal_IDirectSoundBuffer* pThis, MAL_DSBCAPS* pDSBufferCaps)                    { return pThis->lpVtbl->GetCaps(pThis, pDSBufferCaps); }
HRESULT mal_IDirectSoundBuffer_GetCurrentPosition(mal_IDirectSoundBuffer* pThis, DWORD* pCurrentPlayCursor, DWORD* pCurrentWriteCursor) { return pThis->lpVtbl->GetCurrentPosition(pThis, pCurrentPlayCursor, pCurrentWriteCursor); }
HRESULT mal_IDirectSoundBuffer_GetFormat(mal_IDirectSoundBuffer* pThis, WAVEFORMATEX* pFormat, DWORD dwSizeAllocated, DWORD* pSizeWritten) { return pThis->lpVtbl->GetFormat(pThis, pFormat, dwSizeAllocated, pSizeWritten); }
HRESULT mal_IDirectSoundBuffer_GetVolume(mal_IDirectSoundBuffer* pThis, LONG* pVolume)                               { return pThis->lpVtbl->GetVolume(pThis, pVolume); }
HRESULT mal_IDirectSoundBuffer_GetPan(mal_IDirectSoundBuffer* pThis, LONG* pPan)                                     { return pThis->lpVtbl->GetPan(pThis, pPan); }
HRESULT mal_IDirectSoundBuffer_GetFrequency(mal_IDirectSoundBuffer* pThis, DWORD* pFrequency)                        { return pThis->lpVtbl->GetFrequency(pThis, pFrequency); }
HRESULT mal_IDirectSoundBuffer_GetStatus(mal_IDirectSoundBuffer* pThis, DWORD* pStatus)                              { return pThis->lpVtbl->GetStatus(pThis, pStatus); }
HRESULT mal_IDirectSoundBuffer_Initialize(mal_IDirectSoundBuffer* pThis, mal_IDirectSound* pDirectSound, const MAL_DSBUFFERDESC* pDSBufferDesc) { return pThis->lpVtbl->Initialize(pThis, pDirectSound, pDSBufferDesc); }
HRESULT mal_IDirectSoundBuffer_Lock(mal_IDirectSoundBuffer* pThis, DWORD dwOffset, DWORD dwBytes, void** ppAudioPtr1, DWORD* pAudioBytes1, void** ppAudioPtr2, DWORD* pAudioBytes2, DWORD dwFlags) { return pThis->lpVtbl->Lock(pThis, dwOffset, dwBytes, ppAudioPtr1, pAudioBytes1, ppAudioPtr2, pAudioBytes2, dwFlags); }
HRESULT mal_IDirectSoundBuffer_Play(mal_IDirectSoundBuffer* pThis, DWORD dwReserved1, DWORD dwPriority, DWORD dwFlags) { return pThis->lpVtbl->Play(pThis, dwReserved1, dwPriority, dwFlags); }
HRESULT mal_IDirectSoundBuffer_SetCurrentPosition(mal_IDirectSoundBuffer* pThis, DWORD dwNewPosition)                { return pThis->lpVtbl->SetCurrentPosition(pThis, dwNewPosition); }
HRESULT mal_IDirectSoundBuffer_SetFormat(mal_IDirectSoundBuffer* pThis, const WAVEFORMATEX* pFormat)                 { return pThis->lpVtbl->SetFormat(pThis, pFormat); }
HRESULT mal_IDirectSoundBuffer_SetVolume(mal_IDirectSoundBuffer* pThis, LONG volume)                                 { return pThis->lpVtbl->SetVolume(pThis, volume); }
HRESULT mal_IDirectSoundBuffer_SetPan(mal_IDirectSoundBuffer* pThis, LONG pan)                                       { return pThis->lpVtbl->SetPan(pThis, pan); }
HRESULT mal_IDirectSoundBuffer_SetFrequency(mal_IDirectSoundBuffer* pThis, DWORD dwFrequency)                        { return pThis->lpVtbl->SetFrequency(pThis, dwFrequency); }
HRESULT mal_IDirectSoundBuffer_Stop(mal_IDirectSoundBuffer* pThis)                                                   { return pThis->lpVtbl->Stop(pThis); }
HRESULT mal_IDirectSoundBuffer_Unlock(mal_IDirectSoundBuffer* pThis, void* pAudioPtr1, DWORD dwAudioBytes1, void* pAudioPtr2, DWORD dwAudioBytes2) { return pThis->lpVtbl->Unlock(pThis, pAudioPtr1, dwAudioBytes1, pAudioPtr2, dwAudioBytes2); }
HRESULT mal_IDirectSoundBuffer_Restore(mal_IDirectSoundBuffer* pThis)                                                { return pThis->lpVtbl->Restore(pThis); }


// IDirectSoundCapture
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IDirectSoundCapture* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IDirectSoundCapture* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IDirectSoundCapture* pThis);

    // IDirectSoundCapture
    HRESULT (STDMETHODCALLTYPE * CreateCaptureBuffer)(mal_IDirectSoundCapture* pThis, const MAL_DSCBUFFERDESC* pDSCBufferDesc, mal_IDirectSoundCaptureBuffer** ppDSCBuffer, void* pUnkOuter);
    HRESULT (STDMETHODCALLTYPE * GetCaps)            (mal_IDirectSoundCapture* pThis, MAL_DSCCAPS* pDSCCaps);
    HRESULT (STDMETHODCALLTYPE * Initialize)         (mal_IDirectSoundCapture* pThis, const GUID* pGuidDevice);
} mal_IDirectSoundCaptureVtbl;
struct mal_IDirectSoundCapture
{
    mal_IDirectSoundCaptureVtbl* lpVtbl;
};
HRESULT mal_IDirectSoundCapture_QueryInterface(mal_IDirectSoundCapture* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IDirectSoundCapture_AddRef(mal_IDirectSoundCapture* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IDirectSoundCapture_Release(mal_IDirectSoundCapture* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IDirectSoundCapture_CreateCaptureBuffer(mal_IDirectSoundCapture* pThis, const MAL_DSCBUFFERDESC* pDSCBufferDesc, mal_IDirectSoundCaptureBuffer** ppDSCBuffer, void* pUnkOuter) { return pThis->lpVtbl->CreateCaptureBuffer(pThis, pDSCBufferDesc, ppDSCBuffer, pUnkOuter); }
HRESULT mal_IDirectSoundCapture_GetCaps            (mal_IDirectSoundCapture* pThis, MAL_DSCCAPS* pDSCCaps)             { return pThis->lpVtbl->GetCaps(pThis, pDSCCaps); }
HRESULT mal_IDirectSoundCapture_Initialize         (mal_IDirectSoundCapture* pThis, const GUID* pGuidDevice)           { return pThis->lpVtbl->Initialize(pThis, pGuidDevice); }


// IDirectSoundCaptureBuffer
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IDirectSoundCaptureBuffer* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IDirectSoundCaptureBuffer* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IDirectSoundCaptureBuffer* pThis);

    // IDirectSoundCaptureBuffer
    HRESULT (STDMETHODCALLTYPE * GetCaps)           (mal_IDirectSoundCaptureBuffer* pThis, MAL_DSCBCAPS* pDSCBCaps);
    HRESULT (STDMETHODCALLTYPE * GetCurrentPosition)(mal_IDirectSoundCaptureBuffer* pThis, DWORD* pCapturePosition, DWORD* pReadPosition);
    HRESULT (STDMETHODCALLTYPE * GetFormat)         (mal_IDirectSoundCaptureBuffer* pThis, WAVEFORMATEX* pFormat, DWORD dwSizeAllocated, DWORD* pSizeWritten);
    HRESULT (STDMETHODCALLTYPE * GetStatus)         (mal_IDirectSoundCaptureBuffer* pThis, DWORD* pStatus);
    HRESULT (STDMETHODCALLTYPE * Initialize)        (mal_IDirectSoundCaptureBuffer* pThis, mal_IDirectSoundCapture* pDirectSoundCapture, const MAL_DSCBUFFERDESC* pDSCBufferDesc);
    HRESULT (STDMETHODCALLTYPE * Lock)              (mal_IDirectSoundCaptureBuffer* pThis, DWORD dwOffset, DWORD dwBytes, void** ppAudioPtr1, DWORD* pAudioBytes1, void** ppAudioPtr2, DWORD* pAudioBytes2, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE * Start)             (mal_IDirectSoundCaptureBuffer* pThis, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE * Stop)              (mal_IDirectSoundCaptureBuffer* pThis);
    HRESULT (STDMETHODCALLTYPE * Unlock)            (mal_IDirectSoundCaptureBuffer* pThis, void* pAudioPtr1, DWORD dwAudioBytes1, void* pAudioPtr2, DWORD dwAudioBytes2);
} mal_IDirectSoundCaptureBufferVtbl;
struct mal_IDirectSoundCaptureBuffer
{
    mal_IDirectSoundCaptureBufferVtbl* lpVtbl;
};
HRESULT mal_IDirectSoundCaptureBuffer_QueryInterface(mal_IDirectSoundCaptureBuffer* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IDirectSoundCaptureBuffer_AddRef(mal_IDirectSoundCaptureBuffer* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IDirectSoundCaptureBuffer_Release(mal_IDirectSoundCaptureBuffer* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IDirectSoundCaptureBuffer_GetCaps(mal_IDirectSoundCaptureBuffer* pThis, MAL_DSCBCAPS* pDSCBCaps)                       { return pThis->lpVtbl->GetCaps(pThis, pDSCBCaps); }
HRESULT mal_IDirectSoundCaptureBuffer_GetCurrentPosition(mal_IDirectSoundCaptureBuffer* pThis, DWORD* pCapturePosition, DWORD* pReadPosition) { return pThis->lpVtbl->GetCurrentPosition(pThis, pCapturePosition, pReadPosition); }
HRESULT mal_IDirectSoundCaptureBuffer_GetFormat(mal_IDirectSoundCaptureBuffer* pThis, WAVEFORMATEX* pFormat, DWORD dwSizeAllocated, DWORD* pSizeWritten) { return pThis->lpVtbl->GetFormat(pThis, pFormat, dwSizeAllocated, pSizeWritten); }
HRESULT mal_IDirectSoundCaptureBuffer_GetStatus(mal_IDirectSoundCaptureBuffer* pThis, DWORD* pStatus)                              { return pThis->lpVtbl->GetStatus(pThis, pStatus); }
HRESULT mal_IDirectSoundCaptureBuffer_Initialize(mal_IDirectSoundCaptureBuffer* pThis, mal_IDirectSoundCapture* pDirectSoundCapture, const MAL_DSCBUFFERDESC* pDSCBufferDesc) { return pThis->lpVtbl->Initialize(pThis, pDirectSoundCapture, pDSCBufferDesc); }
HRESULT mal_IDirectSoundCaptureBuffer_Lock(mal_IDirectSoundCaptureBuffer* pThis, DWORD dwOffset, DWORD dwBytes, void** ppAudioPtr1, DWORD* pAudioBytes1, void** ppAudioPtr2, DWORD* pAudioBytes2, DWORD dwFlags) { return pThis->lpVtbl->Lock(pThis, dwOffset, dwBytes, ppAudioPtr1, pAudioBytes1, ppAudioPtr2, pAudioBytes2, dwFlags); }
HRESULT mal_IDirectSoundCaptureBuffer_Start(mal_IDirectSoundCaptureBuffer* pThis, DWORD dwFlags)                                   { return pThis->lpVtbl->Start(pThis, dwFlags); }
HRESULT mal_IDirectSoundCaptureBuffer_Stop(mal_IDirectSoundCaptureBuffer* pThis)                                                   { return pThis->lpVtbl->Stop(pThis); }
HRESULT mal_IDirectSoundCaptureBuffer_Unlock(mal_IDirectSoundCaptureBuffer* pThis, void* pAudioPtr1, DWORD dwAudioBytes1, void* pAudioPtr2, DWORD dwAudioBytes2) { return pThis->lpVtbl->Unlock(pThis, pAudioPtr1, dwAudioBytes1, pAudioPtr2, dwAudioBytes2); }


// IDirectSoundNotify
typedef struct
{
    // IUnknown
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(mal_IDirectSoundNotify* pThis, const IID* const riid, void** ppObject);
    ULONG   (STDMETHODCALLTYPE * AddRef)        (mal_IDirectSoundNotify* pThis);
    ULONG   (STDMETHODCALLTYPE * Release)       (mal_IDirectSoundNotify* pThis);

    // IDirectSoundNotify
    HRESULT (STDMETHODCALLTYPE * SetNotificationPositions)(mal_IDirectSoundNotify* pThis, DWORD dwPositionNotifies, const MAL_DSBPOSITIONNOTIFY* pPositionNotifies);
} mal_IDirectSoundNotifyVtbl;
struct mal_IDirectSoundNotify
{
    mal_IDirectSoundNotifyVtbl* lpVtbl;
};
HRESULT mal_IDirectSoundNotify_QueryInterface(mal_IDirectSoundNotify* pThis, const IID* const riid, void** ppObject) { return pThis->lpVtbl->QueryInterface(pThis, riid, ppObject); }
ULONG   mal_IDirectSoundNotify_AddRef(mal_IDirectSoundNotify* pThis)                                                 { return pThis->lpVtbl->AddRef(pThis); }
ULONG   mal_IDirectSoundNotify_Release(mal_IDirectSoundNotify* pThis)                                                { return pThis->lpVtbl->Release(pThis); }
HRESULT mal_IDirectSoundNotify_SetNotificationPositions(mal_IDirectSoundNotify* pThis, DWORD dwPositionNotifies, const MAL_DSBPOSITIONNOTIFY* pPositionNotifies) { return pThis->lpVtbl->SetNotificationPositions(pThis, dwPositionNotifies, pPositionNotifies); }


typedef BOOL    (CALLBACK * mal_DSEnumCallbackAProc)             (LPGUID pDeviceGUID, LPCSTR pDeviceDescription, LPCSTR pModule, LPVOID pContext);
typedef HRESULT (WINAPI   * mal_DirectSoundCreateProc)           (const GUID* pcGuidDevice, mal_IDirectSound** ppDS8, LPUNKNOWN pUnkOuter);
typedef HRESULT (WINAPI   * mal_DirectSoundEnumerateAProc)       (mal_DSEnumCallbackAProc pDSEnumCallback, LPVOID pContext);
typedef HRESULT (WINAPI   * mal_DirectSoundCaptureCreateProc)    (const GUID* pcGuidDevice, mal_IDirectSoundCapture** ppDSC8, LPUNKNOWN pUnkOuter);
typedef HRESULT (WINAPI   * mal_DirectSoundCaptureEnumerateAProc)(mal_DSEnumCallbackAProc pDSEnumCallback, LPVOID pContext);


// Retrieves the channel count and channel map for the given speaker configuration. If the speaker configuration is unknown,
// the channel count and channel map will be left unmodified.
void mal_get_channels_from_speaker_config__dsound(DWORD speakerConfig, WORD* pChannelsOut, DWORD* pChannelMapOut)
{
    WORD channels = 0;
    if (pChannelsOut != NULL) {
        channels = *pChannelsOut;
    }

    DWORD channelMap = 0;
    if (pChannelMapOut != NULL) {
        channelMap = *pChannelMapOut;
    }

    // The speaker configuration is a combination of speaker config and speaker geometry. The lower 8 bits is what we care about. The upper
    // 16 bits is for the geometry.
    switch ((BYTE)(speakerConfig)) {
        case 1 /*DSSPEAKER_HEADPHONE*/:                          channels = 2; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT; break;
        case 2 /*DSSPEAKER_MONO*/:                               channels = 1; channelMap = SPEAKER_FRONT_CENTER; break;
        case 3 /*DSSPEAKER_QUAD*/:                               channels = 4; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT; break;
        case 4 /*DSSPEAKER_STEREO*/:                             channels = 2; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT; break;
        case 5 /*DSSPEAKER_SURROUND*/:                           channels = 4; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER; break;
        case 6 /*DSSPEAKER_5POINT1_BACK*/ /*DSSPEAKER_5POINT1*/: channels = 6; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT; break;
        case 7 /*DSSPEAKER_7POINT1_WIDE*/ /*DSSPEAKER_7POINT1*/: channels = 8; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_FRONT_LEFT_OF_CENTER | SPEAKER_FRONT_RIGHT_OF_CENTER; break;
        case 8 /*DSSPEAKER_7POINT1_SURROUND*/:                   channels = 8; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT; break;
        case 9 /*DSSPEAKER_5POINT1_SURROUND*/:                   channels = 6; channelMap = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | SPEAKER_SIDE_LEFT | SPEAKER_SIDE_RIGHT; break;
        default: break;
    }

    if (pChannelsOut != NULL) {
        *pChannelsOut = channels;
    }

    if (pChannelMapOut != NULL) {
        *pChannelMapOut = channelMap;
    }
}


mal_bool32 mal_context_is_device_id_equal__dsound(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return memcmp(pID0->dsound, pID1->dsound, sizeof(pID0->dsound)) == 0;
}


typedef struct
{
    mal_context* pContext;
    mal_device_type deviceType;
    mal_enum_devices_callback_proc callback;
    void* pUserData;
    mal_bool32 terminated;
} mal_context_enumerate_devices_callback_data__dsound;

BOOL CALLBACK mal_context_enumerate_devices_callback__dsound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
    (void)lpcstrModule;

    mal_context_enumerate_devices_callback_data__dsound* pData = (mal_context_enumerate_devices_callback_data__dsound*)lpContext;
    mal_assert(pData != NULL);

    mal_device_info deviceInfo;
    mal_zero_object(&deviceInfo);

    // ID.
    if (lpGuid != NULL) {
        mal_copy_memory(deviceInfo.id.dsound, lpGuid, 16);
    } else {
        mal_zero_memory(deviceInfo.id.dsound, 16);
    }

    // Name / Description
    mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), lpcstrDescription, (size_t)-1);


    // Call the callback function, but make sure we stop enumerating if the callee requested so.
    pData->terminated = !pData->callback(pData->pContext, pData->deviceType, &deviceInfo, pData->pUserData);
    if (pData->terminated) {
        return FALSE;   // Stop enumeration.
    } else {
        return TRUE;    // Continue enumeration.
    }
}

mal_result mal_context_enumerate_devices__dsound(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    mal_context_enumerate_devices_callback_data__dsound data;
    data.pContext = pContext;
    data.callback = callback;
    data.pUserData = pUserData;
    data.terminated = MAL_FALSE;

    // Playback.
    if (!data.terminated) {
        data.deviceType = mal_device_type_playback;
        ((mal_DirectSoundEnumerateAProc)pContext->dsound.DirectSoundEnumerateA)(mal_context_enumerate_devices_callback__dsound, &data);
    }

    // Capture.
    if (!data.terminated) {
        data.deviceType = mal_device_type_capture;
        ((mal_DirectSoundCaptureEnumerateAProc)pContext->dsound.DirectSoundCaptureEnumerateA)(mal_context_enumerate_devices_callback__dsound, &data);
    }

    return MAL_SUCCESS;
}


typedef struct
{
    const mal_device_id* pDeviceID;
    mal_device_info* pDeviceInfo;
    mal_bool32 found;
} mal_context_get_device_info_callback_data__dsound;

BOOL CALLBACK mal_context_get_device_info_callback__dsound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
    (void)lpcstrModule;

    mal_context_get_device_info_callback_data__dsound* pData = (mal_context_get_device_info_callback_data__dsound*)lpContext;
    mal_assert(pData != NULL);

    if ((pData->pDeviceID == NULL || mal_is_guid_equal(pData->pDeviceID->dsound, &MAL_GUID_NULL)) && (lpGuid == NULL || mal_is_guid_equal(lpGuid, &MAL_GUID_NULL))) {
        // Default device.
        mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), lpcstrDescription, (size_t)-1);
        pData->found = MAL_TRUE;
        return FALSE;   // Stop enumeration.
    } else {
        // Not the default device.
        if (lpGuid != NULL) {
            if (memcmp(pData->pDeviceID->dsound, lpGuid, sizeof(pData->pDeviceID->dsound)) == 0) {
                mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), lpcstrDescription, (size_t)-1);
                pData->found = MAL_TRUE;
                return FALSE;   // Stop enumeration.
            }
        }
    }

    return TRUE;
}

mal_result mal_context_get_device_info__dsound(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    (void)shareMode;

    if (pDeviceID != NULL) {
        // ID.
        mal_copy_memory(pDeviceInfo->id.dsound, pDeviceID->dsound, 16);

        // Name / Description. This is retrieved by enumerating over each device until we find that one that matches the input ID.
        mal_context_get_device_info_callback_data__dsound data;
        data.pDeviceID = pDeviceID;
        data.pDeviceInfo = pDeviceInfo;
        data.found = MAL_FALSE;
        if (deviceType == mal_device_type_playback) {
            ((mal_DirectSoundEnumerateAProc)pContext->dsound.DirectSoundEnumerateA)(mal_context_get_device_info_callback__dsound, &data);
        } else {
            ((mal_DirectSoundCaptureEnumerateAProc)pContext->dsound.DirectSoundCaptureEnumerateA)(mal_context_get_device_info_callback__dsound, &data);
        }

        if (data.found) {
            return MAL_SUCCESS;
        } else {
            return MAL_NO_DEVICE;
        }
    } else {
        // I don't think there's a way to get the name of the default device with DirectSound. In this case we just need to use defaults.

        // ID
        mal_zero_memory(pDeviceInfo->id.dsound, 16);

        // Name / Description/
        if (deviceType == mal_device_type_playback) {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        } else {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_init__dsound(mal_context* pContext)
{
    mal_assert(pContext != NULL);

    pContext->dsound.hDSoundDLL = mal_dlopen("dsound.dll");
    if (pContext->dsound.hDSoundDLL == NULL) {
        return MAL_API_NOT_FOUND;
    }

    pContext->dsound.DirectSoundCreate            = mal_dlsym(pContext->dsound.hDSoundDLL, "DirectSoundCreate");
    pContext->dsound.DirectSoundEnumerateA        = mal_dlsym(pContext->dsound.hDSoundDLL, "DirectSoundEnumerateA");
    pContext->dsound.DirectSoundCaptureCreate     = mal_dlsym(pContext->dsound.hDSoundDLL, "DirectSoundCaptureCreate");
    pContext->dsound.DirectSoundCaptureEnumerateA = mal_dlsym(pContext->dsound.hDSoundDLL, "DirectSoundCaptureEnumerateA");

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__dsound;
    pContext->onEnumDevices   = mal_context_enumerate_devices__dsound;
    pContext->onGetDeviceInfo = mal_context_get_device_info__dsound;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__dsound(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_dsound);

    mal_dlclose(pContext->dsound.hDSoundDLL);

    return MAL_SUCCESS;
}


typedef struct
{
    mal_uint32 deviceCount;
    mal_uint32 infoCount;
    mal_device_info* pInfo;
} mal_device_enum_data__dsound;

BOOL CALLBACK mal_enum_devices_callback__dsound(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
    (void)lpcstrModule;

    mal_device_enum_data__dsound* pData = (mal_device_enum_data__dsound*)lpContext;
    mal_assert(pData != NULL);

    if (pData->pInfo != NULL) {
        if (pData->infoCount > 0) {
            mal_zero_object(pData->pInfo);
            mal_strncpy_s(pData->pInfo->name, sizeof(pData->pInfo->name), lpcstrDescription, (size_t)-1);

            if (lpGuid != NULL) {
                mal_copy_memory(pData->pInfo->id.dsound, lpGuid, 16);
            } else {
                mal_zero_memory(pData->pInfo->id.dsound, 16);
            }

            pData->pInfo += 1;
            pData->infoCount -= 1;
            pData->deviceCount += 1;
        }
    } else {
        pData->deviceCount += 1;
    }

    return TRUE;
}

void mal_device_uninit__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->dsound.pNotify != NULL) {
        mal_IDirectSoundNotify_Release((mal_IDirectSoundNotify*)pDevice->dsound.pNotify);
    }

    if (pDevice->dsound.hStopEvent) {
        CloseHandle(pDevice->dsound.hStopEvent);
    }
    for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
        if (pDevice->dsound.pNotifyEvents[i]) {
            CloseHandle(pDevice->dsound.pNotifyEvents[i]);
        }
    }

    if (pDevice->dsound.pCaptureBuffer != NULL) {
        mal_IDirectSoundCaptureBuffer_Release((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer);
    }
    if (pDevice->dsound.pCapture != NULL) {
        mal_IDirectSoundCapture_Release((mal_IDirectSoundCapture*)pDevice->dsound.pCapture);
    }

    if (pDevice->dsound.pPlaybackBuffer != NULL) {
        mal_IDirectSoundBuffer_Release((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer);
    }
    if (pDevice->dsound.pPlaybackPrimaryBuffer != NULL) {
        mal_IDirectSoundBuffer_Release((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackPrimaryBuffer);
    }
    if (pDevice->dsound.pPlayback != NULL) {
        mal_IDirectSound_Release((mal_IDirectSound*)pDevice->dsound.pPlayback);
    }
}

mal_result mal_device_init__dsound(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->dsound);

    // Check that we have a valid format.
    GUID subformat;
    switch (pConfig->format)
    {
        case mal_format_u8:
        case mal_format_s16:
        case mal_format_s24:
        //case mal_format_s24_32:
        case mal_format_s32:
        {
            subformat = MAL_GUID_KSDATAFORMAT_SUBTYPE_PCM;
        } break;

        case mal_format_f32:
        {
            subformat = MAL_GUID_KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        } break;

        default:
        return MAL_FORMAT_NOT_SUPPORTED;
    }


    WAVEFORMATEXTENSIBLE wf;
    mal_zero_object(&wf);
    wf.Format.cbSize               = sizeof(wf);
    wf.Format.wFormatTag           = WAVE_FORMAT_EXTENSIBLE;
    wf.Format.nChannels            = (WORD)pConfig->channels;
    wf.Format.nSamplesPerSec       = (DWORD)pConfig->sampleRate;
    wf.Format.wBitsPerSample       = (WORD)mal_get_sample_size_in_bytes(pConfig->format)*8;
    wf.Format.nBlockAlign          = (wf.Format.nChannels * wf.Format.wBitsPerSample) / 8;
    wf.Format.nAvgBytesPerSec      = wf.Format.nBlockAlign * wf.Format.nSamplesPerSec;
    wf.Samples.wValidBitsPerSample = wf.Format.wBitsPerSample;
    wf.dwChannelMask               = mal_channel_map_to_channel_mask__win32(pConfig->channelMap, pConfig->channels);
    wf.SubFormat                   = subformat;

    DWORD bufferSizeInBytes = 0;

    // Unfortunately DirectSound uses different APIs and data structures for playback and catpure devices :(
    if (type == mal_device_type_playback) {
        if (FAILED(((mal_DirectSoundCreateProc)pContext->dsound.DirectSoundCreate)((pDeviceID == NULL) ? NULL : (const GUID*)pDeviceID->dsound, (mal_IDirectSound**)&pDevice->dsound.pPlayback, NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] DirectSoundCreate() failed for playback device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        // The cooperative level must be set before doing anything else.
        HWND hWnd = ((MAL_PFN_GetForegroundWindow)pContext->win32.GetForegroundWindow)();
        if (hWnd == NULL) {
            hWnd = ((MAL_PFN_GetDesktopWindow)pContext->win32.GetDesktopWindow)();
        }
        if (FAILED(mal_IDirectSound_SetCooperativeLevel((mal_IDirectSound*)pDevice->dsound.pPlayback, hWnd, (pConfig->shareMode == mal_share_mode_exclusive) ? MAL_DSSCL_EXCLUSIVE : MAL_DSSCL_PRIORITY))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSound_SetCooperateiveLevel() failed for playback device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        MAL_DSBUFFERDESC descDSPrimary;
        mal_zero_object(&descDSPrimary);
        descDSPrimary.dwSize  = sizeof(MAL_DSBUFFERDESC);
        descDSPrimary.dwFlags = MAL_DSBCAPS_PRIMARYBUFFER | MAL_DSBCAPS_CTRLVOLUME;
        if (FAILED(mal_IDirectSound_CreateSoundBuffer((mal_IDirectSound*)pDevice->dsound.pPlayback, &descDSPrimary, (mal_IDirectSoundBuffer**)&pDevice->dsound.pPlaybackPrimaryBuffer, NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSound_CreateSoundBuffer() failed for playback device's primary buffer.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }


        // We may want to make some adjustments to the format if we are using defaults.
        MAL_DSCAPS caps;
        mal_zero_object(&caps);
        caps.dwSize = sizeof(caps);
        if (FAILED(mal_IDirectSound_GetCaps((mal_IDirectSound*)pDevice->dsound.pPlayback, &caps))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSound_GetCaps() failed for playback device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (pDevice->usingDefaultChannels) {
            if ((caps.dwFlags & MAL_DSCAPS_PRIMARYSTEREO) != 0) {
                // It supports at least stereo, but could support more.
                wf.Format.nChannels = 2;

                // Look at the speaker configuration to get a better idea on the channel count.
                DWORD speakerConfig;
                if (SUCCEEDED(mal_IDirectSound_GetSpeakerConfig((mal_IDirectSound*)pDevice->dsound.pPlayback, &speakerConfig))) {
                    mal_get_channels_from_speaker_config__dsound(speakerConfig, &wf.Format.nChannels, &wf.dwChannelMask);
                }
            } else {
                // It does not support stereo, which means we are stuck with mono.
                wf.Format.nChannels = 1;
            }
        }

        if (pDevice->usingDefaultSampleRate) {
            // We base the sample rate on the values returned by GetCaps().
            if ((caps.dwFlags & MAL_DSCAPS_CONTINUOUSRATE) != 0) {
                wf.Format.nSamplesPerSec = mal_get_best_sample_rate_within_range(caps.dwMinSecondarySampleRate, caps.dwMaxSecondarySampleRate);
            } else {
                wf.Format.nSamplesPerSec = caps.dwMaxSecondarySampleRate;
            }
        }

        wf.Format.nBlockAlign     = (wf.Format.nChannels * wf.Format.wBitsPerSample) / 8;
        wf.Format.nAvgBytesPerSec = wf.Format.nBlockAlign * wf.Format.nSamplesPerSec;

        // From MSDN:
        //
        // The method succeeds even if the hardware does not support the requested format; DirectSound sets the buffer to the closest
        // supported format. To determine whether this has happened, an application can call the GetFormat method for the primary buffer
        // and compare the result with the format that was requested with the SetFormat method.
        if (FAILED(mal_IDirectSoundBuffer_SetFormat((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackPrimaryBuffer, (WAVEFORMATEX*)&wf))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] Failed to set format of playback device's primary buffer.", MAL_FORMAT_NOT_SUPPORTED);
        }

        // Get the _actual_ properties of the buffer.
        char rawdata[1024];
        WAVEFORMATEXTENSIBLE* pActualFormat = (WAVEFORMATEXTENSIBLE*)rawdata;
        if (FAILED(mal_IDirectSoundBuffer_GetFormat((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackPrimaryBuffer, (WAVEFORMATEX*)pActualFormat, sizeof(rawdata), NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] Failed to retrieve the actual format of the playback device's primary buffer.", MAL_FORMAT_NOT_SUPPORTED);
        }

        pDevice->internalFormat = mal_format_from_WAVEFORMATEX((WAVEFORMATEX*)pActualFormat);
        pDevice->internalChannels = pActualFormat->Format.nChannels;
        pDevice->internalSampleRate = pActualFormat->Format.nSamplesPerSec;

        // Get the internal channel map based on the channel mask.
        if (pActualFormat->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
            mal_channel_mask_to_channel_map__win32(pActualFormat->dwChannelMask, pDevice->internalChannels, pDevice->internalChannelMap);
        } else {
            mal_channel_mask_to_channel_map__win32(wf.dwChannelMask, pDevice->internalChannels, pDevice->internalChannelMap);
        }

        bufferSizeInBytes = pDevice->bufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);



        // Meaning of dwFlags (from MSDN):
        //
        // DSBCAPS_CTRLPOSITIONNOTIFY
        //   The buffer has position notification capability.
        //
        // DSBCAPS_GLOBALFOCUS
        //   With this flag set, an application using DirectSound can continue to play its buffers if the user switches focus to
        //   another application, even if the new application uses DirectSound.
        //
        // DSBCAPS_GETCURRENTPOSITION2
        //   In the first version of DirectSound, the play cursor was significantly ahead of the actual playing sound on emulated
        //   sound cards; it was directly behind the write cursor. Now, if the DSBCAPS_GETCURRENTPOSITION2 flag is specified, the
        //   application can get a more accurate play cursor.
        MAL_DSBUFFERDESC descDS;
        mal_zero_object(&descDS);
        descDS.dwSize = sizeof(descDS);
        descDS.dwFlags = MAL_DSBCAPS_CTRLPOSITIONNOTIFY | MAL_DSBCAPS_GLOBALFOCUS | MAL_DSBCAPS_GETCURRENTPOSITION2;
        descDS.dwBufferBytes = bufferSizeInBytes;
        descDS.lpwfxFormat = (WAVEFORMATEX*)&wf;
        if (FAILED(mal_IDirectSound_CreateSoundBuffer((mal_IDirectSound*)pDevice->dsound.pPlayback, &descDS, (mal_IDirectSoundBuffer**)&pDevice->dsound.pPlaybackBuffer, NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSound_CreateSoundBuffer() failed for playback device's secondary buffer.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        // Notifications are set up via a DIRECTSOUNDNOTIFY object which is retrieved from the buffer.
        if (FAILED(mal_IDirectSoundBuffer_QueryInterface((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, &MAL_GUID_IID_DirectSoundNotify, (void**)&pDevice->dsound.pNotify))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundBuffer_QueryInterface() failed for playback device's IDirectSoundNotify object.", MAL_API_NOT_FOUND);
        }
    } else {
        // The default buffer size is treated slightly differently for DirectSound which, for some reason, seems to
        // have worse latency with capture than playback (sometimes _much_ worse).
        if (pDevice->usingDefaultBufferSize) {
            pDevice->bufferSizeInFrames *= 2; // <-- Might need to fiddle with this to find a more ideal value. May even be able to just add a fixed amount rather than scaling.
        }

        if (FAILED(((mal_DirectSoundCaptureCreateProc)pContext->dsound.DirectSoundCaptureCreate)((pDeviceID == NULL) ? NULL : (const GUID*)pDeviceID->dsound, (mal_IDirectSoundCapture**)&pDevice->dsound.pCapture, NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] DirectSoundCaptureCreate() failed for capture device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }


        MAL_DSCCAPS caps;
        mal_zero_object(&caps);
        caps.dwSize = sizeof(caps);
        if (FAILED(mal_IDirectSoundCapture_GetCaps((mal_IDirectSoundCapture*)pDevice->dsound.pCapture, &caps))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundCapture_GetCaps() failed for capture device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        wf.Format.nChannels = (WORD)caps.dwChannels;

        // The device can support multiple formats. We just go through the different formats in order of priority and
        // pick the first one. This the same type of system as the WinMM backend.
        wf.Format.wBitsPerSample = 16;
        wf.Format.nSamplesPerSec = 48000;

        if (caps.dwChannels == 1) {
            if ((caps.dwFormats & WAVE_FORMAT_48M16) != 0) {
                wf.Format.nSamplesPerSec = 48000;
            } else if ((caps.dwFormats & WAVE_FORMAT_44M16) != 0) {
                wf.Format.nSamplesPerSec = 44100;
            } else if ((caps.dwFormats & WAVE_FORMAT_2M16) != 0) {
                wf.Format.nSamplesPerSec = 22050;
            } else if ((caps.dwFormats & WAVE_FORMAT_1M16) != 0) {
                wf.Format.nSamplesPerSec = 11025;
            } else if ((caps.dwFormats & WAVE_FORMAT_96M16) != 0) {
                wf.Format.nSamplesPerSec = 96000;
            } else {
                wf.Format.wBitsPerSample = 8;
                if ((caps.dwFormats & WAVE_FORMAT_48M08) != 0) {
                    wf.Format.nSamplesPerSec = 48000;
                } else if ((caps.dwFormats & WAVE_FORMAT_44M08) != 0) {
                    wf.Format.nSamplesPerSec = 44100;
                } else if ((caps.dwFormats & WAVE_FORMAT_2M08) != 0) {
                    wf.Format.nSamplesPerSec = 22050;
                } else if ((caps.dwFormats & WAVE_FORMAT_1M08) != 0) {
                    wf.Format.nSamplesPerSec = 11025;
                } else if ((caps.dwFormats & WAVE_FORMAT_96M08) != 0) {
                    wf.Format.nSamplesPerSec = 96000;
                } else {
                    wf.Format.wBitsPerSample = 16;  // Didn't find it. Just fall back to 16-bit.
                }
            }
        } else if (caps.dwChannels == 2) {
            if ((caps.dwFormats & WAVE_FORMAT_48S16) != 0) {
                wf.Format.nSamplesPerSec = 48000;
            } else if ((caps.dwFormats & WAVE_FORMAT_44S16) != 0) {
                wf.Format.nSamplesPerSec = 44100;
            } else if ((caps.dwFormats & WAVE_FORMAT_2S16) != 0) {
                wf.Format.nSamplesPerSec = 22050;
            } else if ((caps.dwFormats & WAVE_FORMAT_1S16) != 0) {
                wf.Format.nSamplesPerSec = 11025;
            } else if ((caps.dwFormats & WAVE_FORMAT_96S16) != 0) {
                wf.Format.nSamplesPerSec = 96000;
            } else {
                wf.Format.wBitsPerSample = 8;
                if ((caps.dwFormats & WAVE_FORMAT_48S08) != 0) {
                    wf.Format.nSamplesPerSec = 48000;
                } else if ((caps.dwFormats & WAVE_FORMAT_44S08) != 0) {
                    wf.Format.nSamplesPerSec = 44100;
                } else if ((caps.dwFormats & WAVE_FORMAT_2S08) != 0) {
                    wf.Format.nSamplesPerSec = 22050;
                } else if ((caps.dwFormats & WAVE_FORMAT_1S08) != 0) {
                    wf.Format.nSamplesPerSec = 11025;
                } else if ((caps.dwFormats & WAVE_FORMAT_96S08) != 0) {
                    wf.Format.nSamplesPerSec = 96000;
                } else {
                    wf.Format.wBitsPerSample = 16;  // Didn't find it. Just fall back to 16-bit.
                }
            }
        }

        wf.Format.nBlockAlign          = (wf.Format.nChannels * wf.Format.wBitsPerSample) / 8;
        wf.Format.nAvgBytesPerSec      = wf.Format.nBlockAlign * wf.Format.nSamplesPerSec;
        wf.Samples.wValidBitsPerSample = wf.Format.wBitsPerSample;
        wf.SubFormat                   = MAL_GUID_KSDATAFORMAT_SUBTYPE_PCM;

        bufferSizeInBytes = pDevice->bufferSizeInFrames * wf.Format.nChannels * mal_get_sample_size_in_bytes(pDevice->format);

        MAL_DSCBUFFERDESC descDS;
        mal_zero_object(&descDS);
        descDS.dwSize = sizeof(descDS);
        descDS.dwFlags = 0;
        descDS.dwBufferBytes = bufferSizeInBytes;
        descDS.lpwfxFormat = (WAVEFORMATEX*)&wf;
        if (FAILED(mal_IDirectSoundCapture_CreateCaptureBuffer((mal_IDirectSoundCapture*)pDevice->dsound.pCapture, &descDS, (mal_IDirectSoundCaptureBuffer**)&pDevice->dsound.pCaptureBuffer, NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundCapture_CreateCaptureBuffer() failed for capture device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        // Get the _actual_ properties of the buffer.
        char rawdata[1024];
        WAVEFORMATEXTENSIBLE* pActualFormat = (WAVEFORMATEXTENSIBLE*)rawdata;
        if (FAILED(mal_IDirectSoundCaptureBuffer_GetFormat((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, (WAVEFORMATEX*)pActualFormat, sizeof(rawdata), NULL))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] Failed to retrieve the actual format of the capture device's buffer.", MAL_FORMAT_NOT_SUPPORTED);
        }

        pDevice->internalFormat = mal_format_from_WAVEFORMATEX((WAVEFORMATEX*)pActualFormat);
        pDevice->internalChannels = pActualFormat->Format.nChannels;
        pDevice->internalSampleRate = pActualFormat->Format.nSamplesPerSec;

        // Get the internal channel map based on the channel mask.
        if (pActualFormat->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE) {
            mal_channel_mask_to_channel_map__win32(pActualFormat->dwChannelMask, pDevice->internalChannels, pDevice->internalChannelMap);
        } else {
            mal_channel_mask_to_channel_map__win32(wf.dwChannelMask, pDevice->internalChannels, pDevice->internalChannelMap);
        }


        // Notifications are set up via a DIRECTSOUNDNOTIFY object which is retrieved from the buffer.
        if (FAILED(mal_IDirectSoundCaptureBuffer_QueryInterface((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, &MAL_GUID_IID_DirectSoundNotify, (void**)&pDevice->dsound.pNotify))) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundCaptureBuffer_QueryInterface() failed for capture device's IDirectSoundNotify object.", MAL_API_NOT_FOUND);
        }
    }

    // We need a notification for each period. The notification offset is slightly different depending on whether or not the
    // device is a playback or capture device. For a playback device we want to be notified when a period just starts playing,
    // whereas for a capture device we want to be notified when a period has just _finished_ capturing.
    mal_uint32 periodSizeInBytes = pDevice->bufferSizeInFrames / pDevice->periods;
    MAL_DSBPOSITIONNOTIFY notifyPoints[MAL_MAX_PERIODS_DSOUND];  // One notification event for each period.
    for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
        pDevice->dsound.pNotifyEvents[i] = CreateEventA(NULL, FALSE, FALSE, NULL);
        if (pDevice->dsound.pNotifyEvents[i] == NULL) {
            mal_device_uninit__dsound(pDevice);
            return mal_post_error(pDevice, "[DirectSound] Failed to create event for buffer notifications.", MAL_FAILED_TO_CREATE_EVENT);
        }

        // The notification offset is in bytes.
        notifyPoints[i].dwOffset = i * periodSizeInBytes;
        notifyPoints[i].hEventNotify = pDevice->dsound.pNotifyEvents[i];
    }

    if (FAILED(mal_IDirectSoundNotify_SetNotificationPositions((mal_IDirectSoundNotify*)pDevice->dsound.pNotify, pDevice->periods, notifyPoints))) {
        mal_device_uninit__dsound(pDevice);
        return mal_post_error(pDevice, "[DirectSound] IDirectSoundNotify_SetNotificationPositions() failed.", MAL_FAILED_TO_CREATE_EVENT);
    }

    // When the device is playing the worker thread will be waiting on a bunch of notification events. To return from
    // this wait state we need to signal a special event.
    pDevice->dsound.hStopEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (pDevice->dsound.hStopEvent == NULL) {
        mal_device_uninit__dsound(pDevice);
        return mal_post_error(pDevice, "[DirectSound] Failed to create event for main loop break notification.", MAL_FAILED_TO_CREATE_EVENT);
    }

    return MAL_SUCCESS;
}


mal_result mal_device__start_backend__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        // Before playing anything we need to grab an initial group of samples from the client.
        mal_uint32 framesToRead = pDevice->bufferSizeInFrames / pDevice->periods;
        mal_uint32 desiredLockSize = framesToRead * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format);

        void* pLockPtr;
        DWORD actualLockSize;
        void* pLockPtr2;
        DWORD actualLockSize2;
        if (SUCCEEDED(mal_IDirectSoundBuffer_Lock((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, 0, desiredLockSize, &pLockPtr, &actualLockSize, &pLockPtr2, &actualLockSize2, 0))) {
            framesToRead = actualLockSize / mal_get_sample_size_in_bytes(pDevice->format) / pDevice->channels;
            mal_device__read_frames_from_client(pDevice, framesToRead, pLockPtr);
            mal_IDirectSoundBuffer_Unlock((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, pLockPtr, actualLockSize, pLockPtr2, actualLockSize2);

            pDevice->dsound.lastProcessedFrame = framesToRead;
            if (FAILED(mal_IDirectSoundBuffer_Play((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, 0, 0, MAL_DSBPLAY_LOOPING))) {
                return mal_post_error(pDevice, "[DirectSound] IDirectSoundBuffer_Play() failed.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        } else {
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundBuffer_Lock() failed.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
        }
    } else {
        if (FAILED(mal_IDirectSoundCaptureBuffer_Start((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, MAL_DSCBSTART_LOOPING))) {
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundCaptureBuffer_Start() failed.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        if (FAILED(mal_IDirectSoundBuffer_Stop((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer))) {
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundBuffer_Stop() failed.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
        }

        mal_IDirectSoundBuffer_SetCurrentPosition((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, 0);
    } else {
        if (FAILED(mal_IDirectSoundCaptureBuffer_Stop((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer))) {
            return mal_post_error(pDevice, "[DirectSound] IDirectSoundCaptureBuffer_Stop() failed.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // The main loop will be waiting on a bunch of events via the WaitForMultipleObjects() API. One of those events
    // is a special event we use for forcing that function to return.
    pDevice->dsound.breakFromMainLoop = MAL_TRUE;
    SetEvent(pDevice->dsound.hStopEvent);
    return MAL_SUCCESS;
}

mal_bool32 mal_device__get_current_frame__dsound(mal_device* pDevice, mal_uint32* pCurrentPos)
{
    mal_assert(pDevice != NULL);
    mal_assert(pCurrentPos != NULL);
    *pCurrentPos = 0;

    DWORD dwCurrentPosition;
    if (pDevice->type == mal_device_type_playback) {
        if (FAILED(mal_IDirectSoundBuffer_GetCurrentPosition((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, NULL, &dwCurrentPosition))) {
            return MAL_FALSE;
        }
    } else {
        if (FAILED(mal_IDirectSoundCaptureBuffer_GetCurrentPosition((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, &dwCurrentPosition, NULL))) {
            return MAL_FALSE;
        }
    }

    *pCurrentPos = (mal_uint32)dwCurrentPosition / mal_get_sample_size_in_bytes(pDevice->format) / pDevice->channels;
    return MAL_TRUE;
}

mal_uint32 mal_device__get_available_frames__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_uint32 currentFrame;
    if (!mal_device__get_current_frame__dsound(pDevice, &currentFrame)) {
        return 0;
    }

    // In a playback device the last processed frame should always be ahead of the current frame. The space between
    // the last processed and current frame (moving forward, starting from the last processed frame) is the amount
    // of space available to write.
    //
    // For a recording device it's the other way around - the last processed frame is always _behind_ the current
    // frame and the space between is the available space.
    mal_uint32 totalFrameCount = pDevice->bufferSizeInFrames;
    if (pDevice->type == mal_device_type_playback) {
        mal_uint32 committedBeg = currentFrame;
        mal_uint32 committedEnd;
        committedEnd = pDevice->dsound.lastProcessedFrame;
        if (committedEnd <= committedBeg) {
            committedEnd += totalFrameCount;
        }

        mal_uint32 committedSize = (committedEnd - committedBeg);
        mal_assert(committedSize <= totalFrameCount);

        return totalFrameCount - committedSize;
    } else {
        mal_uint32 validBeg = pDevice->dsound.lastProcessedFrame;
        mal_uint32 validEnd = currentFrame;
        if (validEnd < validBeg) {
            validEnd += totalFrameCount;        // Wrap around.
        }

        mal_uint32 validSize = (validEnd - validBeg);
        mal_assert(validSize <= totalFrameCount);

        return validSize;
    }
}

mal_uint32 mal_device__wait_for_frames__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // The timeout to use for putting the thread to sleep is based on the size of the buffer and the period count.
    DWORD timeoutInMilliseconds = (pDevice->bufferSizeInFrames / (pDevice->sampleRate/1000)) / pDevice->periods;
    if (timeoutInMilliseconds < 1) {
        timeoutInMilliseconds = 1;
    }

    unsigned int eventCount = pDevice->periods + 1;
    HANDLE pEvents[MAL_MAX_PERIODS_DSOUND + 1];   // +1 for the stop event.
    mal_copy_memory(pEvents, pDevice->dsound.pNotifyEvents, sizeof(HANDLE) * pDevice->periods);
    pEvents[eventCount-1] = pDevice->dsound.hStopEvent;

    while (!pDevice->dsound.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__get_available_frames__dsound(pDevice);
        if (framesAvailable > 0) {
            return framesAvailable;
        }

        // If we get here it means we weren't able to find any frames. We'll just wait here for a bit.
        WaitForMultipleObjects(eventCount, pEvents, FALSE, timeoutInMilliseconds);
    }

    // We'll get here if the loop was terminated. Just return whatever's available.
    return mal_device__get_available_frames__dsound(pDevice);
}

mal_result mal_device__main_loop__dsound(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // Make sure the stop event is not signaled to ensure we don't end up immediately returning from WaitForMultipleObjects().
    ResetEvent(pDevice->dsound.hStopEvent);

    pDevice->dsound.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->dsound.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__wait_for_frames__dsound(pDevice);
        if (framesAvailable == 0) {
            continue;
        }

        // If it's a playback device, don't bother grabbing more data if the device is being stopped.
        if (pDevice->dsound.breakFromMainLoop && pDevice->type == mal_device_type_playback) {
            return MAL_FALSE;
        }

        DWORD lockOffset = pDevice->dsound.lastProcessedFrame * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format);
        DWORD lockSize   = framesAvailable * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format);

        if (pDevice->type == mal_device_type_playback) {
            void* pLockPtr;
            DWORD actualLockSize;
            void* pLockPtr2;
            DWORD actualLockSize2;
            if (FAILED(mal_IDirectSoundBuffer_Lock((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, lockOffset, lockSize, &pLockPtr, &actualLockSize, &pLockPtr2, &actualLockSize2, 0))) {
                return mal_post_error(pDevice, "[DirectSound] IDirectSoundBuffer_Lock() failed.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
            }

            mal_uint32 frameCount = actualLockSize / mal_get_sample_size_in_bytes(pDevice->format) / pDevice->channels;
            mal_device__read_frames_from_client(pDevice, frameCount, pLockPtr);
            pDevice->dsound.lastProcessedFrame = (pDevice->dsound.lastProcessedFrame + frameCount) % pDevice->bufferSizeInFrames;

            mal_IDirectSoundBuffer_Unlock((mal_IDirectSoundBuffer*)pDevice->dsound.pPlaybackBuffer, pLockPtr, actualLockSize, pLockPtr2, actualLockSize2);
        } else {
            void* pLockPtr;
            DWORD actualLockSize;
            void* pLockPtr2;
            DWORD actualLockSize2;
            if (FAILED(mal_IDirectSoundCaptureBuffer_Lock((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, lockOffset, lockSize, &pLockPtr, &actualLockSize, &pLockPtr2, &actualLockSize2, 0))) {
                return mal_post_error(pDevice, "[DirectSound] IDirectSoundCaptureBuffer_Lock() failed.", MAL_FAILED_TO_MAP_DEVICE_BUFFER);
            }

            mal_uint32 frameCount = actualLockSize / mal_get_sample_size_in_bytes(pDevice->format) / pDevice->channels;
            mal_device__send_frames_to_client(pDevice, frameCount, pLockPtr);
            pDevice->dsound.lastProcessedFrame = (pDevice->dsound.lastProcessedFrame + frameCount) % pDevice->bufferSizeInFrames;

            mal_IDirectSoundCaptureBuffer_Unlock((mal_IDirectSoundCaptureBuffer*)pDevice->dsound.pCaptureBuffer, pLockPtr, actualLockSize, pLockPtr2, actualLockSize2);
        }
    }

    return MAL_SUCCESS;
}
#endif



///////////////////////////////////////////////////////////////////////////////
//
// WinMM Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_WINMM

// Some older compilers don't have WAVEOUTCAPS2A and WAVEINCAPS2A, so we'll need to write this ourselves. These structures
// are exactly the same as the older ones but they have a few GUIDs for manufacturer/product/name identification. I'm keeping
// the names the same as the Win32 library for consistency, but namespaced to avoid naming conflicts with the Win32 version.
typedef struct
{
    WORD wMid;
    WORD wPid;
    MMVERSION vDriverVersion;
    CHAR szPname[MAXPNAMELEN];
    DWORD dwFormats;
    WORD wChannels;
    WORD wReserved1;
    DWORD dwSupport;
    GUID ManufacturerGuid;
    GUID ProductGuid;
    GUID NameGuid;
} MAL_WAVEOUTCAPS2A;
typedef struct
{
    WORD wMid;
    WORD wPid;
    MMVERSION vDriverVersion;
    CHAR szPname[MAXPNAMELEN];
    DWORD dwFormats;
    WORD wChannels;
    WORD wReserved1;
    GUID ManufacturerGuid;
    GUID ProductGuid;
    GUID NameGuid;
} MAL_WAVEINCAPS2A;

typedef UINT     (WINAPI * MAL_PFN_waveOutGetNumDevs)(void);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutGetDevCapsA)(UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutOpen)(LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutClose)(HWAVEOUT hwo);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutPrepareHeader)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutUnprepareHeader)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutWrite)(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveOutReset)(HWAVEOUT hwo);
typedef UINT     (WINAPI * MAL_PFN_waveInGetNumDevs)(void);
typedef MMRESULT (WINAPI * MAL_PFN_waveInGetDevCapsA)(UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic);
typedef MMRESULT (WINAPI * MAL_PFN_waveInOpen)(LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen);
typedef MMRESULT (WINAPI * MAL_PFN_waveInClose)(HWAVEIN hwi);
typedef MMRESULT (WINAPI * MAL_PFN_waveInPrepareHeader)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveInUnprepareHeader)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveInAddBuffer)(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
typedef MMRESULT (WINAPI * MAL_PFN_waveInStart)(HWAVEIN hwi);
typedef MMRESULT (WINAPI * MAL_PFN_waveInReset)(HWAVEIN hwi);

mal_result mal_result_from_MMRESULT(MMRESULT resultMM)
{
    switch (resultMM) {
        case MMSYSERR_NOERROR:      return MAL_SUCCESS;
        case MMSYSERR_BADDEVICEID:  return MAL_INVALID_ARGS;
        case MMSYSERR_INVALHANDLE:  return MAL_INVALID_ARGS;
        case MMSYSERR_NOMEM:        return MAL_OUT_OF_MEMORY;
        case MMSYSERR_INVALFLAG:    return MAL_INVALID_ARGS;
        case MMSYSERR_INVALPARAM:   return MAL_INVALID_ARGS;
        case MMSYSERR_HANDLEBUSY:   return MAL_DEVICE_BUSY;
        case MMSYSERR_ERROR:        return MAL_ERROR;
        default:                    return MAL_ERROR;
    }
}

char* mal_find_last_character(char* str, char ch)
{
    if (str == NULL) {
        return NULL;
    }

    char* last = NULL;
    while (*str != '\0') {
        if (*str == ch) {
            last = str;
        }

        str += 1;
    }

    return last;
}


// Our own "WAVECAPS" structure that contains generic information shared between WAVEOUTCAPS2 and WAVEINCAPS2 so
// we can do things generically and typesafely. Names are being kept the same for consistency.
typedef struct
{
    CHAR szPname[MAXPNAMELEN];
    DWORD dwFormats;
    WORD wChannels;
    GUID NameGuid;
} MAL_WAVECAPSA;

mal_result mal_context_get_device_info_from_WAVECAPS(mal_context* pContext, MAL_WAVECAPSA* pCaps, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    mal_assert(pCaps != NULL);
    mal_assert(pDeviceInfo != NULL);

    // Name / Description
    //
    // Unfortunately the name specified in WAVE(OUT/IN)CAPS2 is limited to 31 characters. This results in an unprofessional looking
    // situation where the names of the devices are truncated. To help work around this, we need to look at the name GUID and try
    // looking in the registry for the full name. If we can't find it there, we need to just fall back to the default name.

    // Set the default to begin with.
    mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), pCaps->szPname, (size_t)-1);

    // Now try the registry. There's a few things to consider here:
    // - The name GUID can be null, in which we case we just need to stick to the original 31 characters.
    // - If the name GUID is not present in the registry we'll also need to stick to the original 31 characters.
    // - I like consistency, so I want the returned device names to be consistent with those returned by WASAPI and DirectSound. The
    //   problem, however is that WASAPI and DirectSound use "<component> (<name>)" format (such as "Speakers (High Definition Audio)"),
    //   but WinMM does not specificy the component name. From my admittedly limited testing, I've notice the component name seems to
    //   usually fit within the 31 characters of the fixed sized buffer, so what I'm going to do is parse that string for the component
    //   name, and then concatenate the name from the registry.
    if (!mal_is_guid_equal(&pCaps->NameGuid, &MAL_GUID_NULL)) {
        wchar_t guidStrW[256];
        if (((MAL_PFN_StringFromGUID2)pContext->win32.StringFromGUID2)(&pCaps->NameGuid, guidStrW, mal_countof(guidStrW)) > 0) {
            char guidStr[256];
            WideCharToMultiByte(CP_UTF8, 0, guidStrW, -1, guidStr, sizeof(guidStr), 0, FALSE);

            char keyStr[1024];
            mal_strcpy_s(keyStr, sizeof(keyStr), "SYSTEM\\CurrentControlSet\\Control\\MediaCategories\\");
            mal_strcat_s(keyStr, sizeof(keyStr), guidStr);

            HKEY hKey;
            LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyStr, 0, KEY_READ, &hKey);
            if (result == ERROR_SUCCESS) {
                BYTE nameFromReg[512];
                DWORD nameFromRegSize = sizeof(nameFromReg);
                result = RegQueryValueExA(hKey, "Name", 0, NULL, (LPBYTE)nameFromReg, (LPDWORD)&nameFromRegSize);
                RegCloseKey(hKey);

                if (result == ERROR_SUCCESS) {
                    // We have the value from the registry, so now we need to construct the name string.
                    char name[1024];
                    if (mal_strcpy_s(name, sizeof(name), pDeviceInfo->name) == 0) {
                        char* nameBeg = mal_find_last_character(name, '(');
                        if (nameBeg != NULL) {
                            size_t leadingLen = (nameBeg - name);
                            mal_strncpy_s(nameBeg + 1, sizeof(name) - leadingLen, (const char*)nameFromReg, (size_t)-1);

                            // The closing ")", if it can fit.
                            if (leadingLen + nameFromRegSize < sizeof(name)-1) {
                                mal_strcat_s(name, sizeof(name), ")");
                            }

                            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), name, (size_t)-1);
                        }
                    }
                }
            }
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info_from_WAVEOUTCAPS2(mal_context* pContext, MAL_WAVEOUTCAPS2A* pCaps, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    mal_assert(pCaps != NULL);
    mal_assert(pDeviceInfo != NULL);

    MAL_WAVECAPSA caps;
    mal_copy_memory(caps.szPname, pCaps->szPname, sizeof(caps.szPname));
    caps.dwFormats = pCaps->dwFormats;
    caps.wChannels = pCaps->wChannels;
    caps.NameGuid = pCaps->NameGuid;
    return mal_context_get_device_info_from_WAVECAPS(pContext, &caps, pDeviceInfo);
}

mal_result mal_context_get_device_info_from_WAVEINCAPS2(mal_context* pContext, MAL_WAVEINCAPS2A* pCaps, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    mal_assert(pCaps != NULL);
    mal_assert(pDeviceInfo != NULL);

    MAL_WAVECAPSA caps;
    mal_copy_memory(caps.szPname, pCaps->szPname, sizeof(caps.szPname));
    caps.dwFormats = pCaps->dwFormats;
    caps.wChannels = pCaps->wChannels;
    caps.NameGuid = pCaps->NameGuid;
    return mal_context_get_device_info_from_WAVECAPS(pContext, &caps, pDeviceInfo);
}


mal_bool32 mal_context_is_device_id_equal__winmm(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return pID0->winmm == pID1->winmm;
}

mal_result mal_context_enumerate_devices__winmm(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    // Playback.
    UINT playbackDeviceCount = ((MAL_PFN_waveOutGetNumDevs)pContext->winmm.waveOutGetNumDevs)();
    for (UINT iPlaybackDevice = 0; iPlaybackDevice < playbackDeviceCount; ++iPlaybackDevice) {
        MAL_WAVEOUTCAPS2A caps;
        mal_zero_object(&caps);
        MMRESULT result = ((MAL_PFN_waveOutGetDevCapsA)pContext->winmm.waveOutGetDevCapsA)(iPlaybackDevice, (WAVEOUTCAPSA*)&caps, sizeof(caps));
        if (result == MMSYSERR_NOERROR) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            deviceInfo.id.winmm = iPlaybackDevice;

            if (mal_context_get_device_info_from_WAVEOUTCAPS2(pContext, &caps, &deviceInfo) == MAL_SUCCESS) {
                mal_bool32 cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    return MAL_SUCCESS; // Enumeration was stopped.
                }
            }
        }
    }

    // Capture.
    UINT captureDeviceCount = ((MAL_PFN_waveInGetNumDevs)pContext->winmm.waveInGetNumDevs)();
    for (UINT iCaptureDevice = 0; iCaptureDevice < captureDeviceCount; ++iCaptureDevice) {
        MAL_WAVEINCAPS2A caps;
        mal_zero_object(&caps);
        MMRESULT result = ((MAL_PFN_waveInGetDevCapsA)pContext->winmm.waveInGetDevCapsA)(iCaptureDevice, (WAVEINCAPSA*)&caps, sizeof(caps));
        if (result == MMSYSERR_NOERROR) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            deviceInfo.id.winmm = iCaptureDevice;

            if (mal_context_get_device_info_from_WAVEINCAPS2(pContext, &caps, &deviceInfo) == MAL_SUCCESS) {
                mal_bool32 cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    return MAL_SUCCESS; // Enumeration was stopped.
                }
            }
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__winmm(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    UINT winMMDeviceID = 0;
    if (pDeviceID != NULL) {
        winMMDeviceID = (UINT)pDeviceID->winmm;
    }

    pDeviceInfo->id.winmm = winMMDeviceID;

    if (deviceType == mal_device_type_playback) {
        MAL_WAVEOUTCAPS2A caps;
        mal_zero_object(&caps);
        MMRESULT result = ((MAL_PFN_waveOutGetDevCapsA)pContext->winmm.waveOutGetDevCapsA)(winMMDeviceID, (WAVEOUTCAPSA*)&caps, sizeof(caps));
        if (result == MMSYSERR_NOERROR) {
            return mal_context_get_device_info_from_WAVEOUTCAPS2(pContext, &caps, pDeviceInfo);
        }
    } else {
        MAL_WAVEINCAPS2A caps;
        mal_zero_object(&caps);
        MMRESULT result = ((MAL_PFN_waveInGetDevCapsA)pContext->winmm.waveInGetDevCapsA)(winMMDeviceID, (WAVEINCAPSA*)&caps, sizeof(caps));
        if (result == MMSYSERR_NOERROR) {
            return mal_context_get_device_info_from_WAVEINCAPS2(pContext, &caps, pDeviceInfo);
        }
    }

    return MAL_NO_DEVICE;
}


mal_result mal_context_init__winmm(mal_context* pContext)
{
    mal_assert(pContext != NULL);

    pContext->winmm.hWinMM = mal_dlopen("winmm.dll");
    if (pContext->winmm.hWinMM == NULL) {
        return MAL_NO_BACKEND;
    }

    pContext->winmm.waveOutGetNumDevs      = mal_dlsym(pContext->winmm.hWinMM, "waveOutGetNumDevs");
    pContext->winmm.waveOutGetDevCapsA     = mal_dlsym(pContext->winmm.hWinMM, "waveOutGetDevCapsA");
    pContext->winmm.waveOutOpen            = mal_dlsym(pContext->winmm.hWinMM, "waveOutOpen");
    pContext->winmm.waveOutClose           = mal_dlsym(pContext->winmm.hWinMM, "waveOutClose");
    pContext->winmm.waveOutPrepareHeader   = mal_dlsym(pContext->winmm.hWinMM, "waveOutPrepareHeader");
    pContext->winmm.waveOutUnprepareHeader = mal_dlsym(pContext->winmm.hWinMM, "waveOutUnprepareHeader");
    pContext->winmm.waveOutWrite           = mal_dlsym(pContext->winmm.hWinMM, "waveOutWrite");
    pContext->winmm.waveOutReset           = mal_dlsym(pContext->winmm.hWinMM, "waveOutReset");
    pContext->winmm.waveInGetNumDevs       = mal_dlsym(pContext->winmm.hWinMM, "waveInGetNumDevs");
    pContext->winmm.waveInGetDevCapsA      = mal_dlsym(pContext->winmm.hWinMM, "waveInGetDevCapsA");
    pContext->winmm.waveInOpen             = mal_dlsym(pContext->winmm.hWinMM, "waveInOpen");
    pContext->winmm.waveInClose            = mal_dlsym(pContext->winmm.hWinMM, "waveInClose");
    pContext->winmm.waveInPrepareHeader    = mal_dlsym(pContext->winmm.hWinMM, "waveInPrepareHeader");
    pContext->winmm.waveInUnprepareHeader  = mal_dlsym(pContext->winmm.hWinMM, "waveInUnprepareHeader");
    pContext->winmm.waveInAddBuffer        = mal_dlsym(pContext->winmm.hWinMM, "waveInAddBuffer");
    pContext->winmm.waveInStart            = mal_dlsym(pContext->winmm.hWinMM, "waveInStart");
    pContext->winmm.waveInReset            = mal_dlsym(pContext->winmm.hWinMM, "waveInReset");

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__winmm;
    pContext->onEnumDevices   = mal_context_enumerate_devices__winmm;
    pContext->onGetDeviceInfo = mal_context_get_device_info__winmm;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__winmm(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_winmm);

    mal_dlclose(pContext->winmm.hWinMM);
    return MAL_SUCCESS;
}

void mal_device_uninit__winmm(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        ((MAL_PFN_waveOutClose)pDevice->pContext->winmm.waveOutClose)((HWAVEOUT)pDevice->winmm.hDevice);
    } else {
        ((MAL_PFN_waveInClose)pDevice->pContext->winmm.waveInClose)((HWAVEIN)pDevice->winmm.hDevice);
    }

    mal_free(pDevice->winmm._pHeapData);
    CloseHandle((HANDLE)pDevice->winmm.hEvent);
}

mal_result mal_device_init__winmm(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_uint32 heapSize;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->winmm);

    UINT winMMDeviceID = 0;
    if (pDeviceID != NULL) {
        winMMDeviceID = (UINT)pDeviceID->winmm;
    }

    const char* errorMsg = "";
    mal_result errorCode = MAL_ERROR;


    // WinMM doesn't seem to have a good way to query the format of the device. Therefore, we'll restrict the formats to the
    // standard formats documented here https://msdn.microsoft.com/en-us/library/windows/desktop/dd743855(v=vs.85).aspx. If
    // that link goes stale, just look up the documentation for WAVEOUTCAPS or WAVEINCAPS.
    WAVEFORMATEX wf;
    mal_zero_object(&wf);
    wf.cbSize          = sizeof(wf);
    wf.wFormatTag      = WAVE_FORMAT_PCM;
    wf.nChannels       = (WORD)pConfig->channels;
    wf.nSamplesPerSec  = (DWORD)pConfig->sampleRate;
    wf.wBitsPerSample  = (WORD)mal_get_sample_size_in_bytes(pConfig->format)*8;

    if (wf.nChannels > 2) {
        wf.nChannels = 2;
    }

    if (wf.wBitsPerSample != 8 && wf.wBitsPerSample != 16) {
        if (wf.wBitsPerSample <= 8) {
            wf.wBitsPerSample = 8;
        } else {
            wf.wBitsPerSample = 16;
        }
    }

    if (wf.nSamplesPerSec <= 11025) {
        wf.nSamplesPerSec = 11025;
    } else if (wf.nSamplesPerSec <= 22050) {
        wf.nSamplesPerSec = 22050;
    } else if (wf.nSamplesPerSec <= 44100) {
        wf.nSamplesPerSec = 44100;
    } else if (wf.nSamplesPerSec <= 48000) {
        wf.nSamplesPerSec = 48000;
    } else {
        wf.nSamplesPerSec = 96000;
    }


    // Change the format based on the closest match of the supported standard formats.
    DWORD dwFormats = 0;
    WORD wChannels = 0;
    if (type == mal_device_type_playback) {
        WAVEOUTCAPSA caps;
        if (((MAL_PFN_waveOutGetDevCapsA)pContext->winmm.waveOutGetDevCapsA)(winMMDeviceID, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            dwFormats = caps.dwFormats;
            wChannels = caps.wChannels;
        } else {
            errorMsg = "[WinMM] Failed to retrieve internal device caps.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
            goto on_error;
        }
    } else {
        WAVEINCAPSA caps;
        if (((MAL_PFN_waveInGetDevCapsA)pContext->winmm.waveInGetDevCapsA)(winMMDeviceID, &caps, sizeof(caps)) == MMSYSERR_NOERROR) {
            dwFormats = caps.dwFormats;
            wChannels = caps.wChannels;
        } else {
            errorMsg = "[WinMM] Failed to retrieve internal device caps.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
            goto on_error;
        }
    }

    if (dwFormats == 0) {
        errorMsg = "[WinMM] Failed to retrieve the supported formats for the internal device.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
        goto on_error;
    }

    if (wChannels == 1) {
        wf.nChannels = 1;
        wf.wBitsPerSample = 16;
        if ((dwFormats & WAVE_FORMAT_48M16) != 0) {
            wf.nSamplesPerSec = 48000;
        } else if ((dwFormats & WAVE_FORMAT_44M16) != 0) {
            wf.nSamplesPerSec = 44100;
        } else if ((dwFormats & WAVE_FORMAT_2M16) != 0) {
            wf.nSamplesPerSec = 22050;
        } else if ((dwFormats & WAVE_FORMAT_1M16) != 0) {
            wf.nSamplesPerSec = 11025;
        } else if ((dwFormats & WAVE_FORMAT_96M16) != 0) {
            wf.nSamplesPerSec = 96000;
        } else {
            wf.wBitsPerSample = 8;
            if ((dwFormats & WAVE_FORMAT_48M08) != 0) {
                wf.nSamplesPerSec = 48000;
            } else if ((dwFormats & WAVE_FORMAT_44M08) != 0) {
                wf.nSamplesPerSec = 44100;
            } else if ((dwFormats & WAVE_FORMAT_2M08) != 0) {
                wf.nSamplesPerSec = 22050;
            } else if ((dwFormats & WAVE_FORMAT_1M08) != 0) {
                wf.nSamplesPerSec = 11025;
            } else if ((dwFormats & WAVE_FORMAT_96M08) != 0) {
                wf.nSamplesPerSec = 96000;
            } else {
                errorMsg = "[WinMM] Could not find appropriate format for internal device.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
                goto on_error;
            }
        }
    } else {
        wf.nChannels = 2;
        wf.wBitsPerSample = 16;
        if ((dwFormats & WAVE_FORMAT_48S16) != 0) {
            wf.nSamplesPerSec = 48000;
        } else if ((dwFormats & WAVE_FORMAT_44S16) != 0) {
            wf.nSamplesPerSec = 44100;
        } else if ((dwFormats & WAVE_FORMAT_2S16) != 0) {
            wf.nSamplesPerSec = 22050;
        } else if ((dwFormats & WAVE_FORMAT_1S16) != 0) {
            wf.nSamplesPerSec = 11025;
        } else if ((dwFormats & WAVE_FORMAT_96S16) != 0) {
            wf.nSamplesPerSec = 96000;
        } else {
            wf.wBitsPerSample = 8;
            if ((dwFormats & WAVE_FORMAT_48S08) != 0) {
                wf.nSamplesPerSec = 48000;
            } else if ((dwFormats & WAVE_FORMAT_44S08) != 0) {
                wf.nSamplesPerSec = 44100;
            } else if ((dwFormats & WAVE_FORMAT_2S08) != 0) {
                wf.nSamplesPerSec = 22050;
            } else if ((dwFormats & WAVE_FORMAT_1S08) != 0) {
                wf.nSamplesPerSec = 11025;
            } else if ((dwFormats & WAVE_FORMAT_96S08) != 0) {
                wf.nSamplesPerSec = 96000;
            } else {
                errorMsg = "[WinMM] Could not find appropriate format for internal device.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
                goto on_error;
            }
        }
    }

    wf.nBlockAlign     = (wf.nChannels * wf.wBitsPerSample) / 8;
    wf.nAvgBytesPerSec = wf.nBlockAlign * wf.nSamplesPerSec;


    // We use an event to know when a new fragment needs to be enqueued.
    pDevice->winmm.hEvent = (mal_handle)CreateEvent(NULL, TRUE, TRUE, NULL);
    if (pDevice->winmm.hEvent == NULL) {
        errorMsg = "[WinMM] Failed to create event for fragment enqueing.", errorCode = MAL_FAILED_TO_CREATE_EVENT;
        goto on_error;
    }


    if (type == mal_device_type_playback) {
        MMRESULT result = ((MAL_PFN_waveOutOpen)pContext->winmm.waveOutOpen)((LPHWAVEOUT)&pDevice->winmm.hDevice, winMMDeviceID, &wf, (DWORD_PTR)pDevice->winmm.hEvent, (DWORD_PTR)pDevice, CALLBACK_EVENT | WAVE_ALLOWSYNC);
        if (result != MMSYSERR_NOERROR) {
            errorMsg = "[WinMM] Failed to open playback device.", errorCode = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
            goto on_error;
        }
    } else {
        MMRESULT result = ((MAL_PFN_waveInOpen)pDevice->pContext->winmm.waveInOpen)((LPHWAVEIN)&pDevice->winmm.hDevice, winMMDeviceID, &wf, (DWORD_PTR)pDevice->winmm.hEvent, (DWORD_PTR)pDevice, CALLBACK_EVENT | WAVE_ALLOWSYNC);
        if (result != MMSYSERR_NOERROR) {
            errorMsg = "[WinMM] Failed to open capture device.", errorCode = MAL_FAILED_TO_OPEN_BACKEND_DEVICE;
            goto on_error;
        }
    }


    // The internal formats need to be set based on the wf object.
    if (wf.wFormatTag == WAVE_FORMAT_PCM) {
        switch (wf.wBitsPerSample) {
            case 8:  pDevice->internalFormat = mal_format_u8;  break;
            case 16: pDevice->internalFormat = mal_format_s16; break;
            case 24: pDevice->internalFormat = mal_format_s24; break;
            case 32: pDevice->internalFormat = mal_format_s32; break;
            default: mal_post_error(pDevice, "[WinMM] The device's internal format is not supported by mini_al.", MAL_FORMAT_NOT_SUPPORTED);
        }
    } else {
        errorMsg = "[WinMM] The device's internal format is not supported by mini_al.", errorCode = MAL_FORMAT_NOT_SUPPORTED;
        goto on_error;
    }

    pDevice->internalChannels = wf.nChannels;
    pDevice->internalSampleRate = wf.nSamplesPerSec;


    // Just use the default channel mapping. WinMM only supports mono or stereo anyway so it'll reliably be left/right order for stereo.
    mal_get_standard_channel_map(mal_standard_channel_map_microsoft, pDevice->internalChannels, pDevice->internalChannelMap);


    // Latency with WinMM seems pretty bad from my testing... Need to increase the default buffer size.
    if (pDevice->usingDefaultBufferSize) {
        if (pDevice->type == mal_device_type_playback) {
            pDevice->bufferSizeInFrames *= 4; // <-- Might need to fiddle with this to find a more ideal value. May even be able to just add a fixed amount rather than scaling.
        } else {
            pDevice->bufferSizeInFrames *= 2;
        }
    }

    // The size of the intermediary buffer needs to be able to fit every fragment.
    pDevice->winmm.fragmentSizeInFrames = pDevice->bufferSizeInFrames / pDevice->periods;
    pDevice->winmm.fragmentSizeInBytes = pDevice->winmm.fragmentSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);

    heapSize = (sizeof(WAVEHDR) * pDevice->periods) + (pDevice->winmm.fragmentSizeInBytes * pDevice->periods);
    pDevice->winmm._pHeapData = (mal_uint8*)mal_malloc(heapSize);
    if (pDevice->winmm._pHeapData == NULL) {
        errorMsg = "[WinMM] Failed to allocate memory for the intermediary buffer.", errorCode = MAL_OUT_OF_MEMORY;
        goto on_error;
    }

    mal_zero_memory(pDevice->winmm._pHeapData, pDevice->winmm.fragmentSizeInBytes * pDevice->periods);

    pDevice->winmm.pWAVEHDR = pDevice->winmm._pHeapData;
    pDevice->winmm.pIntermediaryBuffer = pDevice->winmm._pHeapData + (sizeof(WAVEHDR) * pDevice->periods);


    return MAL_SUCCESS;

on_error:
    if (pDevice->type == mal_device_type_playback) {
        ((MAL_PFN_waveOutClose)pContext->winmm.waveOutClose)((HWAVEOUT)pDevice->winmm.hDevice);
    } else {
        ((MAL_PFN_waveInClose)pContext->winmm.waveInClose)((HWAVEIN)pDevice->winmm.hDevice);
    }

    mal_free(pDevice->winmm._pHeapData);
    return mal_post_error(pDevice, errorMsg, errorCode);
}


mal_result mal_device__start_backend__winmm(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        // Playback. The device is started when we call waveOutWrite() with a block of data. From MSDN:
        //
        //     Unless the device is paused by calling the waveOutPause function, playback begins when the first data block is sent to the device.
        //
        // When starting the device we commit every fragment. We signal the event before calling waveOutWrite().
        mal_uint32 i;
        for (i = 0; i < pDevice->periods; ++i) {
            mal_zero_object(&((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i]);
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData = (LPSTR)(pDevice->winmm.pIntermediaryBuffer + (pDevice->winmm.fragmentSizeInBytes * i));
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwBufferLength = pDevice->winmm.fragmentSizeInBytes;
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwFlags = 0L;
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwLoops = 0L;
            mal_device__read_frames_from_client(pDevice, pDevice->winmm.fragmentSizeInFrames, ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData);

            if (((MAL_PFN_waveOutPrepareHeader)pDevice->pContext->winmm.waveOutPrepareHeader)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                return mal_post_error(pDevice, "[WinMM] Failed to start backend device. Failed to prepare header.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        }

        ResetEvent(pDevice->winmm.hEvent);

        for (i = 0; i < pDevice->periods; ++i) {
            if (((MAL_PFN_waveOutWrite)pDevice->pContext->winmm.waveOutWrite)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
                return mal_post_error(pDevice, "[WinMM] Failed to start backend device. Failed to send data to the backend device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        }
    } else {
        // Capture.
        for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
            mal_zero_object(&((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i]);
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData = (LPSTR)(pDevice->winmm.pIntermediaryBuffer + (pDevice->winmm.fragmentSizeInBytes * i));
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwBufferLength = pDevice->winmm.fragmentSizeInBytes;
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwFlags = 0L;
            ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwLoops = 0L;

            MMRESULT resultMM = ((MAL_PFN_waveInPrepareHeader)pDevice->pContext->winmm.waveInPrepareHeader)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
            if (resultMM != MMSYSERR_NOERROR) {
                mal_post_error(pDevice, "[WinMM] Failed to prepare header for capture device in preparation for adding a new capture buffer for the device.", mal_result_from_MMRESULT(resultMM));
                break;
            }

            resultMM = ((MAL_PFN_waveInAddBuffer)pDevice->pContext->winmm.waveInAddBuffer)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
            if (resultMM != MMSYSERR_NOERROR) {
                mal_post_error(pDevice, "[WinMM] Failed to add new capture buffer to the internal capture device.", mal_result_from_MMRESULT(resultMM));
                break;
            }
        }

        ResetEvent(pDevice->winmm.hEvent);

        if (((MAL_PFN_waveInStart)pDevice->pContext->winmm.waveInStart)((HWAVEIN)pDevice->winmm.hDevice) != MMSYSERR_NOERROR) {
            return mal_post_error(pDevice, "[WinMM] Failed to start backend device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }
    }

    pDevice->winmm.iNextHeader = 0;
    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__winmm(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        MMRESULT resultMM = ((MAL_PFN_waveOutReset)pDevice->pContext->winmm.waveOutReset)((HWAVEOUT)pDevice->winmm.hDevice);
        if (resultMM != MMSYSERR_NOERROR) {
            mal_post_error(pDevice, "[WinMM] WARNING: Failed to reset playback device.", mal_result_from_MMRESULT(resultMM));
        }

        // Unprepare all WAVEHDR structures.
        for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
            resultMM = ((MAL_PFN_waveOutUnprepareHeader)pDevice->pContext->winmm.waveOutUnprepareHeader)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
            if (resultMM != MMSYSERR_NOERROR) {
                mal_post_error(pDevice, "[WinMM] WARNING: Failed to unprepare header for playback device.", mal_result_from_MMRESULT(resultMM));
            }
        }
    } else {
        MMRESULT resultMM = ((MAL_PFN_waveInReset)pDevice->pContext->winmm.waveInReset)((HWAVEIN)pDevice->winmm.hDevice);
        if (resultMM != MMSYSERR_NOERROR) {
            mal_post_error(pDevice, "[WinMM] WARNING: Failed to reset capture device.", mal_result_from_MMRESULT(resultMM));
        }

        // Unprepare all WAVEHDR structures.
        for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
            resultMM = ((MAL_PFN_waveInUnprepareHeader)pDevice->pContext->winmm.waveInUnprepareHeader)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
            if (resultMM != MMSYSERR_NOERROR) {
                mal_post_error(pDevice, "[WinMM] WARNING: Failed to unprepare header for playback device.", mal_result_from_MMRESULT(resultMM));
            }
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__winmm(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->winmm.breakFromMainLoop = MAL_TRUE;
    SetEvent((HANDLE)pDevice->winmm.hEvent);

    return MAL_SUCCESS;
}

mal_result mal_device__main_loop__winmm(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_uint32 counter;

    pDevice->winmm.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->winmm.breakFromMainLoop) {
        // Wait for a block of data to finish processing...
        if (WaitForSingleObject((HANDLE)pDevice->winmm.hEvent, INFINITE) != WAIT_OBJECT_0) {
            break;
        }

        // Break from the main loop if the device isn't started anymore. Likely what's happened is the application
        // has requested that the device be stopped.
        if (!mal_device_is_started(pDevice)) {
            break;
        }

        // Any headers that are marked as done need to be handled. We start by processing the completed blocks. Then we reset the event
        // and then write or add replacement buffers to the device.
        mal_uint32 iFirstHeader = pDevice->winmm.iNextHeader;
        for (counter = 0; counter < pDevice->periods; ++counter) {
            mal_uint32 i = pDevice->winmm.iNextHeader;
            if ((((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwFlags & WHDR_DONE) == 0) {
                break;
            }

            if (pDevice->type == mal_device_type_playback) {
                // Playback.
                MMRESULT resultMM = ((MAL_PFN_waveOutUnprepareHeader)pDevice->pContext->winmm.waveOutUnprepareHeader)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                if (resultMM != MMSYSERR_NOERROR) {
                    mal_post_error(pDevice, "[WinMM] Failed to unprepare header for playback device in preparation for sending a new block of data to the device for playback.", mal_result_from_MMRESULT(resultMM));
                    break;
                }

                mal_zero_object(&((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i]);
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData = (LPSTR)(pDevice->winmm.pIntermediaryBuffer + (pDevice->winmm.fragmentSizeInBytes * i));
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwBufferLength = pDevice->winmm.fragmentSizeInBytes;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwFlags = 0L;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwLoops = 0L;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwUser = 1;     // <-- Used in the next section to identify the buffers that needs to be re-written to the device.
                mal_device__read_frames_from_client(pDevice, pDevice->winmm.fragmentSizeInFrames, ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData);

                resultMM = ((MAL_PFN_waveOutPrepareHeader)pDevice->pContext->winmm.waveOutPrepareHeader)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                if (resultMM != MMSYSERR_NOERROR) {
                    mal_post_error(pDevice, "[WinMM] Failed to prepare header for playback device in preparation for sending a new block of data to the device for playback.", mal_result_from_MMRESULT(resultMM));
                    break;
                }
            } else {
                // Capture.
                mal_uint32 framesCaptured = (mal_uint32)(((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwBytesRecorded) / pDevice->internalChannels / mal_get_sample_size_in_bytes(pDevice->internalFormat);
                if (framesCaptured > 0) {
                    mal_device__send_frames_to_client(pDevice, framesCaptured, ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData);
                }

                MMRESULT resultMM = ((MAL_PFN_waveInUnprepareHeader)pDevice->pContext->winmm.waveInUnprepareHeader)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                if (resultMM != MMSYSERR_NOERROR) {
                    mal_post_error(pDevice, "[WinMM] Failed to unprepare header for capture device in preparation for adding a new capture buffer for the device.", mal_result_from_MMRESULT(resultMM));
                    break;
                }

                mal_zero_object(&((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i]);
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].lpData = (LPSTR)(pDevice->winmm.pIntermediaryBuffer + (pDevice->winmm.fragmentSizeInBytes * i));
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwBufferLength = pDevice->winmm.fragmentSizeInBytes;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwFlags = 0L;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwLoops = 0L;
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwUser = 1;     // <-- Used in the next section to identify the buffers that needs to be re-added to the device.

                resultMM = ((MAL_PFN_waveInPrepareHeader)pDevice->pContext->winmm.waveInPrepareHeader)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                if (resultMM != MMSYSERR_NOERROR) {
                    mal_post_error(pDevice, "[WinMM] Failed to prepare header for capture device in preparation for adding a new capture buffer for the device.", mal_result_from_MMRESULT(resultMM));
                    break;
                }
            }

            pDevice->winmm.iNextHeader = (pDevice->winmm.iNextHeader + 1) % pDevice->periods;
        }

        ResetEvent((HANDLE)pDevice->winmm.hEvent);

        for (counter = 0; counter < pDevice->periods; ++counter) {
            mal_uint32 i = (iFirstHeader + counter) % pDevice->periods;

            if (((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwUser == 1) {
                ((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i].dwUser = 0;

                if (pDevice->type == mal_device_type_playback) {
                    // Playback.
                    MMRESULT resultMM = ((MAL_PFN_waveOutWrite)pDevice->pContext->winmm.waveOutWrite)((HWAVEOUT)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                    if (resultMM != MMSYSERR_NOERROR) {
                        mal_post_error(pDevice, "[WinMM] Failed to write data to the internal playback device.", mal_result_from_MMRESULT(resultMM));
                        break;
                    }
                } else {
                    // Capture.
                    MMRESULT resultMM = ((MAL_PFN_waveInAddBuffer)pDevice->pContext->winmm.waveInAddBuffer)((HWAVEIN)pDevice->winmm.hDevice, &((LPWAVEHDR)pDevice->winmm.pWAVEHDR)[i], sizeof(WAVEHDR));
                    if (resultMM != MMSYSERR_NOERROR) {
                        mal_post_error(pDevice, "[WinMM] Failed to add new capture buffer to the internal capture device.", mal_result_from_MMRESULT(resultMM));
                        break;
                    }
                }
            }
        }
    }

    return MAL_SUCCESS;
}
#endif




///////////////////////////////////////////////////////////////////////////////
//
// ALSA Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_ALSA

#ifdef MAL_NO_RUNTIME_LINKING
#include <alsa/asoundlib.h>
typedef snd_pcm_uframes_t                       mal_snd_pcm_uframes_t;
typedef snd_pcm_sframes_t                       mal_snd_pcm_sframes_t;
typedef snd_pcm_stream_t                        mal_snd_pcm_stream_t;
typedef snd_pcm_format_t                        mal_snd_pcm_format_t;
typedef snd_pcm_access_t                        mal_snd_pcm_access_t;
typedef snd_pcm_t                               mal_snd_pcm_t;
typedef snd_pcm_hw_params_t                     mal_snd_pcm_hw_params_t;
typedef snd_pcm_sw_params_t                     mal_snd_pcm_sw_params_t;
typedef snd_pcm_format_mask_t                   mal_snd_pcm_format_mask_t;
typedef snd_pcm_info_t                          mal_snd_pcm_info_t;
typedef snd_pcm_channel_area_t                  mal_snd_pcm_channel_area_t;
typedef snd_pcm_chmap_t                         mal_snd_pcm_chmap_t;

// snd_pcm_stream_t
#define MAL_SND_PCM_STREAM_PLAYBACK             SND_PCM_STREAM_PLAYBACK
#define MAL_SND_PCM_STREAM_CAPTURE              SND_PCM_STREAM_CAPTURE

// snd_pcm_format_t
#define MAL_SND_PCM_FORMAT_UNKNOWN              SND_PCM_FORMAT_UNKNOWN
#define MAL_SND_PCM_FORMAT_U8                   SND_PCM_FORMAT_U8
#define MAL_SND_PCM_FORMAT_S16_LE               SND_PCM_FORMAT_S16_LE
#define MAL_SND_PCM_FORMAT_S16_BE               SND_PCM_FORMAT_S16_BE
#define MAL_SND_PCM_FORMAT_S24_LE               SND_PCM_FORMAT_S24_LE
#define MAL_SND_PCM_FORMAT_S24_BE               SND_PCM_FORMAT_S24_BE
#define MAL_SND_PCM_FORMAT_S32_LE               SND_PCM_FORMAT_S32_LE
#define MAL_SND_PCM_FORMAT_S32_BE               SND_PCM_FORMAT_S32_BE
#define MAL_SND_PCM_FORMAT_FLOAT_LE             SND_PCM_FORMAT_FLOAT_LE
#define MAL_SND_PCM_FORMAT_FLOAT_BE             SND_PCM_FORMAT_FLOAT_BE
#define MAL_SND_PCM_FORMAT_FLOAT64_LE           SND_PCM_FORMAT_FLOAT64_LE
#define MAL_SND_PCM_FORMAT_FLOAT64_BE           SND_PCM_FORMAT_FLOAT64_BE
#define MAL_SND_PCM_FORMAT_MU_LAW               SND_PCM_FORMAT_MU_LAW
#define MAL_SND_PCM_FORMAT_A_LAW                SND_PCM_FORMAT_A_LAW
#define MAL_SND_PCM_FORMAT_S24_3LE              SND_PCM_FORMAT_S24_3LE
#define MAL_SND_PCM_FORMAT_S24_3BE              SND_PCM_FORMAT_S24_3BE

// mal_snd_pcm_access_t
#define MAL_SND_PCM_ACCESS_MMAP_INTERLEAVED     SND_PCM_ACCESS_MMAP_INTERLEAVED
#define MAL_SND_PCM_ACCESS_MMAP_NONINTERLEAVED  SND_PCM_ACCESS_MMAP_NONINTERLEAVED
#define MAL_SND_PCM_ACCESS_MMAP_COMPLEX         SND_PCM_ACCESS_MMAP_COMPLEX
#define MAL_SND_PCM_ACCESS_RW_INTERLEAVED       SND_PCM_ACCESS_RW_INTERLEAVED
#define MAL_SND_PCM_ACCESS_RW_NONINTERLEAVED    SND_PCM_ACCESS_RW_NONINTERLEAVED

// Channel positions.
#define MAL_SND_CHMAP_UNKNOWN                   SND_CHMAP_UNKNOWN
#define MAL_SND_CHMAP_NA                        SND_CHMAP_NA
#define MAL_SND_CHMAP_MONO                      SND_CHMAP_MONO
#define MAL_SND_CHMAP_FL                        SND_CHMAP_FL
#define MAL_SND_CHMAP_FR                        SND_CHMAP_FR
#define MAL_SND_CHMAP_RL                        SND_CHMAP_RL
#define MAL_SND_CHMAP_RR                        SND_CHMAP_RR
#define MAL_SND_CHMAP_FC                        SND_CHMAP_FC
#define MAL_SND_CHMAP_LFE                       SND_CHMAP_LFE
#define MAL_SND_CHMAP_SL                        SND_CHMAP_SL
#define MAL_SND_CHMAP_SR                        SND_CHMAP_SR
#define MAL_SND_CHMAP_RC                        SND_CHMAP_RC
#define MAL_SND_CHMAP_FLC                       SND_CHMAP_FLC
#define MAL_SND_CHMAP_FRC                       SND_CHMAP_FRC
#define MAL_SND_CHMAP_RLC                       SND_CHMAP_RLC
#define MAL_SND_CHMAP_RRC                       SND_CHMAP_RRC
#define MAL_SND_CHMAP_FLW                       SND_CHMAP_FLW
#define MAL_SND_CHMAP_FRW                       SND_CHMAP_FRW
#define MAL_SND_CHMAP_FLH                       SND_CHMAP_FLH
#define MAL_SND_CHMAP_FCH                       SND_CHMAP_FCH
#define MAL_SND_CHMAP_FRH                       SND_CHMAP_FRH
#define MAL_SND_CHMAP_TC                        SND_CHMAP_TC
#define MAL_SND_CHMAP_TFL                       SND_CHMAP_TFL
#define MAL_SND_CHMAP_TFR                       SND_CHMAP_TFR
#define MAL_SND_CHMAP_TFC                       SND_CHMAP_TFC
#define MAL_SND_CHMAP_TRL                       SND_CHMAP_TRL
#define MAL_SND_CHMAP_TRR                       SND_CHMAP_TRR
#define MAL_SND_CHMAP_TRC                       SND_CHMAP_TRC
#define MAL_SND_CHMAP_TFLC                      SND_CHMAP_TFLC
#define MAL_SND_CHMAP_TFRC                      SND_CHMAP_TFRC
#define MAL_SND_CHMAP_TSL                       SND_CHMAP_TSL
#define MAL_SND_CHMAP_TSR                       SND_CHMAP_TSR
#define MAL_SND_CHMAP_LLFE                      SND_CHMAP_LLFE
#define MAL_SND_CHMAP_RLFE                      SND_CHMAP_RLFE
#define MAL_SND_CHMAP_BC                        SND_CHMAP_BC
#define MAL_SND_CHMAP_BLC                       SND_CHMAP_BLC
#define MAL_SND_CHMAP_BRC                       SND_CHMAP_BRC

// Open mode flags.
#define MAL_SND_PCM_NO_AUTO_RESAMPLE            SND_PCM_NO_AUTO_RESAMPLE
#define MAL_SND_PCM_NO_AUTO_CHANNELS            SND_PCM_NO_AUTO_CHANNELS
#define MAL_SND_PCM_NO_AUTO_FORMAT              SND_PCM_NO_AUTO_FORMAT
#else
#include <errno.h>  // For EPIPE, etc.
typedef unsigned long                           mal_snd_pcm_uframes_t;
typedef long                                    mal_snd_pcm_sframes_t;
typedef int                                     mal_snd_pcm_stream_t;
typedef int                                     mal_snd_pcm_format_t;
typedef int                                     mal_snd_pcm_access_t;
typedef struct mal_snd_pcm_t                    mal_snd_pcm_t;
typedef struct mal_snd_pcm_hw_params_t          mal_snd_pcm_hw_params_t;
typedef struct mal_snd_pcm_sw_params_t          mal_snd_pcm_sw_params_t;
typedef struct mal_snd_pcm_format_mask_t        mal_snd_pcm_format_mask_t;
typedef struct mal_snd_pcm_info_t               mal_snd_pcm_info_t;
typedef struct
{
    void* addr;
    unsigned int first;
    unsigned int step;
} mal_snd_pcm_channel_area_t;
typedef struct
{
    unsigned int channels;
    unsigned int pos[0];
} mal_snd_pcm_chmap_t;

// snd_pcm_stream_t
#define MAL_SND_PCM_STREAM_PLAYBACK             0
#define MAL_SND_PCM_STREAM_CAPTURE              1

// snd_pcm_format_t
#define MAL_SND_PCM_FORMAT_UNKNOWN              -1
#define MAL_SND_PCM_FORMAT_U8                   1
#define MAL_SND_PCM_FORMAT_S16_LE               2
#define MAL_SND_PCM_FORMAT_S16_BE               3
#define MAL_SND_PCM_FORMAT_S24_LE               6
#define MAL_SND_PCM_FORMAT_S24_BE               7
#define MAL_SND_PCM_FORMAT_S32_LE               10
#define MAL_SND_PCM_FORMAT_S32_BE               11
#define MAL_SND_PCM_FORMAT_FLOAT_LE             14
#define MAL_SND_PCM_FORMAT_FLOAT_BE             15
#define MAL_SND_PCM_FORMAT_FLOAT64_LE           16
#define MAL_SND_PCM_FORMAT_FLOAT64_BE           17
#define MAL_SND_PCM_FORMAT_MU_LAW               20
#define MAL_SND_PCM_FORMAT_A_LAW                21
#define MAL_SND_PCM_FORMAT_S24_3LE              32
#define MAL_SND_PCM_FORMAT_S24_3BE              33

// snd_pcm_access_t
#define MAL_SND_PCM_ACCESS_MMAP_INTERLEAVED     0
#define MAL_SND_PCM_ACCESS_MMAP_NONINTERLEAVED  1
#define MAL_SND_PCM_ACCESS_MMAP_COMPLEX         2
#define MAL_SND_PCM_ACCESS_RW_INTERLEAVED       3
#define MAL_SND_PCM_ACCESS_RW_NONINTERLEAVED    4

// Channel positions.
#define MAL_SND_CHMAP_UNKNOWN                   0
#define MAL_SND_CHMAP_NA                        1
#define MAL_SND_CHMAP_MONO                      2
#define MAL_SND_CHMAP_FL                        3
#define MAL_SND_CHMAP_FR                        4
#define MAL_SND_CHMAP_RL                        5
#define MAL_SND_CHMAP_RR                        6
#define MAL_SND_CHMAP_FC                        7
#define MAL_SND_CHMAP_LFE                       8
#define MAL_SND_CHMAP_SL                        9
#define MAL_SND_CHMAP_SR                        10
#define MAL_SND_CHMAP_RC                        11
#define MAL_SND_CHMAP_FLC                       12
#define MAL_SND_CHMAP_FRC                       13
#define MAL_SND_CHMAP_RLC                       14
#define MAL_SND_CHMAP_RRC                       15
#define MAL_SND_CHMAP_FLW                       16
#define MAL_SND_CHMAP_FRW                       17
#define MAL_SND_CHMAP_FLH                       18
#define MAL_SND_CHMAP_FCH                       19
#define MAL_SND_CHMAP_FRH                       20
#define MAL_SND_CHMAP_TC                        21
#define MAL_SND_CHMAP_TFL                       22
#define MAL_SND_CHMAP_TFR                       23
#define MAL_SND_CHMAP_TFC                       24
#define MAL_SND_CHMAP_TRL                       25
#define MAL_SND_CHMAP_TRR                       26
#define MAL_SND_CHMAP_TRC                       27
#define MAL_SND_CHMAP_TFLC                      28
#define MAL_SND_CHMAP_TFRC                      29
#define MAL_SND_CHMAP_TSL                       30
#define MAL_SND_CHMAP_TSR                       31
#define MAL_SND_CHMAP_LLFE                      32
#define MAL_SND_CHMAP_RLFE                      33
#define MAL_SND_CHMAP_BC                        34
#define MAL_SND_CHMAP_BLC                       35
#define MAL_SND_CHMAP_BRC                       36

// Open mode flags.
#define MAL_SND_PCM_NO_AUTO_RESAMPLE            0x00010000
#define MAL_SND_PCM_NO_AUTO_CHANNELS            0x00020000
#define MAL_SND_PCM_NO_AUTO_FORMAT              0x00040000
#endif

typedef int                   (* mal_snd_pcm_open_proc)                          (mal_snd_pcm_t **pcm, const char *name, mal_snd_pcm_stream_t stream, int mode);
typedef int                   (* mal_snd_pcm_close_proc)                         (mal_snd_pcm_t *pcm);
typedef size_t                (* mal_snd_pcm_hw_params_sizeof_proc)              (void);
typedef int                   (* mal_snd_pcm_hw_params_any_proc)                 (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params);
typedef int                   (* mal_snd_pcm_hw_params_set_format_proc)          (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, mal_snd_pcm_format_t val);
typedef int                   (* mal_snd_pcm_hw_params_set_format_first_proc)    (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, mal_snd_pcm_format_t *format);
typedef void                  (* mal_snd_pcm_hw_params_get_format_mask_proc)     (mal_snd_pcm_hw_params_t *params, mal_snd_pcm_format_mask_t *mask);
typedef int                   (* mal_snd_pcm_hw_params_set_channels_near_proc)   (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, unsigned int *val);
typedef int                   (* mal_snd_pcm_hw_params_set_rate_resample_proc)   (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, unsigned int val);
typedef int                   (* mal_snd_pcm_hw_params_set_rate_near_proc)       (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, unsigned int *val, int *dir);
typedef int                   (* mal_snd_pcm_hw_params_set_buffer_size_near_proc)(mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, mal_snd_pcm_uframes_t *val);
typedef int                   (* mal_snd_pcm_hw_params_set_periods_near_proc)    (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, unsigned int *val, int *dir);
typedef int                   (* mal_snd_pcm_hw_params_set_access_proc)          (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params, mal_snd_pcm_access_t _access);
typedef int                   (* mal_snd_pcm_hw_params_get_format_proc)          (const mal_snd_pcm_hw_params_t *params, mal_snd_pcm_format_t *format);
typedef int                   (* mal_snd_pcm_hw_params_get_channels_proc)        (const mal_snd_pcm_hw_params_t *params, unsigned int *val);
typedef int                   (* mal_snd_pcm_hw_params_get_rate_proc)            (const mal_snd_pcm_hw_params_t *params, unsigned int *rate, int *dir);
typedef int                   (* mal_snd_pcm_hw_params_get_buffer_size_proc)     (const mal_snd_pcm_hw_params_t *params, mal_snd_pcm_uframes_t *val);
typedef int                   (* mal_snd_pcm_hw_params_get_periods_proc)         (const mal_snd_pcm_hw_params_t *params, unsigned int *val, int *dir);
typedef int                   (* mal_snd_pcm_hw_params_get_access_proc)          (const mal_snd_pcm_hw_params_t *params, mal_snd_pcm_access_t *_access);
typedef int                   (* mal_snd_pcm_hw_params_proc)                     (mal_snd_pcm_t *pcm, mal_snd_pcm_hw_params_t *params);
typedef size_t                (* mal_snd_pcm_sw_params_sizeof_proc)              (void);
typedef int                   (* mal_snd_pcm_sw_params_current_proc)             (mal_snd_pcm_t *pcm, mal_snd_pcm_sw_params_t *params);
typedef int                   (* mal_snd_pcm_sw_params_set_avail_min_proc)       (mal_snd_pcm_t *pcm, mal_snd_pcm_sw_params_t *params, mal_snd_pcm_uframes_t val);
typedef int                   (* mal_snd_pcm_sw_params_set_start_threshold_proc) (mal_snd_pcm_t *pcm, mal_snd_pcm_sw_params_t *params, mal_snd_pcm_uframes_t val);
typedef int                   (* mal_snd_pcm_sw_params_proc)                     (mal_snd_pcm_t *pcm, mal_snd_pcm_sw_params_t *params);
typedef size_t                (* mal_snd_pcm_format_mask_sizeof_proc)            (void);
typedef int                   (* mal_snd_pcm_format_mask_test_proc)              (const mal_snd_pcm_format_mask_t *mask, mal_snd_pcm_format_t val);
typedef mal_snd_pcm_chmap_t * (* mal_snd_pcm_get_chmap_proc)                     (mal_snd_pcm_t *pcm);
typedef int                   (* mal_snd_pcm_prepare_proc)                       (mal_snd_pcm_t *pcm);
typedef int                   (* mal_snd_pcm_start_proc)                         (mal_snd_pcm_t *pcm);
typedef int                   (* mal_snd_pcm_drop_proc)                          (mal_snd_pcm_t *pcm);
typedef int                   (* mal_snd_device_name_hint_proc)                  (int card, const char *iface, void ***hints);
typedef char *                (* mal_snd_device_name_get_hint_proc)              (const void *hint, const char *id);
typedef int                   (* mal_snd_card_get_index_proc)                    (const char *name);
typedef int                   (* mal_snd_device_name_free_hint_proc)             (void **hints);
typedef int                   (* mal_snd_pcm_mmap_begin_proc)                    (mal_snd_pcm_t *pcm, const mal_snd_pcm_channel_area_t **areas, mal_snd_pcm_uframes_t *offset, mal_snd_pcm_uframes_t *frames);
typedef mal_snd_pcm_sframes_t (* mal_snd_pcm_mmap_commit_proc)                   (mal_snd_pcm_t *pcm, mal_snd_pcm_uframes_t offset, mal_snd_pcm_uframes_t frames);
typedef int                   (* mal_snd_pcm_recover_proc)                       (mal_snd_pcm_t *pcm, int err, int silent);
typedef mal_snd_pcm_sframes_t (* mal_snd_pcm_readi_proc)                         (mal_snd_pcm_t *pcm, void *buffer, mal_snd_pcm_uframes_t size);
typedef mal_snd_pcm_sframes_t (* mal_snd_pcm_writei_proc)                        (mal_snd_pcm_t *pcm, const void *buffer, mal_snd_pcm_uframes_t size);
typedef mal_snd_pcm_sframes_t (* mal_snd_pcm_avail_proc)                         (mal_snd_pcm_t *pcm);
typedef mal_snd_pcm_sframes_t (* mal_snd_pcm_avail_update_proc)                  (mal_snd_pcm_t *pcm);
typedef int                   (* mal_snd_pcm_wait_proc)                          (mal_snd_pcm_t *pcm, int timeout);
typedef int                   (* mal_snd_pcm_info_proc)                          (mal_snd_pcm_t *pcm, mal_snd_pcm_info_t* info);
typedef size_t                (* mal_snd_pcm_info_sizeof_proc)                   ();
typedef const char*           (* mal_snd_pcm_info_get_name_proc)                 (const mal_snd_pcm_info_t* info);
typedef int                   (* mal_snd_config_update_free_global_proc)         ();

mal_snd_pcm_format_t g_mal_ALSAFormats[] = {
    MAL_SND_PCM_FORMAT_UNKNOWN,     // mal_format_unknown
    MAL_SND_PCM_FORMAT_U8,          // mal_format_u8
    MAL_SND_PCM_FORMAT_S16_LE,      // mal_format_s16
    MAL_SND_PCM_FORMAT_S24_3LE,     // mal_format_s24
    //MAL_SND_PCM_FORMAT_S24_LE,      // mal_format_s24_32
    MAL_SND_PCM_FORMAT_S32_LE,      // mal_format_s32
    MAL_SND_PCM_FORMAT_FLOAT_LE     // mal_format_f32
};

// This array specifies each of the common devices that can be used for both playback and capture.
const char* g_malCommonDeviceNamesALSA[] = {
    "default",
    "null",
    "pulse",
    "jack"
};

// This array allows us to blacklist specific playback devices.
const char* g_malBlacklistedPlaybackDeviceNamesALSA[] = {
    ""
};

// This array allows us to blacklist specific capture devices.
const char* g_malBlacklistedCaptureDeviceNamesALSA[] = {
    ""
};


// This array allows mini_al to control device-specific default buffer sizes. This uses a scaling factor. Order is important. If
// any part of the string is present in the device's name, the associated scale will be used.
static struct
{
    const char* name;
    float scale;
} g_malDefaultBufferSizeScalesALSA[] = {
    {"bcm2835 IEC958/HDMI", 20},
    {"bcm2835 ALSA",        20}
};

float mal_find_default_buffer_size_scale__alsa(const char* deviceName)
{
    if (deviceName == NULL) {
        return 1;
    }

    for (size_t i = 0; i < mal_countof(g_malDefaultBufferSizeScalesALSA); ++i) {
        if (strstr(g_malDefaultBufferSizeScalesALSA[i].name, deviceName) != NULL) {
            return g_malDefaultBufferSizeScalesALSA[i].scale;
        }
    }

    return 1;
}

mal_snd_pcm_format_t mal_convert_mal_format_to_alsa_format(mal_format format)
{
    return g_mal_ALSAFormats[format];
}

mal_format mal_convert_alsa_format_to_mal_format(mal_snd_pcm_format_t formatALSA)
{
    switch (formatALSA)
    {
        case MAL_SND_PCM_FORMAT_U8:       return mal_format_u8;
        case MAL_SND_PCM_FORMAT_S16_LE:   return mal_format_s16;
        case MAL_SND_PCM_FORMAT_S24_3LE:  return mal_format_s24;
        //MAL_SND_PCM_FORMAT_S24_LE,        return mal_format_s24_32
        case MAL_SND_PCM_FORMAT_S32_LE:   return mal_format_s32;
        case MAL_SND_PCM_FORMAT_FLOAT_LE: return mal_format_f32;
        default:                          return mal_format_unknown;
    }
}

mal_channel mal_convert_alsa_channel_position_to_mal_channel(unsigned int alsaChannelPos)
{
    switch (alsaChannelPos)
    {
        case MAL_SND_CHMAP_MONO: return MAL_CHANNEL_MONO;
        case MAL_SND_CHMAP_FL:   return MAL_CHANNEL_FRONT_LEFT;
        case MAL_SND_CHMAP_FR:   return MAL_CHANNEL_FRONT_RIGHT;
        case MAL_SND_CHMAP_RL:   return MAL_CHANNEL_BACK_LEFT;
        case MAL_SND_CHMAP_RR:   return MAL_CHANNEL_BACK_RIGHT;
        case MAL_SND_CHMAP_FC:   return MAL_CHANNEL_FRONT_CENTER;
        case MAL_SND_CHMAP_LFE:  return MAL_CHANNEL_LFE;
        case MAL_SND_CHMAP_SL:   return MAL_CHANNEL_SIDE_LEFT;
        case MAL_SND_CHMAP_SR:   return MAL_CHANNEL_SIDE_RIGHT;
        case MAL_SND_CHMAP_RC:   return MAL_CHANNEL_BACK_CENTER;
        case MAL_SND_CHMAP_FLC:  return MAL_CHANNEL_FRONT_LEFT_CENTER;
        case MAL_SND_CHMAP_FRC:  return MAL_CHANNEL_FRONT_RIGHT_CENTER;
        case MAL_SND_CHMAP_RLC:  return 0;
        case MAL_SND_CHMAP_RRC:  return 0;
        case MAL_SND_CHMAP_FLW:  return 0;
        case MAL_SND_CHMAP_FRW:  return 0;
        case MAL_SND_CHMAP_FLH:  return 0;
        case MAL_SND_CHMAP_FCH:  return 0;
        case MAL_SND_CHMAP_FRH:  return 0;
        case MAL_SND_CHMAP_TC:   return MAL_CHANNEL_TOP_CENTER;
        case MAL_SND_CHMAP_TFL:  return MAL_CHANNEL_TOP_FRONT_LEFT;
        case MAL_SND_CHMAP_TFR:  return MAL_CHANNEL_TOP_FRONT_RIGHT;
        case MAL_SND_CHMAP_TFC:  return MAL_CHANNEL_TOP_FRONT_CENTER;
        case MAL_SND_CHMAP_TRL:  return MAL_CHANNEL_TOP_BACK_LEFT;
        case MAL_SND_CHMAP_TRR:  return MAL_CHANNEL_TOP_BACK_RIGHT;
        case MAL_SND_CHMAP_TRC:  return MAL_CHANNEL_TOP_BACK_CENTER;
        default: break;
    }

    return 0;
}

mal_bool32 mal_is_common_device_name__alsa(const char* name)
{
    for (size_t iName = 0; iName < mal_countof(g_malCommonDeviceNamesALSA); ++iName) {
        if (mal_strcmp(name, g_malCommonDeviceNamesALSA[iName]) == 0) {
            return MAL_TRUE;
        }
    }

    return MAL_FALSE;
}


mal_bool32 mal_is_playback_device_blacklisted__alsa(const char* name)
{
    for (size_t iName = 0; iName < mal_countof(g_malBlacklistedPlaybackDeviceNamesALSA); ++iName) {
        if (mal_strcmp(name, g_malBlacklistedPlaybackDeviceNamesALSA[iName]) == 0) {
            return MAL_TRUE;
        }
    }

    return MAL_FALSE;
}

mal_bool32 mal_is_capture_device_blacklisted__alsa(const char* name)
{
    for (size_t iName = 0; iName < mal_countof(g_malBlacklistedCaptureDeviceNamesALSA); ++iName) {
        if (mal_strcmp(name, g_malBlacklistedCaptureDeviceNamesALSA[iName]) == 0) {
            return MAL_TRUE;
        }
    }

    return MAL_FALSE;
}

mal_bool32 mal_is_device_blacklisted__alsa(mal_device_type deviceType, const char* name)
{
    if (deviceType == mal_device_type_playback) {
        return mal_is_playback_device_blacklisted__alsa(name);
    } else {
        return mal_is_capture_device_blacklisted__alsa(name);
    }
}


const char* mal_find_char(const char* str, char c, int* index)
{
    int i = 0;
    for (;;) {
        if (str[i] == '\0') {
            if (index) *index = -1;
            return NULL;
        }

        if (str[i] == c) {
            if (index) *index = i;
            return str + i;
        }

        i += 1;
    }

    // Should never get here, but treat it as though the character was not found to make me feel
    // better inside.
    if (index) *index = -1;
    return NULL;
}

mal_bool32 mal_is_device_name_in_hw_format__alsa(const char* hwid)
{
    // This function is just checking whether or not hwid is in "hw:%d,%d" format.

    if (hwid == NULL) {
        return MAL_FALSE;
    }

    if (hwid[0] != 'h' || hwid[1] != 'w' || hwid[2] != ':') {
        return MAL_FALSE;
    }

    hwid += 3;

    int commaPos;
    const char* dev = mal_find_char(hwid, ',', &commaPos);
    if (dev == NULL) {
        return MAL_FALSE;
    } else {
        dev += 1;   // Skip past the ",".
    }

    // Check if the part between the ":" and the "," contains only numbers. If not, return false.
    for (int i = 0; i < commaPos; ++i) {
        if (hwid[i] < '0' || hwid[i] > '9') {
            return MAL_FALSE;
        }
    }

    // Check if everything after the "," is numeric. If not, return false.
    int i = 0;
    while (dev[i] != '\0') {
        if (dev[i] < '0' || dev[i] > '9') {
            return MAL_FALSE;
        }
        i += 1;
    }

    return MAL_TRUE;
}

int mal_convert_device_name_to_hw_format__alsa(mal_context* pContext, char* dst, size_t dstSize, const char* src)  // Returns 0 on success, non-0 on error.
{
    // src should look something like this: "hw:CARD=I82801AAICH,DEV=0"

    if (dst == NULL) return -1;
    if (dstSize < 7) return -1;     // Absolute minimum size of the output buffer is 7 bytes.

    *dst = '\0';    // Safety.
    if (src == NULL) return -1;

    // If the input name is already in "hw:%d,%d" format, just return that verbatim.
    if (mal_is_device_name_in_hw_format__alsa(src)) {
        return mal_strcpy_s(dst, dstSize, src);
    }


    int colonPos;
    src = mal_find_char(src, ':', &colonPos);
    if (src == NULL) {
        return -1;  // Couldn't find a colon
    }

    char card[256];

    int commaPos;
    const char* dev = mal_find_char(src, ',', &commaPos);
    if (dev == NULL) {
        dev = "0";
        mal_strncpy_s(card, sizeof(card), src+6, (size_t)-1);   // +6 = ":CARD="
    } else {
        dev = dev + 5;  // +5 = ",DEV="
        mal_strncpy_s(card, sizeof(card), src+6, commaPos-6);   // +6 = ":CARD="
    }

    int cardIndex = ((mal_snd_card_get_index_proc)pContext->alsa.snd_card_get_index)(card);
    if (cardIndex < 0) {
        return -2;  // Failed to retrieve the card index.
    }

    //printf("TESTING: CARD=%s,DEV=%s\n", card, dev);


    // Construction.
    dst[0] = 'h'; dst[1] = 'w'; dst[2] = ':';
    if (mal_itoa_s(cardIndex, dst+3, dstSize-3, 10) != 0) {
        return -3;
    }
    if (mal_strcat_s(dst, dstSize, ",") != 0) {
        return -3;
    }
    if (mal_strcat_s(dst, dstSize, dev) != 0) {
        return -3;
    }

    return 0;
}

mal_bool32 mal_does_id_exist_in_list__alsa(mal_device_id* pUniqueIDs, mal_uint32 count, const char* pHWID)
{
    mal_assert(pHWID != NULL);

    for (mal_uint32 i = 0; i < count; ++i) {
        if (mal_strcmp(pUniqueIDs[i].alsa, pHWID) == 0) {
            return MAL_TRUE;
        }
    }

    return MAL_FALSE;
}


mal_bool32 mal_context_is_device_id_equal__alsa(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return mal_strcmp(pID0->alsa, pID1->alsa) == 0;
}

mal_result mal_context_enumerate_devices__alsa(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    mal_bool32 cbResult = MAL_TRUE;

    mal_mutex_lock(&pContext->alsa.internalDeviceEnumLock);

    char** ppDeviceHints;
    if (((mal_snd_device_name_hint_proc)pContext->alsa.snd_device_name_hint)(-1, "pcm", (void***)&ppDeviceHints) < 0) {
        mal_mutex_unlock(&pContext->alsa.internalDeviceEnumLock);
        return MAL_NO_BACKEND;
    }

    mal_device_id* pUniqueIDs = NULL;
    mal_uint32 uniqueIDCount = 0;

    char** ppNextDeviceHint = ppDeviceHints;
    while (*ppNextDeviceHint != NULL) {
        char* NAME = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "NAME");
        char* DESC = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "DESC");
        char* IOID = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "IOID");

        mal_device_type deviceType = mal_device_type_playback;
        if ((IOID == NULL || mal_strcmp(IOID, "Output") == 0)) {
            deviceType = mal_device_type_playback;
        }
        if ((IOID != NULL && mal_strcmp(IOID, "Input" ) == 0)) {
            deviceType = mal_device_type_capture;
        }

        mal_bool32 stopEnumeration = MAL_FALSE;
#if 0
        printf("NAME: %s\n", NAME);
        printf("DESC: %s\n", DESC);
        printf("IOID: %s\n", IOID);

        char hwid2[256];
        mal_convert_device_name_to_hw_format__alsa(pContext, hwid2, sizeof(hwid2), NAME);
        printf("DEVICE ID: %s\n\n", hwid2);
#endif

        char hwid[sizeof(pUniqueIDs->alsa)];
        if (NAME != NULL) {
            if (pContext->config.alsa.useVerboseDeviceEnumeration) {
                // Verbose mode. Use the name exactly as-is.
                mal_strncpy_s(hwid, sizeof(hwid), NAME, (size_t)-1);
            } else {
                // Simplified mode. Use ":%d,%d" format.
                if (mal_convert_device_name_to_hw_format__alsa(pContext, hwid, sizeof(hwid), NAME) == 0) {
                    // At this point, hwid looks like "hw:0,0". In simplified enumeration mode, we actually want to strip off the
                    // plugin name so it looks like ":0,0". The reason for this is that this special format is detected at device
                    // initialization time and is used as an indicator to try and use the most appropriate plugin depending on the
                    // device type and sharing mode.
                    char* dst = hwid;
                    char* src = hwid+2;
                    while ((*dst++ = *src++));
                } else {
                    // Conversion to "hw:%d,%d" failed. Just use the name as-is.
                    mal_strncpy_s(hwid, sizeof(hwid), NAME, (size_t)-1);
                }

                if (mal_does_id_exist_in_list__alsa(pUniqueIDs, uniqueIDCount, hwid)) {
                    goto next_device;   // The device has already been enumerated. Move on to the next one.
                } else {
                    // The device has not yet been enumerated. Make sure it's added to our list so that it's not enumerated again.
                    mal_device_id* pNewUniqueIDs = (mal_device_id*)mal_realloc(pUniqueIDs, sizeof(*pUniqueIDs) * (uniqueIDCount + 1));
                    if (pNewUniqueIDs == NULL) {
                        goto next_device;   // Failed to allocate memory.
                    }

                    pUniqueIDs = pNewUniqueIDs;
                    mal_copy_memory(pUniqueIDs[uniqueIDCount].alsa, hwid, sizeof(hwid));
                    uniqueIDCount += 1;
                }
            }
        } else {
            mal_zero_memory(hwid, sizeof(hwid));
        }

        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.id.alsa, sizeof(deviceInfo.id.alsa), hwid, (size_t)-1);

        // DESC is the friendly name. We treat this slightly differently depending on whether or not we are using verbose
        // device enumeration. In verbose mode we want to take the entire description so that the end-user can distinguish
        // between the subdevices of each card/dev pair. In simplified mode, however, we only want the first part of the
        // description.
        //
        // The value in DESC seems to be split into two lines, with the first line being the name of the device and the
        // second line being a description of the device. I don't like having the description be across two lines because
        // it makes formatting ugly and annoying. I'm therefore deciding to put it all on a single line with the second line
        // being put into parentheses. In simplified mode I'm just stripping the second line entirely.
        if (DESC != NULL) {
            int lfPos;
            const char* line2 = mal_find_char(DESC, '\n', &lfPos);
            if (line2 != NULL) {
                line2 += 1; // Skip past the new-line character.

                if (pContext->config.alsa.useVerboseDeviceEnumeration) {
                    // Verbose mode. Put the second line in brackets.
                    mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), DESC, lfPos);
                    mal_strcat_s (deviceInfo.name, sizeof(deviceInfo.name), " (");
                    mal_strcat_s (deviceInfo.name, sizeof(deviceInfo.name), line2);
                    mal_strcat_s (deviceInfo.name, sizeof(deviceInfo.name), ")");
                } else {
                    // Simplified mode. Strip the second line entirely.
                    mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), DESC, lfPos);
                }
            } else {
                // There's no second line. Just copy the whole description.
                mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), DESC, (size_t)-1);
            }
        }

        if (!mal_is_device_blacklisted__alsa(deviceType, NAME)) {
            cbResult = callback(pContext, deviceType, &deviceInfo, pUserData);
        }

        // Some devices are both playback and capture, but they are only enumerated by ALSA once. We need to fire the callback
        // again for the other device type in this case. We do this for known devices.
        if (cbResult) {
            if (mal_is_common_device_name__alsa(NAME)) {
                if (deviceType == mal_device_type_playback) {
                    if (!mal_is_capture_device_blacklisted__alsa(NAME)) {
                        cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                    }
                } else {
                    if (!mal_is_playback_device_blacklisted__alsa(NAME)) {
                        cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                    }
                }
            }
        }

        if (cbResult == MAL_FALSE) {
            stopEnumeration = MAL_TRUE;
        }

    next_device:
        free(NAME);
        free(DESC);
        free(IOID);
        ppNextDeviceHint += 1;

        // We need to stop enumeration if the callback returned false.
        if (stopEnumeration) {
            break;
        }
    }

    mal_free(pUniqueIDs);
    ((mal_snd_device_name_free_hint_proc)pContext->alsa.snd_device_name_free_hint)((void**)ppDeviceHints);

    mal_mutex_unlock(&pContext->alsa.internalDeviceEnumLock);

    return MAL_SUCCESS;
}


typedef struct
{
    mal_device_type deviceType;
    const mal_device_id* pDeviceID;
    mal_share_mode shareMode;
    mal_device_info* pDeviceInfo;
    mal_bool32 foundDevice;
} mal_context_get_device_info_enum_callback_data__alsa;

mal_bool32 mal_context_get_device_info_enum_callback__alsa(mal_context* pContext, mal_device_type deviceType, const mal_device_info* pDeviceInfo, void* pUserData)
{
    mal_context_get_device_info_enum_callback_data__alsa* pData = (mal_context_get_device_info_enum_callback_data__alsa*)pUserData;
    mal_assert(pData != NULL);

    if (pData->pDeviceID == NULL && mal_strcmp(pDeviceInfo->id.alsa, "default") == 0) {
        mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), pDeviceInfo->name, (size_t)-1);
        pData->foundDevice = MAL_TRUE;
    } else {
        if (pData->deviceType == deviceType && mal_context_is_device_id_equal__alsa(pContext, pData->pDeviceID, &pDeviceInfo->id)) {
            mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), pDeviceInfo->name, (size_t)-1);
            pData->foundDevice = MAL_TRUE;
        }
    }

    // Keep enumerating until we have found the device.
    return !pData->foundDevice;
}

mal_result mal_context_get_device_info__alsa(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);

    // We just enumerate to find basic information about the device.
    mal_context_get_device_info_enum_callback_data__alsa data;
    data.deviceType = deviceType;
    data.pDeviceID = pDeviceID;
    data.shareMode = shareMode;
    data.pDeviceInfo = pDeviceInfo;
    data.foundDevice = MAL_FALSE;
    mal_result result = mal_context_enumerate_devices__alsa(pContext, mal_context_get_device_info_enum_callback__alsa, &data);
    if (result != MAL_SUCCESS) {
        return result;
    }

    if (data.foundDevice) {
        return MAL_SUCCESS;
    } else {
        return MAL_NO_DEVICE;
    }
}

mal_result mal_context_init__alsa(mal_context* pContext)
{
    mal_assert(pContext != NULL);

#ifndef MAL_NO_RUNTIME_LINKING
    pContext->alsa.asoundSO = mal_dlopen("libasound.so");
    if (pContext->alsa.asoundSO == NULL) {
        return MAL_NO_BACKEND;
    }

    pContext->alsa.snd_pcm_open                           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_open");
    pContext->alsa.snd_pcm_close                          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_close");
    pContext->alsa.snd_pcm_hw_params_sizeof               = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_sizeof");
    pContext->alsa.snd_pcm_hw_params_any                  = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_any");
    pContext->alsa.snd_pcm_hw_params_set_format           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_format");
    pContext->alsa.snd_pcm_hw_params_set_format_first     = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_format_first");
    pContext->alsa.snd_pcm_hw_params_get_format_mask      = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_format_mask");
    pContext->alsa.snd_pcm_hw_params_set_channels_near    = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_channels_near");
    pContext->alsa.snd_pcm_hw_params_set_rate_resample    = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_rate_resample");
    pContext->alsa.snd_pcm_hw_params_set_rate_near        = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_rate_near");
    pContext->alsa.snd_pcm_hw_params_set_buffer_size_near = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_buffer_size_near");
    pContext->alsa.snd_pcm_hw_params_set_periods_near     = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_periods_near");
    pContext->alsa.snd_pcm_hw_params_set_access           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_set_access");
    pContext->alsa.snd_pcm_hw_params_get_format           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_format");
    pContext->alsa.snd_pcm_hw_params_get_channels         = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_channels");
    pContext->alsa.snd_pcm_hw_params_get_rate             = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_rate");
    pContext->alsa.snd_pcm_hw_params_get_buffer_size      = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_buffer_size");
    pContext->alsa.snd_pcm_hw_params_get_periods          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_periods");
    pContext->alsa.snd_pcm_hw_params_get_access           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params_get_access");
    pContext->alsa.snd_pcm_hw_params                      = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_hw_params");
    pContext->alsa.snd_pcm_sw_params_sizeof               = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_sw_params_sizeof");
    pContext->alsa.snd_pcm_sw_params_current              = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_sw_params_current");
    pContext->alsa.snd_pcm_sw_params_set_avail_min        = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_sw_params_set_avail_min");
    pContext->alsa.snd_pcm_sw_params_set_start_threshold  = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_sw_params_set_start_threshold");
    pContext->alsa.snd_pcm_sw_params                      = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_sw_params");
    pContext->alsa.snd_pcm_format_mask_sizeof             = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_format_mask_sizeof");
    pContext->alsa.snd_pcm_format_mask_test               = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_format_mask_test");
    pContext->alsa.snd_pcm_get_chmap                      = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_get_chmap");
    pContext->alsa.snd_pcm_prepare                        = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_prepare");
    pContext->alsa.snd_pcm_start                          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_start");
    pContext->alsa.snd_pcm_drop                           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_drop");
    pContext->alsa.snd_device_name_hint                   = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_device_name_hint");
    pContext->alsa.snd_device_name_get_hint               = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_device_name_get_hint");
    pContext->alsa.snd_card_get_index                     = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_card_get_index");
    pContext->alsa.snd_device_name_free_hint              = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_device_name_free_hint");
    pContext->alsa.snd_pcm_mmap_begin                     = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_mmap_begin");
    pContext->alsa.snd_pcm_mmap_commit                    = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_mmap_commit");
    pContext->alsa.snd_pcm_recover                        = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_recover");
    pContext->alsa.snd_pcm_readi                          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_readi");
    pContext->alsa.snd_pcm_writei                         = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_writei");
    pContext->alsa.snd_pcm_avail                          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_avail");
    pContext->alsa.snd_pcm_avail_update                   = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_avail_update");
    pContext->alsa.snd_pcm_wait                           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_wait");
    pContext->alsa.snd_pcm_info                           = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_info");
    pContext->alsa.snd_pcm_info_sizeof                    = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_info_sizeof");
    pContext->alsa.snd_pcm_info_get_name                  = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_pcm_info_get_name");
    pContext->alsa.snd_config_update_free_global          = (mal_proc)mal_dlsym(pContext->alsa.asoundSO, "snd_config_update_free_global");
#else
    // The system below is just for type safety.
    mal_snd_pcm_open_proc                           _snd_pcm_open                           = snd_pcm_open;
    mal_snd_pcm_close_proc                          _snd_pcm_close                          = snd_pcm_close;
    mal_snd_pcm_hw_params_sizeof_proc               _snd_pcm_hw_params_sizeof               = snd_pcm_hw_params_sizeof;
    mal_snd_pcm_hw_params_any_proc                  _snd_pcm_hw_params_any                  = snd_pcm_hw_params_any;
    mal_snd_pcm_hw_params_set_format_proc           _snd_pcm_hw_params_set_format           = snd_pcm_hw_params_set_format;
    mal_snd_pcm_hw_params_set_format_first_proc     _snd_pcm_hw_params_set_format_first     = snd_pcm_hw_params_set_format_first;
    mal_snd_pcm_hw_params_get_format_mask_proc      _snd_pcm_hw_params_get_format_mask      = snd_pcm_hw_params_get_format_mask;
    mal_snd_pcm_hw_params_set_channels_near_proc    _snd_pcm_hw_params_set_channels_near    = snd_pcm_hw_params_set_channels_near;
    mal_snd_pcm_hw_params_set_rate_resample_proc    _snd_pcm_hw_params_set_rate_resample    = snd_pcm_hw_params_set_rate_resample;
    mal_snd_pcm_hw_params_set_rate_near_proc        _snd_pcm_hw_params_set_rate_near        = snd_pcm_hw_params_set_rate_near;
    mal_snd_pcm_hw_params_set_buffer_size_near_proc _snd_pcm_hw_params_set_buffer_size_near = snd_pcm_hw_params_set_buffer_size_near;
    mal_snd_pcm_hw_params_set_periods_near_proc     _snd_pcm_hw_params_set_periods_near     = snd_pcm_hw_params_set_periods_near;
    mal_snd_pcm_hw_params_set_access_proc           _snd_pcm_hw_params_set_access           = snd_pcm_hw_params_set_access;
    mal_snd_pcm_hw_params_get_format_proc           _snd_pcm_hw_params_get_format           = snd_pcm_hw_params_get_format;
    mal_snd_pcm_hw_params_get_channels_proc         _snd_pcm_hw_params_get_channels         = snd_pcm_hw_params_get_channels;
    mal_snd_pcm_hw_params_get_rate_proc             _snd_pcm_hw_params_get_rate             = snd_pcm_hw_params_get_rate;
    mal_snd_pcm_hw_params_get_buffer_size_proc      _snd_pcm_hw_params_get_buffer_size      = snd_pcm_hw_params_get_buffer_size;
    mal_snd_pcm_hw_params_get_periods_proc          _snd_pcm_hw_params_get_periods          = snd_pcm_hw_params_get_periods;
    mal_snd_pcm_hw_params_get_access_proc           _snd_pcm_hw_params_get_access           = snd_pcm_hw_params_get_access;
    mal_snd_pcm_hw_params_proc                      _snd_pcm_hw_params                      = snd_pcm_hw_params;
    mal_snd_pcm_sw_params_sizeof_proc               _snd_pcm_sw_params_sizeof               = snd_pcm_sw_params_sizeof;
    mal_snd_pcm_sw_params_current_proc              _snd_pcm_sw_params_current              = snd_pcm_sw_params_current;
    mal_snd_pcm_sw_params_set_avail_min_proc        _snd_pcm_sw_params_set_avail_min        = snd_pcm_sw_params_set_avail_min;
    mal_snd_pcm_sw_params_set_start_threshold_proc  _snd_pcm_sw_params_set_start_threshold  = snd_pcm_sw_params_set_start_threshold;
    mal_snd_pcm_sw_params_proc                      _snd_pcm_sw_params                      = snd_pcm_sw_params;
    mal_snd_pcm_format_mask_sizeof_proc             _snd_pcm_format_mask_sizeof             = snd_pcm_format_mask_sizeof;
    mal_snd_pcm_format_mask_test_proc               _snd_pcm_format_mask_test               = snd_pcm_format_mask_test;
    mal_snd_pcm_get_chmap_proc                      _snd_pcm_get_chmap                      = snd_pcm_get_chmap;
    mal_snd_pcm_prepare_proc                        _snd_pcm_prepare                        = snd_pcm_prepare;
    mal_snd_pcm_start_proc                          _snd_pcm_start                          = snd_pcm_start;
    mal_snd_pcm_drop_proc                           _snd_pcm_drop                           = snd_pcm_drop;
    mal_snd_device_name_hint_proc                   _snd_device_name_hint                   = snd_device_name_hint;
    mal_snd_device_name_get_hint_proc               _snd_device_name_get_hint               = snd_device_name_get_hint;
    mal_snd_card_get_index_proc                     _snd_card_get_index                     = snd_card_get_index;
    mal_snd_device_name_free_hint_proc              _snd_device_name_free_hint              = snd_device_name_free_hint;
    mal_snd_pcm_mmap_begin_proc                     _snd_pcm_mmap_begin                     = snd_pcm_mmap_begin;
    mal_snd_pcm_mmap_commit_proc                    _snd_pcm_mmap_commit                    = snd_pcm_mmap_commit;
    mal_snd_pcm_recover_proc                        _snd_pcm_recover                        = snd_pcm_recover;
    mal_snd_pcm_readi_proc                          _snd_pcm_readi                          = snd_pcm_readi;
    mal_snd_pcm_writei_proc                         _snd_pcm_writei                         = snd_pcm_writei;
    mal_snd_pcm_avail_proc                          _snd_pcm_avail                          = snd_pcm_avail;
    mal_snd_pcm_avail_update_proc                   _snd_pcm_avail_update                   = snd_pcm_avail_update;
    mal_snd_pcm_wait_proc                           _snd_pcm_wait                           = snd_pcm_wait;
    mal_snd_pcm_info_proc                           _snd_pcm_info                           = snd_pcm_info;
    mal_snd_pcm_info_sizeof_proc                    _snd_pcm_info_sizeof                    = snd_pcm_info_sizeof;
    mal_snd_pcm_info_get_name_proc                  _snd_pcm_info_get_name                  = snd_pcm_info_get_name;
    mal_snd_config_update_free_global_proc          _snd_config_update_free_global          = snd_config_update_free_global;

    pContext->alsa.snd_pcm_open                           = (mal_proc)_snd_pcm_open;
    pContext->alsa.snd_pcm_close                          = (mal_proc)_snd_pcm_close;
    pContext->alsa.snd_pcm_hw_params_sizeof               = (mal_proc)_snd_pcm_hw_params_sizeof;
    pContext->alsa.snd_pcm_hw_params_any                  = (mal_proc)_snd_pcm_hw_params_any;
    pContext->alsa.snd_pcm_hw_params_set_format           = (mal_proc)_snd_pcm_hw_params_set_format;
    pContext->alsa.snd_pcm_hw_params_set_format_first     = (mal_proc)_snd_pcm_hw_params_set_format_first;
    pContext->alsa.snd_pcm_hw_params_get_format_mask      = (mal_proc)_snd_pcm_hw_params_get_format_mask;
    pContext->alsa.snd_pcm_hw_params_set_channels_near    = (mal_proc)_snd_pcm_hw_params_set_channels_near;
    pContext->alsa.snd_pcm_hw_params_set_rate_resample    = (mal_proc)_snd_pcm_hw_params_set_rate_resample;
    pContext->alsa.snd_pcm_hw_params_set_rate_near        = (mal_proc)_snd_pcm_hw_params_set_rate_near;
    pContext->alsa.snd_pcm_hw_params_set_buffer_size_near = (mal_proc)_snd_pcm_hw_params_set_buffer_size_near;
    pContext->alsa.snd_pcm_hw_params_set_periods_near     = (mal_proc)_snd_pcm_hw_params_set_periods_near;
    pContext->alsa.snd_pcm_hw_params_set_access           = (mal_proc)_snd_pcm_hw_params_set_access;
    pContext->alsa.snd_pcm_hw_params_get_format           = (mal_proc)_snd_pcm_hw_params_get_format;
    pContext->alsa.snd_pcm_hw_params_get_channels         = (mal_proc)_snd_pcm_hw_params_get_channels;
    pContext->alsa.snd_pcm_hw_params_get_rate             = (mal_proc)_snd_pcm_hw_params_get_rate;
    pContext->alsa.snd_pcm_hw_params_get_buffer_size      = (mal_proc)_snd_pcm_hw_params_get_buffer_size;
    pContext->alsa.snd_pcm_hw_params_get_periods          = (mal_proc)_snd_pcm_hw_params_get_periods;
    pContext->alsa.snd_pcm_hw_params_get_access           = (mal_proc)_snd_pcm_hw_params_get_access;
    pContext->alsa.snd_pcm_hw_params                      = (mal_proc)_snd_pcm_hw_params;
    pContext->alsa.snd_pcm_sw_params_sizeof               = (mal_proc)_snd_pcm_sw_params_sizeof;
    pContext->alsa.snd_pcm_sw_params_current              = (mal_proc)_snd_pcm_sw_params_current;
    pContext->alsa.snd_pcm_sw_params_set_avail_min        = (mal_proc)_snd_pcm_sw_params_set_avail_min;
    pContext->alsa.snd_pcm_sw_params_set_start_threshold  = (mal_proc)_snd_pcm_sw_params_set_start_threshold;
    pContext->alsa.snd_pcm_sw_params                      = (mal_proc)_snd_pcm_sw_params;
    pContext->alsa.snd_pcm_format_mask_sizeof             = (mal_proc)_snd_pcm_format_mask_sizeof;
    pContext->alsa.snd_pcm_format_mask_test               = (mal_proc)_snd_pcm_format_mask_test;
    pContext->alsa.snd_pcm_get_chmap                      = (mal_proc)_snd_pcm_get_chmap;
    pContext->alsa.snd_pcm_prepare                        = (mal_proc)_snd_pcm_prepare;
    pContext->alsa.snd_pcm_start                          = (mal_proc)_snd_pcm_start;
    pContext->alsa.snd_pcm_drop                           = (mal_proc)_snd_pcm_drop;
    pContext->alsa.snd_device_name_hint                   = (mal_proc)_snd_device_name_hint;
    pContext->alsa.snd_device_name_get_hint               = (mal_proc)_snd_device_name_get_hint;
    pContext->alsa.snd_card_get_index                     = (mal_proc)_snd_card_get_index;
    pContext->alsa.snd_device_name_free_hint              = (mal_proc)_snd_device_name_free_hint;
    pContext->alsa.snd_pcm_mmap_begin                     = (mal_proc)_snd_pcm_mmap_begin;
    pContext->alsa.snd_pcm_mmap_commit                    = (mal_proc)_snd_pcm_mmap_commit;
    pContext->alsa.snd_pcm_recover                        = (mal_proc)_snd_pcm_recover;
    pContext->alsa.snd_pcm_readi                          = (mal_proc)_snd_pcm_readi;
    pContext->alsa.snd_pcm_writei                         = (mal_proc)_snd_pcm_writei;
    pContext->alsa.snd_pcm_avail                          = (mal_proc)_snd_pcm_avail;
    pContext->alsa.snd_pcm_avail_update                   = (mal_proc)_snd_pcm_avail_update;
    pContext->alsa.snd_pcm_wait                           = (mal_proc)_snd_pcm_wait;
    pContext->alsa.snd_pcm_info                           = (mal_proc)_snd_pcm_info;
    pContext->alsa.snd_pcm_info_sizeof                    = (mal_proc)_snd_pcm_info_sizeof;
    pContext->alsa.snd_pcm_info_get_name                  = (mal_proc)_snd_pcm_info_get_name;
    pContext->alsa.snd_config_update_free_global          = (mal_proc)_snd_config_update_free_global;
#endif

    if (mal_mutex_init(pContext, &pContext->alsa.internalDeviceEnumLock) != MAL_SUCCESS) {
        mal_context_post_error(pContext, NULL, "[ALSA] WARNING: Failed to initialize mutex for internal device enumeration.", MAL_ERROR);
    }

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__alsa;
    pContext->onEnumDevices   = mal_context_enumerate_devices__alsa;
    pContext->onGetDeviceInfo = mal_context_get_device_info__alsa;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__alsa(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_alsa);

    // Clean up memory for memory leak checkers.
    ((mal_snd_config_update_free_global_proc)pContext->alsa.snd_config_update_free_global)();

#ifndef MAL_NO_RUNTIME_LINKING
    mal_dlclose(pContext->alsa.asoundSO);
#endif

    mal_mutex_uninit(&pContext->alsa.internalDeviceEnumLock);

    return MAL_SUCCESS;
}


// Waits for a number of frames to become available for either capture or playback. The return
// value is the number of frames available.
//
// This will return early if the main loop is broken with mal_device__break_main_loop().
mal_uint32 mal_device__wait_for_frames__alsa(mal_device* pDevice, mal_bool32* pRequiresRestart)
{
    mal_assert(pDevice != NULL);

    if (pRequiresRestart) *pRequiresRestart = MAL_FALSE;

    while (!pDevice->alsa.breakFromMainLoop) {
        int waitResult = ((mal_snd_pcm_wait_proc)pDevice->pContext->alsa.snd_pcm_wait)((mal_snd_pcm_t*)pDevice->alsa.pPCM, -1);
        if (waitResult < 0) {
            if (waitResult == -EPIPE) {
                if (((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, waitResult, MAL_TRUE) < 0) {
                    return 0;
                }

                if (pRequiresRestart) *pRequiresRestart = MAL_TRUE; // A device recovery means a restart for mmap mode.
            }
        }

        if (pDevice->alsa.breakFromMainLoop) {
            return 0;
        }

        mal_snd_pcm_sframes_t framesAvailable = ((mal_snd_pcm_avail_update_proc)pDevice->pContext->alsa.snd_pcm_avail_update)((mal_snd_pcm_t*)pDevice->alsa.pPCM);
        if (framesAvailable < 0) {
            if (framesAvailable == -EPIPE) {
                if (((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, framesAvailable, MAL_TRUE) < 0) {
                    return 0;
                }

                if (pRequiresRestart) *pRequiresRestart = MAL_TRUE; // A device recovery means a restart for mmap mode.

                // Try again, but if it fails this time just return an error.
                framesAvailable = ((mal_snd_pcm_avail_update_proc)pDevice->pContext->alsa.snd_pcm_avail_update)((mal_snd_pcm_t*)pDevice->alsa.pPCM);
                if (framesAvailable < 0) {
                    return 0;
                }
            }
        }

        // Ideally I'd like to keep the number of frames consistent with the period size, but unfortunately it appears
        // this does not work correctly in some situations. In my testing, this breaks when the period size is <= 1024
        // when using "hw:0,0" in a VirtualBox guest. What's happening is that it looks like snd_pcm_writei() (and
        // snd_pcm_mmap_commit() in MMAP mode) are not physically writing the data to the internal buffers. As a result,
        // snd_pcm_wait() is returning immediately, always reporting the full buffer size as available. I'm not sure if
        // this is me not doing something right, or if it's some kind of driver bug, but to fix this we just need to
        // report the exact value returned by snd_pcm_avail_update() and not clamp it to the period size.
#if 1
        return framesAvailable;
#else
        mal_uint32 periodSizeInFrames = pDevice->bufferSizeInFrames / pDevice->periods;
        if (framesAvailable >= periodSizeInFrames) {
            return periodSizeInFrames;
        }
#endif
    }

    // We'll get here if the loop was terminated. Just return whatever's available.
    mal_snd_pcm_sframes_t framesAvailable = ((mal_snd_pcm_avail_update_proc)pDevice->pContext->alsa.snd_pcm_avail_update)((mal_snd_pcm_t*)pDevice->alsa.pPCM);
    if (framesAvailable < 0) {
        return 0;
    }

    return framesAvailable;
}

mal_bool32 mal_device_write__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);
    if (!mal_device_is_started(pDevice) && mal_device__get_state(pDevice) != MAL_STATE_STARTING) {
        return MAL_FALSE;
    }
    if (pDevice->alsa.breakFromMainLoop) {
        return MAL_FALSE;
    }


    if (pDevice->alsa.isUsingMMap) {
        // mmap.
        mal_bool32 requiresRestart;
        mal_uint32 framesAvailable = mal_device__wait_for_frames__alsa(pDevice, &requiresRestart);
        if (framesAvailable == 0) {
            return MAL_FALSE;
        }

        // Don't bother asking the client for more audio data if we're just stopping the device anyway.
        if (pDevice->alsa.breakFromMainLoop) {
            return MAL_FALSE;
        }

        const mal_snd_pcm_channel_area_t* pAreas;
        mal_snd_pcm_uframes_t mappedOffset;
        mal_snd_pcm_uframes_t mappedFrames = framesAvailable;
        while (framesAvailable > 0) {
            int result = ((mal_snd_pcm_mmap_begin_proc)pDevice->pContext->alsa.snd_pcm_mmap_begin)((mal_snd_pcm_t*)pDevice->alsa.pPCM, &pAreas, &mappedOffset, &mappedFrames);
            if (result < 0) {
                return MAL_FALSE;
            }

            if (mappedFrames > 0) {
                void* pBuffer = (mal_uint8*)pAreas[0].addr + ((pAreas[0].first + (mappedOffset * pAreas[0].step)) / 8);
                mal_device__read_frames_from_client(pDevice, mappedFrames, pBuffer);
            }

            result = ((mal_snd_pcm_mmap_commit_proc)pDevice->pContext->alsa.snd_pcm_mmap_commit)((mal_snd_pcm_t*)pDevice->alsa.pPCM, mappedOffset, mappedFrames);
            if (result < 0 || (mal_snd_pcm_uframes_t)result != mappedFrames) {
                ((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, result, MAL_TRUE);
                return MAL_FALSE;
            }

            if (requiresRestart) {
                if (((mal_snd_pcm_start_proc)pDevice->pContext->alsa.snd_pcm_start)((mal_snd_pcm_t*)pDevice->alsa.pPCM) < 0) {
                    return MAL_FALSE;
                }
            }

            framesAvailable -= mappedFrames;
        }
    } else {
        // readi/writei.
        while (!pDevice->alsa.breakFromMainLoop) {
            mal_uint32 framesAvailable = mal_device__wait_for_frames__alsa(pDevice, NULL);
            if (framesAvailable == 0) {
                continue;
            }

            // Don't bother asking the client for more audio data if we're just stopping the device anyway.
            if (pDevice->alsa.breakFromMainLoop) {
                return MAL_FALSE;
            }

            mal_device__read_frames_from_client(pDevice, framesAvailable, pDevice->alsa.pIntermediaryBuffer);

            mal_snd_pcm_sframes_t framesWritten = ((mal_snd_pcm_writei_proc)pDevice->pContext->alsa.snd_pcm_writei)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pDevice->alsa.pIntermediaryBuffer, framesAvailable);
            if (framesWritten < 0) {
                if (framesWritten == -EAGAIN) {
                    continue;   // Just keep trying...
                } else if (framesWritten == -EPIPE) {
                    // Underrun. Just recover and try writing again.
                    if (((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, framesWritten, MAL_TRUE) < 0) {
                        mal_post_error(pDevice, "[ALSA] Failed to recover device after underrun.", MAL_FAILED_TO_START_BACKEND_DEVICE);
                        return MAL_FALSE;
                    }

                    framesWritten = ((mal_snd_pcm_writei_proc)pDevice->pContext->alsa.snd_pcm_writei)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pDevice->alsa.pIntermediaryBuffer, framesAvailable);
                    if (framesWritten < 0) {
                        mal_post_error(pDevice, "[ALSA] Failed to write data to the internal device.", MAL_FAILED_TO_SEND_DATA_TO_DEVICE);
                        return MAL_FALSE;
                    }

                    break;  // Success.
                } else {
                    mal_post_error(pDevice, "[ALSA] snd_pcm_writei() failed when writing initial data.", MAL_FAILED_TO_SEND_DATA_TO_DEVICE);
                    return MAL_FALSE;
                }
            } else {
                break;  // Success.
            }
        }
    }

    return MAL_TRUE;
}

mal_bool32 mal_device_read__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);
    if (!mal_device_is_started(pDevice)) {
        return MAL_FALSE;
    }
    if (pDevice->alsa.breakFromMainLoop) {
        return MAL_FALSE;
    }

    mal_uint32 framesToSend = 0;
    void* pBuffer = NULL;
    if (pDevice->alsa.pIntermediaryBuffer == NULL) {
        // mmap.
        mal_bool32 requiresRestart;
        mal_uint32 framesAvailable = mal_device__wait_for_frames__alsa(pDevice, &requiresRestart);
        if (framesAvailable == 0) {
            return MAL_FALSE;
        }

        const mal_snd_pcm_channel_area_t* pAreas;
        mal_snd_pcm_uframes_t mappedOffset;
        mal_snd_pcm_uframes_t mappedFrames = framesAvailable;
        while (framesAvailable > 0) {
            int result = ((mal_snd_pcm_mmap_begin_proc)pDevice->pContext->alsa.snd_pcm_mmap_begin)((mal_snd_pcm_t*)pDevice->alsa.pPCM, &pAreas, &mappedOffset, &mappedFrames);
            if (result < 0) {
                return MAL_FALSE;
            }

            if (mappedFrames > 0) {
                void* pBuffer = (mal_uint8*)pAreas[0].addr + ((pAreas[0].first + (mappedOffset * pAreas[0].step)) / 8);
                mal_device__send_frames_to_client(pDevice, mappedFrames, pBuffer);
            }

            result = ((mal_snd_pcm_mmap_commit_proc)pDevice->pContext->alsa.snd_pcm_mmap_commit)((mal_snd_pcm_t*)pDevice->alsa.pPCM, mappedOffset, mappedFrames);
            if (result < 0 || (mal_snd_pcm_uframes_t)result != mappedFrames) {
                ((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, result, MAL_TRUE);
                return MAL_FALSE;
            }

            if (requiresRestart) {
                if (((mal_snd_pcm_start_proc)pDevice->pContext->alsa.snd_pcm_start)((mal_snd_pcm_t*)pDevice->alsa.pPCM) < 0) {
                    return MAL_FALSE;
                }
            }

            framesAvailable -= mappedFrames;
        }
    } else {
        // readi/writei.
        mal_snd_pcm_sframes_t framesRead = 0;
        while (!pDevice->alsa.breakFromMainLoop) {
            mal_uint32 framesAvailable = mal_device__wait_for_frames__alsa(pDevice, NULL);
            if (framesAvailable == 0) {
                continue;
            }

            framesRead = ((mal_snd_pcm_readi_proc)pDevice->pContext->alsa.snd_pcm_readi)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pDevice->alsa.pIntermediaryBuffer, framesAvailable);
            if (framesRead < 0) {
                if (framesRead == -EAGAIN) {
                    continue;   // Just keep trying...
                } else if (framesRead == -EPIPE) {
                    // Overrun. Just recover and try reading again.
                    if (((mal_snd_pcm_recover_proc)pDevice->pContext->alsa.snd_pcm_recover)((mal_snd_pcm_t*)pDevice->alsa.pPCM, framesRead, MAL_TRUE) < 0) {
                        mal_post_error(pDevice, "[ALSA] Failed to recover device after overrun.", MAL_FAILED_TO_START_BACKEND_DEVICE);
                        return MAL_FALSE;
                    }

                    framesRead = ((mal_snd_pcm_readi_proc)pDevice->pContext->alsa.snd_pcm_readi)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pDevice->alsa.pIntermediaryBuffer, framesAvailable);
                    if (framesRead < 0) {
                        mal_post_error(pDevice, "[ALSA] Failed to read data from the internal device.", MAL_FAILED_TO_READ_DATA_FROM_DEVICE);
                        return MAL_FALSE;
                    }

                    break;  // Success.
                } else {
                    return MAL_FALSE;
                }
            } else {
                break;  // Success.
            }
        }

        framesToSend = framesRead;
        pBuffer = pDevice->alsa.pIntermediaryBuffer;
    }

    if (framesToSend > 0) {
        mal_device__send_frames_to_client(pDevice, framesToSend, pBuffer);
    }

    return MAL_TRUE;
}

void mal_device_uninit__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if ((mal_snd_pcm_t*)pDevice->alsa.pPCM) {
        ((mal_snd_pcm_close_proc)pDevice->pContext->alsa.snd_pcm_close)((mal_snd_pcm_t*)pDevice->alsa.pPCM);

        if (pDevice->alsa.pIntermediaryBuffer != NULL) {
            mal_free(pDevice->alsa.pIntermediaryBuffer);
        }
    }
}

mal_result mal_device_init__alsa(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->alsa);

    mal_snd_pcm_format_t formatALSA = mal_convert_mal_format_to_alsa_format(pConfig->format);
    mal_snd_pcm_stream_t stream = (type == mal_device_type_playback) ? MAL_SND_PCM_STREAM_PLAYBACK : MAL_SND_PCM_STREAM_CAPTURE;

    int openMode = MAL_SND_PCM_NO_AUTO_RESAMPLE | MAL_SND_PCM_NO_AUTO_CHANNELS | MAL_SND_PCM_NO_AUTO_FORMAT;

    if (pDeviceID == NULL) {
        // We're opening the default device. I don't know if trying anything other than "default" is necessary, but it makes
        // me feel better to try as hard as we can get to get _something_ working.
        const char* defaultDeviceNames[] = {
            "default",
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL
        };

        if (pConfig->shareMode == mal_share_mode_exclusive) {
            defaultDeviceNames[1] = "hw";
            defaultDeviceNames[2] = "hw:0";
            defaultDeviceNames[3] = "hw:0,0";
        } else {
            if (type == mal_device_type_playback) {
                defaultDeviceNames[1] = "dmix";
                defaultDeviceNames[2] = "dmix:0";
                defaultDeviceNames[3] = "dmix:0,0";
            } else {
                defaultDeviceNames[1] = "dsnoop";
                defaultDeviceNames[2] = "dsnoop:0";
                defaultDeviceNames[3] = "dsnoop:0,0";
            }
            defaultDeviceNames[4] = "hw";
            defaultDeviceNames[5] = "hw:0";
            defaultDeviceNames[6] = "hw:0,0";
        }

        mal_bool32 isDeviceOpen = MAL_FALSE;
        for (size_t i = 0; i < mal_countof(defaultDeviceNames); ++i) {
            if (defaultDeviceNames[i] != NULL && defaultDeviceNames[i][0] != '\0') {
                if (((mal_snd_pcm_open_proc)pContext->alsa.snd_pcm_open)((mal_snd_pcm_t**)&pDevice->alsa.pPCM, defaultDeviceNames[i], stream, openMode) == 0) {
                    isDeviceOpen = MAL_TRUE;
                    break;
                }
            }
        }

        if (!isDeviceOpen) {
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] snd_pcm_open() failed when trying to open an appropriate default device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    } else {
        // We're trying to open a specific device. There's a few things to consider here:
        //
        // mini_al recongnizes a special format of device id that excludes the "hw", "dmix", etc. prefix. It looks like this: ":0,0", ":0,1", etc. When
        // an ID of this format is specified, it indicates to mini_al that it can try different combinations of plugins ("hw", "dmix", etc.) until it
        // finds an appropriate one that works. This comes in very handy when trying to open a device in shared mode ("dmix"), vs exclusive mode ("hw").
        mal_bool32 isDeviceOpen = MAL_FALSE;
        if (pDeviceID->alsa[0] != ':') {
            // The ID is not in ":0,0" format. Use the ID exactly as-is.
            if (((mal_snd_pcm_open_proc)pContext->alsa.snd_pcm_open)((mal_snd_pcm_t**)&pDevice->alsa.pPCM, pDeviceID->alsa, stream, openMode) == 0) {
                isDeviceOpen = MAL_TRUE;
            }
        } else {
            // The ID is in ":0,0" format. Try different plugins depending on the shared mode.
            if (pDeviceID->alsa[1] == '\0') {
                pDeviceID->alsa[0] = '\0';  // An ID of ":" should be converted to "".
            }

            char hwid[256];
            if (pConfig->shareMode == mal_share_mode_shared) {
                if (type == mal_device_type_playback) {
                    mal_strcpy_s(hwid, sizeof(hwid), "dmix");
                } else {
                    mal_strcpy_s(hwid, sizeof(hwid), "dsnoop");
                }

                if (mal_strcat_s(hwid, sizeof(hwid), pDeviceID->alsa) == 0) {
                    if (((mal_snd_pcm_open_proc)pContext->alsa.snd_pcm_open)((mal_snd_pcm_t**)&pDevice->alsa.pPCM, hwid, stream, openMode) == 0) {
                        isDeviceOpen = MAL_TRUE;
                    }
                }
            }

            // If at this point we still don't have an open device it means we're either preferencing exclusive mode or opening with "dmix"/"dsnoop" failed.
            if (!isDeviceOpen) {
                mal_strcpy_s(hwid, sizeof(hwid), "hw");
                if (mal_strcat_s(hwid, sizeof(hwid), pDeviceID->alsa) == 0) {
                    if (((mal_snd_pcm_open_proc)pContext->alsa.snd_pcm_open)((mal_snd_pcm_t**)&pDevice->alsa.pPCM, hwid, stream, openMode) == 0) {
                        isDeviceOpen = MAL_TRUE;
                    }
                }
            }
        }

        if (!isDeviceOpen) {
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] snd_pcm_open() failed.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    }

    // We may need to scale the size of the buffer depending on the device.
    if (pDevice->usingDefaultBufferSize) {
        mal_snd_pcm_info_t* pInfo = (mal_snd_pcm_info_t*)alloca(((mal_snd_pcm_info_sizeof_proc)pContext->alsa.snd_pcm_info_sizeof)());
        mal_zero_memory(pInfo, ((mal_snd_pcm_info_sizeof_proc)pContext->alsa.snd_pcm_info_sizeof)());

        if (((mal_snd_pcm_info_proc)pContext->alsa.snd_pcm_info)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pInfo) == 0) {
            float bufferSizeScale = 1;

            const char* deviceName = ((mal_snd_pcm_info_get_name_proc)pContext->alsa.snd_pcm_info_get_name)(pInfo);
            if (deviceName != NULL) {
                if (mal_strcmp(deviceName, "default") == 0) {
                    // It's the default device. We need to use DESC from snd_device_name_hint().
                    char** ppDeviceHints;
                    if (((mal_snd_device_name_hint_proc)pContext->alsa.snd_device_name_hint)(-1, "pcm", (void***)&ppDeviceHints) < 0) {
                        return MAL_NO_BACKEND;
                    }

                    char** ppNextDeviceHint = ppDeviceHints;
                    while (*ppNextDeviceHint != NULL) {
                        char* NAME = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "NAME");
                        char* DESC = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "DESC");
                        char* IOID = ((mal_snd_device_name_get_hint_proc)pContext->alsa.snd_device_name_get_hint)(*ppNextDeviceHint, "IOID");

                        mal_bool32 foundDevice = MAL_FALSE;
                        if ((type == mal_device_type_playback && (IOID == NULL || mal_strcmp(IOID, "Output") == 0)) ||
                            (type == mal_device_type_capture  && (IOID != NULL && mal_strcmp(IOID, "Input" ) == 0))) {
                            if (mal_strcmp(NAME, deviceName) == 0) {
                                bufferSizeScale = mal_find_default_buffer_size_scale__alsa(DESC);
                                foundDevice = MAL_TRUE;
                            }
                        }

                        free(NAME);
                        free(DESC);
                        free(IOID);
                        ppNextDeviceHint += 1;

                        if (foundDevice) {
                            break;
                        }
                    }

                    ((mal_snd_device_name_free_hint_proc)pContext->alsa.snd_device_name_free_hint)((void**)ppDeviceHints);
                } else {
                    bufferSizeScale = mal_find_default_buffer_size_scale__alsa(deviceName);
                }
            }

            pDevice->bufferSizeInFrames = (mal_uint32)(pDevice->bufferSizeInFrames * bufferSizeScale);
        }
    }


    // Hardware parameters.
    mal_snd_pcm_hw_params_t* pHWParams = (mal_snd_pcm_hw_params_t*)alloca(((mal_snd_pcm_hw_params_sizeof_proc)pContext->alsa.snd_pcm_hw_params_sizeof)());
    mal_zero_memory(pHWParams, ((mal_snd_pcm_hw_params_sizeof_proc)pContext->alsa.snd_pcm_hw_params_sizeof)());

    if (((mal_snd_pcm_hw_params_any_proc)pContext->alsa.snd_pcm_hw_params_any)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to initialize hardware parameters. snd_pcm_hw_params_any() failed.", MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE);
    }


    // MMAP Mode
    //
    // Try using interleaved MMAP access. If this fails, fall back to standard readi/writei.
    pDevice->alsa.isUsingMMap = MAL_FALSE;
    if (!pConfig->alsa.noMMap && pDevice->type != mal_device_type_capture) {    // <-- Disabling MMAP mode for capture devices because I apparently do not have a device that supports it so I can test it... Contributions welcome.
        if (((mal_snd_pcm_hw_params_set_access_proc)pContext->alsa.snd_pcm_hw_params_set_access)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, MAL_SND_PCM_ACCESS_MMAP_INTERLEAVED) == 0) {
            pDevice->alsa.isUsingMMap = MAL_TRUE;
        }
    }

    if (!pDevice->alsa.isUsingMMap) {
        if (((mal_snd_pcm_hw_params_set_access_proc)pContext->alsa.snd_pcm_hw_params_set_access)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, MAL_SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {;
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] Failed to set access mode to neither SND_PCM_ACCESS_MMAP_INTERLEAVED nor SND_PCM_ACCESS_RW_INTERLEAVED. snd_pcm_hw_params_set_access() failed.", MAL_FORMAT_NOT_SUPPORTED);
        }
    }


    // Most important properties first. The documentation for OSS (yes, I know this is ALSA!) recommends format, channels, then sample rate. I can't
    // find any documentation for ALSA specifically, so I'm going to copy the recommendation for OSS.

    // Format.
    // Try getting every supported format.
    mal_snd_pcm_format_mask_t* pFormatMask = (mal_snd_pcm_format_mask_t*)alloca(((mal_snd_pcm_format_mask_sizeof_proc)pContext->alsa.snd_pcm_format_mask_sizeof)());
    mal_zero_memory(pFormatMask, ((mal_snd_pcm_format_mask_sizeof_proc)pContext->alsa.snd_pcm_format_mask_sizeof)());

    ((mal_snd_pcm_hw_params_get_format_mask_proc)pContext->alsa.snd_pcm_hw_params_get_format_mask)(pHWParams, pFormatMask);

    // At this point we should have a list of supported formats, so now we need to find the best one. We first check if the requested format is
    // supported, and if so, use that one. If it's not supported, we just run though a list of formats and try to find the best one.
    if (!((mal_snd_pcm_format_mask_test_proc)pContext->alsa.snd_pcm_format_mask_test)(pFormatMask, formatALSA)) {
        // The requested format is not supported so now try running through the list of formats and return the best one.
        mal_snd_pcm_format_t preferredFormatsALSA[] = {
            MAL_SND_PCM_FORMAT_FLOAT_LE,    // mal_format_f32
            MAL_SND_PCM_FORMAT_S32_LE,      // mal_format_s32
            MAL_SND_PCM_FORMAT_S24_3LE,     // mal_format_s24
            //MAL_SND_PCM_FORMAT_S24_LE,      // mal_format_s24_32
            MAL_SND_PCM_FORMAT_S16_LE,      // mal_format_s16
            MAL_SND_PCM_FORMAT_U8           // mal_format_u8
        };

        formatALSA = MAL_SND_PCM_FORMAT_UNKNOWN;
        for (size_t i = 0; i < (sizeof(preferredFormatsALSA) / sizeof(preferredFormatsALSA[0])); ++i) {
            if (((mal_snd_pcm_format_mask_test_proc)pContext->alsa.snd_pcm_format_mask_test)(pFormatMask, preferredFormatsALSA[i])) {
                formatALSA = preferredFormatsALSA[i];
                break;
            }
        }

        if (formatALSA == MAL_SND_PCM_FORMAT_UNKNOWN) {
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] Format not supported. The device does not support any mini_al formats.", MAL_FORMAT_NOT_SUPPORTED);
        }
    }

    if (((mal_snd_pcm_hw_params_set_format_proc)pContext->alsa.snd_pcm_hw_params_set_format)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, formatALSA) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Format not supported. snd_pcm_hw_params_set_format() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }

    pDevice->internalFormat = mal_convert_alsa_format_to_mal_format(formatALSA);
    if (pDevice->internalFormat == mal_format_unknown) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] The chosen format is not supported by mini_al.", MAL_FORMAT_NOT_SUPPORTED);
    }


    // Channels.
    unsigned int channels = pConfig->channels;
    if (((mal_snd_pcm_hw_params_set_channels_near_proc)pContext->alsa.snd_pcm_hw_params_set_channels_near)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, &channels) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to set channel count. snd_pcm_hw_params_set_channels_near() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }
    pDevice->internalChannels = (mal_uint32)channels;


    // Sample Rate. It appears there's either a bug in ALSA, a bug in some drivers, or I'm doing something silly; but having resampling
    // enabled causes problems with some device configurations when used in conjunction with MMAP access mode. To fix this problem we
    // need to disable resampling.
    //
    // To reproduce this problem, open the "plug:dmix" device, and set the sample rate to 44100. Internally, it looks like dmix uses a
    // sample rate of 48000. The hardware parameters will get set correctly with no errors, but it looks like the 44100 -> 48000 resampling
    // doesn't work properly - but only with MMAP access mode. You will notice skipping/crackling in the audio, and it'll run at a slightly
    // faster rate.
    //
    // mini_al has built-in support for sample rate conversion (albeit low quality at the moment), so disabling resampling should be fine
    // for us. The only problem is that it won't be taking advantage of any kind of hardware-accelerated resampling and it won't be very
    // good quality until I get a chance to improve the quality of mini_al's software sample rate conversion.
    //
    // I don't currently know if the dmix plugin is the only one with this error. Indeed, this is the only one I've been able to reproduce
    // this error with. In the future, we may want to restrict the disabling of resampling to only known bad plugins.
    ((mal_snd_pcm_hw_params_set_rate_resample_proc)pContext->alsa.snd_pcm_hw_params_set_rate_resample)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, 0);

    unsigned int sampleRate = pConfig->sampleRate;
    if (((mal_snd_pcm_hw_params_set_rate_near_proc)pContext->alsa.snd_pcm_hw_params_set_rate_near)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, &sampleRate, 0) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Sample rate not supported. snd_pcm_hw_params_set_rate_near() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }
    pDevice->internalSampleRate = (mal_uint32)sampleRate;


    // Periods.
    mal_uint32 periods = pConfig->periods;
    int dir = 0;
    if (((mal_snd_pcm_hw_params_set_periods_near_proc)pContext->alsa.snd_pcm_hw_params_set_periods_near)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, &periods, &dir) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to set period count. snd_pcm_hw_params_set_periods_near() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }
    pDevice->periods = periods;

    // Buffer Size
    mal_snd_pcm_uframes_t actualBufferSize = pDevice->bufferSizeInFrames;
    if (((mal_snd_pcm_hw_params_set_buffer_size_near_proc)pContext->alsa.snd_pcm_hw_params_set_buffer_size_near)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams, &actualBufferSize) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to set buffer size for device. snd_pcm_hw_params_set_buffer_size() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }
    pDevice->bufferSizeInFrames = actualBufferSize;


    // Apply hardware parameters.
    if (((mal_snd_pcm_hw_params_proc)pContext->alsa.snd_pcm_hw_params)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pHWParams) < 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to set hardware parameters. snd_pcm_hw_params() failed.", MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE);
    }



    // Software parameters.
    mal_snd_pcm_sw_params_t* pSWParams = (mal_snd_pcm_sw_params_t*)alloca(((mal_snd_pcm_sw_params_sizeof_proc)pContext->alsa.snd_pcm_sw_params_sizeof)());
    mal_zero_memory(pSWParams, ((mal_snd_pcm_sw_params_sizeof_proc)pContext->alsa.snd_pcm_sw_params_sizeof)());

    if (((mal_snd_pcm_sw_params_current_proc)pContext->alsa.snd_pcm_sw_params_current)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pSWParams) != 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to initialize software parameters. snd_pcm_sw_params_current() failed.", MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE);
    }

    if (((mal_snd_pcm_sw_params_set_avail_min_proc)pContext->alsa.snd_pcm_sw_params_set_avail_min)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pSWParams, /*(pDevice->sampleRate/1000) * 1*/ mal_prev_power_of_2(pDevice->bufferSizeInFrames/pDevice->periods)) != 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] snd_pcm_sw_params_set_avail_min() failed.", MAL_FORMAT_NOT_SUPPORTED);
    }

    if (type == mal_device_type_playback && !pDevice->alsa.isUsingMMap) {   // Only playback devices in writei/readi mode need a start threshold.
        if (((mal_snd_pcm_sw_params_set_start_threshold_proc)pContext->alsa.snd_pcm_sw_params_set_start_threshold)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pSWParams, /*(pDevice->sampleRate/1000) * 1*/ pDevice->bufferSizeInFrames/pDevice->periods) != 0) { //mal_prev_power_of_2(pDevice->bufferSizeInFrames/pDevice->periods)
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] Failed to set start threshold for playback device. snd_pcm_sw_params_set_start_threshold() failed.", MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE);
        }
    }

    if (((mal_snd_pcm_sw_params_proc)pContext->alsa.snd_pcm_sw_params)((mal_snd_pcm_t*)pDevice->alsa.pPCM, pSWParams) != 0) {
        mal_device_uninit__alsa(pDevice);
        return mal_post_error(pDevice, "[ALSA] Failed to set software parameters. snd_pcm_sw_params() failed.", MAL_FAILED_TO_CONFIGURE_BACKEND_DEVICE);
    }



    // If we're _not_ using mmap we need to use an intermediary buffer.
    if (!pDevice->alsa.isUsingMMap) {
        pDevice->alsa.pIntermediaryBuffer = mal_malloc(pDevice->bufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat));
        if (pDevice->alsa.pIntermediaryBuffer == NULL) {
            mal_device_uninit__alsa(pDevice);
            return mal_post_error(pDevice, "[ALSA] Failed to allocate memory for intermediary buffer.", MAL_OUT_OF_MEMORY);
        }
    }

    // Grab the internal channel map. For now we're not going to bother trying to change the channel map and
    // instead just do it ourselves.
    mal_snd_pcm_chmap_t* pChmap = ((mal_snd_pcm_get_chmap_proc)pContext->alsa.snd_pcm_get_chmap)((mal_snd_pcm_t*)pDevice->alsa.pPCM);
    if (pChmap != NULL) {
        // There are cases where the returned channel map can have a different channel count than was returned by snd_pcm_hw_params_set_channels_near().
        if (pChmap->channels >= pDevice->internalChannels) {
            // Drop excess channels.
            for (mal_uint32 iChannel = 0; iChannel < pDevice->internalChannels; ++iChannel) {
                pDevice->internalChannelMap[iChannel] = mal_convert_alsa_channel_position_to_mal_channel(pChmap->pos[iChannel]);
            }
        } else {
            // Excess channels use defaults. Do an initial fill with defaults, overwrite the first pChmap->channels, validate to ensure there are no duplicate
            // channels. If validation fails, fall back to defaults.

            // Fill with defaults.
            mal_get_standard_channel_map(mal_standard_channel_map_alsa, pDevice->internalChannels, pDevice->internalChannelMap);

            // Overwrite first pChmap->channels channels.
            for (mal_uint32 iChannel = 0; iChannel < pChmap->channels; ++iChannel) {
                pDevice->internalChannelMap[iChannel] = mal_convert_alsa_channel_position_to_mal_channel(pChmap->pos[iChannel]);
            }

            // Validate.
            mal_bool32 isValid = MAL_TRUE;
            for (mal_uint32 i = 0; i < pDevice->internalChannels && isValid; ++i) {
                for (mal_uint32 j = i+1; j < pDevice->internalChannels; ++j) {
                    if (pDevice->internalChannelMap[i] == pDevice->internalChannelMap[j]) {
                        isValid = MAL_FALSE;
                        break;
                    }
                }
            }

            // If our channel map is invalid, fall back to defaults.
            if (!isValid) {
                mal_get_standard_channel_map(mal_standard_channel_map_alsa, pDevice->internalChannels, pDevice->internalChannelMap);
            }
        }

        free(pChmap);
        pChmap = NULL;
    } else {
        // Could not retrieve the channel map. Fall back to a hard-coded assumption.
        mal_get_standard_channel_map(mal_standard_channel_map_alsa, pDevice->internalChannels, pDevice->internalChannelMap);
    }

    return MAL_SUCCESS;
}


mal_result mal_device__start_backend__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // Prepare the device first...
    if (((mal_snd_pcm_prepare_proc)pDevice->pContext->alsa.snd_pcm_prepare)((mal_snd_pcm_t*)pDevice->alsa.pPCM) < 0) {
        return mal_post_error(pDevice, "[ALSA] Failed to prepare device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
    }

    // ... and then grab an initial chunk from the client. After this is done, the device should
    // automatically start playing, since that's how we configured the software parameters.
    if (pDevice->type == mal_device_type_playback) {
        if (!mal_device_write__alsa(pDevice)) {
            return mal_post_error(pDevice, "[ALSA] Failed to write initial chunk of data to the playback device.", MAL_FAILED_TO_SEND_DATA_TO_DEVICE);
        }

        // mmap mode requires an explicit start.
        if (pDevice->alsa.isUsingMMap) {
            if (((mal_snd_pcm_start_proc)pDevice->pContext->alsa.snd_pcm_start)((mal_snd_pcm_t*)pDevice->alsa.pPCM) < 0) {
                return mal_post_error(pDevice, "[ALSA] Failed to start capture device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        }
    } else {
        if (((mal_snd_pcm_start_proc)pDevice->pContext->alsa.snd_pcm_start)((mal_snd_pcm_t*)pDevice->alsa.pPCM) < 0) {
            return mal_post_error(pDevice, "[ALSA] Failed to start capture device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    ((mal_snd_pcm_drop_proc)pDevice->pContext->alsa.snd_pcm_drop)((mal_snd_pcm_t*)pDevice->alsa.pPCM);
    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // First we tell the main loop that we're breaking...
    pDevice->alsa.breakFromMainLoop = MAL_TRUE;

    // Then we need to force snd_pcm_wait() to return.
    //((mal_snd_pcm_drop_proc)pDevice->pContext->alsa.snd_pcm_drop)((mal_snd_pcm_t*)pDevice->alsa.pPCM);

    return MAL_SUCCESS;
}

mal_result mal_device__main_loop__alsa(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->alsa.breakFromMainLoop = MAL_FALSE;
    if (pDevice->type == mal_device_type_playback) {
        // Playback. Read from client, write to device.
        while (!pDevice->alsa.breakFromMainLoop && mal_device_write__alsa(pDevice)) {
        }
    } else {
        // Capture. Read from device, write to client.
        while (!pDevice->alsa.breakFromMainLoop && mal_device_read__alsa(pDevice)) {
        }
    }

    return MAL_SUCCESS;
}
#endif  // ALSA



///////////////////////////////////////////////////////////////////////////////
//
// PulseAudio Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_PULSEAUDIO

// It is assumed pulseaudio.h is available when compile-time linking is being used. We use this for type safety when using
// compile time linking (we don't have this luxury when using runtime linking without headers).
//
// When using compile time linking, each of our mal_* equivalents should use the sames types as defined by the header. The
// reason for this is that it allow us to take advantage of proper type safety.
#ifdef MAL_NO_RUNTIME_LINKING
#include <pulse/pulseaudio.h>

#define MAL_PA_OK                                       PA_OK
#define MAL_PA_ERR_ACCESS                               PA_ERR_ACCESS
#define MAL_PA_ERR_INVALID                              PA_ERR_INVALID
#define MAL_PA_ERR_NOENTITY                             PA_ERR_NOENTITY

#define MAL_PA_CHANNELS_MAX                             PA_CHANNELS_MAX
#define MAL_PA_RATE_MAX                                 PA_RATE_MAX

typedef pa_context_flags_t mal_pa_context_flags_t;
#define MAL_PA_CONTEXT_NOFLAGS                          PA_CONTEXT_NOFLAGS
#define MAL_PA_CONTEXT_NOAUTOSPAWN                      PA_CONTEXT_NOAUTOSPAWN
#define MAL_PA_CONTEXT_NOFAIL                           PA_CONTEXT_NOFAIL

typedef pa_stream_flags_t mal_pa_stream_flags_t;
#define MAL_PA_STREAM_NOFLAGS                           PA_STREAM_NOFLAGS
#define MAL_PA_STREAM_START_CORKED                      PA_STREAM_START_CORKED
#define MAL_PA_STREAM_INTERPOLATE_TIMING                PA_STREAM_INTERPOLATE_TIMING
#define MAL_PA_STREAM_NOT_MONOTONIC                     PA_STREAM_NOT_MONOTONIC
#define MAL_PA_STREAM_AUTO_TIMING_UPDATE                PA_STREAM_AUTO_TIMING_UPDATE
#define MAL_PA_STREAM_NO_REMAP_CHANNELS                 PA_STREAM_NO_REMAP_CHANNELS
#define MAL_PA_STREAM_NO_REMIX_CHANNELS                 PA_STREAM_NO_REMIX_CHANNELS
#define MAL_PA_STREAM_FIX_FORMAT                        PA_STREAM_FIX_FORMAT
#define MAL_PA_STREAM_FIX_RATE                          PA_STREAM_FIX_RATE
#define MAL_PA_STREAM_FIX_CHANNELS                      PA_STREAM_FIX_CHANNELS
#define MAL_PA_STREAM_DONT_MOVE                         PA_STREAM_DONT_MOVE
#define MAL_PA_STREAM_VARIABLE_RATE                     PA_STREAM_VARIABLE_RATE
#define MAL_PA_STREAM_PEAK_DETECT                       PA_STREAM_PEAK_DETECT
#define MAL_PA_STREAM_START_MUTED                       PA_STREAM_START_MUTED
#define MAL_PA_STREAM_ADJUST_LATENCY                    PA_STREAM_ADJUST_LATENCY
#define MAL_PA_STREAM_EARLY_REQUESTS                    PA_STREAM_EARLY_REQUESTS
#define MAL_PA_STREAM_DONT_INHIBIT_AUTO_SUSPEND         PA_STREAM_DONT_INHIBIT_AUTO_SUSPEND
#define MAL_PA_STREAM_START_UNMUTED                     PA_STREAM_START_UNMUTED
#define MAL_PA_STREAM_FAIL_ON_SUSPEND                   PA_STREAM_FAIL_ON_SUSPEND
#define MAL_PA_STREAM_RELATIVE_VOLUME                   PA_STREAM_RELATIVE_VOLUME
#define MAL_PA_STREAM_PASSTHROUGH                       PA_STREAM_PASSTHROUGH

typedef pa_sink_flags_t mal_pa_sink_flags_t;
#define MAL_PA_SINK_NOFLAGS                             PA_SINK_NOFLAGS
#define MAL_PA_SINK_HW_VOLUME_CTRL                      PA_SINK_HW_VOLUME_CTRL
#define MAL_PA_SINK_LATENCY                             PA_SINK_LATENCY
#define MAL_PA_SINK_HARDWARE                            PA_SINK_HARDWARE
#define MAL_PA_SINK_NETWORK                             PA_SINK_NETWORK
#define MAL_PA_SINK_HW_MUTE_CTRL                        PA_SINK_HW_MUTE_CTRL
#define MAL_PA_SINK_DECIBEL_VOLUME                      PA_SINK_DECIBEL_VOLUME
#define MAL_PA_SINK_FLAT_VOLUME                         PA_SINK_FLAT_VOLUME
#define MAL_PA_SINK_DYNAMIC_LATENCY                     PA_SINK_DYNAMIC_LATENCY
#define MAL_PA_SINK_SET_FORMATS                         PA_SINK_SET_FORMATS

typedef pa_source_flags_t mal_pa_source_flags_t;
#define MAL_PA_SOURCE_NOFLAGS                           PA_SOURCE_NOFLAGS
#define MAL_PA_SOURCE_HW_VOLUME_CTRL                    PA_SOURCE_HW_VOLUME_CTRL
#define MAL_PA_SOURCE_LATENCY                           PA_SOURCE_LATENCY
#define MAL_PA_SOURCE_HARDWARE                          PA_SOURCE_HARDWARE
#define MAL_PA_SOURCE_NETWORK                           PA_SOURCE_NETWORK
#define MAL_PA_SOURCE_HW_MUTE_CTRL                      PA_SOURCE_HW_MUTE_CTRL
#define MAL_PA_SOURCE_DECIBEL_VOLUME                    PA_SOURCE_DECIBEL_VOLUME
#define MAL_PA_SOURCE_DYNAMIC_LATENCY                   PA_SOURCE_DYNAMIC_LATENCY
#define MAL_PA_SOURCE_FLAT_VOLUME                       PA_SOURCE_FLAT_VOLUME

typedef pa_context_state_t mal_pa_context_state_t;
#define MAL_PA_CONTEXT_UNCONNECTED                      PA_CONTEXT_UNCONNECTED
#define MAL_PA_CONTEXT_CONNECTING                       PA_CONTEXT_CONNECTING
#define MAL_PA_CONTEXT_AUTHORIZING                      PA_CONTEXT_AUTHORIZING
#define MAL_PA_CONTEXT_SETTING_NAME                     PA_CONTEXT_SETTING_NAME
#define MAL_PA_CONTEXT_READY                            PA_CONTEXT_READY
#define MAL_PA_CONTEXT_FAILED                           PA_CONTEXT_FAILED
#define MAL_PA_CONTEXT_TERMINATED                       PA_CONTEXT_TERMINATED

typedef pa_stream_state_t mal_pa_stream_state_t;
#define MAL_PA_STREAM_UNCONNECTED                       PA_STREAM_UNCONNECTED
#define MAL_PA_STREAM_CREATING                          PA_STREAM_CREATING
#define MAL_PA_STREAM_READY                             PA_STREAM_READY
#define MAL_PA_STREAM_FAILED                            PA_STREAM_FAILED
#define MAL_PA_STREAM_TERMINATED                        PA_STREAM_TERMINATED

typedef pa_operation_state_t mal_pa_operation_state_t;
#define MAL_PA_OPERATION_RUNNING                        PA_OPERATION_RUNNING
#define MAL_PA_OPERATION_DONE                           PA_OPERATION_DONE
#define MAL_PA_OPERATION_CANCELLED                      PA_OPERATION_CANCELLED

typedef pa_sink_state_t mal_pa_sink_state_t;
#define MAL_PA_SINK_INVALID_STATE                       PA_SINK_INVALID_STATE
#define MAL_PA_SINK_RUNNING                             PA_SINK_RUNNING
#define MAL_PA_SINK_IDLE                                PA_SINK_IDLE
#define MAL_PA_SINK_SUSPENDED                           PA_SINK_SUSPENDED

typedef pa_source_state_t mal_pa_source_state_t;
#define MAL_PA_SOURCE_INVALID_STATE                     PA_SOURCE_INVALID_STATE
#define MAL_PA_SOURCE_RUNNING                           PA_SOURCE_RUNNING
#define MAL_PA_SOURCE_IDLE                              PA_SOURCE_IDLE
#define MAL_PA_SOURCE_SUSPENDED                         PA_SOURCE_SUSPENDED

typedef pa_seek_mode_t mal_pa_seek_mode_t;
#define MAL_PA_SEEK_RELATIVE                            PA_SEEK_RELATIVE
#define MAL_PA_SEEK_ABSOLUTE                            PA_SEEK_ABSOLUTE
#define MAL_PA_SEEK_RELATIVE_ON_READ                    PA_SEEK_RELATIVE_ON_READ
#define MAL_PA_SEEK_RELATIVE_END                        PA_SEEK_RELATIVE_END

typedef pa_channel_position_t mal_pa_channel_position_t;
#define MAL_PA_CHANNEL_POSITION_INVALID                 PA_CHANNEL_POSITION_INVALID
#define MAL_PA_CHANNEL_POSITION_MONO                    PA_CHANNEL_POSITION_MONO
#define MAL_PA_CHANNEL_POSITION_FRONT_LEFT              PA_CHANNEL_POSITION_FRONT_LEFT
#define MAL_PA_CHANNEL_POSITION_FRONT_RIGHT             PA_CHANNEL_POSITION_FRONT_RIGHT
#define MAL_PA_CHANNEL_POSITION_FRONT_CENTER            PA_CHANNEL_POSITION_FRONT_CENTER
#define MAL_PA_CHANNEL_POSITION_REAR_CENTER             PA_CHANNEL_POSITION_REAR_CENTER
#define MAL_PA_CHANNEL_POSITION_REAR_LEFT               PA_CHANNEL_POSITION_REAR_LEFT
#define MAL_PA_CHANNEL_POSITION_REAR_RIGHT              PA_CHANNEL_POSITION_REAR_RIGHT
#define MAL_PA_CHANNEL_POSITION_LFE                     PA_CHANNEL_POSITION_LFE
#define MAL_PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER    PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER
#define MAL_PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER   PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER
#define MAL_PA_CHANNEL_POSITION_SIDE_LEFT               PA_CHANNEL_POSITION_SIDE_LEFT
#define MAL_PA_CHANNEL_POSITION_SIDE_RIGHT              PA_CHANNEL_POSITION_SIDE_RIGHT
#define MAL_PA_CHANNEL_POSITION_AUX0                    PA_CHANNEL_POSITION_AUX0
#define MAL_PA_CHANNEL_POSITION_AUX1                    PA_CHANNEL_POSITION_AUX1
#define MAL_PA_CHANNEL_POSITION_AUX2                    PA_CHANNEL_POSITION_AUX2
#define MAL_PA_CHANNEL_POSITION_AUX3                    PA_CHANNEL_POSITION_AUX3
#define MAL_PA_CHANNEL_POSITION_AUX4                    PA_CHANNEL_POSITION_AUX4
#define MAL_PA_CHANNEL_POSITION_AUX5                    PA_CHANNEL_POSITION_AUX5
#define MAL_PA_CHANNEL_POSITION_AUX6                    PA_CHANNEL_POSITION_AUX6
#define MAL_PA_CHANNEL_POSITION_AUX7                    PA_CHANNEL_POSITION_AUX7
#define MAL_PA_CHANNEL_POSITION_AUX8                    PA_CHANNEL_POSITION_AUX8
#define MAL_PA_CHANNEL_POSITION_AUX9                    PA_CHANNEL_POSITION_AUX9
#define MAL_PA_CHANNEL_POSITION_AUX10                   PA_CHANNEL_POSITION_AUX10
#define MAL_PA_CHANNEL_POSITION_AUX11                   PA_CHANNEL_POSITION_AUX11
#define MAL_PA_CHANNEL_POSITION_AUX12                   PA_CHANNEL_POSITION_AUX12
#define MAL_PA_CHANNEL_POSITION_AUX13                   PA_CHANNEL_POSITION_AUX13
#define MAL_PA_CHANNEL_POSITION_AUX14                   PA_CHANNEL_POSITION_AUX14
#define MAL_PA_CHANNEL_POSITION_AUX15                   PA_CHANNEL_POSITION_AUX15
#define MAL_PA_CHANNEL_POSITION_AUX16                   PA_CHANNEL_POSITION_AUX16
#define MAL_PA_CHANNEL_POSITION_AUX17                   PA_CHANNEL_POSITION_AUX17
#define MAL_PA_CHANNEL_POSITION_AUX18                   PA_CHANNEL_POSITION_AUX18
#define MAL_PA_CHANNEL_POSITION_AUX19                   PA_CHANNEL_POSITION_AUX19
#define MAL_PA_CHANNEL_POSITION_AUX20                   PA_CHANNEL_POSITION_AUX20
#define MAL_PA_CHANNEL_POSITION_AUX21                   PA_CHANNEL_POSITION_AUX21
#define MAL_PA_CHANNEL_POSITION_AUX22                   PA_CHANNEL_POSITION_AUX22
#define MAL_PA_CHANNEL_POSITION_AUX23                   PA_CHANNEL_POSITION_AUX23
#define MAL_PA_CHANNEL_POSITION_AUX24                   PA_CHANNEL_POSITION_AUX24
#define MAL_PA_CHANNEL_POSITION_AUX25                   PA_CHANNEL_POSITION_AUX25
#define MAL_PA_CHANNEL_POSITION_AUX26                   PA_CHANNEL_POSITION_AUX26
#define MAL_PA_CHANNEL_POSITION_AUX27                   PA_CHANNEL_POSITION_AUX27
#define MAL_PA_CHANNEL_POSITION_AUX28                   PA_CHANNEL_POSITION_AUX28
#define MAL_PA_CHANNEL_POSITION_AUX29                   PA_CHANNEL_POSITION_AUX29
#define MAL_PA_CHANNEL_POSITION_AUX30                   PA_CHANNEL_POSITION_AUX30
#define MAL_PA_CHANNEL_POSITION_AUX31                   PA_CHANNEL_POSITION_AUX31
#define MAL_PA_CHANNEL_POSITION_TOP_CENTER              PA_CHANNEL_POSITION_TOP_CENTER
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_LEFT          PA_CHANNEL_POSITION_TOP_FRONT_LEFT
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_RIGHT         PA_CHANNEL_POSITION_TOP_FRONT_RIGHT
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_CENTER        PA_CHANNEL_POSITION_TOP_FRONT_CENTER
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_LEFT           PA_CHANNEL_POSITION_TOP_REAR_LEFT
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_RIGHT          PA_CHANNEL_POSITION_TOP_REAR_RIGHT
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_CENTER         PA_CHANNEL_POSITION_TOP_REAR_CENTER
#define MAL_PA_CHANNEL_POSITION_LEFT                    PA_CHANNEL_POSITION_LEFT
#define MAL_PA_CHANNEL_POSITION_RIGHT                   PA_CHANNEL_POSITION_RIGHT
#define MAL_PA_CHANNEL_POSITION_CENTER                  PA_CHANNEL_POSITION_CENTER
#define MAL_PA_CHANNEL_POSITION_SUBWOOFER               PA_CHANNEL_POSITION_SUBWOOFER

typedef pa_channel_map_def_t mal_pa_channel_map_def_t;
#define MAL_PA_CHANNEL_MAP_AIFF                         PA_CHANNEL_MAP_AIFF
#define MAL_PA_CHANNEL_MAP_ALSA                         PA_CHANNEL_MAP_ALSA
#define MAL_PA_CHANNEL_MAP_AUX                          PA_CHANNEL_MAP_AUX
#define MAL_PA_CHANNEL_MAP_WAVEEX                       PA_CHANNEL_MAP_WAVEEX
#define MAL_PA_CHANNEL_MAP_OSS                          PA_CHANNEL_MAP_OSS
#define MAL_PA_CHANNEL_MAP_DEFAULT                      PA_CHANNEL_MAP_DEFAULT

typedef pa_sample_format_t mal_pa_sample_format_t;
#define MAL_PA_SAMPLE_INVALID                           PA_SAMPLE_INVALID
#define MAL_PA_SAMPLE_U8                                PA_SAMPLE_U8
#define MAL_PA_SAMPLE_ALAW                              PA_SAMPLE_ALAW
#define MAL_PA_SAMPLE_ULAW                              PA_SAMPLE_ULAW
#define MAL_PA_SAMPLE_S16LE                             PA_SAMPLE_S16LE
#define MAL_PA_SAMPLE_S16BE                             PA_SAMPLE_S16BE
#define MAL_PA_SAMPLE_FLOAT32LE                         PA_SAMPLE_FLOAT32LE
#define MAL_PA_SAMPLE_FLOAT32BE                         PA_SAMPLE_FLOAT32BE
#define MAL_PA_SAMPLE_S32LE                             PA_SAMPLE_S32LE
#define MAL_PA_SAMPLE_S32BE                             PA_SAMPLE_S32BE
#define MAL_PA_SAMPLE_S24LE                             PA_SAMPLE_S24LE
#define MAL_PA_SAMPLE_S24BE                             PA_SAMPLE_S24BE
#define MAL_PA_SAMPLE_S24_32LE                          PA_SAMPLE_S24_32LE
#define MAL_PA_SAMPLE_S24_32BE                          PA_SAMPLE_S24_32BE

typedef pa_mainloop     mal_pa_mainloop;
typedef pa_mainloop_api mal_pa_mainloop_api;
typedef pa_context      mal_pa_context;
typedef pa_operation    mal_pa_operation;
typedef pa_stream       mal_pa_stream;
typedef pa_spawn_api    mal_pa_spawn_api;
typedef pa_buffer_attr  mal_pa_buffer_attr;
typedef pa_channel_map  mal_pa_channel_map;
typedef pa_cvolume      mal_pa_cvolume;
typedef pa_sample_spec  mal_pa_sample_spec;
typedef pa_sink_info    mal_pa_sink_info;
typedef pa_source_info  mal_pa_source_info;

typedef pa_context_notify_cb_t mal_pa_context_notify_cb_t;
typedef pa_sink_info_cb_t mal_pa_sink_info_cb_t;
typedef pa_source_info_cb_t mal_pa_source_info_cb_t;
typedef pa_stream_success_cb_t mal_pa_stream_success_cb_t;
typedef pa_stream_request_cb_t mal_pa_stream_request_cb_t;
typedef pa_free_cb_t mal_pa_free_cb_t;
#else
#define MAL_PA_OK                                       0
#define MAL_PA_ERR_ACCESS                               1
#define MAL_PA_ERR_INVALID                              2
#define MAL_PA_ERR_NOENTITY                             5

#define MAL_PA_CHANNELS_MAX                             32
#define MAL_PA_RATE_MAX                                 384000

typedef int mal_pa_context_flags_t;
#define MAL_PA_CONTEXT_NOFLAGS                          0x00000000
#define MAL_PA_CONTEXT_NOAUTOSPAWN                      0x00000001
#define MAL_PA_CONTEXT_NOFAIL                           0x00000002

typedef int mal_pa_stream_flags_t;
#define MAL_PA_STREAM_NOFLAGS                           0x00000000
#define MAL_PA_STREAM_START_CORKED                      0x00000001
#define MAL_PA_STREAM_INTERPOLATE_TIMING                0x00000002
#define MAL_PA_STREAM_NOT_MONOTONIC                     0x00000004
#define MAL_PA_STREAM_AUTO_TIMING_UPDATE                0x00000008
#define MAL_PA_STREAM_NO_REMAP_CHANNELS                 0x00000010
#define MAL_PA_STREAM_NO_REMIX_CHANNELS                 0x00000020
#define MAL_PA_STREAM_FIX_FORMAT                        0x00000040
#define MAL_PA_STREAM_FIX_RATE                          0x00000080
#define MAL_PA_STREAM_FIX_CHANNELS                      0x00000100
#define MAL_PA_STREAM_DONT_MOVE                         0x00000200
#define MAL_PA_STREAM_VARIABLE_RATE                     0x00000400
#define MAL_PA_STREAM_PEAK_DETECT                       0x00000800
#define MAL_PA_STREAM_START_MUTED                       0x00001000
#define MAL_PA_STREAM_ADJUST_LATENCY                    0x00002000
#define MAL_PA_STREAM_EARLY_REQUESTS                    0x00004000
#define MAL_PA_STREAM_DONT_INHIBIT_AUTO_SUSPEND         0x00008000
#define MAL_PA_STREAM_START_UNMUTED                     0x00010000
#define MAL_PA_STREAM_FAIL_ON_SUSPEND                   0x00020000
#define MAL_PA_STREAM_RELATIVE_VOLUME                   0x00040000
#define MAL_PA_STREAM_PASSTHROUGH                       0x00080000

typedef int mal_pa_sink_flags_t;
#define MAL_PA_SINK_NOFLAGS                             0x00000000
#define MAL_PA_SINK_HW_VOLUME_CTRL                      0x00000001
#define MAL_PA_SINK_LATENCY                             0x00000002
#define MAL_PA_SINK_HARDWARE                            0x00000004
#define MAL_PA_SINK_NETWORK                             0x00000008
#define MAL_PA_SINK_HW_MUTE_CTRL                        0x00000010
#define MAL_PA_SINK_DECIBEL_VOLUME                      0x00000020
#define MAL_PA_SINK_FLAT_VOLUME                         0x00000040
#define MAL_PA_SINK_DYNAMIC_LATENCY                     0x00000080
#define MAL_PA_SINK_SET_FORMATS                         0x00000100

typedef int mal_pa_source_flags_t;
#define MAL_PA_SOURCE_NOFLAGS                           0x00000000
#define MAL_PA_SOURCE_HW_VOLUME_CTRL                    0x00000001
#define MAL_PA_SOURCE_LATENCY                           0x00000002
#define MAL_PA_SOURCE_HARDWARE                          0x00000004
#define MAL_PA_SOURCE_NETWORK                           0x00000008
#define MAL_PA_SOURCE_HW_MUTE_CTRL                      0x00000010
#define MAL_PA_SOURCE_DECIBEL_VOLUME                    0x00000020
#define MAL_PA_SOURCE_DYNAMIC_LATENCY                   0x00000040
#define MAL_PA_SOURCE_FLAT_VOLUME                       0x00000080

typedef int mal_pa_context_state_t;
#define MAL_PA_CONTEXT_UNCONNECTED                      0
#define MAL_PA_CONTEXT_CONNECTING                       1
#define MAL_PA_CONTEXT_AUTHORIZING                      2
#define MAL_PA_CONTEXT_SETTING_NAME                     3
#define MAL_PA_CONTEXT_READY                            4
#define MAL_PA_CONTEXT_FAILED                           5
#define MAL_PA_CONTEXT_TERMINATED                       6

typedef int mal_pa_stream_state_t;
#define MAL_PA_STREAM_UNCONNECTED                       0
#define MAL_PA_STREAM_CREATING                          1
#define MAL_PA_STREAM_READY                             2
#define MAL_PA_STREAM_FAILED                            3
#define MAL_PA_STREAM_TERMINATED                        4

typedef int mal_pa_operation_state_t;
#define MAL_PA_OPERATION_RUNNING                        0
#define MAL_PA_OPERATION_DONE                           1
#define MAL_PA_OPERATION_CANCELLED                      2

typedef int mal_pa_sink_state_t;
#define MAL_PA_SINK_INVALID_STATE                       -1
#define MAL_PA_SINK_RUNNING                             0
#define MAL_PA_SINK_IDLE                                1
#define MAL_PA_SINK_SUSPENDED                           2

typedef int mal_pa_source_state_t;
#define MAL_PA_SOURCE_INVALID_STATE                     -1
#define MAL_PA_SOURCE_RUNNING                           0
#define MAL_PA_SOURCE_IDLE                              1
#define MAL_PA_SOURCE_SUSPENDED                         2

typedef int mal_pa_seek_mode_t;
#define MAL_PA_SEEK_RELATIVE                            0
#define MAL_PA_SEEK_ABSOLUTE                            1
#define MAL_PA_SEEK_RELATIVE_ON_READ                    2
#define MAL_PA_SEEK_RELATIVE_END                        3

typedef int mal_pa_channel_position_t;
#define MAL_PA_CHANNEL_POSITION_INVALID                 -1
#define MAL_PA_CHANNEL_POSITION_MONO                    0
#define MAL_PA_CHANNEL_POSITION_FRONT_LEFT              1
#define MAL_PA_CHANNEL_POSITION_FRONT_RIGHT             2
#define MAL_PA_CHANNEL_POSITION_FRONT_CENTER            3
#define MAL_PA_CHANNEL_POSITION_REAR_CENTER             4
#define MAL_PA_CHANNEL_POSITION_REAR_LEFT               5
#define MAL_PA_CHANNEL_POSITION_REAR_RIGHT              6
#define MAL_PA_CHANNEL_POSITION_LFE                     7
#define MAL_PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER    8
#define MAL_PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER   9
#define MAL_PA_CHANNEL_POSITION_SIDE_LEFT               10
#define MAL_PA_CHANNEL_POSITION_SIDE_RIGHT              11
#define MAL_PA_CHANNEL_POSITION_AUX0                    12
#define MAL_PA_CHANNEL_POSITION_AUX1                    13
#define MAL_PA_CHANNEL_POSITION_AUX2                    14
#define MAL_PA_CHANNEL_POSITION_AUX3                    15
#define MAL_PA_CHANNEL_POSITION_AUX4                    16
#define MAL_PA_CHANNEL_POSITION_AUX5                    17
#define MAL_PA_CHANNEL_POSITION_AUX6                    18
#define MAL_PA_CHANNEL_POSITION_AUX7                    19
#define MAL_PA_CHANNEL_POSITION_AUX8                    20
#define MAL_PA_CHANNEL_POSITION_AUX9                    21
#define MAL_PA_CHANNEL_POSITION_AUX10                   22
#define MAL_PA_CHANNEL_POSITION_AUX11                   23
#define MAL_PA_CHANNEL_POSITION_AUX12                   24
#define MAL_PA_CHANNEL_POSITION_AUX13                   25
#define MAL_PA_CHANNEL_POSITION_AUX14                   26
#define MAL_PA_CHANNEL_POSITION_AUX15                   27
#define MAL_PA_CHANNEL_POSITION_AUX16                   28
#define MAL_PA_CHANNEL_POSITION_AUX17                   29
#define MAL_PA_CHANNEL_POSITION_AUX18                   30
#define MAL_PA_CHANNEL_POSITION_AUX19                   31
#define MAL_PA_CHANNEL_POSITION_AUX20                   32
#define MAL_PA_CHANNEL_POSITION_AUX21                   33
#define MAL_PA_CHANNEL_POSITION_AUX22                   34
#define MAL_PA_CHANNEL_POSITION_AUX23                   35
#define MAL_PA_CHANNEL_POSITION_AUX24                   36
#define MAL_PA_CHANNEL_POSITION_AUX25                   37
#define MAL_PA_CHANNEL_POSITION_AUX26                   38
#define MAL_PA_CHANNEL_POSITION_AUX27                   39
#define MAL_PA_CHANNEL_POSITION_AUX28                   40
#define MAL_PA_CHANNEL_POSITION_AUX29                   41
#define MAL_PA_CHANNEL_POSITION_AUX30                   42
#define MAL_PA_CHANNEL_POSITION_AUX31                   43
#define MAL_PA_CHANNEL_POSITION_TOP_CENTER              44
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_LEFT          45
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_RIGHT         46
#define MAL_PA_CHANNEL_POSITION_TOP_FRONT_CENTER        47
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_LEFT           48
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_RIGHT          49
#define MAL_PA_CHANNEL_POSITION_TOP_REAR_CENTER         50
#define MAL_PA_CHANNEL_POSITION_LEFT                    MAL_PA_CHANNEL_POSITION_FRONT_LEFT
#define MAL_PA_CHANNEL_POSITION_RIGHT                   MAL_PA_CHANNEL_POSITION_FRONT_RIGHT
#define MAL_PA_CHANNEL_POSITION_CENTER                  MAL_PA_CHANNEL_POSITION_FRONT_CENTER
#define MAL_PA_CHANNEL_POSITION_SUBWOOFER               MAL_PA_CHANNEL_POSITION_LFE

typedef int mal_pa_channel_map_def_t;
#define MAL_PA_CHANNEL_MAP_AIFF                         0
#define MAL_PA_CHANNEL_MAP_ALSA                         1
#define MAL_PA_CHANNEL_MAP_AUX                          2
#define MAL_PA_CHANNEL_MAP_WAVEEX                       3
#define MAL_PA_CHANNEL_MAP_OSS                          4
#define MAL_PA_CHANNEL_MAP_DEFAULT                      MAL_PA_CHANNEL_MAP_AIFF

typedef int mal_pa_sample_format_t;
#define MAL_PA_SAMPLE_INVALID                           -1
#define MAL_PA_SAMPLE_U8                                0
#define MAL_PA_SAMPLE_ALAW                              1
#define MAL_PA_SAMPLE_ULAW                              2
#define MAL_PA_SAMPLE_S16LE                             3
#define MAL_PA_SAMPLE_S16BE                             4
#define MAL_PA_SAMPLE_FLOAT32LE                         5
#define MAL_PA_SAMPLE_FLOAT32BE                         6
#define MAL_PA_SAMPLE_S32LE                             7
#define MAL_PA_SAMPLE_S32BE                             8
#define MAL_PA_SAMPLE_S24LE                             9
#define MAL_PA_SAMPLE_S24BE                             10
#define MAL_PA_SAMPLE_S24_32LE                          11
#define MAL_PA_SAMPLE_S24_32BE                          12

typedef struct mal_pa_mainloop     mal_pa_mainloop;
typedef struct mal_pa_mainloop_api mal_pa_mainloop_api;
typedef struct mal_pa_context      mal_pa_context;
typedef struct mal_pa_operation    mal_pa_operation;
typedef struct mal_pa_stream       mal_pa_stream;
typedef struct mal_pa_spawn_api    mal_pa_spawn_api;

typedef struct
{
    mal_uint32 maxlength;
    mal_uint32 tlength;
    mal_uint32 prebuf;
    mal_uint32 minreq;
    mal_uint32 fragsize;
} mal_pa_buffer_attr;

typedef struct
{
    mal_uint8 channels;
    mal_pa_channel_position_t map[MAL_PA_CHANNELS_MAX];
} mal_pa_channel_map;

typedef struct
{
    mal_uint8 channels;
    mal_uint32 values[MAL_PA_CHANNELS_MAX];
} mal_pa_cvolume;

typedef struct
{
    mal_pa_sample_format_t format;
    mal_uint32 rate;
    mal_uint8 channels;
} mal_pa_sample_spec;

typedef struct
{
    const char* name;
    mal_uint32 index;
    const char* description;
    mal_pa_sample_spec sample_spec;
    mal_pa_channel_map channel_map;
    mal_uint32 owner_module;
    mal_pa_cvolume volume;
    int mute;
    mal_uint32 monitor_source;
    const char* monitor_source_name;
    mal_uint64 latency;
    const char* driver;
    mal_pa_sink_flags_t flags;
    void* proplist;
    mal_uint64 configured_latency;
    mal_uint32 base_volume;
    mal_pa_sink_state_t state;
    mal_uint32 n_volume_steps;
    mal_uint32 card;
    mal_uint32 n_ports;
    void** ports;
    void* active_port;
    mal_uint8 n_formats;
    void** formats;
} mal_pa_sink_info;

typedef struct
{
    const char *name;
    mal_uint32 index;
    const char *description;
    mal_pa_sample_spec sample_spec;
    mal_pa_channel_map channel_map;
    mal_uint32 owner_module;
    mal_pa_cvolume volume;
    int mute;
    mal_uint32 monitor_of_sink;
    const char *monitor_of_sink_name;
    mal_uint64 latency;
    const char *driver;
    mal_pa_source_flags_t flags;
    void* proplist;
    mal_uint64 configured_latency;
    mal_uint32 base_volume;
    mal_pa_source_state_t state;
    mal_uint32 n_volume_steps;
    mal_uint32 card;
    mal_uint32 n_ports;
    void** ports;
    void* active_port;
    mal_uint8 n_formats;
    void** formats;
} mal_pa_source_info;

typedef void (* mal_pa_context_notify_cb_t)(mal_pa_context* c, void* userdata);
typedef void (* mal_pa_sink_info_cb_t)     (mal_pa_context* c, const mal_pa_sink_info* i, int eol, void* userdata);
typedef void (* mal_pa_source_info_cb_t)   (mal_pa_context* c, const mal_pa_source_info* i, int eol, void* userdata);
typedef void (* mal_pa_stream_success_cb_t)(mal_pa_stream* s, int success, void* userdata);
typedef void (* mal_pa_stream_request_cb_t)(mal_pa_stream* s, size_t nbytes, void* userdata);
typedef void (* mal_pa_free_cb_t)          (void* p);
#endif


typedef mal_pa_mainloop*          (* mal_pa_mainloop_new_proc)                   ();
typedef void                      (* mal_pa_mainloop_free_proc)                  (mal_pa_mainloop* m);
typedef mal_pa_mainloop_api*      (* mal_pa_mainloop_get_api_proc)               (mal_pa_mainloop* m);
typedef int                       (* mal_pa_mainloop_iterate_proc)               (mal_pa_mainloop* m, int block, int* retval);
typedef void                      (* mal_pa_mainloop_wakeup_proc)                (mal_pa_mainloop* m);
typedef mal_pa_context*           (* mal_pa_context_new_proc)                    (mal_pa_mainloop_api* mainloop, const char* name);
typedef void                      (* mal_pa_context_unref_proc)                  (mal_pa_context* c);
typedef int                       (* mal_pa_context_connect_proc)                (mal_pa_context* c, const char* server, mal_pa_context_flags_t flags, const mal_pa_spawn_api* api);
typedef void                      (* mal_pa_context_disconnect_proc)             (mal_pa_context* c);
typedef void                      (* mal_pa_context_set_state_callback_proc)     (mal_pa_context* c, mal_pa_context_notify_cb_t cb, void* userdata);
typedef mal_pa_context_state_t    (* mal_pa_context_get_state_proc)              (mal_pa_context* c);
typedef mal_pa_operation*         (* mal_pa_context_get_sink_info_list_proc)     (mal_pa_context* c, mal_pa_sink_info_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_context_get_source_info_list_proc)   (mal_pa_context* c, mal_pa_source_info_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_context_get_sink_info_by_name_proc)  (mal_pa_context* c, const char* name, mal_pa_sink_info_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_context_get_source_info_by_name_proc)(mal_pa_context* c, const char* name, mal_pa_source_info_cb_t cb, void* userdata);
typedef void                      (* mal_pa_operation_unref_proc)                (mal_pa_operation* o);
typedef mal_pa_operation_state_t  (* mal_pa_operation_get_state_proc)            (mal_pa_operation* o);
typedef mal_pa_channel_map*       (* mal_pa_channel_map_init_extend_proc)        (mal_pa_channel_map* m, unsigned channels, mal_pa_channel_map_def_t def);
typedef int                       (* mal_pa_channel_map_valid_proc)              (const mal_pa_channel_map* m);
typedef int                       (* mal_pa_channel_map_compatible_proc)         (const mal_pa_channel_map* m, const mal_pa_sample_spec* ss);
typedef mal_pa_stream*            (* mal_pa_stream_new_proc)                     (mal_pa_context* c, const char* name, const mal_pa_sample_spec* ss, const mal_pa_channel_map* map);
typedef void                      (* mal_pa_stream_unref_proc)                   (mal_pa_stream* s);
typedef int                       (* mal_pa_stream_connect_playback_proc)        (mal_pa_stream* s, const char* dev, const mal_pa_buffer_attr* attr, mal_pa_stream_flags_t flags, const mal_pa_cvolume* volume, mal_pa_stream* sync_stream);
typedef int                       (* mal_pa_stream_connect_record_proc)          (mal_pa_stream* s, const char* dev, const mal_pa_buffer_attr* attr, mal_pa_stream_flags_t flags);
typedef int                       (* mal_pa_stream_disconnect_proc)              (mal_pa_stream* s);
typedef mal_pa_stream_state_t     (* mal_pa_stream_get_state_proc)               (mal_pa_stream* s);
typedef const mal_pa_sample_spec* (* mal_pa_stream_get_sample_spec_proc)         (mal_pa_stream* s);
typedef const mal_pa_channel_map* (* mal_pa_stream_get_channel_map_proc)         (mal_pa_stream* s);
typedef const mal_pa_buffer_attr* (* mal_pa_stream_get_buffer_attr_proc)         (mal_pa_stream* s);
typedef const char*               (* mal_pa_stream_get_device_name_proc)         (mal_pa_stream* s);
typedef void                      (* mal_pa_stream_set_write_callback_proc)      (mal_pa_stream* s, mal_pa_stream_request_cb_t cb, void* userdata);
typedef void                      (* mal_pa_stream_set_read_callback_proc)       (mal_pa_stream* s, mal_pa_stream_request_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_stream_flush_proc)                   (mal_pa_stream* s, mal_pa_stream_success_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_stream_drain_proc)                   (mal_pa_stream* s, mal_pa_stream_success_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_stream_cork_proc)                    (mal_pa_stream* s, int b, mal_pa_stream_success_cb_t cb, void* userdata);
typedef mal_pa_operation*         (* mal_pa_stream_trigger_proc)                 (mal_pa_stream* s, mal_pa_stream_success_cb_t cb, void* userdata);
typedef int                       (* mal_pa_stream_begin_write_proc)             (mal_pa_stream* s, void** data, size_t* nbytes);
typedef int                       (* mal_pa_stream_write_proc)                   (mal_pa_stream* s, const void* data, size_t nbytes, mal_pa_free_cb_t free_cb, int64_t offset, mal_pa_seek_mode_t seek);
typedef int                       (* mal_pa_stream_peek_proc)                    (mal_pa_stream* s, const void** data, size_t* nbytes);
typedef int                       (* mal_pa_stream_drop_proc)                    (mal_pa_stream* s);

typedef struct
{
    mal_uint32 count;
    mal_uint32 capacity;
    mal_device_info* pInfo;
} mal_pulse_device_enum_data;

mal_result mal_result_from_pulse(int result)
{
    switch (result) {
        case MAL_PA_OK:           return MAL_SUCCESS;
        case MAL_PA_ERR_ACCESS:   return MAL_ACCESS_DENIED;
        case MAL_PA_ERR_INVALID:  return MAL_INVALID_ARGS;
        case MAL_PA_ERR_NOENTITY: return MAL_NO_DEVICE;
        default:                  return MAL_ERROR;
    }
}

#if 0
mal_pa_sample_format_t mal_format_to_pulse(mal_format format)
{
    switch (format)
    {
        case mal_format_u8:       return MAL_PA_SAMPLE_U8;
        case mal_format_s16:      return MAL_PA_SAMPLE_S16LE;
        //case mal_format_s16be:    return MAL_PA_SAMPLE_S16BE;
        case mal_format_s24:      return MAL_PA_SAMPLE_S24LE;
        //case mal_format_s24be:    return MAL_PA_SAMPLE_S24BE;
        //case mal_format_s24_32:   return MAL_PA_SAMPLE_S24_32LE;
        //case mal_format_s24_32be: return MAL_PA_SAMPLE_S24_32BE;
        case mal_format_s32:      return MAL_PA_SAMPLE_S32LE;
        //case mal_format_s32be:    return MAL_PA_SAMPLE_S32BE;
        case mal_format_f32:      return MAL_PA_SAMPLE_FLOAT32LE;
        //case mal_format_f32be:    return PA_SAMPLE_FLOAT32BE;

        default: return MAL_PA_SAMPLE_INVALID;
    }
}
#endif

mal_format mal_format_from_pulse(mal_pa_sample_format_t format)
{
    switch (format)
    {
        case MAL_PA_SAMPLE_U8:        return mal_format_u8;
        case MAL_PA_SAMPLE_S16LE:     return mal_format_s16;
        //case MAL_PA_SAMPLE_S16BE:     return mal_format_s16be;
        case MAL_PA_SAMPLE_S24LE:     return mal_format_s24;
        //case MAL_PA_SAMPLE_S24BE:     return mal_format_s24be;
        //case MAL_PA_SAMPLE_S24_32LE:  return mal_format_s24_32;
        //case MAL_PA_SAMPLE_S24_32BE:  return mal_format_s24_32be;
        case MAL_PA_SAMPLE_S32LE:     return mal_format_s32;
        //case MAL_PA_SAMPLE_S32BE:     return mal_format_s32be;
        case MAL_PA_SAMPLE_FLOAT32LE: return mal_format_f32;
        //case MAL_PA_SAMPLE_FLOAT32BE: return mal_format_f32be;

        default: return mal_format_unknown;
    }
}

mal_channel mal_channel_position_from_pulse(mal_pa_channel_position_t position)
{
    switch (position)
    {
        case MAL_PA_CHANNEL_POSITION_INVALID:               return MAL_CHANNEL_NONE;
        case MAL_PA_CHANNEL_POSITION_MONO:                  return MAL_CHANNEL_MONO;
        case MAL_PA_CHANNEL_POSITION_FRONT_LEFT:            return MAL_CHANNEL_FRONT_LEFT;
        case MAL_PA_CHANNEL_POSITION_FRONT_RIGHT:           return MAL_CHANNEL_FRONT_RIGHT;
        case MAL_PA_CHANNEL_POSITION_FRONT_CENTER:          return MAL_CHANNEL_FRONT_CENTER;
        case MAL_PA_CHANNEL_POSITION_REAR_CENTER:           return MAL_CHANNEL_BACK_CENTER;
        case MAL_PA_CHANNEL_POSITION_REAR_LEFT:             return MAL_CHANNEL_BACK_LEFT;
        case MAL_PA_CHANNEL_POSITION_REAR_RIGHT:            return MAL_CHANNEL_BACK_RIGHT;
        case MAL_PA_CHANNEL_POSITION_LFE:                   return MAL_CHANNEL_LFE;
        case MAL_PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER:  return MAL_CHANNEL_FRONT_LEFT_CENTER;
        case MAL_PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER: return MAL_CHANNEL_FRONT_RIGHT_CENTER;
        case MAL_PA_CHANNEL_POSITION_SIDE_LEFT:             return MAL_CHANNEL_SIDE_LEFT;
        case MAL_PA_CHANNEL_POSITION_SIDE_RIGHT:            return MAL_CHANNEL_SIDE_RIGHT;
        case MAL_PA_CHANNEL_POSITION_AUX0:                  return MAL_CHANNEL_AUX_0;
        case MAL_PA_CHANNEL_POSITION_AUX1:                  return MAL_CHANNEL_AUX_1;
        case MAL_PA_CHANNEL_POSITION_AUX2:                  return MAL_CHANNEL_AUX_2;
        case MAL_PA_CHANNEL_POSITION_AUX3:                  return MAL_CHANNEL_AUX_3;
        case MAL_PA_CHANNEL_POSITION_AUX4:                  return MAL_CHANNEL_AUX_4;
        case MAL_PA_CHANNEL_POSITION_AUX5:                  return MAL_CHANNEL_AUX_5;
        case MAL_PA_CHANNEL_POSITION_AUX6:                  return MAL_CHANNEL_AUX_6;
        case MAL_PA_CHANNEL_POSITION_AUX7:                  return MAL_CHANNEL_AUX_7;
        case MAL_PA_CHANNEL_POSITION_AUX8:                  return MAL_CHANNEL_AUX_8;
        case MAL_PA_CHANNEL_POSITION_AUX9:                  return MAL_CHANNEL_AUX_9;
        case MAL_PA_CHANNEL_POSITION_AUX10:                 return MAL_CHANNEL_AUX_10;
        case MAL_PA_CHANNEL_POSITION_AUX11:                 return MAL_CHANNEL_AUX_11;
        case MAL_PA_CHANNEL_POSITION_AUX12:                 return MAL_CHANNEL_AUX_12;
        case MAL_PA_CHANNEL_POSITION_AUX13:                 return MAL_CHANNEL_AUX_13;
        case MAL_PA_CHANNEL_POSITION_AUX14:                 return MAL_CHANNEL_AUX_14;
        case MAL_PA_CHANNEL_POSITION_AUX15:                 return MAL_CHANNEL_AUX_15;
        case MAL_PA_CHANNEL_POSITION_AUX16:                 return MAL_CHANNEL_AUX_16;
        case MAL_PA_CHANNEL_POSITION_AUX17:                 return MAL_CHANNEL_AUX_17;
        case MAL_PA_CHANNEL_POSITION_AUX18:                 return MAL_CHANNEL_AUX_18;
        case MAL_PA_CHANNEL_POSITION_AUX19:                 return MAL_CHANNEL_AUX_19;
        case MAL_PA_CHANNEL_POSITION_AUX20:                 return MAL_CHANNEL_AUX_20;
        case MAL_PA_CHANNEL_POSITION_AUX21:                 return MAL_CHANNEL_AUX_21;
        case MAL_PA_CHANNEL_POSITION_AUX22:                 return MAL_CHANNEL_AUX_22;
        case MAL_PA_CHANNEL_POSITION_AUX23:                 return MAL_CHANNEL_AUX_23;
        case MAL_PA_CHANNEL_POSITION_AUX24:                 return MAL_CHANNEL_AUX_24;
        case MAL_PA_CHANNEL_POSITION_AUX25:                 return MAL_CHANNEL_AUX_25;
        case MAL_PA_CHANNEL_POSITION_AUX26:                 return MAL_CHANNEL_AUX_26;
        case MAL_PA_CHANNEL_POSITION_AUX27:                 return MAL_CHANNEL_AUX_27;
        case MAL_PA_CHANNEL_POSITION_AUX28:                 return MAL_CHANNEL_AUX_28;
        case MAL_PA_CHANNEL_POSITION_AUX29:                 return MAL_CHANNEL_AUX_29;
        case MAL_PA_CHANNEL_POSITION_AUX30:                 return MAL_CHANNEL_AUX_30;
        case MAL_PA_CHANNEL_POSITION_AUX31:                 return MAL_CHANNEL_AUX_31;
        case MAL_PA_CHANNEL_POSITION_TOP_CENTER:            return MAL_CHANNEL_TOP_CENTER;
        case MAL_PA_CHANNEL_POSITION_TOP_FRONT_LEFT:        return MAL_CHANNEL_TOP_FRONT_LEFT;
        case MAL_PA_CHANNEL_POSITION_TOP_FRONT_RIGHT:       return MAL_CHANNEL_TOP_FRONT_RIGHT;
        case MAL_PA_CHANNEL_POSITION_TOP_FRONT_CENTER:      return MAL_CHANNEL_TOP_FRONT_CENTER;
        case MAL_PA_CHANNEL_POSITION_TOP_REAR_LEFT:         return MAL_CHANNEL_TOP_BACK_LEFT;
        case MAL_PA_CHANNEL_POSITION_TOP_REAR_RIGHT:        return MAL_CHANNEL_TOP_BACK_RIGHT;
        case MAL_PA_CHANNEL_POSITION_TOP_REAR_CENTER:       return MAL_CHANNEL_TOP_BACK_CENTER;
        default: return MAL_CHANNEL_NONE;
    }
}

#if 0
mal_pa_channel_position_t mal_channel_position_to_pulse(mal_channel position)
{
    switch (position)
    {
        case MAL_CHANNEL_NONE:               return MAL_PA_CHANNEL_POSITION_INVALID;
        case MAL_CHANNEL_FRONT_LEFT:         return MAL_PA_CHANNEL_POSITION_FRONT_LEFT;
        case MAL_CHANNEL_FRONT_RIGHT:        return MAL_PA_CHANNEL_POSITION_FRONT_RIGHT;
        case MAL_CHANNEL_FRONT_CENTER:       return MAL_PA_CHANNEL_POSITION_FRONT_CENTER;
        case MAL_CHANNEL_LFE:                return MAL_PA_CHANNEL_POSITION_LFE;
        case MAL_CHANNEL_BACK_LEFT:          return MAL_PA_CHANNEL_POSITION_REAR_LEFT;
        case MAL_CHANNEL_BACK_RIGHT:         return MAL_PA_CHANNEL_POSITION_REAR_RIGHT;
        case MAL_CHANNEL_FRONT_LEFT_CENTER:  return MAL_PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER;
        case MAL_CHANNEL_FRONT_RIGHT_CENTER: return MAL_PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER;
        case MAL_CHANNEL_BACK_CENTER:        return MAL_PA_CHANNEL_POSITION_REAR_CENTER;
        case MAL_CHANNEL_SIDE_LEFT:          return MAL_PA_CHANNEL_POSITION_SIDE_LEFT;
        case MAL_CHANNEL_SIDE_RIGHT:         return MAL_PA_CHANNEL_POSITION_SIDE_RIGHT;
        case MAL_CHANNEL_TOP_CENTER:         return MAL_PA_CHANNEL_POSITION_TOP_CENTER;
        case MAL_CHANNEL_TOP_FRONT_LEFT:     return MAL_PA_CHANNEL_POSITION_TOP_FRONT_LEFT;
        case MAL_CHANNEL_TOP_FRONT_CENTER:   return MAL_PA_CHANNEL_POSITION_TOP_FRONT_CENTER;
        case MAL_CHANNEL_TOP_FRONT_RIGHT:    return MAL_PA_CHANNEL_POSITION_TOP_FRONT_RIGHT;
        case MAL_CHANNEL_TOP_BACK_LEFT:      return MAL_PA_CHANNEL_POSITION_TOP_REAR_LEFT;
        case MAL_CHANNEL_TOP_BACK_CENTER:    return MAL_PA_CHANNEL_POSITION_TOP_REAR_CENTER;
        case MAL_CHANNEL_TOP_BACK_RIGHT:     return MAL_PA_CHANNEL_POSITION_TOP_REAR_RIGHT;
        case MAL_CHANNEL_19:                 return MAL_PA_CHANNEL_POSITION_AUX18;
        case MAL_CHANNEL_20:                 return MAL_PA_CHANNEL_POSITION_AUX19;
        case MAL_CHANNEL_21:                 return MAL_PA_CHANNEL_POSITION_AUX20;
        case MAL_CHANNEL_22:                 return MAL_PA_CHANNEL_POSITION_AUX21;
        case MAL_CHANNEL_23:                 return MAL_PA_CHANNEL_POSITION_AUX22;
        case MAL_CHANNEL_24:                 return MAL_PA_CHANNEL_POSITION_AUX23;
        case MAL_CHANNEL_25:                 return MAL_PA_CHANNEL_POSITION_AUX24;
        case MAL_CHANNEL_26:                 return MAL_PA_CHANNEL_POSITION_AUX25;
        case MAL_CHANNEL_27:                 return MAL_PA_CHANNEL_POSITION_AUX26;
        case MAL_CHANNEL_28:                 return MAL_PA_CHANNEL_POSITION_AUX27;
        case MAL_CHANNEL_29:                 return MAL_PA_CHANNEL_POSITION_AUX28;
        case MAL_CHANNEL_30:                 return MAL_PA_CHANNEL_POSITION_AUX29;
        case MAL_CHANNEL_31:                 return MAL_PA_CHANNEL_POSITION_AUX30;
        case MAL_CHANNEL_32:                 return MAL_PA_CHANNEL_POSITION_AUX31;
        default: return (mal_pa_channel_position_t)position;
    }
}
#endif


mal_result mal_wait_for_operation__pulse(mal_context* pContext, mal_pa_mainloop* pMainLoop, mal_pa_operation* pOP)
{
    mal_assert(pContext != NULL);
    mal_assert(pMainLoop != NULL);
    mal_assert(pOP != NULL);

    while (((mal_pa_operation_get_state_proc)pContext->pulse.pa_operation_get_state)(pOP) != MAL_PA_OPERATION_DONE) {
        int error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)(pMainLoop, 1, NULL);
        if (error < 0) {
            return mal_result_from_pulse(error);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__wait_for_operation__pulse(mal_device* pDevice, mal_pa_operation* pOP)
{
    mal_assert(pDevice != NULL);
    mal_assert(pOP != NULL);

    return mal_wait_for_operation__pulse(pDevice->pContext, (mal_pa_mainloop*)pDevice->pulse.pMainLoop, pOP);
}


mal_bool32 mal_context_is_device_id_equal__pulse(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return mal_strcmp(pID0->pulse, pID1->pulse) == 0;
}


typedef struct
{
    mal_context* pContext;
    mal_enum_devices_callback_proc callback;
    void* pUserData;
    mal_bool32 isTerminated;
} mal_context_enumerate_devices_callback_data__pulse;

void mal_context_enumerate_devices_sink_callback__pulse(mal_pa_context* pPulseContext, const mal_pa_sink_info* pSinkInfo, int endOfList, void* pUserData)
{
    mal_context_enumerate_devices_callback_data__pulse* pData = (mal_context_enumerate_devices_callback_data__pulse*)pUserData;
    mal_assert(pData != NULL);

    if (endOfList || pData->isTerminated) {
        return;
    }

    mal_device_info deviceInfo;
    mal_zero_object(&deviceInfo);

    // The name from PulseAudio is the ID for mini_al.
    if (pSinkInfo->name != NULL) {
        mal_strncpy_s(deviceInfo.id.pulse, sizeof(deviceInfo.id.pulse), pSinkInfo->name, (size_t)-1);
    }

    // The description from PulseAudio is the name for mini_al.
    if (pSinkInfo->description != NULL) {
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), pSinkInfo->description, (size_t)-1);
    }

    pData->isTerminated = !pData->callback(pData->pContext, mal_device_type_playback, &deviceInfo, pData->pUserData);
}

void mal_context_enumerate_devices_source_callback__pulse(mal_pa_context* pPulseContext, const mal_pa_source_info* pSinkInfo, int endOfList, void* pUserData)
{
    mal_context_enumerate_devices_callback_data__pulse* pData = (mal_context_enumerate_devices_callback_data__pulse*)pUserData;
    mal_assert(pData != NULL);

    if (endOfList || pData->isTerminated) {
        return;
    }

    mal_device_info deviceInfo;
    mal_zero_object(&deviceInfo);

    // The name from PulseAudio is the ID for mini_al.
    if (pSinkInfo->name != NULL) {
        mal_strncpy_s(deviceInfo.id.pulse, sizeof(deviceInfo.id.pulse), pSinkInfo->name, (size_t)-1);
    }

    // The description from PulseAudio is the name for mini_al.
    if (pSinkInfo->description != NULL) {
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), pSinkInfo->description, (size_t)-1);
    }

    pData->isTerminated = !pData->callback(pData->pContext, mal_device_type_capture, &deviceInfo, pData->pUserData);
}

mal_result mal_context_enumerate_devices__pulse(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    mal_result result = MAL_SUCCESS;

    mal_context_enumerate_devices_callback_data__pulse callbackData;
    callbackData.pContext = pContext;
    callbackData.callback = callback;
    callbackData.pUserData = pUserData;
    callbackData.isTerminated = MAL_FALSE;

    mal_pa_operation* pOP = NULL;

    mal_pa_mainloop* pMainLoop = ((mal_pa_mainloop_new_proc)pContext->pulse.pa_mainloop_new)();
    if (pMainLoop == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    mal_pa_mainloop_api* pAPI = ((mal_pa_mainloop_get_api_proc)pContext->pulse.pa_mainloop_get_api)(pMainLoop);
    if (pAPI == NULL) {
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    mal_pa_context* pPulseContext = ((mal_pa_context_new_proc)pContext->pulse.pa_context_new)(pAPI, pContext->config.pulse.pApplicationName);
    if (pPulseContext == NULL) {
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    int error = ((mal_pa_context_connect_proc)pContext->pulse.pa_context_connect)(pPulseContext, pContext->config.pulse.pServerName, 0, NULL);
    if (error != MAL_PA_OK) {
        ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)(pPulseContext);
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return mal_result_from_pulse(error);
    }

    while (((mal_pa_context_get_state_proc)pContext->pulse.pa_context_get_state)(pPulseContext) != MAL_PA_CONTEXT_READY) {
        error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)(pMainLoop, 1, NULL);
        if (error < 0) {
            result = mal_result_from_pulse(error);
            goto done;
        }
    }


    // Playback.
    if (!callbackData.isTerminated) {
        pOP = ((mal_pa_context_get_sink_info_list_proc)pContext->pulse.pa_context_get_sink_info_list)(pPulseContext, mal_context_enumerate_devices_sink_callback__pulse, &callbackData);
        if (pOP == NULL) {
            result = MAL_ERROR;
            goto done;
        }

        result = mal_wait_for_operation__pulse(pContext, pMainLoop, pOP);
        ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
        if (result != MAL_SUCCESS) {
            goto done;
        }
    }


    // Capture.
    if (!callbackData.isTerminated) {
        pOP = ((mal_pa_context_get_source_info_list_proc)pContext->pulse.pa_context_get_source_info_list)(pPulseContext, mal_context_enumerate_devices_source_callback__pulse, &callbackData);
        if (pOP == NULL) {
            result = MAL_ERROR;
            goto done;
        }

        result = mal_wait_for_operation__pulse(pContext, pMainLoop, pOP);
        ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
        if (result != MAL_SUCCESS) {
            goto done;
        }
    }

done:
    ((mal_pa_context_disconnect_proc)pContext->pulse.pa_context_disconnect)(pPulseContext);
    ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)(pPulseContext);
    ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
    return result;
}


typedef struct
{
    mal_device_info* pDeviceInfo;
    mal_bool32 foundDevice;
} mal_context_get_device_info_callback_data__pulse;

void mal_context_get_device_info_sink_callback__pulse(mal_pa_context* pPulseContext, const mal_pa_sink_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_context_get_device_info_callback_data__pulse* pData = (mal_context_get_device_info_callback_data__pulse*)pUserData;
    mal_assert(pData != NULL);
    pData->foundDevice = MAL_TRUE;

    if (pInfo->name != NULL) {
        mal_strncpy_s(pData->pDeviceInfo->id.pulse, sizeof(pData->pDeviceInfo->id.pulse), pInfo->name, (size_t)-1);
    }

    if (pInfo->description != NULL) {
        mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), pInfo->description, (size_t)-1);
    }
}

void mal_context_get_device_info_source_callback__pulse(mal_pa_context* pPulseContext, const mal_pa_source_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_context_get_device_info_callback_data__pulse* pData = (mal_context_get_device_info_callback_data__pulse*)pUserData;
    mal_assert(pData != NULL);
    pData->foundDevice = MAL_TRUE;

    if (pInfo->name != NULL) {
        mal_strncpy_s(pData->pDeviceInfo->id.pulse, sizeof(pData->pDeviceInfo->id.pulse), pInfo->name, (size_t)-1);
    }

    if (pInfo->description != NULL) {
        mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), pInfo->description, (size_t)-1);
    }
}

mal_result mal_context_get_device_info__pulse(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    mal_result result = MAL_SUCCESS;

    mal_context_get_device_info_callback_data__pulse callbackData;
    callbackData.pDeviceInfo = pDeviceInfo;
    callbackData.foundDevice = MAL_FALSE;

    mal_pa_operation* pOP = NULL;

    mal_pa_mainloop* pMainLoop = ((mal_pa_mainloop_new_proc)pContext->pulse.pa_mainloop_new)();
    if (pMainLoop == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    mal_pa_mainloop_api* pAPI = ((mal_pa_mainloop_get_api_proc)pContext->pulse.pa_mainloop_get_api)(pMainLoop);
    if (pAPI == NULL) {
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    mal_pa_context* pPulseContext = ((mal_pa_context_new_proc)pContext->pulse.pa_context_new)(pAPI, pContext->config.pulse.pApplicationName);
    if (pPulseContext == NULL) {
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    int error = ((mal_pa_context_connect_proc)pContext->pulse.pa_context_connect)(pPulseContext, pContext->config.pulse.pServerName, 0, NULL);
    if (error != MAL_PA_OK) {
        ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)(pPulseContext);
        ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
        return mal_result_from_pulse(error);
    }

    while (((mal_pa_context_get_state_proc)pContext->pulse.pa_context_get_state)(pPulseContext) != MAL_PA_CONTEXT_READY) {
        error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)(pMainLoop, 1, NULL);
        if (error < 0) {
            result = mal_result_from_pulse(error);
            goto done;
        }
    }

    if (deviceType == mal_device_type_playback) {
        pOP = ((mal_pa_context_get_sink_info_by_name_proc)pContext->pulse.pa_context_get_sink_info_by_name)(pPulseContext, pDeviceID->pulse, mal_context_get_device_info_sink_callback__pulse, &callbackData);
    } else {
        pOP = ((mal_pa_context_get_source_info_by_name_proc)pContext->pulse.pa_context_get_source_info_by_name)(pPulseContext, pDeviceID->pulse, mal_context_get_device_info_source_callback__pulse, &callbackData);
    }

    if (pOP != NULL) {
        mal_wait_for_operation__pulse(pContext, pMainLoop, pOP);
        ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
    } else {
        result = MAL_ERROR;
        goto done;
    }

    if (!callbackData.foundDevice) {
        result = MAL_NO_DEVICE;
        goto done;
    }


done:
    ((mal_pa_context_disconnect_proc)pContext->pulse.pa_context_disconnect)(pPulseContext);
    ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)(pPulseContext);
    ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)(pMainLoop);
    return result;
}


mal_result mal_context_init__pulse(mal_context* pContext)
{
    mal_assert(pContext != NULL);

#ifndef MAL_NO_RUNTIME_LINKING
    // libpulse.so
    const char* libpulseNames[] = {
        "libpulse.so",
        "libpulse.so.0"
    };

    for (size_t i = 0; i < mal_countof(libpulseNames); ++i) {
        pContext->pulse.pulseSO = mal_dlopen(libpulseNames[i]);
        if (pContext->pulse.pulseSO != NULL) {
            break;
        }
    }

    if (pContext->pulse.pulseSO == NULL) {
        return MAL_NO_BACKEND;
    }

    pContext->pulse.pa_mainloop_new                    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_mainloop_new");
    pContext->pulse.pa_mainloop_free                   = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_mainloop_free");
    pContext->pulse.pa_mainloop_get_api                = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_mainloop_get_api");
    pContext->pulse.pa_mainloop_iterate                = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_mainloop_iterate");
    pContext->pulse.pa_mainloop_wakeup                 = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_mainloop_wakeup");
    pContext->pulse.pa_context_new                     = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_new");
    pContext->pulse.pa_context_unref                   = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_unref");
    pContext->pulse.pa_context_connect                 = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_connect");
    pContext->pulse.pa_context_disconnect              = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_disconnect");
    pContext->pulse.pa_context_set_state_callback      = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_set_state_callback");
    pContext->pulse.pa_context_get_state               = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_get_state");
    pContext->pulse.pa_context_get_sink_info_list      = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_get_sink_info_list");
    pContext->pulse.pa_context_get_source_info_list    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_get_source_info_list");
    pContext->pulse.pa_context_get_sink_info_by_name   = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_get_sink_info_by_name");
    pContext->pulse.pa_context_get_source_info_by_name = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_context_get_source_info_by_name");
    pContext->pulse.pa_operation_unref                 = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_operation_unref");
    pContext->pulse.pa_operation_get_state             = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_operation_get_state");
    pContext->pulse.pa_channel_map_init_extend         = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_channel_map_init_extend");
    pContext->pulse.pa_channel_map_valid               = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_channel_map_valid");
    pContext->pulse.pa_channel_map_compatible          = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_channel_map_compatible");
    pContext->pulse.pa_stream_new                      = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_new");
    pContext->pulse.pa_stream_unref                    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_unref");
    pContext->pulse.pa_stream_connect_playback         = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_connect_playback");
    pContext->pulse.pa_stream_connect_record           = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_connect_record");
    pContext->pulse.pa_stream_disconnect               = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_disconnect");
    pContext->pulse.pa_stream_get_state                = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_get_state");
    pContext->pulse.pa_stream_get_sample_spec          = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_get_sample_spec");
    pContext->pulse.pa_stream_get_channel_map          = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_get_channel_map");
    pContext->pulse.pa_stream_get_buffer_attr          = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_get_buffer_attr");
    pContext->pulse.pa_stream_get_device_name          = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_get_device_name");
    pContext->pulse.pa_stream_set_write_callback       = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_set_write_callback");
    pContext->pulse.pa_stream_set_read_callback        = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_set_read_callback");
    pContext->pulse.pa_stream_flush                    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_flush");
    pContext->pulse.pa_stream_drain                    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_drain");
    pContext->pulse.pa_stream_cork                     = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_cork");
    pContext->pulse.pa_stream_trigger                  = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_trigger");
    pContext->pulse.pa_stream_begin_write              = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_begin_write");
    pContext->pulse.pa_stream_write                    = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_write");
    pContext->pulse.pa_stream_peek                     = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_peek");
    pContext->pulse.pa_stream_drop                     = (mal_proc)mal_dlsym(pContext->pulse.pulseSO, "pa_stream_drop");
#else
    // This strange assignment system is just for type safety.
    mal_pa_mainloop_new_proc                    _pa_mainloop_new                   = pa_mainloop_new;
    mal_pa_mainloop_free_proc                   _pa_mainloop_free                  = pa_mainloop_free;
    mal_pa_mainloop_get_api_proc                _pa_mainloop_get_api               = pa_mainloop_get_api;
    mal_pa_mainloop_iterate_proc                _pa_mainloop_iterate               = pa_mainloop_iterate;
    mal_pa_mainloop_wakeup_proc                 _pa_mainloop_wakeup                = pa_mainloop_wakeup;
    mal_pa_context_new_proc                     _pa_context_new                    = pa_context_new;
    mal_pa_context_unref_proc                   _pa_context_unref                  = pa_context_unref;
    mal_pa_context_connect_proc                 _pa_context_connect                = pa_context_connect;
    mal_pa_context_disconnect_proc              _pa_context_disconnect             = pa_context_disconnect;
    mal_pa_context_set_state_callback_proc      _pa_context_set_state_callback     = pa_context_set_state_callback;
    mal_pa_context_get_state_proc               _pa_context_get_state              = pa_context_get_state;
    mal_pa_context_get_sink_info_list_proc      _pa_context_get_sink_info_list     = pa_context_get_sink_info_list;
    mal_pa_context_get_source_info_list_proc    _pa_context_get_source_info_list   = pa_context_get_source_info_list;
    mal_pa_context_get_sink_info_by_name_proc   _pa_context_get_sink_info_by_name  = pa_context_get_sink_info_by_name;
    mal_pa_context_get_source_info_by_name_proc _pa_context_get_source_info_by_name= pa_context_get_source_info_by_name;
    mal_pa_operation_unref_proc                 _pa_operation_unref                = pa_operation_unref;
    mal_pa_operation_get_state_proc             _pa_operation_get_state            = pa_operation_get_state;
    mal_pa_channel_map_init_extend_proc         _pa_channel_map_init_extend        = pa_channel_map_init_extend;
    mal_pa_channel_map_valid_proc               _pa_channel_map_valid              = pa_channel_map_valid;
    mal_pa_channel_map_compatible_proc          _pa_channel_map_compatible         = pa_channel_map_compatible;
    mal_pa_stream_new_proc                      _pa_stream_new                     = pa_stream_new;
    mal_pa_stream_unref_proc                    _pa_stream_unref                   = pa_stream_unref;
    mal_pa_stream_connect_playback_proc         _pa_stream_connect_playback        = pa_stream_connect_playback;
    mal_pa_stream_connect_record_proc           _pa_stream_connect_record          = pa_stream_connect_record;
    mal_pa_stream_disconnect_proc               _pa_stream_disconnect              = pa_stream_disconnect;
    mal_pa_stream_get_state_proc                _pa_stream_get_state               = pa_stream_get_state;
    mal_pa_stream_get_sample_spec_proc          _pa_stream_get_sample_spec         = pa_stream_get_sample_spec;
    mal_pa_stream_get_channel_map_proc          _pa_stream_get_channel_map         = pa_stream_get_channel_map;
    mal_pa_stream_get_buffer_attr_proc          _pa_stream_get_buffer_attr         = pa_stream_get_buffer_attr;
    mal_pa_stream_get_device_name_proc          _pa_stream_get_device_name         = pa_stream_get_device_name;
    mal_pa_stream_set_write_callback_proc       _pa_stream_set_write_callback      = pa_stream_set_write_callback;
    mal_pa_stream_set_read_callback_proc        _pa_stream_set_read_callback       = pa_stream_set_read_callback;
    mal_pa_stream_flush_proc                    _pa_stream_flush                   = pa_stream_flush;
    mal_pa_stream_drain_proc                    _pa_stream_drain                   = pa_stream_drain;
    mal_pa_stream_cork_proc                     _pa_stream_cork                    = pa_stream_cork;
    mal_pa_stream_trigger_proc                  _pa_stream_trigger                 = pa_stream_trigger;
    mal_pa_stream_begin_write_proc              _pa_stream_begin_write             = pa_stream_begin_write;
    mal_pa_stream_write_proc                    _pa_stream_write                   = pa_stream_write;
    mal_pa_stream_peek_proc                     _pa_stream_peek                    = pa_stream_peek;
    mal_pa_stream_drop_proc                     _pa_stream_drop                    = pa_stream_drop;

    pContext->pulse.pa_mainloop_new                    = (mal_proc)_pa_mainloop_new;
    pContext->pulse.pa_mainloop_free                   = (mal_proc)_pa_mainloop_free;
    pContext->pulse.pa_mainloop_get_api                = (mal_proc)_pa_mainloop_get_api;
    pContext->pulse.pa_mainloop_iterate                = (mal_proc)_pa_mainloop_iterate;
    pContext->pulse.pa_mainloop_wakeup                 = (mal_proc)_pa_mainloop_wakeup;
    pContext->pulse.pa_context_new                     = (mal_proc)_pa_context_new;
    pContext->pulse.pa_context_unref                   = (mal_proc)_pa_context_unref;
    pContext->pulse.pa_context_connect                 = (mal_proc)_pa_context_connect;
    pContext->pulse.pa_context_disconnect              = (mal_proc)_pa_context_disconnect;
    pContext->pulse.pa_context_set_state_callback      = (mal_proc)_pa_context_set_state_callback;
    pContext->pulse.pa_context_get_state               = (mal_proc)_pa_context_get_state;
    pContext->pulse.pa_context_get_sink_info_list      = (mal_proc)_pa_context_get_sink_info_list;
    pContext->pulse.pa_context_get_source_info_list    = (mal_proc)_pa_context_get_source_info_list;
    pContext->pulse.pa_context_get_sink_info_by_name   = (mal_proc)_pa_context_get_sink_info_by_name;
    pContext->pulse.pa_context_get_source_info_by_name = (mal_proc)_pa_context_get_source_info_by_name;
    pContext->pulse.pa_operation_unref                 = (mal_proc)_pa_operation_unref;
    pContext->pulse.pa_operation_get_state             = (mal_proc)_pa_operation_get_state;
    pContext->pulse.pa_channel_map_init_extend         = (mal_proc)_pa_channel_map_init_extend;
    pContext->pulse.pa_channel_map_valid               = (mal_proc)_pa_channel_map_valid;
    pContext->pulse.pa_channel_map_compatible          = (mal_proc)_pa_channel_map_compatible;
    pContext->pulse.pa_stream_new                      = (mal_proc)_pa_stream_new;
    pContext->pulse.pa_stream_unref                    = (mal_proc)_pa_stream_unref;
    pContext->pulse.pa_stream_connect_playback         = (mal_proc)_pa_stream_connect_playback;
    pContext->pulse.pa_stream_connect_record           = (mal_proc)_pa_stream_connect_record;
    pContext->pulse.pa_stream_disconnect               = (mal_proc)_pa_stream_disconnect;
    pContext->pulse.pa_stream_get_state                = (mal_proc)_pa_stream_get_state;
    pContext->pulse.pa_stream_get_sample_spec          = (mal_proc)_pa_stream_get_sample_spec;
    pContext->pulse.pa_stream_get_channel_map          = (mal_proc)_pa_stream_get_channel_map;
    pContext->pulse.pa_stream_get_buffer_attr          = (mal_proc)_pa_stream_get_buffer_attr;
    pContext->pulse.pa_stream_get_device_name          = (mal_proc)_pa_stream_get_device_name;
    pContext->pulse.pa_stream_set_write_callback       = (mal_proc)_pa_stream_set_write_callback;
    pContext->pulse.pa_stream_set_read_callback        = (mal_proc)_pa_stream_set_read_callback;
    pContext->pulse.pa_stream_flush                    = (mal_proc)_pa_stream_flush;
    pContext->pulse.pa_stream_drain                    = (mal_proc)_pa_stream_drain;
    pContext->pulse.pa_stream_cork                     = (mal_proc)_pa_stream_cork;
    pContext->pulse.pa_stream_trigger                  = (mal_proc)_pa_stream_trigger;
    pContext->pulse.pa_stream_begin_write              = (mal_proc)_pa_stream_begin_write;
    pContext->pulse.pa_stream_write                    = (mal_proc)_pa_stream_write;
    pContext->pulse.pa_stream_peek                     = (mal_proc)_pa_stream_peek;
    pContext->pulse.pa_stream_drop                     = (mal_proc)_pa_stream_drop;
#endif

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__pulse;
    pContext->onEnumDevices   = mal_context_enumerate_devices__pulse;
    pContext->onGetDeviceInfo = mal_context_get_device_info__pulse;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__pulse(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_pulseaudio);

#ifndef MAL_NO_RUNTIME_LINKING
    mal_dlclose(pContext->pulse.pulseSO);
#endif

    return MAL_SUCCESS;
}


void mal_pulse_device_state_callback(mal_pa_context* pPulseContext, void* pUserData)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    pDevice->pulse.pulseContextState = ((mal_pa_context_get_state_proc)pContext->pulse.pa_context_get_state)(pPulseContext);
}

void mal_pulse_device_write_callback(mal_pa_stream* pStream, size_t sizeInBytes, void* pUserData)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    size_t bytesRemaining = sizeInBytes;
    while (bytesRemaining > 0) {
        size_t bytesToReadFromClient = bytesRemaining;
        if (bytesToReadFromClient > 0xFFFFFFFF) {
            bytesToReadFromClient = 0xFFFFFFFF;
        }

        void* pBuffer = NULL;
        int error = ((mal_pa_stream_begin_write_proc)pContext->pulse.pa_stream_begin_write)((mal_pa_stream*)pDevice->pulse.pStream, &pBuffer, &bytesToReadFromClient);
        if (error < 0) {
            mal_post_error(pDevice, "[PulseAudio] Failed to retrieve write buffer for sending data to the device.", mal_result_from_pulse(error));
            return;
        }

        if (pBuffer != NULL && bytesToReadFromClient > 0) {
            mal_uint32 framesToReadFromClient = (mal_uint32)bytesToReadFromClient / (pDevice->internalChannels*mal_get_sample_size_in_bytes(pDevice->internalFormat));
            if (framesToReadFromClient > 0) {
                mal_device__read_frames_from_client(pDevice, framesToReadFromClient, pBuffer);

                error = ((mal_pa_stream_write_proc)pContext->pulse.pa_stream_write)((mal_pa_stream*)pDevice->pulse.pStream, pBuffer, bytesToReadFromClient, NULL, 0, MAL_PA_SEEK_RELATIVE);
                if (error < 0) {
                    mal_post_error(pDevice, "[PulseAudio] Failed to write data to the PulseAudio stream.", mal_result_from_pulse(error));
                    return;
                }
            }

            bytesRemaining -= bytesToReadFromClient;
        }
    }
}

void mal_pulse_device_read_callback(mal_pa_stream* pStream, size_t sizeInBytes, void* pUserData)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    size_t bytesRemaining = sizeInBytes;
    while (bytesRemaining > 0) {
        size_t bytesToSendToClient = bytesRemaining;
        if (bytesToSendToClient > 0xFFFFFFFF) {
            bytesToSendToClient = 0xFFFFFFFF;
        }

        const void* pBuffer = NULL;
        int error = ((mal_pa_stream_peek_proc)pContext->pulse.pa_stream_peek)((mal_pa_stream*)pDevice->pulse.pStream, &pBuffer, &sizeInBytes);
        if (error < 0) {
            mal_post_error(pDevice, "[PulseAudio] Failed to retrieve read buffer for reading data from the device.", mal_result_from_pulse(error));
            return;
        }

        if (pBuffer != NULL) {
            mal_uint32 framesToSendToClient = (mal_uint32)bytesToSendToClient / (pDevice->internalChannels*mal_get_sample_size_in_bytes(pDevice->internalFormat));
            if (framesToSendToClient > 0) {
                mal_device__send_frames_to_client(pDevice, framesToSendToClient, pBuffer);
            }
        }

        error = ((mal_pa_stream_drop_proc)pContext->pulse.pa_stream_drop)((mal_pa_stream*)pDevice->pulse.pStream);
        if (error < 0) {
            mal_post_error(pDevice, "[PulseAudio] Failed to drop fragment from the PulseAudio stream.", mal_result_from_pulse(error));
        }

        bytesRemaining -= bytesToSendToClient;
    }
}

void mal_device_sink_info_callback(mal_pa_context* pPulseContext, const mal_pa_sink_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_pa_sink_info* pInfoOut = (mal_pa_sink_info*)pUserData;
    mal_assert(pInfoOut != NULL);

    *pInfoOut = *pInfo;
}

void mal_device_source_info_callback(mal_pa_context* pPulseContext, const mal_pa_source_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_pa_source_info* pInfoOut = (mal_pa_source_info*)pUserData;
    mal_assert(pInfoOut != NULL);

    *pInfoOut = *pInfo;
}

void mal_device_sink_name_callback(mal_pa_context* pPulseContext, const mal_pa_sink_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), pInfo->description, (size_t)-1);
}

void mal_device_source_name_callback(mal_pa_context* pPulseContext, const mal_pa_source_info* pInfo, int endOfList, void* pUserData)
{
    if (endOfList > 0) {
        return;
    }

    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), pInfo->description, (size_t)-1);
}

void mal_device_uninit__pulse(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    ((mal_pa_stream_disconnect_proc)pContext->pulse.pa_stream_disconnect)((mal_pa_stream*)pDevice->pulse.pStream);
    ((mal_pa_stream_unref_proc)pContext->pulse.pa_stream_unref)((mal_pa_stream*)pDevice->pulse.pStream);
    ((mal_pa_context_disconnect_proc)pContext->pulse.pa_context_disconnect)((mal_pa_context*)pDevice->pulse.pPulseContext);
    ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)((mal_pa_context*)pDevice->pulse.pPulseContext);
    ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)((mal_pa_mainloop*)pDevice->pulse.pMainLoop);
}

mal_result mal_device_init__pulse(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->pulse);

    mal_result result = MAL_SUCCESS;
    int error = 0;

    const char* dev = NULL;
    if (pDeviceID != NULL) {
        dev = pDeviceID->pulse;
    }

    mal_pa_sink_info sinkInfo;
    mal_pa_source_info sourceInfo;
    mal_pa_operation* pOP = NULL;

    mal_pa_sample_spec ss;
    mal_pa_channel_map cmap;
    mal_pa_buffer_attr attr;

    const mal_pa_sample_spec* pActualSS   = NULL;
    const mal_pa_channel_map* pActualCMap = NULL;
    const mal_pa_buffer_attr* pActualAttr = NULL;



    pDevice->pulse.pMainLoop = ((mal_pa_mainloop_new_proc)pContext->pulse.pa_mainloop_new)();
    if (pDevice->pulse.pMainLoop == NULL) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to create main loop for device.", MAL_FAILED_TO_INIT_BACKEND);
        goto on_error0;
    }

    pDevice->pulse.pAPI = ((mal_pa_mainloop_get_api_proc)pContext->pulse.pa_mainloop_get_api)((mal_pa_mainloop*)pDevice->pulse.pMainLoop);
    if (pDevice->pulse.pAPI == NULL) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to retrieve PulseAudio main loop.", MAL_FAILED_TO_INIT_BACKEND);
        goto on_error1;
    }

    pDevice->pulse.pPulseContext = ((mal_pa_context_new_proc)pContext->pulse.pa_context_new)((mal_pa_mainloop_api*)pDevice->pulse.pAPI, pContext->config.pulse.pApplicationName);
    if (pDevice->pulse.pPulseContext == NULL) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to create PulseAudio context for device.", MAL_FAILED_TO_INIT_BACKEND);
        goto on_error1;
    }

    error = ((mal_pa_context_connect_proc)pContext->pulse.pa_context_connect)((mal_pa_context*)pDevice->pulse.pPulseContext, pContext->config.pulse.pServerName, (pContext->config.pulse.tryAutoSpawn) ? 0 : MAL_PA_CONTEXT_NOAUTOSPAWN, NULL);
    if (error != MAL_PA_OK) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to connect PulseAudio context.", mal_result_from_pulse(error));
        goto on_error2;
    }


    pDevice->pulse.pulseContextState = MAL_PA_CONTEXT_UNCONNECTED;
    ((mal_pa_context_set_state_callback_proc)pContext->pulse.pa_context_set_state_callback)((mal_pa_context*)pDevice->pulse.pPulseContext, mal_pulse_device_state_callback, pDevice);

    // Wait for PulseAudio to get itself ready before returning.
    for (;;) {
        if (pDevice->pulse.pulseContextState == MAL_PA_CONTEXT_READY) {
            break;
        } else {
            error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)((mal_pa_mainloop*)pDevice->pulse.pMainLoop, 1, NULL);    // 1 = block.
            if (error < 0) {
                result = mal_post_error(pDevice, "[PulseAudio] The PulseAudio main loop returned an error while connecting the PulseAudio context.", mal_result_from_pulse(error));
                goto on_error3;
            }
            continue;
        }

        // An error may have occurred.
        if (pDevice->pulse.pulseContextState == MAL_PA_CONTEXT_FAILED || pDevice->pulse.pulseContextState == MAL_PA_CONTEXT_TERMINATED) {
            result = mal_post_error(pDevice, "[PulseAudio] An error occurred while connecting the PulseAudio context.", MAL_ERROR);
            goto on_error3;
        }

        error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)((mal_pa_mainloop*)pDevice->pulse.pMainLoop, 1, NULL);
        if (error < 0) {
            result = mal_post_error(pDevice, "[PulseAudio] The PulseAudio main loop returned an error while connecting the PulseAudio context.", mal_result_from_pulse(error));
            goto on_error3;
        }
    }

    
    if (type == mal_device_type_playback) {
        pOP = ((mal_pa_context_get_sink_info_by_name_proc)pContext->pulse.pa_context_get_sink_info_by_name)((mal_pa_context*)pDevice->pulse.pPulseContext, dev, mal_device_sink_info_callback, &sinkInfo);
    } else {
        pOP = ((mal_pa_context_get_source_info_by_name_proc)pContext->pulse.pa_context_get_source_info_by_name)((mal_pa_context*)pDevice->pulse.pPulseContext, dev, mal_device_source_info_callback, &sourceInfo);
    }

    if (pOP != NULL) {
        mal_device__wait_for_operation__pulse(pDevice, pOP);
        ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
    }


#if 0
    mal_pa_sample_spec deviceSS;
    mal_pa_channel_map deviceCMap;
    if (type == mal_device_type_playback) {
        deviceSS = sinkInfo.sample_spec;
        deviceCMap = sinkInfo.channel_map;
    } else {
        deviceSS = sourceInfo.sample_spec;
        deviceCMap = sourceInfo.channel_map;
    }

    if (pDevice->usingDefaultFormat) {
        ss.format = deviceSS.format;
    } else {
        ss.format = mal_format_to_pulse(pConfig->format);
    }
    if (ss.format == MAL_PA_SAMPLE_INVALID) {
        ss.format = MAL_PA_SAMPLE_S16LE;
    }

    if (pDevice->usingDefaultChannels) {
        ss.channels = deviceSS.channels;
    } else {
        ss.channels = pConfig->channels;
    }

    if (pDevice->usingDefaultSampleRate) {
        ss.rate = deviceSS.rate;
    } else {
        ss.rate = pConfig->sampleRate;
    }


    if (pDevice->usingDefaultChannelMap) {
        cmap = deviceCMap;
    } else {
        cmap.channels = pConfig->channels;
        for (mal_uint32 iChannel = 0; iChannel < pConfig->channels; ++iChannel) {
            cmap.map[iChannel] = mal_channel_position_to_pulse(pConfig->channelMap[iChannel]);
        }

        if (((mal_pa_channel_map_valid_proc)pContext->pulse.pa_channel_map_valid)(&cmap) == 0 || ((mal_pa_channel_map_compatible_proc)pContext->pulse.pa_channel_map_compatible)(&cmap, &ss) == 0) {
            ((mal_pa_channel_map_init_extend_proc)pContext->pulse.pa_channel_map_init_extend)(&cmap, ss.channels, MAL_PA_CHANNEL_MAP_DEFAULT);     // The channel map is invalid, so just fall back to the default.
        }
    }
#else
    if (type == mal_device_type_playback) {
        ss = sinkInfo.sample_spec;
        cmap = sinkInfo.channel_map;
    } else {
        ss = sourceInfo.sample_spec;
        cmap = sourceInfo.channel_map;
    }
#endif

    attr.maxlength = pConfig->bufferSizeInFrames * mal_get_sample_size_in_bytes(mal_format_from_pulse(ss.format))*ss.channels;
    attr.tlength   = attr.maxlength / pConfig->periods;
    attr.prebuf    = (mal_uint32)-1;
    attr.minreq    = attr.tlength;
    attr.fragsize  = attr.tlength;

    char streamName[256];
    if (pConfig->pulse.pStreamName != NULL) {
        mal_strncpy_s(streamName, sizeof(streamName), pConfig->pulse.pStreamName, (size_t)-1);
    } else {
        static int g_StreamCounter = 0;
        mal_strcpy_s(streamName, sizeof(streamName), "mini_al:");
        mal_itoa_s(g_StreamCounter, streamName + 8, sizeof(streamName)-8, 10);  // 8 = strlen("mini_al:")
        g_StreamCounter += 1;
    }

    pDevice->pulse.pStream = ((mal_pa_stream_new_proc)pContext->pulse.pa_stream_new)((mal_pa_context*)pDevice->pulse.pPulseContext, streamName, &ss, &cmap);
    if (pDevice->pulse.pStream == NULL) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to create PulseAudio stream.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        goto on_error3;
    }



    if (type == mal_device_type_playback) {
        error = ((mal_pa_stream_connect_playback_proc)pContext->pulse.pa_stream_connect_playback)((mal_pa_stream*)pDevice->pulse.pStream, dev, &attr, MAL_PA_STREAM_START_CORKED, NULL, NULL);
    } else {
        error = ((mal_pa_stream_connect_record_proc)pContext->pulse.pa_stream_connect_record)((mal_pa_stream*)pDevice->pulse.pStream, dev, &attr, MAL_PA_STREAM_START_CORKED);
    }

    if (error != MAL_PA_OK) {
        result = mal_post_error(pDevice, "[PulseAudio] Failed to connect PulseAudio stream.", mal_result_from_pulse(error));
        goto on_error4;
    }

    while (((mal_pa_stream_get_state_proc)pContext->pulse.pa_stream_get_state)((mal_pa_stream*)pDevice->pulse.pStream) != MAL_PA_STREAM_READY) {
        error = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)((mal_pa_mainloop*)pDevice->pulse.pMainLoop, 1, NULL);
        if (error < 0) {
            result = mal_post_error(pDevice, "[PulseAudio] The PulseAudio main loop returned an error while connecting the PulseAudio stream.", mal_result_from_pulse(error));
            goto on_error5;
        }
    }


    // Internal format.
    pActualSS = ((mal_pa_stream_get_sample_spec_proc)pContext->pulse.pa_stream_get_sample_spec)((mal_pa_stream*)pDevice->pulse.pStream);
    if (pActualSS != NULL) {
        ss = *pActualSS;
    }

    pDevice->internalFormat = mal_format_from_pulse(ss.format);
    pDevice->internalChannels = ss.channels;
    pDevice->internalSampleRate = ss.rate;


    // Internal channel map.
    pActualCMap = ((mal_pa_stream_get_channel_map_proc)pContext->pulse.pa_stream_get_channel_map)((mal_pa_stream*)pDevice->pulse.pStream);
    if (pActualCMap != NULL) {
        cmap = *pActualCMap;
    }

    for (mal_uint32 iChannel = 0; iChannel < pDevice->internalChannels; ++iChannel) {
        pDevice->internalChannelMap[iChannel] = mal_channel_position_from_pulse(cmap.map[iChannel]);
    }


    // Buffer size.
    pActualAttr = ((mal_pa_stream_get_buffer_attr_proc)pContext->pulse.pa_stream_get_buffer_attr)((mal_pa_stream*)pDevice->pulse.pStream);
    if (pActualAttr != NULL) {
        attr = *pActualAttr;
    }

    pDevice->bufferSizeInFrames = attr.maxlength / (mal_get_sample_size_in_bytes(pDevice->internalFormat)*pDevice->internalChannels);
    pDevice->periods = attr.maxlength / attr.tlength;


    // Grab the name of the device if we can.
    dev = ((mal_pa_stream_get_device_name_proc)pContext->pulse.pa_stream_get_device_name)((mal_pa_stream*)pDevice->pulse.pStream);
    if (dev != NULL) {
        mal_pa_operation* pOP = NULL;
        if (type == mal_device_type_playback) {
            pOP = ((mal_pa_context_get_sink_info_by_name_proc)pContext->pulse.pa_context_get_sink_info_by_name)((mal_pa_context*)pDevice->pulse.pPulseContext, dev, mal_device_sink_name_callback, pDevice);
        } else {
            pOP = ((mal_pa_context_get_source_info_by_name_proc)pContext->pulse.pa_context_get_source_info_by_name)((mal_pa_context*)pDevice->pulse.pPulseContext, dev, mal_device_source_name_callback, pDevice);
        }

        if (pOP != NULL) {
            mal_device__wait_for_operation__pulse(pDevice, pOP);
            ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
        }
    }


    // Set callbacks for reading and writing data to/from the PulseAudio stream.
    if (type == mal_device_type_playback) {
        ((mal_pa_stream_set_write_callback_proc)pContext->pulse.pa_stream_set_write_callback)((mal_pa_stream*)pDevice->pulse.pStream, mal_pulse_device_write_callback, pDevice);
    } else {
        ((mal_pa_stream_set_read_callback_proc)pContext->pulse.pa_stream_set_read_callback)((mal_pa_stream*)pDevice->pulse.pStream, mal_pulse_device_read_callback, pDevice);
    }


    pDevice->pulse.fragmentSizeInBytes = attr.tlength;

    return MAL_SUCCESS;


on_error5: ((mal_pa_stream_disconnect_proc)pContext->pulse.pa_stream_disconnect)((mal_pa_stream*)pDevice->pulse.pStream);
on_error4: ((mal_pa_stream_unref_proc)pContext->pulse.pa_stream_unref)((mal_pa_stream*)pDevice->pulse.pStream);
on_error3: ((mal_pa_context_disconnect_proc)pContext->pulse.pa_context_disconnect)((mal_pa_context*)pDevice->pulse.pPulseContext);
on_error2: ((mal_pa_context_unref_proc)pContext->pulse.pa_context_unref)((mal_pa_context*)pDevice->pulse.pPulseContext);
on_error1: ((mal_pa_mainloop_free_proc)pContext->pulse.pa_mainloop_free)((mal_pa_mainloop*)pDevice->pulse.pMainLoop);
on_error0:
    return result;
}


void mal_pulse_operation_complete_callback(mal_pa_stream* pStream, int success, void* pUserData)
{
    mal_bool32* pIsSuccessful = (mal_bool32*)pUserData;
    mal_assert(pIsSuccessful != NULL);

    *pIsSuccessful = (mal_bool32)success;
}

mal_result mal_device__cork_stream__pulse(mal_device* pDevice, int cork)
{
    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    mal_bool32 wasSuccessful = MAL_FALSE;
    mal_pa_operation* pOP = ((mal_pa_stream_cork_proc)pContext->pulse.pa_stream_cork)((mal_pa_stream*)pDevice->pulse.pStream, cork, mal_pulse_operation_complete_callback, &wasSuccessful);
    if (pOP == NULL) {
        return mal_post_error(pDevice, "[PulseAudio] Failed to cork PulseAudio stream.", (cork == 0) ? MAL_FAILED_TO_START_BACKEND_DEVICE : MAL_FAILED_TO_STOP_BACKEND_DEVICE);
    }

    mal_result result = mal_device__wait_for_operation__pulse(pDevice, pOP);
    ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);

    if (result != MAL_SUCCESS) {
        return mal_post_error(pDevice, "[PulseAudio] An error occurred while waiting for the PulseAudio stream to cork.", result);
    }

    if (!wasSuccessful) {
        if (cork) {
            return mal_post_error(pDevice, "[PulseAudio] Failed to stop PulseAudio stream.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
        } else {
            return mal_post_error(pDevice, "[PulseAudio] Failed to start PulseAudio stream.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__start_backend__pulse(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    // For both playback and capture we need to uncork the stream. Afterwards, for playback we need to fill in an initial chunk
    // of data, equal to the trigger length. That should then start actual playback.
    mal_result result = mal_device__cork_stream__pulse(pDevice, 0);
    if (result != MAL_SUCCESS) {
        return result;
    }

    // A playback device is started by simply writing data to it. For capture we do nothing.
    if (pDevice->type == mal_device_type_playback) {
        // Playback.
        mal_pulse_device_write_callback((mal_pa_stream*)pDevice->pulse.pStream, pDevice->pulse.fragmentSizeInBytes, pDevice);

        // Force an immediate start of the device just to be sure.
        mal_pa_operation* pOP = ((mal_pa_stream_trigger_proc)pContext->pulse.pa_stream_trigger)((mal_pa_stream*)pDevice->pulse.pStream, NULL, NULL);
        if (pOP != NULL) {
             mal_device__wait_for_operation__pulse(pDevice, pOP);
            ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);
        }
    } else {
        // Capture. Do nothing.
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__pulse(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    mal_result result = mal_device__cork_stream__pulse(pDevice, 1);
    if (result != MAL_SUCCESS) {
        return result;
    }

    // For playback, buffers need to be flushed. For capture they need to be drained.
    mal_bool32 wasSuccessful;
    mal_pa_operation* pOP = NULL;
    if (pDevice->type == mal_device_type_playback) {
        pOP = ((mal_pa_stream_flush_proc)pContext->pulse.pa_stream_flush)((mal_pa_stream*)pDevice->pulse.pStream, mal_pulse_operation_complete_callback, &wasSuccessful);
    } else {
        pOP = ((mal_pa_stream_drain_proc)pContext->pulse.pa_stream_drain)((mal_pa_stream*)pDevice->pulse.pStream, mal_pulse_operation_complete_callback, &wasSuccessful);
    }

    if (pOP == NULL) {
        return mal_post_error(pDevice, "[PulseAudio] Failed to flush buffers after stopping PulseAudio stream.", MAL_ERROR);
    }

    result = mal_device__wait_for_operation__pulse(pDevice, pOP);
    ((mal_pa_operation_unref_proc)pContext->pulse.pa_operation_unref)(pOP);

    if (result != MAL_SUCCESS) {
        return mal_post_error(pDevice, "[PulseAudio] An error occurred while waiting for the PulseAudio stream to flush.", result);
    }

    if (!wasSuccessful) {
        return mal_post_error(pDevice, "[PulseAudio] Failed to flush buffers after stopping PulseAudio stream.", MAL_ERROR);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__pulse(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    pDevice->pulse.breakFromMainLoop = MAL_TRUE;
    ((mal_pa_mainloop_wakeup_proc)pContext->pulse.pa_mainloop_wakeup)((mal_pa_mainloop*)pDevice->pulse.pMainLoop);

    return MAL_SUCCESS;
}

mal_result mal_device__main_loop__pulse(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    pDevice->pulse.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->pulse.breakFromMainLoop) {
        // Break from the main loop if the device isn't started anymore. Likely what's happened is the application
        // has requested that the device be stopped.
        if (!mal_device_is_started(pDevice)) {
            break;
        }

        int resultPA = ((mal_pa_mainloop_iterate_proc)pContext->pulse.pa_mainloop_iterate)((mal_pa_mainloop*)pDevice->pulse.pMainLoop, 1, NULL);
        if (resultPA < 0) {
            break;  // Some error occurred.
        }
    }

    return MAL_SUCCESS;
}
#endif


///////////////////////////////////////////////////////////////////////////////
//
// JACK Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_JACK

// It is assumed jack.h is available when compile-time linking is being used.
#ifdef MAL_NO_RUNTIME_LINKING
#include <jack/jack.h>

typedef jack_nframes_t              mal_jack_nframes_t;
typedef jack_options_t              mal_jack_options_t;
typedef jack_status_t               mal_jack_status_t;
typedef jack_client_t               mal_jack_client_t;
typedef jack_port_t                 mal_jack_port_t;
typedef JackProcessCallback         mal_JackProcessCallback;
typedef JackBufferSizeCallback      mal_JackBufferSizeCallback;
typedef JackShutdownCallback        mal_JackShutdownCallback;
#define MAL_JACK_DEFAULT_AUDIO_TYPE JACK_DEFAULT_AUDIO_TYPE
#define mal_JackNoStartServer       JackNoStartServer
#define mal_JackPortIsInput         JackPortIsInput
#define mal_JackPortIsOutput        JackPortIsOutput
#define mal_JackPortIsPhysical      JackPortIsPhysical
#else
typedef mal_uint32               mal_jack_nframes_t;
typedef int                      mal_jack_options_t;
typedef int                      mal_jack_status_t;
typedef struct mal_jack_client_t mal_jack_client_t;
typedef struct mal_jack_port_t   mal_jack_port_t;
typedef int  (* mal_JackProcessCallback)   (mal_jack_nframes_t nframes, void* arg);
typedef int  (* mal_JackBufferSizeCallback)(mal_jack_nframes_t nframes, void* arg);
typedef void (* mal_JackShutdownCallback)  (void* arg);
#define MAL_JACK_DEFAULT_AUDIO_TYPE "32 bit float mono audio"
#define mal_JackNoStartServer       1
#define mal_JackPortIsInput         1
#define mal_JackPortIsOutput        2
#define mal_JackPortIsPhysical      4
#endif

typedef mal_jack_client_t* (* mal_jack_client_open_proc)             (const char* client_name, mal_jack_options_t options, mal_jack_status_t* status, ...);
typedef int                (* mal_jack_client_close_proc)            (mal_jack_client_t* client);
typedef int                (* mal_jack_client_name_size_proc)        ();
typedef int                (* mal_jack_set_process_callback_proc)    (mal_jack_client_t* client, mal_JackProcessCallback process_callback, void* arg);
typedef int                (* mal_jack_set_buffer_size_callback_proc)(mal_jack_client_t* client, mal_JackBufferSizeCallback bufsize_callback, void* arg);
typedef void               (* mal_jack_on_shutdown_proc)             (mal_jack_client_t* client, mal_JackShutdownCallback function, void* arg);
typedef mal_jack_nframes_t (* mal_jack_get_sample_rate_proc)         (mal_jack_client_t* client);
typedef mal_jack_nframes_t (* mal_jack_get_buffer_size_proc)         (mal_jack_client_t* client);
typedef const char**       (* mal_jack_get_ports_proc)               (mal_jack_client_t* client, const char* port_name_pattern, const char* type_name_pattern, unsigned long flags);
typedef int                (* mal_jack_activate_proc)                (mal_jack_client_t* client);
typedef int                (* mal_jack_deactivate_proc)              (mal_jack_client_t* client);
typedef int                (* mal_jack_connect_proc)                 (mal_jack_client_t* client, const char* source_port, const char* destination_port);
typedef mal_jack_port_t*   (* mal_jack_port_register_proc)           (mal_jack_client_t* client, const char* port_name, const char* port_type, unsigned long flags, unsigned long buffer_size);
typedef const char*        (* mal_jack_port_name_proc)               (const mal_jack_port_t* port);
typedef void*              (* mal_jack_port_get_buffer_proc)         (mal_jack_port_t* port, mal_jack_nframes_t nframes);
typedef void               (* mal_jack_free_proc)                    (void* ptr);

mal_bool32 mal_context_is_device_id_equal__jack(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return pID0->jack == pID1->jack;
}

mal_result mal_context_enumerate_devices__jack(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    mal_bool32 cbResult = MAL_TRUE;

    // Playback.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
    }

    // Capture.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__jack(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    if (pDeviceID != NULL && pDeviceID->jack != 0) {
        return MAL_NO_DEVICE;   // Don't know the device.
    }

    // Name / Description
    if (deviceType == mal_device_type_playback) {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
    } else {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_init__jack(mal_context* pContext)
{
    mal_assert(pContext != NULL);

#ifndef MAL_NO_RUNTIME_LINKING
    // libjack.so
    const char* libjackNames[] = {
#ifdef MAL_WIN32
        "libjack.dll"
#else
        "libjack.so",
        "libjack.so.0"
#endif
    };

    for (size_t i = 0; i < mal_countof(libjackNames); ++i) {
        pContext->jack.jackSO = mal_dlopen(libjackNames[i]);
        if (pContext->jack.jackSO != NULL) {
            break;
        }
    }

    if (pContext->jack.jackSO == NULL) {
        return MAL_NO_BACKEND;
    }

    pContext->jack.jack_client_open              = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_client_open");
    pContext->jack.jack_client_close             = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_client_close");
    pContext->jack.jack_client_name_size         = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_client_name_size");
    pContext->jack.jack_set_process_callback     = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_set_process_callback");
    pContext->jack.jack_set_buffer_size_callback = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_set_buffer_size_callback");
    pContext->jack.jack_on_shutdown              = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_on_shutdown");
    pContext->jack.jack_get_sample_rate          = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_get_sample_rate");
    pContext->jack.jack_get_buffer_size          = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_get_buffer_size");
    pContext->jack.jack_get_ports                = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_get_ports");
    pContext->jack.jack_activate                 = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_activate");
    pContext->jack.jack_deactivate               = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_deactivate");
    pContext->jack.jack_connect                  = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_connect");
    pContext->jack.jack_port_register            = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_port_register");
    pContext->jack.jack_port_name                = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_port_name");
    pContext->jack.jack_port_get_buffer          = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_port_get_buffer");
    pContext->jack.jack_free                     = (mal_proc)mal_dlsym(pContext->jack.jackSO, "jack_free");
#else
    // This strange assignment system is here just to ensure type safety of mini_al's function pointer
    // types. If anything differs slightly the compiler should throw a warning.
    mal_jack_client_open_proc              _jack_client_open              = jack_client_open;
    mal_jack_client_close_proc             _jack_client_close             = jack_client_close;
    mal_jack_client_name_size_proc         _jack_client_name_size         = jack_client_name_size;
    mal_jack_set_process_callback_proc     _jack_set_process_callback     = jack_set_process_callback;
    mal_jack_set_buffer_size_callback_proc _jack_set_buffer_size_callback = jack_set_buffer_size_callback;
    mal_jack_on_shutdown_proc              _jack_on_shutdown              = jack_on_shutdown;
    mal_jack_get_sample_rate_proc          _jack_get_sample_rate          = jack_get_sample_rate;
    mal_jack_get_buffer_size_proc          _jack_get_buffer_size          = jack_get_buffer_size;
    mal_jack_get_ports_proc                _jack_get_ports                = jack_get_ports;
    mal_jack_activate_proc                 _jack_activate                 = jack_activate;
    mal_jack_deactivate_proc               _jack_deactivate               = jack_deactivate;
    mal_jack_connect_proc                  _jack_connect                  = jack_connect;
    mal_jack_port_register_proc            _jack_port_register            = jack_port_register;
    mal_jack_port_name_proc                _jack_port_name                = jack_port_name;
    mal_jack_port_get_buffer_proc          _jack_port_get_buffer          = jack_port_get_buffer;
    mal_jack_free_proc                     _jack_free                     = jack_free;

    pContext->jack.jack_client_open              = (mal_proc)_jack_client_open;
    pContext->jack.jack_client_close             = (mal_proc)_jack_client_close;
    pContext->jack.jack_client_name_size         = (mal_proc)_jack_client_name_size;
    pContext->jack.jack_set_process_callback     = (mal_proc)_jack_set_process_callback;
    pContext->jack.jack_set_buffer_size_callback = (mal_proc)_jack_set_buffer_size_callback;
    pContext->jack.jack_on_shutdown              = (mal_proc)_jack_on_shutdown;
    pContext->jack.jack_get_sample_rate          = (mal_proc)_jack_get_sample_rate;
    pContext->jack.jack_get_buffer_size          = (mal_proc)_jack_get_buffer_size;
    pContext->jack.jack_get_ports                = (mal_proc)_jack_get_ports;
    pContext->jack.jack_activate                 = (mal_proc)_jack_activate;
    pContext->jack.jack_deactivate               = (mal_proc)_jack_deactivate;
    pContext->jack.jack_connect                  = (mal_proc)_jack_connect;
    pContext->jack.jack_port_register            = (mal_proc)_jack_port_register;
    pContext->jack.jack_port_name                = (mal_proc)_jack_port_name;
    pContext->jack.jack_port_get_buffer          = (mal_proc)_jack_port_get_buffer;
    pContext->jack.jack_free                     = (mal_proc)_jack_free;
#endif

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__jack;
    pContext->onEnumDevices   = mal_context_enumerate_devices__jack;
    pContext->onGetDeviceInfo = mal_context_get_device_info__jack;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__jack(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_jack);

#ifndef MAL_NO_RUNTIME_LINKING
    mal_dlclose(pContext->jack.jackSO);
#endif

    return MAL_SUCCESS;
}

void mal_device_uninit__jack(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    if (pDevice->jack.pClient != NULL) {
        ((mal_jack_client_close_proc)pContext->jack.jack_client_close)((mal_jack_client_t*)pDevice->jack.pClient);
    }
}

void mal_device__jack_shutdown_callback(void* pUserData)
{
    // JACK died. Stop the device.
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_device_stop(pDevice);
}

int mal_device__jack_buffer_size_callback(mal_jack_nframes_t frameCount, void* pUserData)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    float* pNewBuffer = (float*)mal_realloc(pDevice->jack.pIntermediaryBuffer, frameCount * (pDevice->internalChannels*mal_get_sample_size_in_bytes(pDevice->internalFormat)));
    if (pNewBuffer == NULL) {
        return MAL_OUT_OF_MEMORY;
    }

    pDevice->jack.pIntermediaryBuffer = pNewBuffer;
    pDevice->bufferSizeInFrames = frameCount * pDevice->periods;

    return 0;
}

int mal_device__jack_process_callback(mal_jack_nframes_t frameCount, void* pUserData)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    if (pDevice->type == mal_device_type_playback) {
        mal_device__read_frames_from_client(pDevice, frameCount, pDevice->jack.pIntermediaryBuffer);

        // Channels need to be separated.
        for (mal_uint32 iChannel = 0; iChannel < pDevice->internalChannels; ++iChannel) {
            float* pDst = (float*)((mal_jack_port_get_buffer_proc)pContext->jack.jack_port_get_buffer)((mal_jack_port_t*)pDevice->jack.pPorts[iChannel], frameCount);
            if (pDst != NULL) {
                const float* pSrc = pDevice->jack.pIntermediaryBuffer + iChannel;
                for (mal_jack_nframes_t iFrame = 0; iFrame < frameCount; ++iFrame) {
                    *pDst = *pSrc;

                    pDst += 1;
                    pSrc += pDevice->internalChannels;
                }
            }
        }
    } else {
        // Channels need to be interleaved.
        for (mal_uint32 iChannel = 0; iChannel < pDevice->internalChannels; ++iChannel) {
            const float* pSrc = (const float*)((mal_jack_port_get_buffer_proc)pContext->jack.jack_port_get_buffer)((mal_jack_port_t*)pDevice->jack.pPorts[iChannel], frameCount);
            if (pSrc != NULL) {
                float* pDst = pDevice->jack.pIntermediaryBuffer + iChannel;
                for (mal_jack_nframes_t iFrame = 0; iFrame < frameCount; ++iFrame) {
                    *pDst = *pSrc;

                    pDst += pDevice->internalChannels;
                    pSrc += 1;
                }
            }
        }

        mal_device__send_frames_to_client(pDevice, frameCount, pDevice->jack.pIntermediaryBuffer);
    }

    return 0;
}

mal_result mal_device_init__jack(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    mal_assert(pContext != NULL);
    mal_assert(pConfig != NULL);
    mal_assert(pDevice != NULL);

    (void)pContext;

    // Only supporting default devices with JACK.
    if (pDeviceID != NULL && pDeviceID->jack != 0) {
        return MAL_NO_DEVICE;
    }


    // Open the client.
    size_t maxClientNameSize = ((mal_jack_client_name_size_proc)pContext->jack.jack_client_name_size)(); // Includes null terminator.

    char clientName[256];
    mal_strncpy_s(clientName, mal_min(sizeof(clientName), maxClientNameSize), (pContext->config.jack.pClientName != NULL) ? pContext->config.jack.pClientName : "mini_al", (size_t)-1);

    mal_jack_status_t status;
    pDevice->jack.pClient = ((mal_jack_client_open_proc)pContext->jack.jack_client_open)(clientName, (pContext->config.jack.tryStartServer) ? 0 : mal_JackNoStartServer, &status, NULL);
    if (pDevice->jack.pClient == NULL) {
        return mal_post_error(pDevice, "[JACK] Failed to open client.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    // Callbacks.
    if (((mal_jack_set_process_callback_proc)pContext->jack.jack_set_process_callback)((mal_jack_client_t*)pDevice->jack.pClient, mal_device__jack_process_callback, pDevice) != 0) {
        return mal_post_error(pDevice, "[JACK] Failed to set process callback.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }
    if (((mal_jack_set_buffer_size_callback_proc)pContext->jack.jack_set_buffer_size_callback)((mal_jack_client_t*)pDevice->jack.pClient, mal_device__jack_buffer_size_callback, pDevice) != 0) {
        return mal_post_error(pDevice, "[JACK] Failed to set buffer size callback.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    ((mal_jack_on_shutdown_proc)pContext->jack.jack_on_shutdown)((mal_jack_client_t*)pDevice->jack.pClient, mal_device__jack_shutdown_callback, pDevice);


    // The format is always f32.
    pDevice->internalFormat = mal_format_f32;

    // A port is a channel.
    unsigned long serverPortFlags;
    unsigned long clientPortFlags;
    if (type == mal_device_type_playback) {
        serverPortFlags = mal_JackPortIsInput;
        clientPortFlags = mal_JackPortIsOutput;
    } else {
        serverPortFlags = mal_JackPortIsOutput;
        clientPortFlags = mal_JackPortIsInput;
    }

    const char** ppPorts = ((mal_jack_get_ports_proc)pContext->jack.jack_get_ports)((mal_jack_client_t*)pDevice->jack.pClient, NULL, NULL, mal_JackPortIsPhysical | serverPortFlags);
    if (ppPorts == NULL) {
        return mal_post_error(pDevice, "[JACK] Failed to query physical ports.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    pDevice->internalChannels = 0;
    while (ppPorts[pDevice->internalChannels] != NULL) {
        char name[64];
        if (type == mal_device_type_playback) {
            mal_strcpy_s(name, sizeof(name), "playback");
            mal_itoa_s((int)pDevice->internalChannels, name+8, sizeof(name)-8, 10); // 8 = length of "playback"
        } else {
            mal_strcpy_s(name, sizeof(name), "capture");
            mal_itoa_s((int)pDevice->internalChannels, name+7, sizeof(name)-7, 10); // 7 = length of "capture"
        }

        pDevice->jack.pPorts[pDevice->internalChannels] = ((mal_jack_port_register_proc)pContext->jack.jack_port_register)((mal_jack_client_t*)pDevice->jack.pClient, name, MAL_JACK_DEFAULT_AUDIO_TYPE, clientPortFlags, 0);
        if (pDevice->jack.pPorts[pDevice->internalChannels] == NULL) {
            ((mal_jack_free_proc)pContext->jack.jack_free)((void*)ppPorts);
            mal_device_uninit__jack(pDevice);
            return mal_post_error(pDevice, "[JACK] Failed to register ports.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        pDevice->internalChannels += 1;
    }

    ((mal_jack_free_proc)pContext->jack.jack_free)((void*)ppPorts);
    ppPorts = NULL;

    // We set the sample rate here, but apparently this can change. This is incompatible with mini_al, so changing sample rates will not be supported.
    pDevice->internalSampleRate = ((mal_jack_get_sample_rate_proc)pContext->jack.jack_get_sample_rate)((mal_jack_client_t*)pDevice->jack.pClient);

    // I don't think the channel map can be queried, so just use defaults for now.
    mal_get_standard_channel_map(mal_standard_channel_map_alsa, pDevice->internalChannels, pDevice->internalChannelMap);

    // The buffer size in frames can change.
    pDevice->periods = 2;
    pDevice->bufferSizeInFrames = ((mal_jack_get_buffer_size_proc)pContext->jack.jack_get_buffer_size)((mal_jack_client_t*)pDevice->jack.pClient) * pDevice->periods;

    // Initial allocation for the intermediary buffer.
    pDevice->jack.pIntermediaryBuffer = (float*)mal_malloc((pDevice->bufferSizeInFrames/pDevice->periods)*(pDevice->internalChannels*mal_get_sample_size_in_bytes(pDevice->internalFormat)));
    if (pDevice->jack.pIntermediaryBuffer == NULL) {
        mal_device_uninit__jack(pDevice);
        return MAL_OUT_OF_MEMORY;
    }

    return MAL_SUCCESS;
}


mal_result mal_device__start_backend__jack(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    int resultJACK = ((mal_jack_activate_proc)pContext->jack.jack_activate)((mal_jack_client_t*)pDevice->jack.pClient);
    if (resultJACK != 0) {
        return mal_post_error(pDevice, "[JACK] Failed to activate the JACK client.", MAL_FAILED_TO_START_BACKEND_DEVICE);
    }

    const char** ppServerPorts;
    if (pDevice->type == mal_device_type_playback) {
        ppServerPorts = ((mal_jack_get_ports_proc)pContext->jack.jack_get_ports)((mal_jack_client_t*)pDevice->jack.pClient, NULL, NULL, mal_JackPortIsPhysical | mal_JackPortIsInput);
    } else {
        ppServerPorts = ((mal_jack_get_ports_proc)pContext->jack.jack_get_ports)((mal_jack_client_t*)pDevice->jack.pClient, NULL, NULL, mal_JackPortIsPhysical | mal_JackPortIsOutput);
    }

    if (ppServerPorts == NULL) {
        ((mal_jack_deactivate_proc)pContext->jack.jack_deactivate)((mal_jack_client_t*)pDevice->jack.pClient);
        return mal_post_error(pDevice, "[JACK] Failed to retrieve physical ports.", MAL_ERROR);
    }

    for (size_t i = 0; ppServerPorts[i] != NULL; ++i) {
        const char* pServerPort = ppServerPorts[i];
        mal_assert(pServerPort != NULL);

        const char* pClientPort = ((mal_jack_port_name_proc)pContext->jack.jack_port_name)((mal_jack_port_t*)pDevice->jack.pPorts[i]);
        mal_assert(pClientPort != NULL);

        if (pDevice->type == mal_device_type_playback) {
            resultJACK = ((mal_jack_connect_proc)pContext->jack.jack_connect)((mal_jack_client_t*)pDevice->jack.pClient, pClientPort, pServerPort);
        } else {
            resultJACK = ((mal_jack_connect_proc)pContext->jack.jack_connect)((mal_jack_client_t*)pDevice->jack.pClient, pServerPort, pClientPort);
        }

        if (resultJACK != 0) {
            ((mal_jack_free_proc)pContext->jack.jack_free)((void*)ppServerPorts);
            ((mal_jack_deactivate_proc)pContext->jack.jack_deactivate)((mal_jack_client_t*)pDevice->jack.pClient);
            return mal_post_error(pDevice, "[JACK] Failed to connect ports.", MAL_ERROR);
        }
    }

    ((mal_jack_free_proc)pContext->jack.jack_free)((void*)ppServerPorts);

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__jack(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_context* pContext = pDevice->pContext;
    mal_assert(pContext != NULL);

    if (((mal_jack_deactivate_proc)pContext->jack.jack_deactivate)((mal_jack_client_t*)pDevice->jack.pClient) != 0) {
        return mal_post_error(pDevice, "[JACK] An error occurred when deactivating the JACK client.", MAL_ERROR);
    }

    return MAL_SUCCESS;
}
#endif



///////////////////////////////////////////////////////////////////////////////
//
// OSS Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_OSS
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#ifndef SNDCTL_DSP_HALT
#define SNDCTL_DSP_HALT SNDCTL_DSP_RESET
#endif

int mal_open_temp_device__oss()
{
    // The OSS sample code uses "/dev/mixer" as the device for getting system properties so I'm going to do the same.
    int fd = open("/dev/mixer", O_RDONLY, 0);
    if (fd >= 0) {
        return fd;
    }

    return -1;
}

mal_bool32 mal_context_is_device_id_equal__oss(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return mal_strcmp(pID0->oss, pID1->oss) == 0;
}

mal_result mal_context_enumerate_devices__oss(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    int fd = mal_open_temp_device__oss();
    if (fd == -1) {
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to open a temporary device for retrieving system information used for device enumeration.", MAL_NO_BACKEND);
    }

    oss_sysinfo si;
    int result = ioctl(fd, SNDCTL_SYSINFO, &si);
    if (result != -1) {
        for (int iAudioDevice = 0; iAudioDevice < si.numaudios; ++iAudioDevice) {
            oss_audioinfo ai;
            ai.dev = iAudioDevice;
            result = ioctl(fd, SNDCTL_AUDIOINFO, &ai);
            if (result != -1) {
                if (ai.devnode[0] != '\0') {    // <-- Can be blank, according to documentation.
                    mal_device_info deviceInfo;
                    mal_zero_object(&deviceInfo);

                    // ID
                    mal_strncpy_s(deviceInfo.id.oss, sizeof(deviceInfo.id.oss), ai.devnode, (size_t)-1);

                    // The human readable device name should be in the "ai.handle" variable, but it can
                    // sometimes be empty in which case we just fall back to "ai.name" which is less user
                    // friendly, but usually has a value.
                    if (ai.handle[0] != '\0') {
                        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), ai.handle, (size_t)-1);
                    } else {
                        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), ai.name, (size_t)-1);
                    }

                    // The device can be both playback and capture.
                    mal_bool32 isTerminating = MAL_FALSE;
                    if (!isTerminating && (ai.caps & PCM_CAP_OUTPUT) != 0) {
                        isTerminating = !callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                    }
                    if (!isTerminating && (ai.caps & PCM_CAP_INPUT) != 0) {
                        isTerminating = !callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                    }

                    if (isTerminating) {
                        break;
                    }
                }
            }
        }
    } else {
        close(fd);
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to retrieve system information for device enumeration.", MAL_NO_BACKEND);
    }

    close(fd);
    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__oss(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    // Handle the default device a little differently.
    if (pDeviceID == NULL) {
        if (deviceType == mal_device_type_playback) {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        } else {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        }

        return MAL_SUCCESS;
    }


    // If we get here it means we are _not_ using the default device.
    mal_bool32 foundDevice = MAL_FALSE;

    int fd = mal_open_temp_device__oss();
    if (fd == -1) {
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to open a temporary device for retrieving system information used for device enumeration.", MAL_NO_BACKEND);
    }

    oss_sysinfo si;
    int result = ioctl(fd, SNDCTL_SYSINFO, &si);
    if (result != -1) {
        for (int iAudioDevice = 0; iAudioDevice < si.numaudios; ++iAudioDevice) {
            oss_audioinfo ai;
            ai.dev = iAudioDevice;
            result = ioctl(fd, SNDCTL_AUDIOINFO, &ai);
            if (result != -1) {
                if (mal_strcmp(ai.devnode, pDeviceID->oss) == 0) {
                    // It has the same name, so now just confirm the type.
                    if ((deviceType == mal_device_type_playback && ((ai.caps & PCM_CAP_OUTPUT) != 0)) ||
                        (deviceType == mal_device_type_capture  && ((ai.caps & PCM_CAP_INPUT)  != 0))) {
                        // ID
                        mal_strncpy_s(pDeviceInfo->id.oss, sizeof(pDeviceInfo->id.oss), ai.devnode, (size_t)-1);

                        // The human readable device name should be in the "ai.handle" variable, but it can
                        // sometimes be empty in which case we just fall back to "ai.name" which is less user
                        // friendly, but usually has a value.
                        if (ai.handle[0] != '\0') {
                            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), ai.handle, (size_t)-1);
                        } else {
                            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), ai.name, (size_t)-1);
                        }

                        foundDevice = MAL_TRUE;
                        break;
                    }
                }
            }
        }
    } else {
        close(fd);
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to retrieve system information for device enumeration.", MAL_NO_BACKEND);
    }


    close(fd);

    if (foundDevice) {
        return MAL_SUCCESS;
    } else {
        return MAL_NO_DEVICE;
    }
}

mal_result mal_context_init__oss(mal_context* pContext)
{
    mal_assert(pContext != NULL);

    // Try opening a temporary device first so we can get version information. This is closed at the end.
    int fd = mal_open_temp_device__oss();
    if (fd == -1) {
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to open temporary device for retrieving system properties.", MAL_NO_BACKEND);   // Looks liks OSS isn't installed, or there are no available devices.
    }

    // Grab the OSS version.
    int ossVersion = 0;
    int result = ioctl(fd, OSS_GETVERSION, &ossVersion);
    if (result == -1) {
        close(fd);
        return mal_context_post_error(pContext, NULL, "[OSS] Failed to retrieve OSS version.", MAL_NO_BACKEND);
    }

    pContext->oss.versionMajor = ((ossVersion & 0xFF0000) >> 16);
    pContext->oss.versionMinor = ((ossVersion & 0x00FF00) >> 8);

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__oss;
    pContext->onEnumDevices   = mal_context_enumerate_devices__oss;
    pContext->onGetDeviceInfo = mal_context_get_device_info__oss;

    close(fd);
    return MAL_SUCCESS;
}

mal_result mal_context_uninit__oss(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_oss);

    (void)pContext;
    return MAL_SUCCESS;
}

void mal_device_uninit__oss(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    close(pDevice->oss.fd);
    mal_free(pDevice->oss.pIntermediaryBuffer);
}

mal_result mal_device_init__oss(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->oss);

    char deviceName[64];
    if (pDeviceID != NULL) {
        mal_strncpy_s(deviceName, sizeof(deviceName), pDeviceID->oss, (size_t)-1);
    } else {
        mal_strncpy_s(deviceName, sizeof(deviceName), "/dev/dsp", (size_t)-1);
    }

    pDevice->oss.fd = open(deviceName, (type == mal_device_type_playback) ? O_WRONLY : O_RDONLY, 0);
    if (pDevice->oss.fd == -1) {
        return mal_post_error(pDevice, "[OSS] Failed to open device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
    }

    // The OSS documantation is very clear about the order we should be initializing the device's properties:
    //   1) Format
    //   2) Channels
    //   3) Sample rate.

    // Format.
    int ossFormat = AFMT_U8;
    switch (pDevice->format) {
        case mal_format_s16: ossFormat = AFMT_S16_LE; break;
        case mal_format_s24: ossFormat = AFMT_S32_LE; break;
        case mal_format_s32: ossFormat = AFMT_S32_LE; break;
        case mal_format_f32: ossFormat = AFMT_S32_LE; break;
        case mal_format_u8:
        default: ossFormat = AFMT_U8; break;
    }
    int result = ioctl(pDevice->oss.fd, SNDCTL_DSP_SETFMT, &ossFormat);
    if (result == -1) {
        close(pDevice->oss.fd);
        return mal_post_error(pDevice, "[OSS] Failed to set format.", MAL_FORMAT_NOT_SUPPORTED);
    }

    switch (ossFormat) {
        case AFMT_U8:     pDevice->internalFormat = mal_format_u8;  break;
        case AFMT_S16_LE: pDevice->internalFormat = mal_format_s16; break;
        case AFMT_S32_LE: pDevice->internalFormat = mal_format_s32; break;
        default: mal_post_error(pDevice, "[OSS] The device's internal format is not supported by mini_al.", MAL_FORMAT_NOT_SUPPORTED);
    }


    // Channels.
    int ossChannels = (int)pConfig->channels;
    result = ioctl(pDevice->oss.fd, SNDCTL_DSP_CHANNELS, &ossChannels);
    if (result == -1) {
        close(pDevice->oss.fd);
        return mal_post_error(pDevice, "[OSS] Failed to set channel count.", MAL_FORMAT_NOT_SUPPORTED);
    }

    pDevice->internalChannels = ossChannels;


    // Sample rate.
    int ossSampleRate = (int)pConfig->sampleRate;
    result = ioctl(pDevice->oss.fd, SNDCTL_DSP_SPEED, &ossSampleRate);
    if (result == -1) {
        close(pDevice->oss.fd);
        return mal_post_error(pDevice, "[OSS] Failed to set sample rate.", MAL_FORMAT_NOT_SUPPORTED);
    }

    pDevice->internalSampleRate = ossSampleRate;



    // The documentation says that the fragment settings should be set as soon as possible, but I'm not sure if
    // it should be done before or after format/channels/rate.
    //
    // OSS wants the fragment size in bytes and a power of 2. When setting, we specify the power, not the actual
    // value.
    mal_uint32 fragmentSizeInBytes = mal_round_to_power_of_2(pDevice->bufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat));
    if (fragmentSizeInBytes < 16) {
        fragmentSizeInBytes = 16;
    }

    mal_uint32 ossFragmentSizePower = 4;
    fragmentSizeInBytes >>= 4;
    while (fragmentSizeInBytes >>= 1) {
        ossFragmentSizePower += 1;
    }

    int ossFragment = (int)((pDevice->periods << 16) | ossFragmentSizePower);
    result = ioctl(pDevice->oss.fd, SNDCTL_DSP_SETFRAGMENT, &ossFragment);
    if (result == -1) {
        close(pDevice->oss.fd);
        return mal_post_error(pDevice, "[OSS] Failed to set fragment size and period count.", MAL_FORMAT_NOT_SUPPORTED);
    }

    int actualFragmentSizeInBytes = 1 << (ossFragment & 0xFFFF);
    pDevice->oss.fragmentSizeInFrames = actualFragmentSizeInBytes / mal_get_sample_size_in_bytes(pDevice->internalFormat) / pDevice->internalChannels;

    pDevice->periods = (mal_uint32)(ossFragment >> 16);
    pDevice->bufferSizeInFrames = (mal_uint32)(pDevice->oss.fragmentSizeInFrames * pDevice->periods);

    // Set the internal channel map. Not sure if this can be queried. For now just using our default assumptions.
    mal_get_standard_channel_map(mal_standard_channel_map_default, pDevice->internalChannels, pDevice->internalChannelMap);


    // When not using MMAP mode, we need to use an intermediary buffer for the client <-> device transfer. We do
    // everything by the size of a fragment.
    pDevice->oss.pIntermediaryBuffer = mal_malloc(actualFragmentSizeInBytes);
    if (pDevice->oss.pIntermediaryBuffer == NULL) {
        close(pDevice->oss.fd);
        return mal_post_error(pDevice, "[OSS] Failed to allocate memory for intermediary buffer.", MAL_OUT_OF_MEMORY);
    }

    return MAL_SUCCESS;
}


mal_result mal_device__start_backend__oss(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // The device is started by the next calls to read() and write(). For playback it's simple - just read
    // data from the client, then write it to the device with write() which will in turn start the device.
    // For capture it's a bit less intuitive - we do nothing (it'll be started automatically by the first
    // call to read().
    if (pDevice->type == mal_device_type_playback) {
        // Playback.
        mal_device__read_frames_from_client(pDevice, pDevice->oss.fragmentSizeInFrames, pDevice->oss.pIntermediaryBuffer);

        int bytesWritten = write(pDevice->oss.fd, pDevice->oss.pIntermediaryBuffer, pDevice->oss.fragmentSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat));
        if (bytesWritten == -1) {
            return mal_post_error(pDevice, "[OSS] Failed to send initial chunk of data to the device.", MAL_FAILED_TO_SEND_DATA_TO_DEVICE);
        }
    } else {
        // Capture. Do nothing.
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__oss(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // We want to use SNDCTL_DSP_HALT. From the documentation:
    //
    //   In multithreaded applications SNDCTL_DSP_HALT (SNDCTL_DSP_RESET) must only be called by the thread
    //   that actually reads/writes the audio device. It must not be called by some master thread to kill the
    //   audio thread. The audio thread will not stop or get any kind of notification that the device was
    //   stopped by the master thread. The device gets stopped but the next read or write call will silently
    //   restart the device.
    //
    // This is actually safe in our case, because this function is only ever called from within our worker
    // thread anyway. Just keep this in mind, though...

    int result = ioctl(pDevice->oss.fd, SNDCTL_DSP_HALT, 0);
    if (result == -1) {
        return mal_post_error(pDevice, "[OSS] Failed to stop device. SNDCTL_DSP_HALT failed.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__oss(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->oss.breakFromMainLoop = MAL_TRUE;
    return MAL_SUCCESS;
}

mal_result mal_device__main_loop__oss(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->oss.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->oss.breakFromMainLoop) {
        // Break from the main loop if the device isn't started anymore. Likely what's happened is the application
        // has requested that the device be stopped.
        if (!mal_device_is_started(pDevice)) {
            break;
        }

        if (pDevice->type == mal_device_type_playback) {
            // Playback.
            mal_device__read_frames_from_client(pDevice, pDevice->oss.fragmentSizeInFrames, pDevice->oss.pIntermediaryBuffer);

            int bytesWritten = write(pDevice->oss.fd, pDevice->oss.pIntermediaryBuffer, pDevice->oss.fragmentSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat));
            if (bytesWritten < 0) {
                return mal_post_error(pDevice, "[OSS] Failed to send data from the client to the device.", MAL_FAILED_TO_SEND_DATA_TO_DEVICE);
            }
        } else {
            // Capture.
            int bytesRead = read(pDevice->oss.fd, pDevice->oss.pIntermediaryBuffer, pDevice->oss.fragmentSizeInFrames * mal_get_sample_size_in_bytes(pDevice->internalFormat));
            if (bytesRead < 0) {
                return mal_post_error(pDevice, "[OSS] Failed to read data from the device to be sent to the client.", MAL_FAILED_TO_READ_DATA_FROM_DEVICE);
            }

            mal_uint32 framesRead = (mal_uint32)bytesRead / pDevice->internalChannels / mal_get_sample_size_in_bytes(pDevice->internalFormat);
            mal_device__send_frames_to_client(pDevice, framesRead, pDevice->oss.pIntermediaryBuffer);
        }
    }

    return MAL_SUCCESS;
}
#endif  // OSS


///////////////////////////////////////////////////////////////////////////////
//
// OpenSL|ES Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_OPENSL
#include <SLES/OpenSLES.h>
#ifdef MAL_ANDROID
#include <SLES/OpenSLES_Android.h>
#endif

// OpenSL|ES has one-per-application objects :(
SLObjectItf g_malEngineObjectSL = NULL;
SLEngineItf g_malEngineSL = NULL;
mal_uint32 g_malOpenSLInitCounter = 0;

#define MAL_OPENSL_OBJ(p)         (*((SLObjectItf)(p)))
#define MAL_OPENSL_OUTPUTMIX(p)   (*((SLOutputMixItf)(p)))
#define MAL_OPENSL_PLAY(p)        (*((SLPlayItf)(p)))
#define MAL_OPENSL_RECORD(p)      (*((SLRecordItf)(p)))

#ifdef MAL_ANDROID
#define MAL_OPENSL_BUFFERQUEUE(p) (*((SLAndroidSimpleBufferQueueItf)(p)))
#else
#define MAL_OPENSL_BUFFERQUEUE(p) (*((SLBufferQueueItf)(p)))
#endif

// Converts an individual OpenSL-style channel identifier (SL_SPEAKER_FRONT_LEFT, etc.) to mini_al.
mal_uint8 mal_channel_id_to_mal__opensl(SLuint32 id)
{
    switch (id)
    {
        case SL_SPEAKER_FRONT_LEFT:            return MAL_CHANNEL_FRONT_LEFT;
        case SL_SPEAKER_FRONT_RIGHT:           return MAL_CHANNEL_FRONT_RIGHT;
        case SL_SPEAKER_FRONT_CENTER:          return MAL_CHANNEL_FRONT_CENTER;
        case SL_SPEAKER_LOW_FREQUENCY:         return MAL_CHANNEL_LFE;
        case SL_SPEAKER_BACK_LEFT:             return MAL_CHANNEL_BACK_LEFT;
        case SL_SPEAKER_BACK_RIGHT:            return MAL_CHANNEL_BACK_RIGHT;
        case SL_SPEAKER_FRONT_LEFT_OF_CENTER:  return MAL_CHANNEL_FRONT_LEFT_CENTER;
        case SL_SPEAKER_FRONT_RIGHT_OF_CENTER: return MAL_CHANNEL_FRONT_RIGHT_CENTER;
        case SL_SPEAKER_BACK_CENTER:           return MAL_CHANNEL_BACK_CENTER;
        case SL_SPEAKER_SIDE_LEFT:             return MAL_CHANNEL_SIDE_LEFT;
        case SL_SPEAKER_SIDE_RIGHT:            return MAL_CHANNEL_SIDE_RIGHT;
        case SL_SPEAKER_TOP_CENTER:            return MAL_CHANNEL_TOP_CENTER;
        case SL_SPEAKER_TOP_FRONT_LEFT:        return MAL_CHANNEL_TOP_FRONT_LEFT;
        case SL_SPEAKER_TOP_FRONT_CENTER:      return MAL_CHANNEL_TOP_FRONT_CENTER;
        case SL_SPEAKER_TOP_FRONT_RIGHT:       return MAL_CHANNEL_TOP_FRONT_RIGHT;
        case SL_SPEAKER_TOP_BACK_LEFT:         return MAL_CHANNEL_TOP_BACK_LEFT;
        case SL_SPEAKER_TOP_BACK_CENTER:       return MAL_CHANNEL_TOP_BACK_CENTER;
        case SL_SPEAKER_TOP_BACK_RIGHT:        return MAL_CHANNEL_TOP_BACK_RIGHT;
        default: return 0;
    }
}

// Converts an individual mini_al channel identifier (MAL_CHANNEL_FRONT_LEFT, etc.) to OpenSL-style.
SLuint32 mal_channel_id_to_opensl(mal_uint8 id)
{
    switch (id)
    {
        case MAL_CHANNEL_FRONT_LEFT:         return SL_SPEAKER_FRONT_LEFT;
        case MAL_CHANNEL_FRONT_RIGHT:        return SL_SPEAKER_FRONT_RIGHT;
        case MAL_CHANNEL_FRONT_CENTER:       return SL_SPEAKER_FRONT_CENTER;
        case MAL_CHANNEL_LFE:                return SL_SPEAKER_LOW_FREQUENCY;
        case MAL_CHANNEL_BACK_LEFT:          return SL_SPEAKER_BACK_LEFT;
        case MAL_CHANNEL_BACK_RIGHT:         return SL_SPEAKER_BACK_RIGHT;
        case MAL_CHANNEL_FRONT_LEFT_CENTER:  return SL_SPEAKER_FRONT_LEFT_OF_CENTER;
        case MAL_CHANNEL_FRONT_RIGHT_CENTER: return SL_SPEAKER_FRONT_RIGHT_OF_CENTER;
        case MAL_CHANNEL_BACK_CENTER:        return SL_SPEAKER_BACK_CENTER;
        case MAL_CHANNEL_SIDE_LEFT:          return SL_SPEAKER_SIDE_LEFT;
        case MAL_CHANNEL_SIDE_RIGHT:         return SL_SPEAKER_SIDE_RIGHT;
        case MAL_CHANNEL_TOP_CENTER:         return SL_SPEAKER_TOP_CENTER;
        case MAL_CHANNEL_TOP_FRONT_LEFT:     return SL_SPEAKER_TOP_FRONT_LEFT;
        case MAL_CHANNEL_TOP_FRONT_CENTER:   return SL_SPEAKER_TOP_FRONT_CENTER;
        case MAL_CHANNEL_TOP_FRONT_RIGHT:    return SL_SPEAKER_TOP_FRONT_RIGHT;
        case MAL_CHANNEL_TOP_BACK_LEFT:      return SL_SPEAKER_TOP_BACK_LEFT;
        case MAL_CHANNEL_TOP_BACK_CENTER:    return SL_SPEAKER_TOP_BACK_CENTER;
        case MAL_CHANNEL_TOP_BACK_RIGHT:     return SL_SPEAKER_TOP_BACK_RIGHT;
        default: return 0;
    }
}

// Converts a channel mapping to an OpenSL-style channel mask.
SLuint32 mal_channel_map_to_channel_mask__opensl(const mal_channel channelMap[MAL_MAX_CHANNELS], mal_uint32 channels)
{
    SLuint32 channelMask = 0;
    for (mal_uint32 iChannel = 0; iChannel < channels; ++iChannel) {
        channelMask |= mal_channel_id_to_opensl(channelMap[iChannel]);
    }

    return channelMask;
}

// Converts an OpenSL-style channel mask to a mini_al channel map.
void mal_channel_mask_to_channel_map__opensl(SLuint32 channelMask, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    if (channels == 2 && channelMask == 0) {
        channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
        channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
    } else {
        // Just iterate over each bit.
        mal_uint32 iChannel = 0;
        for (mal_uint32 iBit = 0; iBit < 32; ++iBit) {
            SLuint32 bitValue = (channelMask & (1UL << iBit));
            if (bitValue != 0) {
                // The bit is set.
                channelMap[iChannel] = mal_channel_id_to_mal__opensl(bitValue);
                iChannel += 1;
            }
        }
    }
}

SLuint32 mal_round_to_standard_sample_rate__opensl(SLuint32 samplesPerSec)
{
    if (samplesPerSec <= SL_SAMPLINGRATE_8) {
        return SL_SAMPLINGRATE_8;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_11_025) {
        return SL_SAMPLINGRATE_11_025;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_12) {
        return SL_SAMPLINGRATE_12;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_16) {
        return SL_SAMPLINGRATE_16;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_22_05) {
        return SL_SAMPLINGRATE_22_05;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_24) {
        return SL_SAMPLINGRATE_24;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_32) {
        return SL_SAMPLINGRATE_32;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_44_1) {
        return SL_SAMPLINGRATE_44_1;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_48) {
        return SL_SAMPLINGRATE_48;
    }

    // Android doesn't support more than 48000.
#ifndef MAL_ANDROID
    if (samplesPerSec <= SL_SAMPLINGRATE_64) {
        return SL_SAMPLINGRATE_64;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_88_2) {
        return SL_SAMPLINGRATE_88_2;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_96) {
        return SL_SAMPLINGRATE_96;
    }
    if (samplesPerSec <= SL_SAMPLINGRATE_192) {
        return SL_SAMPLINGRATE_192;
    }
#endif

    return SL_SAMPLINGRATE_16;
}


mal_bool32 mal_context_is_device_id_equal__opensl(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return pID0->opensl == pID1->opensl;
}

mal_result mal_context_enumerate_devices__opensl(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    // TODO: Test Me.
    //
    // This is currently untested, so for now we are just returning default devices.
#if 0 && !defined(MAL_ANDROID)
    mal_bool32 isTerminated = MAL_FALSE;

    SLuint32 pDeviceIDs[128];
    SLint32 deviceCount = sizeof(pDeviceIDs) / sizeof(pDeviceIDs[0]);

    SLAudioIODeviceCapabilitiesItf deviceCaps;
    SLresult resultSL = (*g_malEngineObjectSL)->GetInterface(g_malEngineObjectSL, SL_IID_AUDIOIODEVICECAPABILITIES, &deviceCaps);
    if (resultSL != SL_RESULT_SUCCESS) {
        // The interface may not be supported so just report a default device.
        goto return_default_device;
    }

    // Playback
    if (!isTerminated) {
        resultSL = (*deviceCaps)->GetAvailableAudioOutputs(deviceCaps, &deviceCount, pDeviceIDs);
        if (resultSL != SL_RESULT_SUCCESS) {
            return MAL_NO_DEVICE;
        }

        for (SLint32 iDevice = 0; iDevice < deviceCount; ++iDevice) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            deviceInfo.id.opensl = pDeviceIDs[iDevice];

            SLAudioOutputDescriptor desc;
            resultSL = (*deviceCaps)->QueryAudioOutputCapabilities(deviceCaps, deviceInfo.id.opensl, &desc);
            if (resultSL == SL_RESULT_SUCCESS) {
                mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), (const char*)desc.pDeviceName, (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }
            }
        }
    }

    // Capture
    if (!isTerminated) {
        resultSL = (*deviceCaps)->GetAvailableAudioInputs(deviceCaps, &deviceCount, pDeviceIDs);
        if (resultSL != SL_RESULT_SUCCESS) {
            return MAL_NO_DEVICE;
        }

        for (SLint32 iDevice = 0; iDevice < deviceCount; ++iDevice) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            deviceInfo.id.opensl = pDeviceIDs[iDevice];

            SLAudioInputDescriptor desc;
            resultSL = (*deviceCaps)->QueryAudioInputCapabilities(deviceCaps, deviceInfo.id.opensl, &desc);
            if (resultSL == SL_RESULT_SUCCESS) {
                mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), (const char*)desc.deviceName, (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }
            }
        }
    }

    return MAL_SUCCESS;
#else
    goto return_default_device;
#endif

return_default_device:
    mal_bool32 cbResult = MAL_TRUE;

    // Playback.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
    }

    // Capture.
    if (cbResult) {
        mal_device_info deviceInfo;
        mal_zero_object(&deviceInfo);
        mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__opensl(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    // TODO: Test Me.
    //
    // This is currently untested, so for now we are just returning default devices.
#if 0 && !defined(MAL_ANDROID)
    SLAudioIODeviceCapabilitiesItf deviceCaps;
    SLresult resultSL = (*g_malEngineObjectSL)->GetInterface(g_malEngineObjectSL, SL_IID_AUDIOIODEVICECAPABILITIES, &deviceCaps);
    if (resultSL != SL_RESULT_SUCCESS) {
        // The interface may not be supported so just report a default device.
        goto return_default_device;
    }

    if (deviceType == mal_device_type_playback) {
        SLAudioOutputDescriptor desc;
        resultSL = (*deviceCaps)->QueryAudioOutputCapabilities(deviceCaps, pDeviceID->opensl, &desc);
        if (resultSL != SL_RESULT_SUCCESS) {
            return MAL_NO_DEVICE;
        }

        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), (const char*)desc.pDeviceName, (size_t)-1);
    } else {
        SLAudioInputDescriptor desc;
        resultSL = (*deviceCaps)->QueryAudioInputCapabilities(deviceCaps, pDeviceID->opensl, &desc);
        if (resultSL != SL_RESULT_SUCCESS) {
            return MAL_NO_DEVICE;
        }

        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), (const char*)desc.deviceName, (size_t)-1);
    }
#else
    goto return_default_device;
#endif

return_default_device:
    if (pDeviceID != NULL) {
        if ((deviceType == mal_device_type_playback && pDeviceID->opensl != SL_DEFAULTDEVICEID_AUDIOOUTPUT) ||
            (deviceType == mal_device_type_capture  && pDeviceID->opensl != SL_DEFAULTDEVICEID_AUDIOINPUT)) {
            return MAL_NO_DEVICE;   // Don't know the device.
        }
    }

    // Name / Description
    if (deviceType == mal_device_type_playback) {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
    } else {
        mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
    }

    return MAL_SUCCESS;
}

mal_result mal_context_init__opensl(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    (void)pContext;

    // Initialize global data first if applicable.
    if (mal_atomic_increment_32(&g_malOpenSLInitCounter) == 1) {
        SLresult resultSL = slCreateEngine(&g_malEngineObjectSL, 0, NULL, 0, NULL, NULL);
        if (resultSL != SL_RESULT_SUCCESS) {
            mal_atomic_decrement_32(&g_malOpenSLInitCounter);
            return MAL_NO_BACKEND;
        }

        (*g_malEngineObjectSL)->Realize(g_malEngineObjectSL, SL_BOOLEAN_FALSE);

        resultSL = (*g_malEngineObjectSL)->GetInterface(g_malEngineObjectSL, SL_IID_ENGINE, &g_malEngineSL);
        if (resultSL != SL_RESULT_SUCCESS) {
            (*g_malEngineObjectSL)->Destroy(g_malEngineObjectSL);
            mal_atomic_decrement_32(&g_malOpenSLInitCounter);
            return MAL_NO_BACKEND;
        }
    }

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__opensl;
    pContext->onEnumDevices   = mal_context_enumerate_devices__opensl;
    pContext->onGetDeviceInfo = mal_context_get_device_info__opensl;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__opensl(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_opensl);
    (void)pContext;

    // Uninit global data.
    if (g_malOpenSLInitCounter > 0) {
        if (mal_atomic_decrement_32(&g_malOpenSLInitCounter) == 0) {
            (*g_malEngineObjectSL)->Destroy(g_malEngineObjectSL);
        }
    }

    return MAL_SUCCESS;
}


#ifdef MAL_ANDROID
//void mal_buffer_queue_callback__opensl_android(SLAndroidSimpleBufferQueueItf pBufferQueue, SLuint32 eventFlags, const void* pBuffer, SLuint32 bufferSize, SLuint32 dataUsed, void* pContext)
void mal_buffer_queue_callback__opensl_android(SLAndroidSimpleBufferQueueItf pBufferQueue, void* pUserData)
{
    (void)pBufferQueue;

    // For now, only supporting Android implementations of OpenSL|ES since that's the only one I've
    // been able to test with and I currently depend on Android-specific extensions (simple buffer
    // queues).
#ifndef MAL_ANDROID
    return MAL_NO_BACKEND;
#endif

    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    // For now, don't do anything unless the buffer was fully processed. From what I can tell, it looks like
    // OpenSL|ES 1.1 improves on buffer queues to the point that we could much more intelligently handle this,
    // but unfortunately it looks like Android is only supporting OpenSL|ES 1.0.1 for now :(
    if (pDevice->state != MAL_STATE_STARTED) {
        return;
    }

    size_t periodSizeInBytes = pDevice->opensl.periodSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);
    mal_uint8* pBuffer = pDevice->opensl.pBuffer + (pDevice->opensl.currentBufferIndex * periodSizeInBytes);

    if (pDevice->type == mal_device_type_playback) {
        if (pDevice->state != MAL_STATE_STARTED) {
            return;
        }

        mal_device__read_frames_from_client(pDevice, pDevice->opensl.periodSizeInFrames, pBuffer);

        SLresult resultSL = MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->Enqueue((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, pBuffer, periodSizeInBytes);
        if (resultSL != SL_RESULT_SUCCESS) {
            return;
        }
    } else {
        mal_device__send_frames_to_client(pDevice, pDevice->opensl.periodSizeInFrames, pBuffer);

        SLresult resultSL = MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->Enqueue((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, pBuffer, periodSizeInBytes);
        if (resultSL != SL_RESULT_SUCCESS) {
            return;
        }
    }

    pDevice->opensl.currentBufferIndex = (pDevice->opensl.currentBufferIndex + 1) % pDevice->periods;
}
#endif

void mal_device_uninit__opensl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    // Uninit device.
    if (pDevice->type == mal_device_type_playback) {
        if (pDevice->opensl.pAudioPlayerObj) MAL_OPENSL_OBJ(pDevice->opensl.pAudioPlayerObj)->Destroy((SLObjectItf)pDevice->opensl.pAudioPlayerObj);
        if (pDevice->opensl.pOutputMixObj) MAL_OPENSL_OBJ(pDevice->opensl.pOutputMixObj)->Destroy((SLObjectItf)pDevice->opensl.pOutputMixObj);
    } else {
        if (pDevice->opensl.pAudioRecorderObj) MAL_OPENSL_OBJ(pDevice->opensl.pAudioRecorderObj)->Destroy((SLObjectItf)pDevice->opensl.pAudioRecorderObj);
    }

    mal_free(pDevice->opensl.pBuffer);
}

mal_result mal_device_init__opensl(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    (void)pContext;

    // For now, only supporting Android implementations of OpenSL|ES since that's the only one I've
    // been able to test with and I currently depend on Android-specific extensions (simple buffer
    // queues).
#ifndef MAL_ANDROID
    return MAL_NO_BACKEND;
#endif

    // Use s32 as the internal format for when floating point is specified.
    if (pConfig->format == mal_format_f32) {
        pDevice->internalFormat = mal_format_s32;
    }

    // Now we can start initializing the device properly.
    mal_assert(pDevice != NULL);
    mal_zero_object(&pDevice->opensl);

    pDevice->opensl.currentBufferIndex = 0;
    pDevice->opensl.periodSizeInFrames = pDevice->bufferSizeInFrames / pConfig->periods;
    pDevice->bufferSizeInFrames = pDevice->opensl.periodSizeInFrames * pConfig->periods;

    SLDataLocator_AndroidSimpleBufferQueue queue;
    queue.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    queue.numBuffers = pConfig->periods;

    SLDataFormat_PCM* pFormat = NULL;

#if defined(MAL_ANDROID) && __ANDROID_API__ >= 21
    SLAndroidDataFormat_PCM_EX pcmEx;
    if (pDevice->format == mal_format_f32 /*|| pDevice->format == mal_format_f64*/) {
        pcmEx.formatType = SL_ANDROID_DATAFORMAT_PCM_EX;
        pcmEx.representation = SL_ANDROID_PCM_REPRESENTATION_FLOAT;
    } else {
        pcmEx.formatType = SL_DATAFORMAT_PCM;
    }
    pFormat = (SLDataFormat_PCM*)&pcmEx;
#else
    SLDataFormat_PCM pcm;
    pcm.formatType = SL_DATAFORMAT_PCM;
    pFormat = &pcm;
#endif

    pFormat->numChannels   = pDevice->channels;
    pFormat->samplesPerSec = mal_round_to_standard_sample_rate__opensl(pDevice->sampleRate * 1000);  // In millihertz.
    pFormat->bitsPerSample = mal_get_sample_size_in_bytes(pDevice->format)*8;
    pFormat->containerSize = pFormat->bitsPerSample;  // Always tightly packed for now.
    pFormat->channelMask   = mal_channel_map_to_channel_mask__opensl(pConfig->channelMap, pFormat->numChannels);
    pFormat->endianness    = SL_BYTEORDER_LITTLEENDIAN;

    // Android has a few restrictions on the format as documented here: https://developer.android.com/ndk/guides/audio/opensl-for-android.html
    //  - Only mono and stereo is supported.
    //  - Only u8 and s16 formats are supported.
    //  - Maximum sample rate of 48000.
#ifdef MAL_ANDROID
    if (pFormat->numChannels > 2) {
        pFormat->numChannels = 2;
    }
#if __ANDROID_API__ >= 21
    if (pFormat->formatType == SL_ANDROID_DATAFORMAT_PCM_EX) {
        // It's floating point.
        mal_assert(pcmEx.representation == SL_ANDROID_PCM_REPRESENTATION_FLOAT);
        if (pFormat->bitsPerSample > 32) {
            pFormat->bitsPerSample = 32;
        }
    } else {
        if (pFormat->bitsPerSample > 16) {
            pFormat->bitsPerSample = 16;
        }
    }
#else
    if (pFormat->bitsPerSample > 16) {
        pFormat->bitsPerSample = 16;
    }
#endif
    pFormat->containerSize = pFormat->bitsPerSample;  // Always tightly packed for now.

    if (pFormat->samplesPerSec > SL_SAMPLINGRATE_48) {
        pFormat->samplesPerSec = SL_SAMPLINGRATE_48;
    }
#endif

    if (type == mal_device_type_playback) {
        SLresult resultSL = (*g_malEngineSL)->CreateOutputMix(g_malEngineSL, (SLObjectItf*)&pDevice->opensl.pOutputMixObj, 0, NULL, NULL);
        if (resultSL != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to create output mix.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pOutputMixObj)->Realize((SLObjectItf)pDevice->opensl.pOutputMixObj, SL_BOOLEAN_FALSE)) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to realize output mix object.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pOutputMixObj)->GetInterface((SLObjectItf)pDevice->opensl.pOutputMixObj, SL_IID_OUTPUTMIX, &pDevice->opensl.pOutputMix) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to retrieve SL_IID_OUTPUTMIX interface.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        // Set the output device.
        if (pDeviceID != NULL) {
            MAL_OPENSL_OUTPUTMIX(pDevice->opensl.pOutputMix)->ReRoute((SLOutputMixItf)pDevice->opensl.pOutputMix, 1, &pDeviceID->opensl);
        }

        SLDataSource source;
        source.pLocator = &queue;
        source.pFormat = pFormat;

        SLDataLocator_OutputMix outmixLocator;
        outmixLocator.locatorType = SL_DATALOCATOR_OUTPUTMIX;
        outmixLocator.outputMix = (SLObjectItf)pDevice->opensl.pOutputMixObj;

        SLDataSink sink;
        sink.pLocator = &outmixLocator;
        sink.pFormat = NULL;

        const SLInterfaceID itfIDs1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
        const SLboolean itfIDsRequired1[] = {SL_BOOLEAN_TRUE};
        resultSL = (*g_malEngineSL)->CreateAudioPlayer(g_malEngineSL, (SLObjectItf*)&pDevice->opensl.pAudioPlayerObj, &source, &sink, 1, itfIDs1, itfIDsRequired1);
        if (resultSL == SL_RESULT_CONTENT_UNSUPPORTED) {
            // Unsupported format. Fall back to something safer and try again. If this fails, just abort.
            pFormat->formatType = SL_DATAFORMAT_PCM;
            pFormat->numChannels = 2;
            pFormat->samplesPerSec = SL_SAMPLINGRATE_16;
            pFormat->bitsPerSample = 16;
            pFormat->containerSize = pFormat->bitsPerSample;  // Always tightly packed for now.
            pFormat->channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            resultSL = (*g_malEngineSL)->CreateAudioPlayer(g_malEngineSL, (SLObjectItf*)&pDevice->opensl.pAudioPlayerObj, &source, &sink, 1, itfIDs1, itfIDsRequired1);
        }

        if (resultSL != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to create audio player.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }


        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioPlayerObj)->Realize((SLObjectItf)pDevice->opensl.pAudioPlayerObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to realize audio player.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioPlayerObj)->GetInterface((SLObjectItf)pDevice->opensl.pAudioPlayerObj, SL_IID_PLAY, &pDevice->opensl.pAudioPlayer) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to retrieve SL_IID_PLAY interface.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioPlayerObj)->GetInterface((SLObjectItf)pDevice->opensl.pAudioPlayerObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &pDevice->opensl.pBufferQueue) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to retrieve SL_IID_ANDROIDSIMPLEBUFFERQUEUE interface.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->RegisterCallback((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, mal_buffer_queue_callback__opensl_android, pDevice) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to register buffer queue callback.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    } else {
        SLDataLocator_IODevice locatorDevice;
        locatorDevice.locatorType = SL_DATALOCATOR_IODEVICE;
        locatorDevice.deviceType = SL_IODEVICE_AUDIOINPUT;
        locatorDevice.deviceID = (pDeviceID == NULL) ? SL_DEFAULTDEVICEID_AUDIOINPUT : pDeviceID->opensl;
        locatorDevice.device = NULL;

        SLDataSource source;
        source.pLocator = &locatorDevice;
        source.pFormat = NULL;

        SLDataSink sink;
        sink.pLocator = &queue;
        sink.pFormat = pFormat;

        const SLInterfaceID itfIDs1[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
        const SLboolean itfIDsRequired1[] = {SL_BOOLEAN_TRUE};
        SLresult resultSL = (*g_malEngineSL)->CreateAudioRecorder(g_malEngineSL, (SLObjectItf*)&pDevice->opensl.pAudioRecorderObj, &source, &sink, 1, itfIDs1, itfIDsRequired1);
        if (resultSL == SL_RESULT_CONTENT_UNSUPPORTED) {
            // Unsupported format. Fall back to something safer and try again. If this fails, just abort.
            pFormat->formatType = SL_DATAFORMAT_PCM;
            pFormat->numChannels = 1;
            pFormat->samplesPerSec = SL_SAMPLINGRATE_16;
            pFormat->bitsPerSample = 16;
            pFormat->containerSize = pFormat->bitsPerSample;  // Always tightly packed for now.
            pFormat->channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
            resultSL = (*g_malEngineSL)->CreateAudioRecorder(g_malEngineSL, (SLObjectItf*)&pDevice->opensl.pAudioRecorderObj, &source, &sink, 1, itfIDs1, itfIDsRequired1);
        }

        if (resultSL != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to create audio recorder.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioRecorderObj)->Realize((SLObjectItf)pDevice->opensl.pAudioRecorderObj, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to realize audio recorder.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioRecorderObj)->GetInterface((SLObjectItf)pDevice->opensl.pAudioRecorderObj, SL_IID_RECORD, &pDevice->opensl.pAudioRecorder) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to retrieve SL_IID_RECORD interface.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_OBJ(pDevice->opensl.pAudioRecorderObj)->GetInterface((SLObjectItf)pDevice->opensl.pAudioRecorderObj, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &pDevice->opensl.pBufferQueue) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to retrieve SL_IID_ANDROIDSIMPLEBUFFERQUEUE interface.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }

        if (MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->RegisterCallback((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, mal_buffer_queue_callback__opensl_android, pDevice) != SL_RESULT_SUCCESS) {
            mal_device_uninit__opensl(pDevice);
            return mal_post_error(pDevice, "[OpenSL] Failed to register buffer queue callback.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    }


    // The internal format is determined by the pFormat object.
    mal_bool32 isFloatingPoint = MAL_FALSE;
#if defined(MAL_ANDROID) && __ANDROID_API__ >= 21
    if (pFormat->formatType == SL_ANDROID_DATAFORMAT_PCM_EX) {
        mal_assert(pcmEx.representation == SL_ANDROID_PCM_REPRESENTATION_FLOAT);
        isFloatingPoint = MAL_TRUE;
    }
#endif
    if (isFloatingPoint) {
        if (pFormat->bitsPerSample == 32) {
            pDevice->internalFormat = mal_format_f32;
        }
#if 0
        if (pFormat->bitsPerSample == 64) {
            pDevice->internalFormat = mal_format_f64;
        }
#endif
    } else {
        if (pFormat->bitsPerSample == 8) {
            pDevice->internalFormat = mal_format_u8;
        } else if (pFormat->bitsPerSample == 16) {
            pDevice->internalFormat = mal_format_s16;
        } else if (pFormat->bitsPerSample == 24) {
            pDevice->internalFormat = mal_format_s24;
        } else if (pFormat->bitsPerSample == 32) {
            pDevice->internalFormat = mal_format_s32;
        }
    }

    pDevice->internalChannels = pFormat->numChannels;
    pDevice->internalSampleRate = pFormat->samplesPerSec / 1000;
    mal_channel_mask_to_channel_map__opensl(pFormat->channelMask, pDevice->internalChannels, pDevice->internalChannelMap);


    size_t bufferSizeInBytes = pDevice->bufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);
    pDevice->opensl.pBuffer = (mal_uint8*)mal_malloc(bufferSizeInBytes);
    if (pDevice->opensl.pBuffer == NULL) {
        mal_device_uninit__opensl(pDevice);
        return mal_post_error(pDevice, "[OpenSL] Failed to allocate memory for data buffer.", MAL_OUT_OF_MEMORY);
    }

    mal_zero_memory(pDevice->opensl.pBuffer, bufferSizeInBytes);

    return MAL_SUCCESS;
}

mal_result mal_device__start_backend__opensl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        SLresult resultSL = MAL_OPENSL_PLAY(pDevice->opensl.pAudioPlayer)->SetPlayState((SLPlayItf)pDevice->opensl.pAudioPlayer, SL_PLAYSTATE_PLAYING);
        if (resultSL != SL_RESULT_SUCCESS) {
            return mal_post_error(pDevice, "[OpenSL] Failed to start internal playback device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }

        // We need to enqueue a buffer for each period.
        mal_device__read_frames_from_client(pDevice, pDevice->bufferSizeInFrames, pDevice->opensl.pBuffer);

        size_t periodSizeInBytes = pDevice->opensl.periodSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);
        for (mal_uint32 iPeriod = 0; iPeriod < pDevice->periods; ++iPeriod) {
            resultSL = MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->Enqueue((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, pDevice->opensl.pBuffer + (periodSizeInBytes * iPeriod), periodSizeInBytes);
            if (resultSL != SL_RESULT_SUCCESS) {
                MAL_OPENSL_PLAY(pDevice->opensl.pAudioPlayer)->SetPlayState((SLPlayItf)pDevice->opensl.pAudioPlayer, SL_PLAYSTATE_STOPPED);
                return mal_post_error(pDevice, "[OpenSL] Failed to enqueue buffer for playback device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        }
    } else {
        SLresult resultSL = MAL_OPENSL_RECORD(pDevice->opensl.pAudioRecorder)->SetRecordState((SLRecordItf)pDevice->opensl.pAudioRecorder, SL_RECORDSTATE_RECORDING);
        if (resultSL != SL_RESULT_SUCCESS) {
            return mal_post_error(pDevice, "[OpenSL] Failed to start internal capture device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
        }

        size_t periodSizeInBytes = pDevice->opensl.periodSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat);
        for (mal_uint32 iPeriod = 0; iPeriod < pDevice->periods; ++iPeriod) {
            resultSL = MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->Enqueue((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue, pDevice->opensl.pBuffer + (periodSizeInBytes * iPeriod), periodSizeInBytes);
            if (resultSL != SL_RESULT_SUCCESS) {
                MAL_OPENSL_RECORD(pDevice->opensl.pAudioRecorder)->SetRecordState((SLRecordItf)pDevice->opensl.pAudioRecorder, SL_RECORDSTATE_STOPPED);
                return mal_post_error(pDevice, "[OpenSL] Failed to enqueue buffer for capture device.", MAL_FAILED_TO_START_BACKEND_DEVICE);
            }
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__opensl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        SLresult resultSL = MAL_OPENSL_PLAY(pDevice->opensl.pAudioPlayer)->SetPlayState((SLPlayItf)pDevice->opensl.pAudioPlayer, SL_PLAYSTATE_STOPPED);
        if (resultSL != SL_RESULT_SUCCESS) {
            return mal_post_error(pDevice, "[OpenSL] Failed to stop internal playback device.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
        }
    } else {
        SLresult resultSL = MAL_OPENSL_RECORD(pDevice->opensl.pAudioRecorder)->SetRecordState((SLRecordItf)pDevice->opensl.pAudioRecorder, SL_RECORDSTATE_STOPPED);
        if (resultSL != SL_RESULT_SUCCESS) {
            return mal_post_error(pDevice, "[OpenSL] Failed to stop internal capture device.", MAL_FAILED_TO_STOP_BACKEND_DEVICE);
        }
    }

    // Make sure any queued buffers are cleared.
    MAL_OPENSL_BUFFERQUEUE(pDevice->opensl.pBufferQueue)->Clear((SLAndroidSimpleBufferQueueItf)pDevice->opensl.pBufferQueue);

    // Make sure the client is aware that the device has stopped. There may be an OpenSL|ES callback for this, but I haven't found it.
    mal_device__set_state(pDevice, MAL_STATE_STOPPED);
    if (pDevice->onStop) {
        pDevice->onStop(pDevice);
    }

    return MAL_SUCCESS;
}
#endif  // OpenSL|ES

///////////////////////////////////////////////////////////////////////////////
//
// OpenAL Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_OPENAL
#ifdef MAL_WIN32
#define MAL_AL_APIENTRY __cdecl
#else
#define MAL_AL_APIENTRY
#endif

#ifdef MAL_NO_RUNTIME_LINKING
    #if defined(MAL_APPLE)
        #include <OpenAL/al.h>
        #include <OpenAL/alc.h>
    #else
        #include <AL/al.h>
        #include <AL/alc.h>
    #endif
#endif

typedef struct mal_ALCdevice_struct  mal_ALCdevice;
typedef struct mal_ALCcontext_struct mal_ALCcontext;
typedef char                         mal_ALCboolean;
typedef char                         mal_ALCchar;
typedef signed char                  mal_ALCbyte;
typedef unsigned char                mal_ALCubyte;
typedef short                        mal_ALCshort;
typedef unsigned short               mal_ALCushort;
typedef int                          mal_ALCint;
typedef unsigned int                 mal_ALCuint;
typedef int                          mal_ALCsizei;
typedef int                          mal_ALCenum;
typedef float                        mal_ALCfloat;
typedef double                       mal_ALCdouble;
typedef void                         mal_ALCvoid;

typedef mal_ALCboolean               mal_ALboolean;
typedef mal_ALCchar                  mal_ALchar;
typedef mal_ALCbyte                  mal_ALbyte;
typedef mal_ALCubyte                 mal_ALubyte;
typedef mal_ALCshort                 mal_ALshort;
typedef mal_ALCushort                mal_ALushort;
typedef mal_ALCint                   mal_ALint;
typedef mal_ALCuint                  mal_ALuint;
typedef mal_ALCsizei                 mal_ALsizei;
typedef mal_ALCenum                  mal_ALenum;
typedef mal_ALCfloat                 mal_ALfloat;
typedef mal_ALCdouble                mal_ALdouble;
typedef mal_ALCvoid                  mal_ALvoid;

#define MAL_ALC_DEVICE_SPECIFIER            0x1005
#define MAL_ALC_CAPTURE_DEVICE_SPECIFIER    0x310
#define MAL_ALC_CAPTURE_SAMPLES             0x312

#define MAL_AL_SOURCE_STATE                 0x1010
#define MAL_AL_INITIAL                      0x1011
#define MAL_AL_PLAYING                      0x1012
#define MAL_AL_PAUSED                       0x1013
#define MAL_AL_STOPPED                      0x1014
#define MAL_AL_BUFFERS_PROCESSED            0x1016

#define MAL_AL_FORMAT_MONO8                 0x1100
#define MAL_AL_FORMAT_MONO16                0x1101
#define MAL_AL_FORMAT_STEREO8               0x1102
#define MAL_AL_FORMAT_STEREO16              0x1103
#define MAL_AL_FORMAT_MONO_FLOAT32          0x10010
#define MAL_AL_FORMAT_STEREO_FLOAT32        0x10011
#define MAL_AL_FORMAT_51CHN16               0x120B
#define MAL_AL_FORMAT_51CHN32               0x120C
#define MAL_AL_FORMAT_51CHN8                0x120A
#define MAL_AL_FORMAT_61CHN16               0x120E
#define MAL_AL_FORMAT_61CHN32               0x120F
#define MAL_AL_FORMAT_61CHN8                0x120D
#define MAL_AL_FORMAT_71CHN16               0x1211
#define MAL_AL_FORMAT_71CHN32               0x1212
#define MAL_AL_FORMAT_71CHN8                0x1210
#define MAL_AL_FORMAT_QUAD16                0x1205
#define MAL_AL_FORMAT_QUAD32                0x1206
#define MAL_AL_FORMAT_QUAD8                 0x1204
#define MAL_AL_FORMAT_REAR16                0x1208
#define MAL_AL_FORMAT_REAR32                0x1209
#define MAL_AL_FORMAT_REAR8                 0x1207

typedef mal_ALCcontext*    (MAL_AL_APIENTRY * MAL_LPALCCREATECONTEXT)      (mal_ALCdevice *device, const mal_ALCint *attrlist);
typedef mal_ALCboolean     (MAL_AL_APIENTRY * MAL_LPALCMAKECONTEXTCURRENT) (mal_ALCcontext *context);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCPROCESSCONTEXT)     (mal_ALCcontext *context);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCSUSPENDCONTEXT)     (mal_ALCcontext *context);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCDESTROYCONTEXT)     (mal_ALCcontext *context);
typedef mal_ALCcontext*    (MAL_AL_APIENTRY * MAL_LPALCGETCURRENTCONTEXT)  (void);
typedef mal_ALCdevice*     (MAL_AL_APIENTRY * MAL_LPALCGETCONTEXTSDEVICE)  (mal_ALCcontext *context);
typedef mal_ALCdevice*     (MAL_AL_APIENTRY * MAL_LPALCOPENDEVICE)         (const mal_ALCchar *devicename);
typedef mal_ALCboolean     (MAL_AL_APIENTRY * MAL_LPALCCLOSEDEVICE)        (mal_ALCdevice *device);
typedef mal_ALCenum        (MAL_AL_APIENTRY * MAL_LPALCGETERROR)           (mal_ALCdevice *device);
typedef mal_ALCboolean     (MAL_AL_APIENTRY * MAL_LPALCISEXTENSIONPRESENT) (mal_ALCdevice *device, const mal_ALCchar *extname);
typedef void*              (MAL_AL_APIENTRY * MAL_LPALCGETPROCADDRESS)     (mal_ALCdevice *device, const mal_ALCchar *funcname);
typedef mal_ALCenum        (MAL_AL_APIENTRY * MAL_LPALCGETENUMVALUE)       (mal_ALCdevice *device, const mal_ALCchar *enumname);
typedef const mal_ALCchar* (MAL_AL_APIENTRY * MAL_LPALCGETSTRING)          (mal_ALCdevice *device, mal_ALCenum param);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCGETINTEGERV)        (mal_ALCdevice *device, mal_ALCenum param, mal_ALCsizei size, mal_ALCint *values);
typedef mal_ALCdevice*     (MAL_AL_APIENTRY * MAL_LPALCCAPTUREOPENDEVICE)  (const mal_ALCchar *devicename, mal_ALCuint frequency, mal_ALCenum format, mal_ALCsizei buffersize);
typedef mal_ALCboolean     (MAL_AL_APIENTRY * MAL_LPALCCAPTURECLOSEDEVICE) (mal_ALCdevice *device);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCCAPTURESTART)       (mal_ALCdevice *device);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCCAPTURESTOP)        (mal_ALCdevice *device);
typedef void               (MAL_AL_APIENTRY * MAL_LPALCCAPTURESAMPLES)     (mal_ALCdevice *device, mal_ALCvoid *buffer, mal_ALCsizei samples);

typedef void               (MAL_AL_APIENTRY * MAL_LPALENABLE)              (mal_ALenum capability);
typedef void               (MAL_AL_APIENTRY * MAL_LPALDISABLE)             (mal_ALenum capability);
typedef mal_ALboolean      (MAL_AL_APIENTRY * MAL_LPALISENABLED)           (mal_ALenum capability);
typedef const mal_ALchar*  (MAL_AL_APIENTRY * MAL_LPALGETSTRING)           (mal_ALenum param);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBOOLEANV)         (mal_ALenum param, mal_ALboolean *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETINTEGERV)         (mal_ALenum param, mal_ALint *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETFLOATV)           (mal_ALenum param, mal_ALfloat *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETDOUBLEV)          (mal_ALenum param, mal_ALdouble *values);
typedef mal_ALboolean      (MAL_AL_APIENTRY * MAL_LPALGETBOOLEAN)          (mal_ALenum param);
typedef mal_ALint          (MAL_AL_APIENTRY * MAL_LPALGETINTEGER)          (mal_ALenum param);
typedef mal_ALfloat        (MAL_AL_APIENTRY * MAL_LPALGETFLOAT)            (mal_ALenum param);
typedef mal_ALdouble       (MAL_AL_APIENTRY * MAL_LPALGETDOUBLE)           (mal_ALenum param);
typedef mal_ALenum         (MAL_AL_APIENTRY * MAL_LPALGETERROR)            (void);
typedef mal_ALboolean      (MAL_AL_APIENTRY * MAL_LPALISEXTENSIONPRESENT)  (const mal_ALchar *extname);
typedef void*              (MAL_AL_APIENTRY * MAL_LPALGETPROCADDRESS)      (const mal_ALchar *fname);
typedef mal_ALenum         (MAL_AL_APIENTRY * MAL_LPALGETENUMVALUE)        (const mal_ALchar *ename);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGENSOURCES)          (mal_ALsizei n, mal_ALuint *sources);
typedef void               (MAL_AL_APIENTRY * MAL_LPALDELETESOURCES)       (mal_ALsizei n, const mal_ALuint *sources);
typedef mal_ALboolean      (MAL_AL_APIENTRY * MAL_LPALISSOURCE)            (mal_ALuint source);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEF)             (mal_ALuint source, mal_ALenum param, mal_ALfloat value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCE3F)            (mal_ALuint source, mal_ALenum param, mal_ALfloat value1, mal_ALfloat value2, mal_ALfloat value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEFV)            (mal_ALuint source, mal_ALenum param, const mal_ALfloat *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEI)             (mal_ALuint source, mal_ALenum param, mal_ALint value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCE3I)            (mal_ALuint source, mal_ALenum param, mal_ALint value1, mal_ALint value2, mal_ALint value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEIV)            (mal_ALuint source, mal_ALenum param, const mal_ALint *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCEF)          (mal_ALuint source, mal_ALenum param, mal_ALfloat *value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCE3F)         (mal_ALuint source, mal_ALenum param, mal_ALfloat *value1, mal_ALfloat *value2, mal_ALfloat *value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCEFV)         (mal_ALuint source, mal_ALenum param, mal_ALfloat *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCEI)          (mal_ALuint source, mal_ALenum param, mal_ALint *value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCE3I)         (mal_ALuint source, mal_ALenum param, mal_ALint *value1, mal_ALint *value2, mal_ALint *value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETSOURCEIV)         (mal_ALuint source, mal_ALenum param, mal_ALint *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEPLAYV)         (mal_ALsizei n, const mal_ALuint *sources);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCESTOPV)         (mal_ALsizei n, const mal_ALuint *sources);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEREWINDV)       (mal_ALsizei n, const mal_ALuint *sources);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEPAUSEV)        (mal_ALsizei n, const mal_ALuint *sources);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEPLAY)          (mal_ALuint source);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCESTOP)          (mal_ALuint source);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEREWIND)        (mal_ALuint source);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEPAUSE)         (mal_ALuint source);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEQUEUEBUFFERS)  (mal_ALuint source, mal_ALsizei nb, const mal_ALuint *buffers);
typedef void               (MAL_AL_APIENTRY * MAL_LPALSOURCEUNQUEUEBUFFERS)(mal_ALuint source, mal_ALsizei nb, mal_ALuint *buffers);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGENBUFFERS)          (mal_ALsizei n, mal_ALuint *buffers);
typedef void               (MAL_AL_APIENTRY * MAL_LPALDELETEBUFFERS)       (mal_ALsizei n, const mal_ALuint *buffers);
typedef mal_ALboolean      (MAL_AL_APIENTRY * MAL_LPALISBUFFER)            (mal_ALuint buffer);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFERDATA)          (mal_ALuint buffer, mal_ALenum format, const mal_ALvoid *data, mal_ALsizei size, mal_ALsizei freq);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFERF)             (mal_ALuint buffer, mal_ALenum param, mal_ALfloat value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFER3F)            (mal_ALuint buffer, mal_ALenum param, mal_ALfloat value1, mal_ALfloat value2, mal_ALfloat value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFERFV)            (mal_ALuint buffer, mal_ALenum param, const mal_ALfloat *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFERI)             (mal_ALuint buffer, mal_ALenum param, mal_ALint value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFER3I)            (mal_ALuint buffer, mal_ALenum param, mal_ALint value1, mal_ALint value2, mal_ALint value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALBUFFERIV)            (mal_ALuint buffer, mal_ALenum param, const mal_ALint *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFERF)          (mal_ALuint buffer, mal_ALenum param, mal_ALfloat *value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFER3F)         (mal_ALuint buffer, mal_ALenum param, mal_ALfloat *value1, mal_ALfloat *value2, mal_ALfloat *value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFERFV)         (mal_ALuint buffer, mal_ALenum param, mal_ALfloat *values);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFERI)          (mal_ALuint buffer, mal_ALenum param, mal_ALint *value);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFER3I)         (mal_ALuint buffer, mal_ALenum param, mal_ALint *value1, mal_ALint *value2, mal_ALint *value3);
typedef void               (MAL_AL_APIENTRY * MAL_LPALGETBUFFERIV)         (mal_ALuint buffer, mal_ALenum param, mal_ALint *values);

mal_bool32 mal_context_is_device_id_equal__openal(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return mal_strcmp(pID0->openal, pID1->openal) == 0;
}

mal_result mal_context_enumerate_devices__openal(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

    if (pContext->openal.isEnumerationSupported) {
        mal_bool32 isTerminated = MAL_FALSE;

        // Playback
        if (!isTerminated) {
            const mal_ALCchar* pPlaybackDeviceNames = ((MAL_LPALCGETSTRING)pContext->openal.alcGetString)(NULL, MAL_ALC_DEVICE_SPECIFIER);
            if (pPlaybackDeviceNames == NULL) {
                return MAL_NO_DEVICE;
            }

            // Each device is stored in pDeviceNames, separated by a null-terminator. The string itself is double-null-terminated.
            const mal_ALCchar* pNextPlaybackDeviceName = pPlaybackDeviceNames;
            while (pNextPlaybackDeviceName[0] != '\0') {
                mal_device_info deviceInfo;
                mal_zero_object(&deviceInfo);
                mal_strncpy_s(deviceInfo.id.openal, sizeof(deviceInfo.id.openal), (const char*)pNextPlaybackDeviceName, (size_t)-1);
                mal_strncpy_s(deviceInfo.name,      sizeof(deviceInfo.name),      (const char*)pNextPlaybackDeviceName, (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }

                // Move to the next device name.
                while (*pNextPlaybackDeviceName != '\0') {
                    pNextPlaybackDeviceName += 1;
                }

                // Skip past the null terminator.
                pNextPlaybackDeviceName += 1;
            };
        }

        // Capture
        if (!isTerminated) {
            const mal_ALCchar* pCaptureDeviceNames = ((MAL_LPALCGETSTRING)pContext->openal.alcGetString)(NULL, MAL_ALC_CAPTURE_DEVICE_SPECIFIER);
            if (pCaptureDeviceNames == NULL) {
                return MAL_NO_DEVICE;
            }

            const mal_ALCchar* pNextCaptureDeviceName = pCaptureDeviceNames;
            while (pNextCaptureDeviceName[0] != '\0') {
                mal_device_info deviceInfo;
                mal_zero_object(&deviceInfo);
                mal_strncpy_s(deviceInfo.id.openal, sizeof(deviceInfo.id.openal), (const char*)pNextCaptureDeviceName, (size_t)-1);
                mal_strncpy_s(deviceInfo.name,      sizeof(deviceInfo.name),      (const char*)pNextCaptureDeviceName, (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }

                // Move to the next device name.
                while (*pNextCaptureDeviceName != '\0') {
                    pNextCaptureDeviceName += 1;
                }

                // Skip past the null terminator.
                pNextCaptureDeviceName += 1;
            };
        }
    } else {
        // Enumeration is not supported. Use default devices.
        mal_bool32 cbResult = MAL_TRUE;

        // Playback.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
        }

        // Capture.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
        }
    }

    return MAL_SUCCESS;
}


typedef struct
{
    mal_device_type deviceType;
    const mal_device_id* pDeviceID;
    mal_share_mode shareMode;
    mal_device_info* pDeviceInfo;
    mal_bool32 foundDevice;
} mal_context_get_device_info_enum_callback_data__openal;

mal_bool32 mal_context_get_device_info_enum_callback__openal(mal_context* pContext, mal_device_type deviceType, const mal_device_info* pDeviceInfo, void* pUserData)
{
    mal_context_get_device_info_enum_callback_data__openal* pData = (mal_context_get_device_info_enum_callback_data__openal*)pUserData;
    mal_assert(pData != NULL);

    if (pData->deviceType == deviceType && mal_context_is_device_id_equal__openal(pContext, pData->pDeviceID, &pDeviceInfo->id)) {
        mal_strncpy_s(pData->pDeviceInfo->name, sizeof(pData->pDeviceInfo->name), pDeviceInfo->name, (size_t)-1);
        pData->foundDevice = MAL_TRUE;
    }

    // Keep enumerating until we have found the device.
    return !pData->foundDevice;
}

mal_result mal_context_get_device_info__openal(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

    // Name / Description
    if (pDeviceID == NULL) {
        if (deviceType == mal_device_type_playback) {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        } else {
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        }

        return MAL_SUCCESS;
    } else {
        mal_context_get_device_info_enum_callback_data__openal data;
        data.deviceType = deviceType;
        data.pDeviceID = pDeviceID;
        data.shareMode = shareMode;
        data.pDeviceInfo = pDeviceInfo;
        data.foundDevice = MAL_FALSE;
        mal_result result = mal_context_enumerate_devices__openal(pContext, mal_context_get_device_info_enum_callback__openal, &data);
        if (result != MAL_SUCCESS) {
            return result;
        }

        if (data.foundDevice) {
            return MAL_SUCCESS;
        } else {
            return MAL_NO_DEVICE;
        }
    }
}

mal_result mal_context_init__openal(mal_context* pContext)
{
    mal_assert(pContext != NULL);

#ifndef MAL_NO_RUNTIME_LINKING
    const char* libNames[] = {
#if defined(MAL_WIN32)
        "OpenAL32.dll",
        "soft_oal.dll"
#endif
#if defined(MAL_UNIX) && !defined(MAL_APPLE)
        "libopenal.so",
        "libopenal.so.1"
#endif
#if defined(MAL_APPLE)
        "OpenAL.framework/OpenAL"
#endif
    };

    for (size_t i = 0; i < mal_countof(libNames); ++i) {
        pContext->openal.hOpenAL = mal_dlopen(libNames[i]);
        if (pContext->openal.hOpenAL != NULL) {
            break;
        }
    }

    if (pContext->openal.hOpenAL == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    pContext->openal.alcCreateContext       = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCreateContext");
    pContext->openal.alcMakeContextCurrent  = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcMakeContextCurrent");
    pContext->openal.alcProcessContext      = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcProcessContext");
    pContext->openal.alcSuspendContext      = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcSuspendContext");
    pContext->openal.alcDestroyContext      = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcDestroyContext");
    pContext->openal.alcGetCurrentContext   = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetCurrentContext");
    pContext->openal.alcGetContextsDevice   = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetContextsDevice");
    pContext->openal.alcOpenDevice          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcOpenDevice");
    pContext->openal.alcCloseDevice         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCloseDevice");
    pContext->openal.alcGetError            = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetError");
    pContext->openal.alcIsExtensionPresent  = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcIsExtensionPresent");
    pContext->openal.alcGetProcAddress      = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetProcAddress");
    pContext->openal.alcGetEnumValue        = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetEnumValue");
    pContext->openal.alcGetString           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetString");
    pContext->openal.alcGetIntegerv         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcGetIntegerv");
    pContext->openal.alcCaptureOpenDevice   = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCaptureOpenDevice");
    pContext->openal.alcCaptureCloseDevice  = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCaptureCloseDevice");
    pContext->openal.alcCaptureStart        = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCaptureStart");
    pContext->openal.alcCaptureStop         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCaptureStop");
    pContext->openal.alcCaptureSamples      = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alcCaptureSamples");

    pContext->openal.alEnable               = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alEnable");
    pContext->openal.alDisable              = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alDisable");
    pContext->openal.alIsEnabled            = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alIsEnabled");
    pContext->openal.alGetString            = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetString");
    pContext->openal.alGetBooleanv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBooleanv");
    pContext->openal.alGetIntegerv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetIntegerv");
    pContext->openal.alGetFloatv            = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetFloatv");
    pContext->openal.alGetDoublev           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetDoublev");
    pContext->openal.alGetBoolean           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBoolean");
    pContext->openal.alGetInteger           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetInteger");
    pContext->openal.alGetFloat             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetFloat");
    pContext->openal.alGetDouble            = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetDouble");
    pContext->openal.alGetError             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetError");
    pContext->openal.alIsExtensionPresent   = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alIsExtensionPresent");
    pContext->openal.alGetProcAddress       = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetProcAddress");
    pContext->openal.alGetEnumValue         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetEnumValue");
    pContext->openal.alGenSources           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGenSources");
    pContext->openal.alDeleteSources        = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alDeleteSources");
    pContext->openal.alIsSource             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alIsSource");
    pContext->openal.alSourcef              = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcef");
    pContext->openal.alSource3f             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSource3f");
    pContext->openal.alSourcefv             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcefv");
    pContext->openal.alSourcei              = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcei");
    pContext->openal.alSource3i             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSource3i");
    pContext->openal.alSourceiv             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceiv");
    pContext->openal.alGetSourcef           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSourcef");
    pContext->openal.alGetSource3f          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSource3f");
    pContext->openal.alGetSourcefv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSourcefv");
    pContext->openal.alGetSourcei           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSourcei");
    pContext->openal.alGetSource3i          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSource3i");
    pContext->openal.alGetSourceiv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetSourceiv");
    pContext->openal.alSourcePlayv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcePlayv");
    pContext->openal.alSourceStopv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceStopv");
    pContext->openal.alSourceRewindv        = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceRewindv");
    pContext->openal.alSourcePausev         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcePausev");
    pContext->openal.alSourcePlay           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcePlay");
    pContext->openal.alSourceStop           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceStop");
    pContext->openal.alSourceRewind         = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceRewind");
    pContext->openal.alSourcePause          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourcePause");
    pContext->openal.alSourceQueueBuffers   = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceQueueBuffers");
    pContext->openal.alSourceUnqueueBuffers = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alSourceUnqueueBuffers");
    pContext->openal.alGenBuffers           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGenBuffers");
    pContext->openal.alDeleteBuffers        = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alDeleteBuffers");
    pContext->openal.alIsBuffer             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alIsBuffer");
    pContext->openal.alBufferData           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBufferData");
    pContext->openal.alBufferf              = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBufferf");
    pContext->openal.alBuffer3f             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBuffer3f");
    pContext->openal.alBufferfv             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBufferfv");
    pContext->openal.alBufferi              = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBufferi");
    pContext->openal.alBuffer3i             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBuffer3i");
    pContext->openal.alBufferiv             = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alBufferiv");
    pContext->openal.alGetBufferf           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBufferf");
    pContext->openal.alGetBuffer3f          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBuffer3f");
    pContext->openal.alGetBufferfv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBufferfv");
    pContext->openal.alGetBufferi           = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBufferi");
    pContext->openal.alGetBuffer3i          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBuffer3i");
    pContext->openal.alGetBufferiv          = (mal_proc)mal_dlsym(pContext->openal.hOpenAL, "alGetBufferiv");
#else
    pContext->openal.alcCreateContext       = (mal_proc)alcCreateContext;
    pContext->openal.alcMakeContextCurrent  = (mal_proc)alcMakeContextCurrent;
    pContext->openal.alcProcessContext      = (mal_proc)alcProcessContext;
    pContext->openal.alcSuspendContext      = (mal_proc)alcSuspendContext;
    pContext->openal.alcDestroyContext      = (mal_proc)alcDestroyContext;
    pContext->openal.alcGetCurrentContext   = (mal_proc)alcGetCurrentContext;
    pContext->openal.alcGetContextsDevice   = (mal_proc)alcGetContextsDevice;
    pContext->openal.alcOpenDevice          = (mal_proc)alcOpenDevice;
    pContext->openal.alcCloseDevice         = (mal_proc)alcCloseDevice;
    pContext->openal.alcGetError            = (mal_proc)alcGetError;
    pContext->openal.alcIsExtensionPresent  = (mal_proc)alcIsExtensionPresent;
    pContext->openal.alcGetProcAddress      = (mal_proc)alcGetProcAddress;
    pContext->openal.alcGetEnumValue        = (mal_proc)alcGetEnumValue;
    pContext->openal.alcGetString           = (mal_proc)alcGetString;
    pContext->openal.alcGetIntegerv         = (mal_proc)alcGetIntegerv;
    pContext->openal.alcCaptureOpenDevice   = (mal_proc)alcCaptureOpenDevice;
    pContext->openal.alcCaptureCloseDevice  = (mal_proc)alcCaptureCloseDevice;
    pContext->openal.alcCaptureStart        = (mal_proc)alcCaptureStart;
    pContext->openal.alcCaptureStop         = (mal_proc)alcCaptureStop;
    pContext->openal.alcCaptureSamples      = (mal_proc)alcCaptureSamples;

    pContext->openal.alEnable               = (mal_proc)alEnable;
    pContext->openal.alDisable              = (mal_proc)alDisable;
    pContext->openal.alIsEnabled            = (mal_proc)alIsEnabled;
    pContext->openal.alGetString            = (mal_proc)alGetString;
    pContext->openal.alGetBooleanv          = (mal_proc)alGetBooleanv;
    pContext->openal.alGetIntegerv          = (mal_proc)alGetIntegerv;
    pContext->openal.alGetFloatv            = (mal_proc)alGetFloatv;
    pContext->openal.alGetDoublev           = (mal_proc)alGetDoublev;
    pContext->openal.alGetBoolean           = (mal_proc)alGetBoolean;
    pContext->openal.alGetInteger           = (mal_proc)alGetInteger;
    pContext->openal.alGetFloat             = (mal_proc)alGetFloat;
    pContext->openal.alGetDouble            = (mal_proc)alGetDouble;
    pContext->openal.alGetError             = (mal_proc)alGetError;
    pContext->openal.alIsExtensionPresent   = (mal_proc)alIsExtensionPresent;
    pContext->openal.alGetProcAddress       = (mal_proc)alGetProcAddress;
    pContext->openal.alGetEnumValue         = (mal_proc)alGetEnumValue;
    pContext->openal.alGenSources           = (mal_proc)alGenSources;
    pContext->openal.alDeleteSources        = (mal_proc)alDeleteSources;
    pContext->openal.alIsSource             = (mal_proc)alIsSource;
    pContext->openal.alSourcef              = (mal_proc)alSourcef;
    pContext->openal.alSource3f             = (mal_proc)alSource3f;
    pContext->openal.alSourcefv             = (mal_proc)alSourcefv;
    pContext->openal.alSourcei              = (mal_proc)alSourcei;
    pContext->openal.alSource3i             = (mal_proc)alSource3i;
    pContext->openal.alSourceiv             = (mal_proc)alSourceiv;
    pContext->openal.alGetSourcef           = (mal_proc)alGetSourcef;
    pContext->openal.alGetSource3f          = (mal_proc)alGetSource3f;
    pContext->openal.alGetSourcefv          = (mal_proc)alGetSourcefv;
    pContext->openal.alGetSourcei           = (mal_proc)alGetSourcei;
    pContext->openal.alGetSource3i          = (mal_proc)alGetSource3i;
    pContext->openal.alGetSourceiv          = (mal_proc)alGetSourceiv;
    pContext->openal.alSourcePlayv          = (mal_proc)alSourcePlayv;
    pContext->openal.alSourceStopv          = (mal_proc)alSourceStopv;
    pContext->openal.alSourceRewindv        = (mal_proc)alSourceRewindv;
    pContext->openal.alSourcePausev         = (mal_proc)alSourcePausev;
    pContext->openal.alSourcePlay           = (mal_proc)alSourcePlay;
    pContext->openal.alSourceStop           = (mal_proc)alSourceStop;
    pContext->openal.alSourceRewind         = (mal_proc)alSourceRewind;
    pContext->openal.alSourcePause          = (mal_proc)alSourcePause;
    pContext->openal.alSourceQueueBuffers   = (mal_proc)alSourceQueueBuffers;
    pContext->openal.alSourceUnqueueBuffers = (mal_proc)alSourceUnqueueBuffers;
    pContext->openal.alGenBuffers           = (mal_proc)alGenBuffers;
    pContext->openal.alDeleteBuffers        = (mal_proc)alDeleteBuffers;
    pContext->openal.alIsBuffer             = (mal_proc)alIsBuffer;
    pContext->openal.alBufferData           = (mal_proc)alBufferData;
    pContext->openal.alBufferf              = (mal_proc)alBufferf;
    pContext->openal.alBuffer3f             = (mal_proc)alBuffer3f;
    pContext->openal.alBufferfv             = (mal_proc)alBufferfv;
    pContext->openal.alBufferi              = (mal_proc)alBufferi;
    pContext->openal.alBuffer3i             = (mal_proc)alBuffer3i;
    pContext->openal.alBufferiv             = (mal_proc)alBufferiv;
    pContext->openal.alGetBufferf           = (mal_proc)alGetBufferf;
    pContext->openal.alGetBuffer3f          = (mal_proc)alGetBuffer3f;
    pContext->openal.alGetBufferfv          = (mal_proc)alGetBufferfv;
    pContext->openal.alGetBufferi           = (mal_proc)alGetBufferi;
    pContext->openal.alGetBuffer3i          = (mal_proc)alGetBuffer3i;
    pContext->openal.alGetBufferiv          = (mal_proc)alGetBufferiv;
#endif

    // We depend on the ALC_ENUMERATION_EXT extension for enumeration. If this is not supported we fall back to default devices.
    pContext->openal.isEnumerationSupported = ((MAL_LPALCISEXTENSIONPRESENT)pContext->openal.alcIsExtensionPresent)(NULL, "ALC_ENUMERATION_EXT");
    pContext->openal.isFloat32Supported     = ((MAL_LPALISEXTENSIONPRESENT)pContext->openal.alIsExtensionPresent)("AL_EXT_float32");
    pContext->openal.isMCFormatsSupported   = ((MAL_LPALISEXTENSIONPRESENT)pContext->openal.alIsExtensionPresent)("AL_EXT_MCFORMATS");

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__openal;
    pContext->onEnumDevices   = mal_context_enumerate_devices__openal;
    pContext->onGetDeviceInfo = mal_context_get_device_info__openal;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__openal(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_openal);

#ifndef MAL_NO_RUNTIME_LINKING
    mal_dlclose(pContext->openal.hOpenAL);
#endif

    return MAL_SUCCESS;
}

void mal_device_uninit__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    
    // Delete buffers and source first.
    ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)((mal_ALCcontext*)pDevice->openal.pContextALC);
    if (pDevice->openal.sourceAL != 0) {
        ((MAL_LPALDELETESOURCES)pDevice->pContext->openal.alDeleteSources)(1, (const mal_ALuint*)&pDevice->openal.sourceAL);
    }
    if (pDevice->periods > 0 && pDevice->openal.buffersAL[0] != 0) {
        ((MAL_LPALDELETEBUFFERS)pDevice->pContext->openal.alDeleteBuffers)(pDevice->periods, (const mal_ALuint*)pDevice->openal.buffersAL);
    }


    // Now that resources have been deleted we can destroy the OpenAL context and close the device.
    ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)(NULL);
    ((MAL_LPALCDESTROYCONTEXT)pDevice->pContext->openal.alcDestroyContext)((mal_ALCcontext*)pDevice->openal.pContextALC);

    if (pDevice->type == mal_device_type_playback) {
        ((MAL_LPALCCLOSEDEVICE)pDevice->pContext->openal.alcCloseDevice)((mal_ALCdevice*)pDevice->openal.pDeviceALC);
    } else {
        ((MAL_LPALCCAPTURECLOSEDEVICE)pDevice->pContext->openal.alcCaptureCloseDevice)((mal_ALCdevice*)pDevice->openal.pDeviceALC);
    }

    mal_free(pDevice->openal.pIntermediaryBuffer);
}

mal_result mal_device_init__openal(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    if (pDevice->periods > MAL_MAX_PERIODS_OPENAL) {
        pDevice->periods = MAL_MAX_PERIODS_OPENAL;
    }

    // OpenAL has bad latency in my testing :(
    if (pDevice->usingDefaultBufferSize) {
        pDevice->bufferSizeInFrames *= 4;
    }

    mal_ALCsizei bufferSizeInSamplesAL = pDevice->bufferSizeInFrames;
    mal_ALCuint frequencyAL = pConfig->sampleRate;

    mal_uint32 channelsAL = 0;

    // OpenAL currently only supports only mono and stereo. TODO: Check for the AL_EXT_MCFORMATS extension and use one of those formats for quad, 5.1, etc.
    mal_ALCenum formatAL = 0;
    if (pConfig->channels == 1) {
        // Mono.
        channelsAL = 1;
        if (pConfig->format == mal_format_f32) {
            if (pContext->openal.isFloat32Supported) {
                formatAL = MAL_AL_FORMAT_MONO_FLOAT32;
            } else {
                formatAL = MAL_AL_FORMAT_MONO16;
            }
        } else if (pConfig->format == mal_format_s32) {
            formatAL = MAL_AL_FORMAT_MONO16;
        } else if (pConfig->format == mal_format_s24) {
            formatAL = MAL_AL_FORMAT_MONO16;
        } else if (pConfig->format == mal_format_s16) {
            formatAL = MAL_AL_FORMAT_MONO16;
        } else if (pConfig->format == mal_format_u8) {
            formatAL = MAL_AL_FORMAT_MONO8;
        }
    } else {
        // Stereo.
        channelsAL = 2;
        if (pConfig->format == mal_format_f32) {
            if (pContext->openal.isFloat32Supported) {
                formatAL = MAL_AL_FORMAT_STEREO_FLOAT32;
            } else {
                formatAL = MAL_AL_FORMAT_STEREO16;
            }
        } else if (pConfig->format == mal_format_s32) {
            formatAL = MAL_AL_FORMAT_STEREO16;
        } else if (pConfig->format == mal_format_s24) {
            formatAL = MAL_AL_FORMAT_STEREO16;
        } else if (pConfig->format == mal_format_s16) {
            formatAL = MAL_AL_FORMAT_STEREO16;
        } else if (pConfig->format == mal_format_u8) {
            formatAL = MAL_AL_FORMAT_STEREO8;
        }
    }

    if (formatAL == 0) {
        return mal_context_post_error(pContext, NULL, "[OpenAL] Format not supported.", MAL_FORMAT_NOT_SUPPORTED);
    }

    bufferSizeInSamplesAL *= channelsAL;


    // OpenAL feels a bit unintuitive to me... The global object is a device, and it would appear that each device can have
    // many context's...
    mal_ALCdevice* pDeviceALC = NULL;
    if (type == mal_device_type_playback) {
        pDeviceALC = ((MAL_LPALCOPENDEVICE)pContext->openal.alcOpenDevice)((pDeviceID == NULL) ? NULL : pDeviceID->openal);
    } else {
        pDeviceALC = ((MAL_LPALCCAPTUREOPENDEVICE)pContext->openal.alcCaptureOpenDevice)((pDeviceID == NULL) ? NULL : pDeviceID->openal, frequencyAL, formatAL, bufferSizeInSamplesAL);
    }

    if (pDeviceALC == NULL) {
        return mal_context_post_error(pContext, NULL, "[OpenAL] Failed to open device.", MAL_FAILED_TO_INIT_BACKEND);
    }

    // A context is only required for playback.
    mal_ALCcontext* pContextALC = NULL;
    if (pDevice->type == mal_device_type_playback) {
        pContextALC = ((MAL_LPALCCREATECONTEXT)pContext->openal.alcCreateContext)(pDeviceALC, NULL);
        if (pContextALC == NULL) {
            ((MAL_LPALCCLOSEDEVICE)pDevice->pContext->openal.alcCloseDevice)(pDeviceALC);
            return mal_context_post_error(pContext, NULL, "[OpenAL] Failed to open OpenAL context.", MAL_FAILED_TO_INIT_BACKEND);
        }

        ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)(pContextALC);

        mal_ALuint sourceAL;
        ((MAL_LPALGENSOURCES)pDevice->pContext->openal.alGenSources)(1, &sourceAL);
        pDevice->openal.sourceAL = sourceAL;

        // We create the buffers, but only fill and queue them when the device is started.
        mal_ALuint buffersAL[MAL_MAX_PERIODS_OPENAL];
        ((MAL_LPALGENBUFFERS)pDevice->pContext->openal.alGenBuffers)(pDevice->periods, buffersAL);
        for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
            pDevice->openal.buffersAL[i] = buffersAL[i];
        }
    }

    pDevice->internalChannels = channelsAL;
    pDevice->internalSampleRate = frequencyAL;

    switch (formatAL)
    {
        case MAL_AL_FORMAT_MONO8:
        case MAL_AL_FORMAT_STEREO8:
        case MAL_AL_FORMAT_REAR8:
        case MAL_AL_FORMAT_QUAD8:
        case MAL_AL_FORMAT_51CHN8:
        case MAL_AL_FORMAT_61CHN8:
        case MAL_AL_FORMAT_71CHN8:
        {
            pDevice->internalFormat = mal_format_u8;
        } break;

        case MAL_AL_FORMAT_MONO16:
        case MAL_AL_FORMAT_STEREO16:
        case MAL_AL_FORMAT_REAR16:
        case MAL_AL_FORMAT_QUAD16:
        case MAL_AL_FORMAT_51CHN16:
        case MAL_AL_FORMAT_61CHN16:
        case MAL_AL_FORMAT_71CHN16:
        {
            pDevice->internalFormat = mal_format_s16;
        } break;

        case MAL_AL_FORMAT_REAR32:
        case MAL_AL_FORMAT_QUAD32:
        case MAL_AL_FORMAT_51CHN32:
        case MAL_AL_FORMAT_61CHN32:
        case MAL_AL_FORMAT_71CHN32:
        {
            pDevice->internalFormat = mal_format_s32;
        } break;

        case MAL_AL_FORMAT_MONO_FLOAT32:
        case MAL_AL_FORMAT_STEREO_FLOAT32:
        {
            pDevice->internalFormat = mal_format_f32;
        } break;
    }

    // From what I can tell, the ordering of channels is fixed for OpenAL.
    switch (formatAL)
    {
        case MAL_AL_FORMAT_MONO8:
        case MAL_AL_FORMAT_MONO16:
        case MAL_AL_FORMAT_MONO_FLOAT32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case MAL_AL_FORMAT_STEREO8:
        case MAL_AL_FORMAT_STEREO16:
        case MAL_AL_FORMAT_STEREO_FLOAT32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
        } break;

        case MAL_AL_FORMAT_REAR8:
        case MAL_AL_FORMAT_REAR16:
        case MAL_AL_FORMAT_REAR32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_BACK_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case MAL_AL_FORMAT_QUAD8:
        case MAL_AL_FORMAT_QUAD16:
        case MAL_AL_FORMAT_QUAD32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            pDevice->internalChannelMap[2] = MAL_CHANNEL_BACK_LEFT;
            pDevice->internalChannelMap[3] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case MAL_AL_FORMAT_51CHN8:
        case MAL_AL_FORMAT_51CHN16:
        case MAL_AL_FORMAT_51CHN32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            pDevice->internalChannelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            pDevice->internalChannelMap[3] = MAL_CHANNEL_LFE;
            pDevice->internalChannelMap[4] = MAL_CHANNEL_BACK_LEFT;
            pDevice->internalChannelMap[5] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case MAL_AL_FORMAT_61CHN8:
        case MAL_AL_FORMAT_61CHN16:
        case MAL_AL_FORMAT_61CHN32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            pDevice->internalChannelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            pDevice->internalChannelMap[3] = MAL_CHANNEL_LFE;
            pDevice->internalChannelMap[4] = MAL_CHANNEL_BACK_CENTER;
            pDevice->internalChannelMap[5] = MAL_CHANNEL_SIDE_LEFT;
            pDevice->internalChannelMap[6] = MAL_CHANNEL_SIDE_RIGHT;
        } break;

        case MAL_AL_FORMAT_71CHN8:
        case MAL_AL_FORMAT_71CHN16:
        case MAL_AL_FORMAT_71CHN32:
        {
            pDevice->internalChannelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            pDevice->internalChannelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            pDevice->internalChannelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            pDevice->internalChannelMap[3] = MAL_CHANNEL_LFE;
            pDevice->internalChannelMap[4] = MAL_CHANNEL_BACK_LEFT;
            pDevice->internalChannelMap[5] = MAL_CHANNEL_BACK_RIGHT;
            pDevice->internalChannelMap[6] = MAL_CHANNEL_SIDE_LEFT;
            pDevice->internalChannelMap[7] = MAL_CHANNEL_SIDE_RIGHT;
        } break;

        default: break;
    }

    pDevice->openal.pDeviceALC = pDeviceALC;
    pDevice->openal.pContextALC = pContextALC;
    pDevice->openal.formatAL = formatAL;
    pDevice->openal.subBufferSizeInFrames = pDevice->bufferSizeInFrames / pDevice->periods;
    pDevice->openal.pIntermediaryBuffer = (mal_uint8*)mal_malloc(pDevice->openal.subBufferSizeInFrames * channelsAL * mal_get_sample_size_in_bytes(pDevice->internalFormat));
    if (pDevice->openal.pIntermediaryBuffer == NULL) {
        mal_device_uninit__openal(pDevice);
        return mal_context_post_error(pContext, NULL, "[OpenAL] Failed to allocate memory for intermediary buffer.", MAL_OUT_OF_MEMORY);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__start_backend__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        // Playback.
        //
        // When starting playback we want to ensure each buffer is filled and queued before playing the source.
        pDevice->openal.iNextBuffer = 0;

        ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)((mal_ALCcontext*)pDevice->openal.pContextALC);

        for (mal_uint32 i = 0; i < pDevice->periods; ++i) {
            mal_device__read_frames_from_client(pDevice, pDevice->openal.subBufferSizeInFrames, pDevice->openal.pIntermediaryBuffer);

            mal_ALuint bufferAL = pDevice->openal.buffersAL[i];
            ((MAL_LPALBUFFERDATA)pDevice->pContext->openal.alBufferData)(bufferAL, pDevice->openal.formatAL, pDevice->openal.pIntermediaryBuffer, pDevice->openal.subBufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat), pDevice->internalSampleRate);
            ((MAL_LPALSOURCEQUEUEBUFFERS)pDevice->pContext->openal.alSourceQueueBuffers)(pDevice->openal.sourceAL, 1, &bufferAL);
        }

        // Start the source only after filling and queueing each buffer.
        ((MAL_LPALSOURCEPLAY)pDevice->pContext->openal.alSourcePlay)(pDevice->openal.sourceAL);
    } else {
        // Capture.
        ((MAL_LPALCCAPTURESTART)pDevice->pContext->openal.alcCaptureStart)((mal_ALCdevice*)pDevice->openal.pDeviceALC);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)((mal_ALCcontext*)pDevice->openal.pContextALC);
        ((MAL_LPALSOURCESTOP)pDevice->pContext->openal.alSourceStop)(pDevice->openal.sourceAL);
    } else {
        ((MAL_LPALCCAPTURESTOP)pDevice->pContext->openal.alcCaptureStop)((mal_ALCdevice*)pDevice->openal.pDeviceALC);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__break_main_loop__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->openal.breakFromMainLoop = MAL_TRUE;
    return MAL_SUCCESS;
}

mal_uint32 mal_device__get_available_frames__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    if (pDevice->type == mal_device_type_playback) {
        ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)((mal_ALCcontext*)pDevice->openal.pContextALC);

        mal_ALint processedBufferCount = 0;
        ((MAL_LPALGETSOURCEI)pDevice->pContext->openal.alGetSourcei)(pDevice->openal.sourceAL, MAL_AL_BUFFERS_PROCESSED, &processedBufferCount);

        return processedBufferCount * pDevice->openal.subBufferSizeInFrames;
    } else {
        mal_ALint samplesAvailable = 0;
        ((MAL_LPALCGETINTEGERV)pDevice->pContext->openal.alcGetIntegerv)((mal_ALCdevice*)pDevice->openal.pDeviceALC, MAL_ALC_CAPTURE_SAMPLES, 1, &samplesAvailable);

        return samplesAvailable / pDevice->channels;
    }
}

mal_uint32 mal_device__wait_for_frames__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    while (!pDevice->openal.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__get_available_frames__openal(pDevice);
        if (framesAvailable > 0) {
            return framesAvailable;
        }

        mal_sleep(1);
    }

    // We'll get here if the loop was terminated. When capturing we want to return whatever is available. For playback we just drop it.
    if (pDevice->type == mal_device_type_playback) {
        return 0;
    } else {
        return mal_device__get_available_frames__openal(pDevice);
    }
}

mal_result mal_device__main_loop__openal(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    pDevice->openal.breakFromMainLoop = MAL_FALSE;
    while (!pDevice->openal.breakFromMainLoop) {
        mal_uint32 framesAvailable = mal_device__wait_for_frames__openal(pDevice);
        if (framesAvailable == 0) {
            continue;
        }

        // If it's a playback device, don't bother grabbing more data if the device is being stopped.
        if (pDevice->openal.breakFromMainLoop && pDevice->type == mal_device_type_playback) {
            return MAL_FALSE;
        }

        if (pDevice->type == mal_device_type_playback) {
            while (framesAvailable > 0) {
                mal_uint32 framesToRead = (framesAvailable > pDevice->openal.subBufferSizeInFrames) ? pDevice->openal.subBufferSizeInFrames : framesAvailable;

                mal_ALuint bufferAL = pDevice->openal.buffersAL[pDevice->openal.iNextBuffer];
                pDevice->openal.iNextBuffer = (pDevice->openal.iNextBuffer + 1) % pDevice->periods;

                mal_device__read_frames_from_client(pDevice, framesToRead, pDevice->openal.pIntermediaryBuffer);

                ((MAL_LPALCMAKECONTEXTCURRENT)pDevice->pContext->openal.alcMakeContextCurrent)((mal_ALCcontext*)pDevice->openal.pContextALC);
                ((MAL_LPALSOURCEUNQUEUEBUFFERS)pDevice->pContext->openal.alSourceUnqueueBuffers)(pDevice->openal.sourceAL, 1, &bufferAL);
                ((MAL_LPALBUFFERDATA)pDevice->pContext->openal.alBufferData)(bufferAL, pDevice->openal.formatAL, pDevice->openal.pIntermediaryBuffer, pDevice->openal.subBufferSizeInFrames * pDevice->internalChannels * mal_get_sample_size_in_bytes(pDevice->internalFormat), pDevice->internalSampleRate);
                ((MAL_LPALSOURCEQUEUEBUFFERS)pDevice->pContext->openal.alSourceQueueBuffers)(pDevice->openal.sourceAL, 1, &bufferAL);

                framesAvailable -= framesToRead;
            }


            // There's a chance the source has stopped playing due to there not being any buffer's queue. Make sure it's restarted.
            mal_ALenum state;
            ((MAL_LPALGETSOURCEI)pDevice->pContext->openal.alGetSourcei)(pDevice->openal.sourceAL, MAL_AL_SOURCE_STATE, &state);

            if (state != MAL_AL_PLAYING) {
                ((MAL_LPALSOURCEPLAY)pDevice->pContext->openal.alSourcePlay)(pDevice->openal.sourceAL);
            }
        } else {
            while (framesAvailable > 0) {
                mal_uint32 framesToSend = (framesAvailable > pDevice->openal.subBufferSizeInFrames) ? pDevice->openal.subBufferSizeInFrames : framesAvailable;
                ((MAL_LPALCCAPTURESAMPLES)pDevice->pContext->openal.alcCaptureSamples)((mal_ALCdevice*)pDevice->openal.pDeviceALC, pDevice->openal.pIntermediaryBuffer, framesToSend);

                mal_device__send_frames_to_client(pDevice, framesToSend, pDevice->openal.pIntermediaryBuffer);
                framesAvailable -= framesToSend;
            }
        }
    }

    return MAL_SUCCESS;
}
#endif  // OpenAL



///////////////////////////////////////////////////////////////////////////////
//
// SDL Backend
//
///////////////////////////////////////////////////////////////////////////////
#ifdef MAL_HAS_SDL

//#define MAL_USE_SDL_1

#define MAL_SDL_INIT_AUDIO                      0x00000010
#define MAL_AUDIO_U8                            0x0008
#define MAL_AUDIO_S16                           0x8010
#define MAL_AUDIO_S32                           0x8020
#define MAL_AUDIO_F32                           0x8120
#define MAL_SDL_AUDIO_ALLOW_FREQUENCY_CHANGE    0x00000001
#define MAL_SDL_AUDIO_ALLOW_FORMAT_CHANGE       0x00000002
#define MAL_SDL_AUDIO_ALLOW_CHANNELS_CHANGE     0x00000004
#define MAL_SDL_AUDIO_ALLOW_ANY_CHANGE          (MAL_SDL_AUDIO_ALLOW_FREQUENCY_CHANGE | MAL_SDL_AUDIO_ALLOW_FORMAT_CHANGE | MAL_SDL_AUDIO_ALLOW_CHANNELS_CHANGE)

// If we are linking at compile time we'll just #include SDL.h. Otherwise we can just redeclare some stuff to avoid the
// need for development packages to be installed.
#ifdef MAL_NO_RUNTIME_LINKING
    #define SDL_MAIN_HANDLED
    #ifdef MAL_EMSCRIPTEN
        #include <SDL/SDL.h>

        // For now just use SDL 1.2 with Emscripten. This avoids the need for "-s USE_SDL=2" at compile time.
        #ifndef MAL_USE_SDL_1
        #define MAL_USE_SDL_1
        #endif
    #else
        #include <SDL2/SDL.h>
    #endif

    typedef SDL_AudioCallback   MAL_SDL_AudioCallback;
    typedef SDL_AudioSpec       MAL_SDL_AudioSpec;
    typedef SDL_AudioFormat     MAL_SDL_AudioFormat;
    typedef SDL_AudioDeviceID   MAL_SDL_AudioDeviceID;
#else
    typedef void (* MAL_SDL_AudioCallback)(void* userdata, mal_uint8* stream, int len);
    typedef mal_uint16 MAL_SDL_AudioFormat;
    typedef mal_uint32 MAL_SDL_AudioDeviceID;

    typedef struct MAL_SDL_AudioSpec
    {
        int freq;
        MAL_SDL_AudioFormat format;
        mal_uint8 channels;
        mal_uint8 silence;
        mal_uint16 samples;
        mal_uint16 padding;
        mal_uint32 size;
        MAL_SDL_AudioCallback callback;
        void* userdata;
    } MAL_SDL_AudioSpec;
#endif

typedef int                   (* MAL_PFN_SDL_InitSubSystem)(mal_uint32 flags);
typedef void                  (* MAL_PFN_SDL_QuitSubSystem)(mal_uint32 flags);
typedef int                   (* MAL_PFN_SDL_GetNumAudioDevices)(int iscapture);
typedef const char*           (* MAL_PFN_SDL_GetAudioDeviceName)(int index, int iscapture);
typedef void                  (* MAL_PFN_SDL_CloseAudio)(void);
typedef void                  (* MAL_PFN_SDL_CloseAudioDevice)(MAL_SDL_AudioDeviceID dev);
typedef int                   (* MAL_PFN_SDL_OpenAudio)(MAL_SDL_AudioSpec* desired, MAL_SDL_AudioSpec* obtained);
typedef MAL_SDL_AudioDeviceID (* MAL_PFN_SDL_OpenAudioDevice)(const char* device, int iscapture, const MAL_SDL_AudioSpec* desired, MAL_SDL_AudioSpec* obtained, int allowed_changes);
typedef void                  (* MAL_PFN_SDL_PauseAudio)(int pause_on);
typedef void                  (* MAL_PFN_SDL_PauseAudioDevice)(MAL_SDL_AudioDeviceID dev, int pause_on);

MAL_SDL_AudioFormat mal_format_to_sdl(mal_format format)
{
    switch (format)
    {
    case mal_format_unknown: return 0;
    case mal_format_u8:      return MAL_AUDIO_U8;
    case mal_format_s16:     return MAL_AUDIO_S16;
    case mal_format_s24:     return MAL_AUDIO_S32;  // Closest match.
    case mal_format_s32:     return MAL_AUDIO_S32;
    default:                 return 0;
    }
}

mal_format mal_format_from_sdl(MAL_SDL_AudioFormat format)
{
    switch (format)
    {
        case MAL_AUDIO_U8:  return mal_format_u8;
        case MAL_AUDIO_S16: return mal_format_s16;
        case MAL_AUDIO_S32: return mal_format_s32;
        case MAL_AUDIO_F32: return mal_format_f32;
        default:            return mal_format_unknown;
    }
}

mal_bool32 mal_context_is_device_id_equal__sdl(mal_context* pContext, const mal_device_id* pID0, const mal_device_id* pID1)
{
    mal_assert(pContext != NULL);
    mal_assert(pID0 != NULL);
    mal_assert(pID1 != NULL);
    (void)pContext;

    return pID0->sdl == pID1->sdl;
}

mal_result mal_context_enumerate_devices__sdl(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    mal_assert(pContext != NULL);
    mal_assert(callback != NULL);

#ifndef MAL_USE_SDL_1
    if (!pContext->sdl.usingSDL1) {
        mal_bool32 isTerminated = MAL_FALSE;

        // Playback
        if (!isTerminated) {
            int deviceCount = ((MAL_PFN_SDL_GetNumAudioDevices)pContext->sdl.SDL_GetNumAudioDevices)(0);
            for (int i = 0; i < deviceCount; ++i) {
                mal_device_info deviceInfo;
                mal_zero_object(&deviceInfo);

                deviceInfo.id.sdl = i;
                mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), ((MAL_PFN_SDL_GetAudioDeviceName)pContext->sdl.SDL_GetAudioDeviceName)(i, 0), (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }
            }
        }

        // Capture
        if (!isTerminated) {
            int deviceCount = ((MAL_PFN_SDL_GetNumAudioDevices)pContext->sdl.SDL_GetNumAudioDevices)(1);
            for (int i = 0; i < deviceCount; ++i) {
                mal_device_info deviceInfo;
                mal_zero_object(&deviceInfo);

                deviceInfo.id.sdl = i;
                mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), ((MAL_PFN_SDL_GetAudioDeviceName)pContext->sdl.SDL_GetAudioDeviceName)(i, 1), (size_t)-1);

                mal_bool32 cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
                if (cbResult == MAL_FALSE) {
                    isTerminated = MAL_TRUE;
                    break;
                }
            }
        }
    } else
#endif
    {
        // SDL1 only uses default devices.
        mal_bool32 cbResult = MAL_TRUE;

        // Playback.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_playback, &deviceInfo, pUserData);
        }

        // Capture.
        if (cbResult) {
            mal_device_info deviceInfo;
            mal_zero_object(&deviceInfo);
            mal_strncpy_s(deviceInfo.name, sizeof(deviceInfo.name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
            cbResult = callback(pContext, mal_device_type_capture, &deviceInfo, pUserData);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_get_device_info__sdl(mal_context* pContext, mal_device_type deviceType, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    mal_assert(pContext != NULL);
    (void)shareMode;

#ifndef MAL_USE_SDL_1
    if (!pContext->sdl.usingSDL1) {
        if (pDeviceID == NULL) {
            if (deviceType == mal_device_type_playback) {
                pDeviceInfo->id.sdl = 0;
                mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
            } else {
                pDeviceInfo->id.sdl = 0;
                mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
            }
        } else {
            pDeviceInfo->id.sdl = pDeviceID->sdl;
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), ((MAL_PFN_SDL_GetAudioDeviceName)pContext->sdl.SDL_GetAudioDeviceName)(pDeviceID->sdl, (deviceType == mal_device_type_playback) ? 0 : 1), (size_t)-1);
        }
    } else
#endif
    {
        // SDL1 uses default devices.
        if (deviceType == mal_device_type_playback) {
            pDeviceInfo->id.sdl = 0;
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
        } else {
            pDeviceInfo->id.sdl = 0;
            mal_strncpy_s(pDeviceInfo->name, sizeof(pDeviceInfo->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
        }
    }

    return MAL_SUCCESS;
}

mal_result mal_context_init__sdl(mal_context* pContext)
{
    mal_assert(pContext != NULL);

#ifndef MAL_NO_RUNTIME_LINKING
    // Run-time linking.
    const char* libNames[] = {
#if defined(MAL_WIN32)
        "SDL2.dll",
        "SDL.dll"
#elif defined(MAL_APPLE)
        "libSDL2-2.0.0.dylib",  // Can any Mac users out there comfirm these library names?
        "libSDL-1.2.0.dylib"
#else
        "libSDL2-2.0.so.0",
        "libSDL-1.2.so.0"
#endif
    };

    for (size_t i = 0; i < mal_countof(libNames); ++i) {
        pContext->sdl.hSDL = mal_dlopen(libNames[i]);
        if (pContext->sdl.hSDL != NULL) {
            break;
        }
    }

    if (pContext->sdl.hSDL == NULL) {
        return MAL_NO_BACKEND;  // Couldn't find SDL2.dll, etc. Most likely it's not installed.
    }

    pContext->sdl.SDL_InitSubSystem      = mal_dlsym(pContext->sdl.hSDL, "SDL_InitSubSystem");
    pContext->sdl.SDL_QuitSubSystem      = mal_dlsym(pContext->sdl.hSDL, "SDL_QuitSubSystem");
    pContext->sdl.SDL_CloseAudio         = mal_dlsym(pContext->sdl.hSDL, "SDL_CloseAudio");
    pContext->sdl.SDL_OpenAudio          = mal_dlsym(pContext->sdl.hSDL, "SDL_OpenAudio");
    pContext->sdl.SDL_PauseAudio         = mal_dlsym(pContext->sdl.hSDL, "SDL_PauseAudio");
#ifndef MAL_USE_SDL_1
    pContext->sdl.SDL_GetNumAudioDevices = mal_dlsym(pContext->sdl.hSDL, "SDL_GetNumAudioDevices");
    pContext->sdl.SDL_GetAudioDeviceName = mal_dlsym(pContext->sdl.hSDL, "SDL_GetAudioDeviceName");
    pContext->sdl.SDL_CloseAudioDevice   = mal_dlsym(pContext->sdl.hSDL, "SDL_CloseAudioDevice");
    pContext->sdl.SDL_OpenAudioDevice    = mal_dlsym(pContext->sdl.hSDL, "SDL_OpenAudioDevice");
    pContext->sdl.SDL_PauseAudioDevice   = mal_dlsym(pContext->sdl.hSDL, "SDL_PauseAudioDevice");
#endif
#else
    // Compile-time linking.
    pContext->sdl.SDL_InitSubSystem      = (mal_proc)SDL_InitSubSystem;
    pContext->sdl.SDL_QuitSubSystem      = (mal_proc)SDL_QuitSubSystem;
    pContext->sdl.SDL_CloseAudio         = (mal_proc)SDL_CloseAudio;
    pContext->sdl.SDL_OpenAudio          = (mal_proc)SDL_OpenAudio;
    pContext->sdl.SDL_PauseAudio         = (mal_proc)SDL_PauseAudio;
#ifndef MAL_USE_SDL_1
    pContext->sdl.SDL_GetNumAudioDevices = (mal_proc)SDL_GetNumAudioDevices;
    pContext->sdl.SDL_GetAudioDeviceName = (mal_proc)SDL_GetAudioDeviceName;
    pContext->sdl.SDL_CloseAudioDevice   = (mal_proc)SDL_CloseAudioDevice;
    pContext->sdl.SDL_OpenAudioDevice    = (mal_proc)SDL_OpenAudioDevice;
    pContext->sdl.SDL_PauseAudioDevice   = (mal_proc)SDL_PauseAudioDevice;
#endif
#endif

    // We need to determine whether or not we are using SDL2 or SDL1. We can know this by looking at whether or not certain
    // function pointers are NULL.
    if (pContext->sdl.SDL_GetNumAudioDevices == NULL ||
        pContext->sdl.SDL_GetAudioDeviceName == NULL ||
        pContext->sdl.SDL_CloseAudioDevice   == NULL ||
        pContext->sdl.SDL_OpenAudioDevice    == NULL ||
        pContext->sdl.SDL_PauseAudioDevice   == NULL) {
        pContext->sdl.usingSDL1 = MAL_TRUE;
    }

    int resultSDL = ((MAL_PFN_SDL_InitSubSystem)pContext->sdl.SDL_InitSubSystem)(MAL_SDL_INIT_AUDIO);
    if (resultSDL != 0) {
        return MAL_ERROR;
    }

    pContext->onDeviceIDEqual = mal_context_is_device_id_equal__sdl;
    pContext->onEnumDevices   = mal_context_enumerate_devices__sdl;
    pContext->onGetDeviceInfo = mal_context_get_device_info__sdl;

    return MAL_SUCCESS;
}

mal_result mal_context_uninit__sdl(mal_context* pContext)
{
    mal_assert(pContext != NULL);
    mal_assert(pContext->backend == mal_backend_sdl);

    ((MAL_PFN_SDL_QuitSubSystem)pContext->sdl.SDL_QuitSubSystem)(MAL_SDL_INIT_AUDIO);
    return MAL_SUCCESS;
}

void mal_device_uninit__sdl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

#ifndef MAL_USE_SDL_1
    if (!pDevice->pContext->sdl.usingSDL1) {
        ((MAL_PFN_SDL_CloseAudioDevice)pDevice->pContext->sdl.SDL_CloseAudioDevice)(pDevice->sdl.deviceID);
    } else
#endif
    {
        ((MAL_PFN_SDL_CloseAudio)pDevice->pContext->sdl.SDL_CloseAudio)();
    }
}


void mal_audio_callback__sdl(void* pUserData, mal_uint8* pBuffer, int bufferSizeInBytes)
{
    mal_device* pDevice = (mal_device*)pUserData;
    mal_assert(pDevice != NULL);

    mal_uint32 bufferSizeInFrames = (mal_uint32)bufferSizeInBytes / mal_get_sample_size_in_bytes(pDevice->internalFormat) / pDevice->internalChannels;

    if (pDevice->type == mal_device_type_playback) {
        mal_device__read_frames_from_client(pDevice, bufferSizeInFrames, pBuffer);
    } else {
        mal_device__send_frames_to_client(pDevice, bufferSizeInFrames, pBuffer);
    }
}

mal_result mal_device_init__sdl(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, mal_device* pDevice)
{
    mal_assert(pContext != NULL);
    mal_assert(pConfig != NULL);
    mal_assert(pDevice != NULL);

    (void)pContext;

    // SDL wants the buffer size to be a power of 2. The SDL_AudioSpec property for this is only a Uint16, so we need
    // to explicitly clamp this because it will be easy to overflow.
    mal_uint32 bufferSize = pConfig->bufferSizeInFrames;
    if (bufferSize > 32768) {
        bufferSize = 32768;
    } else {
        bufferSize = mal_next_power_of_2(bufferSize);
    }

    mal_assert(bufferSize <= 32768);


    MAL_SDL_AudioSpec desiredSpec, obtainedSpec;
    mal_zero_memory(&desiredSpec, sizeof(desiredSpec));
    desiredSpec.freq     = (int)pConfig->sampleRate;
    desiredSpec.format   = mal_format_to_sdl(pConfig->format);
    desiredSpec.channels = (mal_uint8)pConfig->channels;
    desiredSpec.samples  = (mal_uint16)bufferSize;
    desiredSpec.callback = mal_audio_callback__sdl;
    desiredSpec.userdata = pDevice;

    // Fall back to f32 if we don't have an appropriate mapping between mini_al and SDL.
    if (desiredSpec.format == 0) {
        desiredSpec.format = MAL_AUDIO_F32;
    }

#ifndef MAL_USE_SDL_1
    if (!pDevice->pContext->sdl.usingSDL1) {
        int isCapture = (type == mal_device_type_playback) ? 0 : 1;

        const char* pDeviceName = NULL;
        if (pDeviceID != NULL) {
            pDeviceName = ((MAL_PFN_SDL_GetAudioDeviceName)pDevice->pContext->sdl.SDL_GetAudioDeviceName)(pDeviceID->sdl, isCapture);
        }

        pDevice->sdl.deviceID = ((MAL_PFN_SDL_OpenAudioDevice)pDevice->pContext->sdl.SDL_OpenAudioDevice)(pDeviceName, isCapture, &desiredSpec, &obtainedSpec, MAL_SDL_AUDIO_ALLOW_ANY_CHANGE);
        if (pDevice->sdl.deviceID == 0) {
            return mal_post_error(pDevice, "Failed to open SDL device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    } else
#endif
    {
        // SDL1 uses default devices.
        (void)pDeviceID;

        // SDL1 only supports playback as far as I can tell.
        if (type != mal_device_type_playback) {
            return MAL_NO_DEVICE;
        }

        // SDL1 does not support floating point formats.
        if (desiredSpec.format == MAL_AUDIO_F32) {
            desiredSpec.format  = MAL_AUDIO_S16;
        }

        pDevice->sdl.deviceID = ((MAL_PFN_SDL_OpenAudio)pDevice->pContext->sdl.SDL_OpenAudio)(&desiredSpec, &obtainedSpec);
        if (pDevice->sdl.deviceID != 0) {
            return mal_post_error(pDevice, "Failed to open SDL device.", MAL_FAILED_TO_OPEN_BACKEND_DEVICE);
        }
    }

    pDevice->internalFormat     = mal_format_from_sdl(obtainedSpec.format);
    pDevice->internalChannels   = obtainedSpec.channels;
    pDevice->internalSampleRate = (mal_uint32)obtainedSpec.freq;
    pDevice->bufferSizeInFrames = obtainedSpec.samples;
    pDevice->periods            = 1;    // SDL doesn't seem to tell us what the period count is. Just set this 1.

#if 0
    printf("=== SDL CONFIG ===\n");
    printf("REQUESTED -> RECEIVED\n");
    printf("    FORMAT:                 %s -> %s\n", mal_get_format_name(pConfig->format), mal_get_format_name(pDevice->internalFormat));
    printf("    CHANNELS:               %d -> %d\n", desiredSpec.channels, obtainedSpec.channels);
    printf("    SAMPLE RATE:            %d -> %d\n", desiredSpec.freq, obtainedSpec.freq);
    printf("    BUFFER SIZE IN SAMPLES: %d -> %d\n", desiredSpec.samples, obtainedSpec.samples);
#endif

    return MAL_SUCCESS;
}

mal_result mal_device__start_backend__sdl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

#ifndef MAL_USE_SDL_1
    if (!pDevice->pContext->sdl.usingSDL1) {
        ((MAL_PFN_SDL_PauseAudioDevice)pDevice->pContext->sdl.SDL_PauseAudioDevice)(pDevice->sdl.deviceID, 0);
    } else
#endif
    {
        ((MAL_PFN_SDL_PauseAudio)pDevice->pContext->sdl.SDL_PauseAudio)(0);
    }

    return MAL_SUCCESS;
}

mal_result mal_device__stop_backend__sdl(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

#ifndef MAL_USE_SDL_1
    if (!pDevice->pContext->sdl.usingSDL1) {
        ((MAL_PFN_SDL_PauseAudioDevice)pDevice->pContext->sdl.SDL_PauseAudioDevice)(pDevice->sdl.deviceID, 1);
    } else
#endif
    {
        ((MAL_PFN_SDL_PauseAudio)pDevice->pContext->sdl.SDL_PauseAudio)(1);
    }

    return MAL_SUCCESS;
}
#endif  // SDL




mal_bool32 mal__is_channel_map_valid(const mal_channel* channelMap, mal_uint32 channels)
{
    // A blank channel map should be allowed, in which case it should use an appropriate default which will depend on context.
    if (channelMap[0] != MAL_CHANNEL_NONE) {
        if (channels == 0) {
            return MAL_FALSE;   // No channels.
        }

        // A channel cannot be present in the channel map more than once.
        for (mal_uint32 iChannel = 0; iChannel < channels; ++iChannel) {
            for (mal_uint32 jChannel = iChannel + 1; jChannel < channels; ++jChannel) {
                if (channelMap[iChannel] == channelMap[jChannel]) {
                    return MAL_FALSE;
                }
            }
        }
    }

    return MAL_TRUE;
}


mal_result mal_device__start_backend(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_result result = MAL_NO_BACKEND;
#ifdef MAL_HAS_WASAPI
    if (pDevice->pContext->backend == mal_backend_wasapi) {
        result = mal_device__start_backend__wasapi(pDevice);
    }
#endif
#ifdef MAL_HAS_DSOUND
    if (pDevice->pContext->backend == mal_backend_dsound) {
        result = mal_device__start_backend__dsound(pDevice);
    }
#endif
#ifdef MAL_HAS_WINMM
    if (pDevice->pContext->backend == mal_backend_winmm) {
        result = mal_device__start_backend__winmm(pDevice);
    }
#endif
#ifdef MAL_HAS_ALSA
    if (pDevice->pContext->backend == mal_backend_alsa) {
        result = mal_device__start_backend__alsa(pDevice);
    }
#endif
#ifdef MAL_HAS_PULSEAUDIO
    if (pDevice->pContext->backend == mal_backend_pulseaudio) {
        result = mal_device__start_backend__pulse(pDevice);
    }
#endif
#ifdef MAL_HAS_OSS
    if (pDevice->pContext->backend == mal_backend_oss) {
        result = mal_device__start_backend__oss(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENAL
    if (pDevice->pContext->backend == mal_backend_openal) {
        result = mal_device__start_backend__openal(pDevice);
    }
#endif
#ifdef MAL_HAS_NULL
    if (pDevice->pContext->backend == mal_backend_null) {
        result = mal_device__start_backend__null(pDevice);
    }
#endif

    return result;
}

mal_result mal_device__stop_backend(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_result result = MAL_NO_BACKEND;
#ifdef MAL_HAS_WASAPI
    if (pDevice->pContext->backend == mal_backend_wasapi) {
        result = mal_device__stop_backend__wasapi(pDevice);
    }
#endif
#ifdef MAL_HAS_DSOUND
    if (pDevice->pContext->backend == mal_backend_dsound) {
        result = mal_device__stop_backend__dsound(pDevice);
    }
#endif
#ifdef MAL_HAS_WINMM
    if (pDevice->pContext->backend == mal_backend_winmm) {
        result = mal_device__stop_backend__winmm(pDevice);
    }
#endif
#ifdef MAL_HAS_ALSA
    if (pDevice->pContext->backend == mal_backend_alsa) {
        result = mal_device__stop_backend__alsa(pDevice);
    }
#endif
#ifdef MAL_HAS_PULSEAUDIO
    if (pDevice->pContext->backend == mal_backend_pulseaudio) {
        result = mal_device__stop_backend__pulse(pDevice);
    }
#endif
#ifdef MAL_HAS_OSS
    if (pDevice->pContext->backend == mal_backend_oss) {
        result = mal_device__stop_backend__oss(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENAL
    if (pDevice->pContext->backend == mal_backend_openal) {
        result = mal_device__stop_backend__openal(pDevice);
    }
#endif
#ifdef MAL_HAS_NULL
    if (pDevice->pContext->backend == mal_backend_null) {
        result = mal_device__stop_backend__null(pDevice);
    }
#endif

    return result;
}

mal_result mal_device__break_main_loop(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_result result = MAL_NO_BACKEND;
#ifdef MAL_HAS_WASAPI
    if (pDevice->pContext->backend == mal_backend_wasapi) {
        result = mal_device__break_main_loop__wasapi(pDevice);
    }
#endif
#ifdef MAL_HAS_DSOUND
    if (pDevice->pContext->backend == mal_backend_dsound) {
        result = mal_device__break_main_loop__dsound(pDevice);
    }
#endif
#ifdef MAL_HAS_WINMM
    if (pDevice->pContext->backend == mal_backend_winmm) {
        result = mal_device__break_main_loop__winmm(pDevice);
    }
#endif
#ifdef MAL_HAS_ALSA
    if (pDevice->pContext->backend == mal_backend_alsa) {
        result = mal_device__break_main_loop__alsa(pDevice);
    }
#endif
#ifdef MAL_HAS_PULSEAUDIO
    if (pDevice->pContext->backend == mal_backend_pulseaudio) {
        result = mal_device__break_main_loop__pulse(pDevice);
    }
#endif
#ifdef MAL_HAS_OSS
    if (pDevice->pContext->backend == mal_backend_oss) {
        result = mal_device__break_main_loop__oss(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENAL
    if (pDevice->pContext->backend == mal_backend_openal) {
        result = mal_device__break_main_loop__openal(pDevice);
    }
#endif
#ifdef MAL_HAS_NULL
    if (pDevice->pContext->backend == mal_backend_null) {
        result = mal_device__break_main_loop__null(pDevice);
    }
#endif

    return result;
}

mal_result mal_device__main_loop(mal_device* pDevice)
{
    mal_assert(pDevice != NULL);

    mal_result result = MAL_NO_BACKEND;
#ifdef MAL_HAS_WASAPI
    if (pDevice->pContext->backend == mal_backend_wasapi) {
        result = mal_device__main_loop__wasapi(pDevice);
    }
#endif
#ifdef MAL_HAS_DSOUND
    if (pDevice->pContext->backend == mal_backend_dsound) {
        result = mal_device__main_loop__dsound(pDevice);
    }
#endif
#ifdef MAL_HAS_WINMM
    if (pDevice->pContext->backend == mal_backend_winmm) {
        result = mal_device__main_loop__winmm(pDevice);
    }
#endif
#ifdef MAL_HAS_ALSA
    if (pDevice->pContext->backend == mal_backend_alsa) {
        result = mal_device__main_loop__alsa(pDevice);
    }
#endif
#ifdef MAL_HAS_PULSEAUDIO
    if (pDevice->pContext->backend == mal_backend_pulseaudio) {
        result = mal_device__main_loop__pulse(pDevice);
    }
#endif
#ifdef MAL_HAS_OSS
    if (pDevice->pContext->backend == mal_backend_oss) {
        result = mal_device__main_loop__oss(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENAL
    if (pDevice->pContext->backend == mal_backend_openal) {
        result = mal_device__main_loop__openal(pDevice);
    }
#endif
#ifdef MAL_HAS_NULL
    if (pDevice->pContext->backend == mal_backend_null) {
        result = mal_device__main_loop__null(pDevice);
    }
#endif

    return result;
}

mal_thread_result MAL_THREADCALL mal_worker_thread(void* pData)
{
    mal_device* pDevice = (mal_device*)pData;
    mal_assert(pDevice != NULL);

#ifdef MAL_WIN32
    mal_CoInitializeEx(pDevice->pContext, NULL, 0); // 0 = COINIT_MULTITHREADED
#endif

    // This is only used to prevent posting onStop() when the device is first initialized.
    mal_bool32 skipNextStopEvent = MAL_TRUE;

    for (;;) {
        // At the start of iteration the device is stopped - we must explicitly mark it as such.
        mal_device__stop_backend(pDevice);

        if (!skipNextStopEvent) {
            mal_stop_proc onStop = pDevice->onStop;
            if (onStop) {
                onStop(pDevice);
            }
        } else {
            skipNextStopEvent = MAL_FALSE;
        }


        // Let the other threads know that the device has stopped.
        mal_device__set_state(pDevice, MAL_STATE_STOPPED);
        mal_event_signal(&pDevice->stopEvent);

        // We use an event to wait for a request to wake up.
        mal_event_wait(&pDevice->wakeupEvent);

        // Default result code.
        pDevice->workResult = MAL_SUCCESS;

        // Just break if we're terminating.
        if (mal_device__get_state(pDevice) == MAL_STATE_UNINITIALIZED) {
            break;
        }


        // Getting here means we just started the device and we need to wait for the device to
        // either deliver us data (recording) or request more data (playback).
        mal_assert(mal_device__get_state(pDevice) == MAL_STATE_STARTING);

        pDevice->workResult = mal_device__start_backend(pDevice);
        if (pDevice->workResult != MAL_SUCCESS) {
            mal_event_signal(&pDevice->startEvent);
            continue;
        }

        // The thread that requested the device to start playing is waiting for this thread to start the
        // device for real, which is now.
        mal_device__set_state(pDevice, MAL_STATE_STARTED);
        mal_event_signal(&pDevice->startEvent);

        // Now we just enter the main loop. The main loop can be broken with mal_device__break_main_loop().
        mal_device__main_loop(pDevice);
    }

    // Make sure we aren't continuously waiting on a stop event.
    mal_event_signal(&pDevice->stopEvent);  // <-- Is this still needed?

#ifdef MAL_WIN32
    mal_CoUninitialize(pDevice->pContext);
#endif

    return (mal_thread_result)0;
}


// Helper for determining whether or not the given device is initialized.
mal_bool32 mal_device__is_initialized(mal_device* pDevice)
{
    if (pDevice == NULL) return MAL_FALSE;
    return mal_device__get_state(pDevice) != MAL_STATE_UNINITIALIZED;
}


#ifdef MAL_WIN32
mal_result mal_context_uninit_backend_apis__win32(mal_context* pContext)
{
    mal_CoUninitialize(pContext);
    mal_dlclose(pContext->win32.hUser32DLL);
    mal_dlclose(pContext->win32.hOle32DLL);

    return MAL_SUCCESS;
}

mal_result mal_context_init_backend_apis__win32(mal_context* pContext)
{
#ifdef MAL_WIN32_DESKTOP
    // Ole32.dll
    pContext->win32.hOle32DLL = mal_dlopen("ole32.dll");
    if (pContext->win32.hOle32DLL == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    pContext->win32.CoInitializeEx   = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "CoInitializeEx");
    pContext->win32.CoUninitialize   = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "CoUninitialize");
    pContext->win32.CoCreateInstance = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "CoCreateInstance");
    pContext->win32.CoTaskMemFree    = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "CoTaskMemFree");
    pContext->win32.PropVariantClear = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "PropVariantClear");
    pContext->win32.StringFromGUID2  = (mal_proc)mal_dlsym(pContext->win32.hOle32DLL, "StringFromGUID2");


    // User32.dll
    pContext->win32.hUser32DLL = mal_dlopen("user32.dll");
    if (pContext->win32.hUser32DLL == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    pContext->win32.GetForegroundWindow = (mal_proc)mal_dlsym(pContext->win32.hUser32DLL, "GetForegroundWindow");
    pContext->win32.GetDesktopWindow    = (mal_proc)mal_dlsym(pContext->win32.hUser32DLL, "GetDesktopWindow");
#endif

    mal_CoInitializeEx(pContext, NULL, 0);  // 0 = COINIT_MULTITHREADED
    return MAL_SUCCESS;
}
#else
mal_result mal_context_uninit_backend_apis__nix(mal_context* pContext)
{
#if defined(MAL_USE_RUNTIME_LINKING_FOR_PTHREAD) && !defined(MAL_NO_RUNTIME_LINKING)
    mal_dlclose(pContext->posix.pthreadSO);
#else
    (void)pContext;
#endif

    return MAL_SUCCESS;
}

mal_result mal_context_init_backend_apis__nix(mal_context* pContext)
{
    // pthread
#if defined(MAL_USE_RUNTIME_LINKING_FOR_PTHREAD) && !defined(MAL_NO_RUNTIME_LINKING)
    const char* libpthreadFileNames[] = {
        "libpthread.so",
        "libpthread.so.0",
        "libpthread.dylib"
    };

    for (size_t i = 0; i < sizeof(libpthreadFileNames) / sizeof(libpthreadFileNames[0]); ++i) {
        pContext->posix.pthreadSO = mal_dlopen(libpthreadFileNames[i]);
        if (pContext->posix.pthreadSO != NULL) {
            break;
        }
    }

    if (pContext->posix.pthreadSO == NULL) {
        return MAL_FAILED_TO_INIT_BACKEND;
    }

    pContext->posix.pthread_create              = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_create");
    pContext->posix.pthread_join                = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_join");
    pContext->posix.pthread_mutex_init          = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_mutex_init");
    pContext->posix.pthread_mutex_destroy       = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_mutex_destroy");
    pContext->posix.pthread_mutex_lock          = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_mutex_lock");
    pContext->posix.pthread_mutex_unlock        = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_mutex_unlock");
    pContext->posix.pthread_cond_init           = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_cond_init");
    pContext->posix.pthread_cond_destroy        = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_cond_destroy");
    pContext->posix.pthread_cond_wait           = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_cond_wait");
    pContext->posix.pthread_cond_signal         = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_cond_signal");
    pContext->posix.pthread_attr_init           = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_attr_init");
    pContext->posix.pthread_attr_destroy        = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_attr_destroy");
    pContext->posix.pthread_attr_setschedpolicy = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_attr_setschedpolicy");
    pContext->posix.pthread_attr_getschedparam  = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_attr_getschedparam");
    pContext->posix.pthread_attr_setschedparam  = (mal_proc)mal_dlsym(pContext->posix.pthreadSO, "pthread_attr_setschedparam");
#else
    pContext->posix.pthread_create              = (mal_proc)pthread_create;
    pContext->posix.pthread_join                = (mal_proc)pthread_join;
    pContext->posix.pthread_mutex_init          = (mal_proc)pthread_mutex_init;
    pContext->posix.pthread_mutex_destroy       = (mal_proc)pthread_mutex_destroy;
    pContext->posix.pthread_mutex_lock          = (mal_proc)pthread_mutex_lock;
    pContext->posix.pthread_mutex_unlock        = (mal_proc)pthread_mutex_unlock;
    pContext->posix.pthread_cond_init           = (mal_proc)pthread_cond_init;
    pContext->posix.pthread_cond_destroy        = (mal_proc)pthread_cond_destroy;
    pContext->posix.pthread_cond_wait           = (mal_proc)pthread_cond_wait;
    pContext->posix.pthread_cond_signal         = (mal_proc)pthread_cond_signal;
    pContext->posix.pthread_attr_init           = (mal_proc)pthread_attr_init;
    pContext->posix.pthread_attr_destroy        = (mal_proc)pthread_attr_destroy;
    pContext->posix.pthread_attr_setschedpolicy = (mal_proc)pthread_attr_setschedpolicy;
    pContext->posix.pthread_attr_getschedparam  = (mal_proc)pthread_attr_getschedparam;
    pContext->posix.pthread_attr_setschedparam  = (mal_proc)pthread_attr_setschedparam;
#endif

    return MAL_SUCCESS;
}
#endif

mal_result mal_context_init_backend_apis(mal_context* pContext)
{
    mal_result result;
#ifdef MAL_WIN32
    result = mal_context_init_backend_apis__win32(pContext);
#else
    result = mal_context_init_backend_apis__nix(pContext);
#endif

    return result;
}

mal_result mal_context_uninit_backend_apis(mal_context* pContext)
{
    mal_result result;
#ifdef MAL_WIN32
    result = mal_context_uninit_backend_apis__win32(pContext);
#else
    result = mal_context_uninit_backend_apis__nix(pContext);
#endif

    return result;
}

const mal_backend g_malDefaultBackends[] = {
    mal_backend_wasapi,
    mal_backend_dsound,
    mal_backend_winmm,
    mal_backend_oss,
    mal_backend_pulseaudio,
    mal_backend_alsa,
    mal_backend_jack,
    mal_backend_opensl,
    mal_backend_openal,
    mal_backend_sdl,
    mal_backend_null
};

mal_bool32 mal_is_backend_asynchronous(mal_backend backend)
{
    return
        backend == mal_backend_jack   ||
        backend == mal_backend_opensl ||
        backend == mal_backend_sdl;
}

mal_result mal_context_init(const mal_backend backends[], mal_uint32 backendCount, const mal_context_config* pConfig, mal_context* pContext)
{
    if (pContext == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pContext);

    // Always make sure the config is set first to ensure properties are available as soon as possible.
    if (pConfig != NULL) {
        pContext->config = *pConfig;
    } else {
        pContext->config = mal_context_config_init(NULL);
    }

    // Backend APIs need to be initialized first. This is where external libraries will be loaded and linked.
    mal_result result = mal_context_init_backend_apis(pContext);
    if (result != MAL_SUCCESS) {
        return result;
    }

    if (backends == NULL) {
        backends = g_malDefaultBackends;
        backendCount = mal_countof(g_malDefaultBackends);
    }

    mal_assert(backends != NULL);

    for (mal_uint32 iBackend = 0; iBackend < backendCount; ++iBackend) {
        mal_backend backend = backends[iBackend];

        result = MAL_NO_BACKEND;
        switch (backend) {
        #ifdef MAL_HAS_WASAPI
            case mal_backend_wasapi:
            {
                result = mal_context_init__wasapi(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_DSOUND
            case mal_backend_dsound:
            {
                result = mal_context_init__dsound(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_WINMM
            case mal_backend_winmm:
            {
                result = mal_context_init__winmm(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_ALSA
            case mal_backend_alsa:
            {
                result = mal_context_init__alsa(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_PULSEAUDIO
            case mal_backend_pulseaudio:
            {
                result = mal_context_init__pulse(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_JACK
            case mal_backend_jack:
            {
                result = mal_context_init__jack(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_OSS
            case mal_backend_oss:
            {
                result = mal_context_init__oss(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_OPENSL
            case mal_backend_opensl:
            {
                result = mal_context_init__opensl(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_OPENAL
            case mal_backend_openal:
            {
                result = mal_context_init__openal(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_SDL
            case mal_backend_sdl:
            {
                result = mal_context_init__sdl(pContext);
            } break;
        #endif
        #ifdef MAL_HAS_NULL
            case mal_backend_null:
            {
                result = mal_context_init__null(pContext);
            } break;
        #endif

            default: break;
        }

        // If this iteration was successful, return.
        if (result == MAL_SUCCESS) {
            result = mal_mutex_init(pContext, &pContext->deviceEnumLock);
            if (result != MAL_SUCCESS) {
                mal_context_post_error(pContext, NULL, "WARNING: Failed to initialize mutex for device enumeration. mal_context_get_devices() is not thread safe.", MAL_FAILED_TO_CREATE_MUTEX);
            }
            result = mal_mutex_init(pContext, &pContext->deviceInfoLock);
            if (result != MAL_SUCCESS) {
                mal_context_post_error(pContext, NULL, "WARNING: Failed to initialize mutex for device info retrieval. mal_context_get_device_info() is not thread safe.", MAL_FAILED_TO_CREATE_MUTEX);
            }

            pContext->backend = backend;
            return result;
        }
    }

    // If we get here it means an error occurred.
    mal_zero_object(pContext);  // Safety.
    return MAL_NO_BACKEND;
}

mal_result mal_context_uninit(mal_context* pContext)
{
    if (pContext == NULL) return MAL_INVALID_ARGS;

    switch (pContext->backend) {
    #ifdef MAL_HAS_WASAPI
        case mal_backend_wasapi:
        {
            mal_context_uninit__wasapi(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_DSOUND
        case mal_backend_dsound:
        {
            mal_context_uninit__dsound(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_WINMM
        case mal_backend_winmm:
        {
            mal_context_uninit__winmm(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_ALSA
        case mal_backend_alsa:
        {
            mal_context_uninit__alsa(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_PULSEAUDIO
        case mal_backend_pulseaudio:
        {
            mal_context_uninit__pulse(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_JACK
        case mal_backend_jack:
        {
            mal_context_uninit__jack(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_OSS
        case mal_backend_oss:
        {
            mal_context_uninit__oss(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_OPENSL
        case mal_backend_opensl:
        {
            mal_context_uninit__opensl(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_OPENAL
        case mal_backend_openal:
        {
            mal_context_uninit__openal(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_SDL
        case mal_backend_sdl:
        {
            mal_context_uninit__sdl(pContext);
        } break;
    #endif
    #ifdef MAL_HAS_NULL
        case mal_backend_null:
        {
            mal_context_uninit__null(pContext);
        } break;
    #endif

        default: break;
    }

    mal_context_uninit_backend_apis(pContext);
    mal_mutex_uninit(&pContext->deviceEnumLock);
    mal_mutex_uninit(&pContext->deviceInfoLock);
    mal_free(pContext->pDeviceInfos);

    return MAL_SUCCESS;
}


mal_result mal_context_enumerate_devices(mal_context* pContext, mal_enum_devices_callback_proc callback, void* pUserData)
{
    if (pContext == NULL || pContext->onEnumDevices == NULL || callback == NULL) return MAL_INVALID_ARGS;

    mal_result result;
    mal_mutex_lock(&pContext->deviceEnumLock);
    {
        result = pContext->onEnumDevices(pContext, callback, pUserData);
    }
    mal_mutex_unlock(&pContext->deviceEnumLock);

    return result;
}


mal_bool32 mal_context_get_devices__enum_callback(mal_context* pContext, mal_device_type type, const mal_device_info* pInfo, void* pUserData)
{
    (void)pUserData;

    // We need to insert the device info into our main internal buffer. Where it goes depends on the device type. If it's a capture device
    // it's just appended to the end. If it's a playback device it's inserted just before the first capture device.

    // First make sure we have room. Since the number of devices we add to the list is usually relatively small I've decided to use a
    // simple fixed size increment for buffer expansion.
    const mal_uint32 bufferExpansionCount = 2;
    const mal_uint32 totalDeviceInfoCount = pContext->playbackDeviceInfoCount + pContext->captureDeviceInfoCount;

    if (pContext->deviceInfoCapacity >= totalDeviceInfoCount) {
        mal_uint32 newCapacity = totalDeviceInfoCount + bufferExpansionCount;
        mal_device_info* pNewInfos = (mal_device_info*)mal_realloc(pContext->pDeviceInfos, sizeof(*pContext->pDeviceInfos)*newCapacity);
        if (pNewInfos == NULL) {
            return MAL_FALSE;   // Out of memory.
        }

        pContext->pDeviceInfos = pNewInfos;
        pContext->deviceInfoCapacity = newCapacity;
    }

    if (type == mal_device_type_playback) {
        // Playback. Insert just before the first capture device.

        // The first thing to do is move all of the capture devices down a slot.
        mal_uint32 iFirstCaptureDevice = pContext->playbackDeviceInfoCount;
        for (size_t iCaptureDevice = totalDeviceInfoCount; iCaptureDevice > iFirstCaptureDevice; --iCaptureDevice) {
            pContext->pDeviceInfos[iCaptureDevice] = pContext->pDeviceInfos[iCaptureDevice-1];
        }

        // Now just insert where the first capture device was before moving it down a slot.
        pContext->pDeviceInfos[iFirstCaptureDevice] = *pInfo;
        pContext->playbackDeviceInfoCount += 1;
    } else {
        // Capture. Insert at the end.
        pContext->pDeviceInfos[totalDeviceInfoCount] = *pInfo;
        pContext->captureDeviceInfoCount += 1;
    }

    return MAL_TRUE;
}

mal_result mal_context_get_devices(mal_context* pContext, mal_device_info** ppPlaybackDeviceInfos, mal_uint32* pPlaybackDeviceCount, mal_device_info** ppCaptureDeviceInfos, mal_uint32* pCaptureDeviceCount)
{
    // Safety.
    if (ppPlaybackDeviceInfos != NULL) *ppPlaybackDeviceInfos = NULL;
    if (pPlaybackDeviceCount  != NULL) *pPlaybackDeviceCount  = 0;
    if (ppCaptureDeviceInfos  != NULL) *ppCaptureDeviceInfos  = NULL;
    if (pCaptureDeviceCount   != NULL) *pCaptureDeviceCount   = 0;

    if (pContext == NULL || pContext->onEnumDevices == NULL) return MAL_INVALID_ARGS;

    // Note that we don't use mal_context_enumerate_devices() here because we want to do locking at a higher level.
    mal_result result;
    mal_mutex_lock(&pContext->deviceEnumLock);
    {
        // Reset everything first.
        pContext->playbackDeviceInfoCount = 0;
        pContext->captureDeviceInfoCount = 0;

        // Now enumerate over available devices.
        result = pContext->onEnumDevices(pContext, mal_context_get_devices__enum_callback, NULL);
        if (result == MAL_SUCCESS) {
            // Playback devices.
            if (ppPlaybackDeviceInfos != NULL) {
                *ppPlaybackDeviceInfos = pContext->pDeviceInfos;
            }
            if (pPlaybackDeviceCount != NULL) {
                *pPlaybackDeviceCount = pContext->playbackDeviceInfoCount;
            }

            // Capture devices.
            if (ppCaptureDeviceInfos != NULL) {
                *ppCaptureDeviceInfos = pContext->pDeviceInfos + pContext->playbackDeviceInfoCount; // Capture devices come after playback devices.
            }
            if (pCaptureDeviceCount != NULL) {
                *pCaptureDeviceCount = pContext->captureDeviceInfoCount;
            }
        }
    }
    mal_mutex_unlock(&pContext->deviceEnumLock);

    return result;
}

mal_result mal_context_get_device_info(mal_context* pContext, mal_device_type type, const mal_device_id* pDeviceID, mal_share_mode shareMode, mal_device_info* pDeviceInfo)
{
    // NOTE: Do not clear pDeviceInfo on entry. The reason is the pDeviceID may actually point to pDeviceInfo->id which will break things.
    if (pContext == NULL || pDeviceInfo == NULL) {
        return MAL_INVALID_ARGS;
    }

    mal_device_info deviceInfo;
    mal_zero_object(&deviceInfo);

    // Help the backend out by copying over the device ID if we have one.
    if (pDeviceID != NULL) {
        mal_copy_memory(&deviceInfo.id, pDeviceID, sizeof(*pDeviceID));
    }

    // The backend may have an optimized device info retrieval function. If so, try that first.
    if (pContext->onGetDeviceInfo != NULL) {
        mal_result result;
        mal_mutex_lock(&pContext->deviceInfoLock);
        {
            result = pContext->onGetDeviceInfo(pContext, type, pDeviceID, shareMode, &deviceInfo);
        }
        mal_mutex_unlock(&pContext->deviceInfoLock);

        *pDeviceInfo = deviceInfo;
        return result;
    }

    // Getting here means onGetDeviceInfo has not been set.
    return MAL_ERROR;
}


mal_result mal_device_init(mal_context* pContext, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, void* pUserData, mal_device* pDevice)
{
    if (pContext == NULL) {
        return mal_device_init_ex(NULL, 0, NULL, type, pDeviceID, pConfig, pUserData, pDevice);
    }


    if (pDevice == NULL) {
        return mal_post_error(pDevice, "mal_device_init() called with invalid arguments (pDevice == NULL).",  MAL_INVALID_ARGS);
    }

    // The config is allowed to be NULL, in which case we default to mal_device_config_init_default().
    mal_device_config config;
    if (pConfig == NULL) {
        config = mal_device_config_init_default();
    } else {
        config = *pConfig;
    }

    // Basic config validation.
    if (config.channels > MAL_MAX_CHANNELS) {
        return mal_post_error(pDevice, "mal_device_init() called with an invalid config. Channel count cannot exceed 32.", MAL_INVALID_DEVICE_CONFIG);
    }
    if (!mal__is_channel_map_valid(config.channelMap, config.channels)) {
        return mal_post_error(pDevice, "mal_device_init() called with invalid config. Channel map is invalid.", MAL_INVALID_DEVICE_CONFIG);
    }


    mal_zero_object(pDevice);
    pDevice->pContext = pContext;

    // Set the user data and log callback ASAP to ensure it is available for the entire initialization process.
    pDevice->pUserData = pUserData;
    pDevice->onStop = config.onStopCallback;
    pDevice->onSend = config.onSendCallback;
    pDevice->onRecv = config.onRecvCallback;

    if (((size_t)pDevice % sizeof(pDevice)) != 0) {
        if (pContext->config.onLog) {
            pContext->config.onLog(pContext, pDevice, "WARNING: mal_device_init() called for a device that is not properly aligned. Thread safety is not supported.");
        }
    }


    // When passing in 0 for the format/channels/rate/chmap it means the device will be using whatever is chosen by the backend. If everything is set
    // to defaults it means the format conversion pipeline will run on a fast path where data transfer is just passed straight through to the backend.
    if (config.format == mal_format_unknown) {
        config.format = MAL_DEFAULT_FORMAT;
        pDevice->usingDefaultFormat = MAL_TRUE;
    }
    if (config.channels == 0) {
        config.channels = MAL_DEFAULT_CHANNELS;
        pDevice->usingDefaultChannels = MAL_TRUE;
    }
    if (config.sampleRate == 0) {
        config.sampleRate = MAL_DEFAULT_SAMPLE_RATE;
        pDevice->usingDefaultSampleRate = MAL_TRUE;
    }
    if (config.channelMap[0] == MAL_CHANNEL_NONE) {
        pDevice->usingDefaultChannelMap = MAL_TRUE;
    }


    // Default buffer size and periods.
    if (config.bufferSizeInFrames == 0) {
        config.bufferSizeInFrames = (config.sampleRate/1000) * MAL_DEFAULT_BUFFER_SIZE_IN_MILLISECONDS;
        pDevice->usingDefaultBufferSize = MAL_TRUE;
    }
    if (config.periods == 0) {
        config.periods = MAL_DEFAULT_PERIODS;
        pDevice->usingDefaultPeriods = MAL_TRUE;
    }

    pDevice->type = type;
    pDevice->format = config.format;
    pDevice->channels = config.channels;
    pDevice->sampleRate = config.sampleRate;
    mal_copy_memory(pDevice->channelMap, config.channelMap, sizeof(config.channelMap[0]) * config.channels);
    pDevice->bufferSizeInFrames = config.bufferSizeInFrames;
    pDevice->periods = config.periods;

    // The internal format, channel count and sample rate can be modified by the backend.
    pDevice->internalFormat = pDevice->format;
    pDevice->internalChannels = pDevice->channels;
    pDevice->internalSampleRate = pDevice->sampleRate;
    mal_copy_memory(pDevice->internalChannelMap, pDevice->channelMap, sizeof(pDevice->channelMap));

    if (mal_mutex_init(pContext, &pDevice->lock) != MAL_SUCCESS) {
        return mal_post_error(pDevice, "Failed to create mutex.", MAL_FAILED_TO_CREATE_MUTEX);
    }

    // When the device is started, the worker thread is the one that does the actual startup of the backend device. We
    // use a semaphore to wait for the background thread to finish the work. The same applies for stopping the device.
    //
    // Each of these semaphores is released internally by the worker thread when the work is completed. The start
    // semaphore is also used to wake up the worker thread.
    if (mal_event_init(pContext, &pDevice->wakeupEvent) != MAL_SUCCESS) {
        mal_mutex_uninit(&pDevice->lock);
        return mal_post_error(pDevice, "Failed to create worker thread wakeup event.", MAL_FAILED_TO_CREATE_EVENT);
    }
    if (mal_event_init(pContext, &pDevice->startEvent) != MAL_SUCCESS) {
        mal_event_uninit(&pDevice->wakeupEvent);
        mal_mutex_uninit(&pDevice->lock);
        return mal_post_error(pDevice, "Failed to create worker thread start event.", MAL_FAILED_TO_CREATE_EVENT);
    }
    if (mal_event_init(pContext, &pDevice->stopEvent) != MAL_SUCCESS) {
        mal_event_uninit(&pDevice->startEvent);
        mal_event_uninit(&pDevice->wakeupEvent);
        mal_mutex_uninit(&pDevice->lock);
        return mal_post_error(pDevice, "Failed to create worker thread stop event.", MAL_FAILED_TO_CREATE_EVENT);
    }


    mal_result result = MAL_NO_BACKEND;
    switch (pContext->backend)
    {
    #ifdef MAL_HAS_WASAPI
        case mal_backend_wasapi:
        {
            result = mal_device_init__wasapi(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_DSOUND
        case mal_backend_dsound:
        {
            result = mal_device_init__dsound(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_WINMM
        case mal_backend_winmm:
        {
            result = mal_device_init__winmm(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_ALSA
        case mal_backend_alsa:
        {
            result = mal_device_init__alsa(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_PULSEAUDIO
        case mal_backend_pulseaudio:
        {
            result = mal_device_init__pulse(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_JACK
        case mal_backend_jack:
        {
            result = mal_device_init__jack(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_OSS
        case mal_backend_oss:
        {
            result = mal_device_init__oss(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_OPENSL
        case mal_backend_opensl:
        {
            result = mal_device_init__opensl(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_OPENAL
        case mal_backend_openal:
        {
            result = mal_device_init__openal(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_SDL
        case mal_backend_sdl:
        {
            result = mal_device_init__sdl(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif
    #ifdef MAL_HAS_NULL
        case mal_backend_null:
        {
            result = mal_device_init__null(pContext, type, pDeviceID, &config, pDevice);
        } break;
    #endif

        default: break;
    }

    if (result != MAL_SUCCESS) {
        return MAL_NO_BACKEND;  // The error message will have been posted with mal_post_error() by the source of the error so don't bother calling it here.
    }


    // If the backend did not fill out a name for the device, try a generic method.
    if (pDevice->name[0] == '\0') {
        if (mal_context__try_get_device_name_by_id(pContext, type, pDeviceID, pDevice->name, sizeof(pDevice->name)) != MAL_SUCCESS) {
            // We failed to get the device name, so fall back to some generic names.
            if (pDeviceID == NULL) {
                if (type == mal_device_type_playback) {
                    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), MAL_DEFAULT_PLAYBACK_DEVICE_NAME, (size_t)-1);
                } else {
                    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), MAL_DEFAULT_CAPTURE_DEVICE_NAME, (size_t)-1);
                }
            } else {
                if (type == mal_device_type_playback) {
                    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), "Playback Device", (size_t)-1);
                } else {
                    mal_strncpy_s(pDevice->name, sizeof(pDevice->name), "Capture Device", (size_t)-1);
                }
            }
        }
    }

    // If the format/channels/rate is using defaults we need to set these to be the same as the internal config.
    if (pDevice->usingDefaultFormat) {
        pDevice->format = pDevice->internalFormat;
    }
    if (pDevice->usingDefaultChannels) {
        pDevice->channels = pDevice->internalChannels;
    }
    if (pDevice->usingDefaultSampleRate) {
        pDevice->sampleRate = pDevice->internalSampleRate;
    }
    if (pDevice->usingDefaultChannelMap) {
        mal_copy_memory(pDevice->channelMap, pDevice->internalChannelMap, sizeof(pDevice->channelMap));
    }


    // We need a DSP object which is where samples are moved through in order to convert them to the
    // format required by the backend.
    mal_dsp_config dspConfig;
    dspConfig.cacheSizeInFrames = pDevice->bufferSizeInFrames;
    if (type == mal_device_type_playback) {
        dspConfig.formatIn      = pDevice->format;
        dspConfig.channelsIn    = pDevice->channels;
        dspConfig.sampleRateIn  = pDevice->sampleRate;
        mal_copy_memory(dspConfig.channelMapIn, pDevice->channelMap, sizeof(dspConfig.channelMapIn));
        dspConfig.formatOut     = pDevice->internalFormat;
        dspConfig.channelsOut   = pDevice->internalChannels;
        dspConfig.sampleRateOut = pDevice->internalSampleRate;
        mal_copy_memory(dspConfig.channelMapOut, pDevice->internalChannelMap, sizeof(dspConfig.channelMapOut));
        mal_dsp_init(&dspConfig, mal_device__on_read_from_client, pDevice, &pDevice->dsp);
    } else {
        dspConfig.formatIn      = pDevice->internalFormat;
        dspConfig.channelsIn    = pDevice->internalChannels;
        dspConfig.sampleRateIn  = pDevice->internalSampleRate;
        mal_copy_memory(dspConfig.channelMapIn, pDevice->internalChannelMap, sizeof(dspConfig.channelMapIn));
        dspConfig.formatOut     = pDevice->format;
        dspConfig.channelsOut   = pDevice->channels;
        dspConfig.sampleRateOut = pDevice->sampleRate;
        mal_copy_memory(dspConfig.channelMapOut, pDevice->channelMap, sizeof(dspConfig.channelMapOut));
        mal_dsp_init(&dspConfig, mal_device__on_read_from_device, pDevice, &pDevice->dsp);
    }



    // Some backends don't require the worker thread.
    if (!mal_is_backend_asynchronous(pContext->backend)) {
        // The worker thread.
        if (mal_thread_create(pContext, &pDevice->thread, mal_worker_thread, pDevice) != MAL_SUCCESS) {
            mal_device_uninit(pDevice);
            return mal_post_error(pDevice, "Failed to create worker thread.", MAL_FAILED_TO_CREATE_THREAD);
        }

        // Wait for the worker thread to put the device into it's stopped state for real.
        mal_event_wait(&pDevice->stopEvent);
    } else {
        mal_device__set_state(pDevice, MAL_STATE_STOPPED);
    }

    mal_assert(mal_device__get_state(pDevice) == MAL_STATE_STOPPED);
    return MAL_SUCCESS;
}

mal_result mal_device_init_ex(const mal_backend backends[], mal_uint32 backendCount, const mal_context_config* pContextConfig, mal_device_type type, mal_device_id* pDeviceID, const mal_device_config* pConfig, void* pUserData, mal_device* pDevice)
{
    mal_context* pContext = (mal_context*)mal_malloc(sizeof(*pContext));
    if (pContext == NULL) {
        return MAL_OUT_OF_MEMORY;
    }

    if (backends == NULL) {
        backends = g_malDefaultBackends;
        backendCount = mal_countof(g_malDefaultBackends);
    }

    mal_result result = MAL_NO_BACKEND;

    for (mal_uint32 iBackend = 0; iBackend < backendCount; ++iBackend) {
        result = mal_context_init(&backends[iBackend], 1, pContextConfig, pContext);
        if (result == MAL_SUCCESS) {
            result = mal_device_init(pContext, type, pDeviceID, pConfig, pUserData, pDevice);
            if (result == MAL_SUCCESS) {
                break;  // Success.
            } else {
                mal_context_uninit(pContext);   // Failure.
            }
        }
    }

    if (result != MAL_SUCCESS) {
        mal_free(pContext);
        return result;
    }

    pDevice->isOwnerOfContext = MAL_TRUE;
    return result;
}

void mal_device_uninit(mal_device* pDevice)
{
    if (!mal_device__is_initialized(pDevice)) return;

    // Make sure the device is stopped first. The backends will probably handle this naturally,
    // but I like to do it explicitly for my own sanity.
    if (mal_device_is_started(pDevice)) {
        while (mal_device_stop(pDevice) == MAL_DEVICE_BUSY) {
            mal_sleep(1);
        }
    }

    // Putting the device into an uninitialized state will make the worker thread return.
    mal_device__set_state(pDevice, MAL_STATE_UNINITIALIZED);

    // Wake up the worker thread and wait for it to properly terminate.
    if (!mal_is_backend_asynchronous(pDevice->pContext->backend)) {
        mal_event_signal(&pDevice->wakeupEvent);
        mal_thread_wait(&pDevice->thread);
    }

    mal_event_uninit(&pDevice->stopEvent);
    mal_event_uninit(&pDevice->startEvent);
    mal_event_uninit(&pDevice->wakeupEvent);
    mal_mutex_uninit(&pDevice->lock);

#ifdef MAL_HAS_WASAPI
    if (pDevice->pContext->backend == mal_backend_wasapi) {
        mal_device_uninit__wasapi(pDevice);
    }
#endif
#ifdef MAL_HAS_DSOUND
    if (pDevice->pContext->backend == mal_backend_dsound) {
        mal_device_uninit__dsound(pDevice);
    }
#endif
#ifdef MAL_HAS_WINMM
    if (pDevice->pContext->backend == mal_backend_winmm) {
        mal_device_uninit__winmm(pDevice);
    }
#endif
#ifdef MAL_HAS_ALSA
    if (pDevice->pContext->backend == mal_backend_alsa) {
        mal_device_uninit__alsa(pDevice);
    }
#endif
#ifdef MAL_HAS_PULSEAUDIO
    if (pDevice->pContext->backend == mal_backend_pulseaudio) {
        mal_device_uninit__pulse(pDevice);
    }
#endif
#ifdef MAL_HAS_JACK
    if (pDevice->pContext->backend == mal_backend_jack) {
        mal_device_uninit__jack(pDevice);
    }
#endif
#ifdef MAL_HAS_OSS
    if (pDevice->pContext->backend == mal_backend_oss) {
        mal_device_uninit__oss(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENSL
    if (pDevice->pContext->backend == mal_backend_opensl) {
        mal_device_uninit__opensl(pDevice);
    }
#endif
#ifdef MAL_HAS_OPENAL
    if (pDevice->pContext->backend == mal_backend_openal) {
        mal_device_uninit__openal(pDevice);
    }
#endif
#ifdef MAL_HAS_SDL
    if (pDevice->pContext->backend == mal_backend_sdl) {
        mal_device_uninit__sdl(pDevice);
    }
#endif
#ifdef MAL_HAS_NULL
    if (pDevice->pContext->backend == mal_backend_null) {
        mal_device_uninit__null(pDevice);
    }
#endif


    if (pDevice->isOwnerOfContext) {
        mal_context_uninit(pDevice->pContext);
        mal_free(pDevice->pContext);
    }

    mal_zero_object(pDevice);
}

void mal_device_set_recv_callback(mal_device* pDevice, mal_recv_proc proc)
{
    if (pDevice == NULL) return;
    mal_atomic_exchange_ptr(&pDevice->onRecv, proc);
}

void mal_device_set_send_callback(mal_device* pDevice, mal_send_proc proc)
{
    if (pDevice == NULL) return;
    mal_atomic_exchange_ptr(&pDevice->onSend, proc);
}

void mal_device_set_stop_callback(mal_device* pDevice, mal_stop_proc proc)
{
    if (pDevice == NULL) return;
    mal_atomic_exchange_ptr(&pDevice->onStop, proc);
}

mal_result mal_device_start(mal_device* pDevice)
{
    if (pDevice == NULL) return mal_post_error(pDevice, "mal_device_start() called with invalid arguments (pDevice == NULL).", MAL_INVALID_ARGS);
    if (mal_device__get_state(pDevice) == MAL_STATE_UNINITIALIZED) return mal_post_error(pDevice, "mal_device_start() called for an uninitialized device.", MAL_DEVICE_NOT_INITIALIZED);

    mal_result result = MAL_ERROR;
    mal_mutex_lock(&pDevice->lock);
    {
        // Be a bit more descriptive if the device is already started or is already in the process of starting. This is likely
        // a bug with the application.
        if (mal_device__get_state(pDevice) == MAL_STATE_STARTING) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_start() called while another thread is already starting it.", MAL_DEVICE_ALREADY_STARTING);
        }
        if (mal_device__get_state(pDevice) == MAL_STATE_STARTED) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_start() called for a device that's already started.", MAL_DEVICE_ALREADY_STARTED);
        }

        // The device needs to be in a stopped state. If it's not, we just let the caller know the device is busy.
        if (mal_device__get_state(pDevice) != MAL_STATE_STOPPED) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_start() called while another thread is in the process of stopping it.", MAL_DEVICE_BUSY);
        }

        mal_device__set_state(pDevice, MAL_STATE_STARTING);

        // Asynchronous backends need to be handled differently.
#ifdef MAL_HAS_JACK
        if (pDevice->pContext->backend == mal_backend_jack) {
            result = mal_device__start_backend__jack(pDevice);
            if (result == MAL_SUCCESS) {
                mal_device__set_state(pDevice, MAL_STATE_STARTED);
            }
        } else
#endif
#ifdef MAL_HAS_OPENSL
        if (pDevice->pContext->backend == mal_backend_opensl) {
            result = mal_device__start_backend__opensl(pDevice);
            if (result == MAL_SUCCESS) {
                mal_device__set_state(pDevice, MAL_STATE_STARTED);
            }
        } else
#endif
#ifdef MAL_HAS_SDL
        if (pDevice->pContext->backend == mal_backend_sdl) {
            result = mal_device__start_backend__sdl(pDevice);
            if (result == MAL_SUCCESS) {
                mal_device__set_state(pDevice, MAL_STATE_STARTED);
            }
        } else
#endif
        // Synchronous backends.
        {
            mal_event_signal(&pDevice->wakeupEvent);

            // Wait for the worker thread to finish starting the device. Note that the worker thread will be the one
            // who puts the device into the started state. Don't call mal_device__set_state() here.
            mal_event_wait(&pDevice->startEvent);
            result = pDevice->workResult;
        }
    }
    mal_mutex_unlock(&pDevice->lock);

    return result;
}

mal_result mal_device_stop(mal_device* pDevice)
{
    if (pDevice == NULL) return mal_post_error(pDevice, "mal_device_stop() called with invalid arguments (pDevice == NULL).", MAL_INVALID_ARGS);
    if (mal_device__get_state(pDevice) == MAL_STATE_UNINITIALIZED) return mal_post_error(pDevice, "mal_device_stop() called for an uninitialized device.", MAL_DEVICE_NOT_INITIALIZED);

    mal_result result = MAL_ERROR;
    mal_mutex_lock(&pDevice->lock);
    {
        // Be a bit more descriptive if the device is already stopped or is already in the process of stopping. This is likely
        // a bug with the application.
        if (mal_device__get_state(pDevice) == MAL_STATE_STOPPING) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_stop() called while another thread is already stopping it.", MAL_DEVICE_ALREADY_STOPPING);
        }
        if (mal_device__get_state(pDevice) == MAL_STATE_STOPPED) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_stop() called for a device that's already stopped.", MAL_DEVICE_ALREADY_STOPPED);
        }

        // The device needs to be in a started state. If it's not, we just let the caller know the device is busy.
        if (mal_device__get_state(pDevice) != MAL_STATE_STARTED) {
            mal_mutex_unlock(&pDevice->lock);
            return mal_post_error(pDevice, "mal_device_stop() called while another thread is in the process of starting it.", MAL_DEVICE_BUSY);
        }

        mal_device__set_state(pDevice, MAL_STATE_STOPPING);

        // There's no need to wake up the thread like we do when starting.

        // Asynchronous backends need to be handled differently.
#ifdef MAL_HAS_JACK
        if (pDevice->pContext->backend == mal_backend_jack) {
            mal_device__stop_backend__jack(pDevice);
        } else
#endif
#ifdef MAL_HAS_OPENSL
        if (pDevice->pContext->backend == mal_backend_opensl) {
            mal_device__stop_backend__opensl(pDevice);
        } else
#endif
#ifdef MAL_HAS_SDL
        if (pDevice->pContext->backend == mal_backend_sdl) {
            mal_device__stop_backend__sdl(pDevice);
        } else
#endif
        // Synchronous backends.
        {
            // When we get here the worker thread is likely in a wait state while waiting for the backend device to deliver or request
            // audio data. We need to force these to return as quickly as possible.
            mal_device__break_main_loop(pDevice);

            // We need to wait for the worker thread to become available for work before returning. Note that the worker thread will be
            // the one who puts the device into the stopped state. Don't call mal_device__set_state() here.
            mal_event_wait(&pDevice->stopEvent);
            result = MAL_SUCCESS;
        }
    }
    mal_mutex_unlock(&pDevice->lock);

    return result;
}

mal_bool32 mal_device_is_started(mal_device* pDevice)
{
    if (pDevice == NULL) return MAL_FALSE;
    return mal_device__get_state(pDevice) == MAL_STATE_STARTED;
}

mal_uint32 mal_device_get_buffer_size_in_bytes(mal_device* pDevice)
{
    if (pDevice == NULL) return 0;
    return pDevice->bufferSizeInFrames * pDevice->channels * mal_get_sample_size_in_bytes(pDevice->format);
}

mal_uint32 mal_get_sample_size_in_bytes(mal_format format)
{
    mal_uint32 sizes[] = {
        0,  // unknown
        1,  // u8
        2,  // s16
        3,  // s24
        4,  // s32
        4,  // f32
    };
    return sizes[format];
}

mal_context_config mal_context_config_init(mal_log_proc onLog)
{
    mal_context_config config;
    mal_zero_object(&config);

    config.onLog = onLog;

    return config;
}


mal_device_config mal_device_config_init_default()
{
    mal_device_config config;
    mal_zero_object(&config);

    return config;
}

mal_device_config mal_device_config_init_default_capture(mal_recv_proc onRecvCallback)
{
    mal_device_config config = mal_device_config_init_default();
    config.onRecvCallback = onRecvCallback;

    return config;
}

mal_device_config mal_device_config_init_default_playback(mal_send_proc onSendCallback)
{
    mal_device_config config = mal_device_config_init_default();
    config.onSendCallback = onSendCallback;

    return config;
}


mal_device_config mal_device_config_init_ex(mal_format format, mal_uint32 channels, mal_uint32 sampleRate, mal_channel channelMap[MAL_MAX_CHANNELS], mal_recv_proc onRecvCallback, mal_send_proc onSendCallback)
{
    mal_device_config config = mal_device_config_init_default();

    config.format = format;
    config.channels = channels;
    config.sampleRate = sampleRate;
    config.onRecvCallback = onRecvCallback;
    config.onSendCallback = onSendCallback;

    if (channelMap == NULL) {
        if (channels > 8) {
            mal_zero_memory(channelMap, sizeof(mal_channel)*MAL_MAX_CHANNELS);
        } else {
            mal_get_standard_channel_map(mal_standard_channel_map_default, channels, config.channelMap);
        }
    } else {
        mal_copy_memory(config.channelMap, channelMap, sizeof(config.channelMap));
    }

    return config;
}


void mal_get_standard_channel_map_microsoft(mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    // Based off the speaker configurations mentioned here: https://docs.microsoft.com/en-us/windows-hardware/drivers/ddi/content/ksmedia/ns-ksmedia-ksaudio_channel_config
    switch (channels)
    {
        case 1:
        {
            channelMap[0] = MAL_CHANNEL_MONO;
        } break;

        case 2:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
        } break;

        case 3: // Not defined, but best guess.
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case 4:
        {
#ifndef MAL_USE_QUAD_MICROSOFT_CHANNEL_MAP
            // Surround. Using the Surround profile has the advantage of the 3rd channel (MAL_CHANNEL_FRONT_CENTER) mapping nicely
            // with higher channel counts.
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_BACK_CENTER;
#else
            // Quad.
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
#endif
        } break;

        case 5: // Not defined, but best guess.
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_BACK_LEFT;
            channelMap[4] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 6:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[5] = MAL_CHANNEL_SIDE_RIGHT;
        } break;

        case 7: // Not defined, but best guess.
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_BACK_CENTER;
            channelMap[5] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[6] = MAL_CHANNEL_SIDE_RIGHT;
        } break;

        case 8:
        default:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_BACK_LEFT;
            channelMap[5] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[6] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[7] = MAL_CHANNEL_SIDE_RIGHT;
        } break;
    }

    // Remainder.
    if (channels > 8) {
        for (mal_uint32 iChannel = 8; iChannel < MAL_MAX_CHANNELS; ++iChannel) {
            channelMap[iChannel] = (mal_channel)(MAL_CHANNEL_AUX_0 + (iChannel-8));
        }
    }
}

void mal_get_standard_channel_map_alsa(mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    switch (channels)
    {
        case 1:
        {
            channelMap[0] = MAL_CHANNEL_MONO;
        } break;

        case 2:
        {
            channelMap[0] = MAL_CHANNEL_LEFT;
            channelMap[1] = MAL_CHANNEL_RIGHT;
        } break;

        case 3:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case 4:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 5:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[4] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case 6:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[4] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[5] = MAL_CHANNEL_LFE;
        } break;

        case 7:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[4] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[5] = MAL_CHANNEL_LFE;
            channelMap[6] = MAL_CHANNEL_BACK_CENTER;
        } break;

        case 8:
        default:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[4] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[5] = MAL_CHANNEL_LFE;
            channelMap[6] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[7] = MAL_CHANNEL_SIDE_RIGHT;
        } break;
    }

    // Remainder.
    if (channels > 8) {
        for (mal_uint32 iChannel = 8; iChannel < MAL_MAX_CHANNELS; ++iChannel) {
            channelMap[iChannel] = (mal_channel)(MAL_CHANNEL_AUX_0 + (iChannel-8));
        }
    }
}

void mal_get_standard_channel_map_rfc3551(mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    switch (channels)
    {
        case 1:
        {
            channelMap[0] = MAL_CHANNEL_MONO;
        } break;

        case 2:
        {
            channelMap[0] = MAL_CHANNEL_LEFT;
            channelMap[1] = MAL_CHANNEL_RIGHT;
        } break;

        case 3:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case 4:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[3] = MAL_CHANNEL_BACK_CENTER;
        } break;

        case 5:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_BACK_LEFT;
            channelMap[4] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 6:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[4] = MAL_CHANNEL_SIDE_RIGHT;
            channelMap[5] = MAL_CHANNEL_BACK_CENTER;
        } break;
    }

    // Remainder.
    if (channels > 8) {
        for (mal_uint32 iChannel = 6; iChannel < MAL_MAX_CHANNELS; ++iChannel) {
            channelMap[iChannel] = (mal_channel)(MAL_CHANNEL_AUX_0 + (iChannel-6));
        }
    }
}

void mal_get_standard_channel_map_flac(mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    switch (channels)
    {
        case 1:
        {
            channelMap[0] = MAL_CHANNEL_MONO;
        } break;

        case 2:
        {
            channelMap[0] = MAL_CHANNEL_LEFT;
            channelMap[1] = MAL_CHANNEL_RIGHT;
        } break;

        case 3:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
        } break;

        case 4:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 5:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_BACK_LEFT;
            channelMap[4] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 6:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_BACK_LEFT;
            channelMap[5] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 7:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_BACK_CENTER;
            channelMap[5] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[6] = MAL_CHANNEL_SIDE_RIGHT;
        } break;

        case 8:
        default:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[3] = MAL_CHANNEL_LFE;
            channelMap[4] = MAL_CHANNEL_BACK_LEFT;
            channelMap[5] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[6] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[7] = MAL_CHANNEL_SIDE_RIGHT;
        } break;
    }

    // Remainder.
    if (channels > 8) {
        for (mal_uint32 iChannel = 8; iChannel < MAL_MAX_CHANNELS; ++iChannel) {
            channelMap[iChannel] = (mal_channel)(MAL_CHANNEL_AUX_0 + (iChannel-8));
        }
    }
}

void mal_get_standard_channel_map_vorbis(mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    // In Vorbis' type 0 channel mapping, the first two channels are not always the standard left/right - it
    // will have the center speaker where the right usually goes. Why?!
    switch (channels)
    {
        case 1:
        {
            channelMap[0] = MAL_CHANNEL_MONO;
        } break;

        case 2:
        {
            channelMap[0] = MAL_CHANNEL_LEFT;
            channelMap[1] = MAL_CHANNEL_RIGHT;
        } break;

        case 3:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
        } break;

        case 4:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[2] = MAL_CHANNEL_BACK_LEFT;
            channelMap[3] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 5:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[3] = MAL_CHANNEL_BACK_LEFT;
            channelMap[4] = MAL_CHANNEL_BACK_RIGHT;
        } break;

        case 6:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[3] = MAL_CHANNEL_BACK_LEFT;
            channelMap[4] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[5] = MAL_CHANNEL_LFE;
        } break;

        case 7:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[3] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[4] = MAL_CHANNEL_SIDE_RIGHT;
            channelMap[5] = MAL_CHANNEL_BACK_CENTER;
            channelMap[6] = MAL_CHANNEL_LFE;
        } break;

        case 8:
        default:
        {
            channelMap[0] = MAL_CHANNEL_FRONT_LEFT;
            channelMap[1] = MAL_CHANNEL_FRONT_CENTER;
            channelMap[2] = MAL_CHANNEL_FRONT_RIGHT;
            channelMap[3] = MAL_CHANNEL_SIDE_LEFT;
            channelMap[4] = MAL_CHANNEL_SIDE_RIGHT;
            channelMap[5] = MAL_CHANNEL_BACK_LEFT;
            channelMap[6] = MAL_CHANNEL_BACK_RIGHT;
            channelMap[7] = MAL_CHANNEL_LFE;
        } break;
    }

    // Remainder.
    if (channels > 8) {
        for (mal_uint32 iChannel = 8; iChannel < MAL_MAX_CHANNELS; ++iChannel) {
            channelMap[iChannel] = (mal_channel)(MAL_CHANNEL_AUX_0 + (iChannel-8));
        }
    }
}

void mal_get_standard_channel_map(mal_standard_channel_map standardChannelMap, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    switch (standardChannelMap)
    {
        case mal_standard_channel_map_alsa:
        {
            mal_get_standard_channel_map_alsa(channels, channelMap);
        } break;

        case mal_standard_channel_map_rfc3551:
        {
            mal_get_standard_channel_map_rfc3551(channels, channelMap);
        } break;

        case mal_standard_channel_map_flac:
        {
            mal_get_standard_channel_map_flac(channels, channelMap);
        } break;

        case mal_standard_channel_map_vorbis:
        {
            mal_get_standard_channel_map_vorbis(channels, channelMap);
        } break;

        case mal_standard_channel_map_microsoft:
        default:
        {
            mal_get_standard_channel_map_microsoft(channels, channelMap);
        } break;
    }
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Format Conversion.
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#define MAL_USE_REFERENCE_CONVERSION_APIS   1
#define MAL_USE_SSE

// u8
void mal_pcm_u8_to_u8(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;
    mal_copy_memory(dst, src, count * sizeof(mal_uint8));
}


void mal_pcm_u8_to_s16__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int16* dst_s16 = (mal_int16*)dst;
    const mal_uint8* src_u8 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int16 x = src_u8[i];
        x = x - 128;
        x = x << 8;
        dst_s16[i] = x;
    }
}

void mal_pcm_u8_to_s16__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s16__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_u8_to_s16__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s16__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_u8_to_s16(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_u8_to_s16__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_u8_to_s16__sse(dst, src, count, ditherMode);
#else
    mal_pcm_u8_to_s16__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_u8_to_s24__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_s24 = (mal_uint8*)dst;
    const mal_uint8* src_u8 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int16 x = src_u8[i];
        x = x - 128;

        dst_s24[i*3+0] = 0;
        dst_s24[i*3+1] = 0;
        dst_s24[i*3+2] = (mal_uint8)((mal_int8)x);
    }
}

void mal_pcm_u8_to_s24__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s24__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_u8_to_s24__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s24__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_u8_to_s24(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_u8_to_s24__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_u8_to_s24__sse(dst, src, count, ditherMode);
#else
    mal_pcm_u8_to_s24__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_u8_to_s32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int32* dst_s32 = (mal_int32*)dst;
    const mal_uint8* src_u8 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int32 x = src_u8[i];
        x = x - 128;
        x = x << 24;
        dst_s32[i] = x;
    }
}

void mal_pcm_u8_to_s32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_u8_to_s32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_s32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_u8_to_s32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_u8_to_s32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_u8_to_s32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_u8_to_s32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_u8_to_f32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    float* dst_f32 = (float*)dst;
    const mal_uint8* src_u8 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = (float)src_u8[i];
        x = x * 0.00784313725490196078f;    // 0..255 to 0..2
        x = x - 1;                          // 0..2 to -1..1

        dst_f32[i] = x;
    }
}

void mal_pcm_u8_to_f32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_f32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_u8_to_f32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_u8_to_f32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_u8_to_f32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_u8_to_f32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_u8_to_f32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_u8_to_f32__optimized(dst, src, count, ditherMode);
#endif
#endif
}



void mal_pcm_interleave_u8__reference(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const mal_uint8** src_u8 = (const mal_uint8**)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_u8[iFrame*channels + iChannel] = src_u8[iChannel][iFrame];
        }
    }
}

void mal_pcm_interleave_u8__optimized(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const mal_uint8** src_u8 = (const mal_uint8**)src;

    if (channels == 1) {
        mal_copy_memory(dst, src[0], frameCount * sizeof(mal_uint8));
    } else if (channels == 2) {
        mal_uint64 iFrame;
        for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
            dst_u8[iFrame*2 + 0] = src_u8[0][iFrame];
            dst_u8[iFrame*2 + 1] = src_u8[1][iFrame];
        }
    } else {
        mal_uint64 iFrame;
        for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
            mal_uint32 iChannel;
            for (iChannel = 0; iChannel < channels; iChannel += 1) {
                dst_u8[iFrame*channels + iChannel] = src_u8[iChannel][iFrame];
            }
        }
    }
}

void mal_pcm_interleave_u8(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_interleave_u8__reference(dst, src, frameCount, channels); 
#else
    mal_pcm_interleave_u8__optimized(dst, src, frameCount, channels);
#endif
}


void mal_pcm_deinterleave_u8__reference(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_uint8** dst_u8 = (mal_uint8**)dst;
    const mal_uint8* src_u8 = (const mal_uint8*)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_u8[iChannel][iFrame] = src_u8[iFrame*channels + iChannel];
        }
    }
}

void mal_pcm_deinterleave_u8__optimized(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_deinterleave_u8__reference(dst, src, frameCount, channels);
}

void mal_pcm_deinterleave_u8(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_deinterleave_u8__reference(dst, src, frameCount, channels); 
#else
    mal_pcm_deinterleave_u8__optimized(dst, src, frameCount, channels);
#endif
}


// s16
void mal_pcm_s16_to_u8__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const mal_int16* src_s16 = (const mal_int16*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int16 x = src_s16[i];
        x = x >> 8;
        x = x + 128;
        dst_u8[i] = (mal_uint8)x;
    }
}

void mal_pcm_s16_to_u8__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_u8__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s16_to_u8__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_u8__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s16_to_u8(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s16_to_u8__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s16_to_u8__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s16_to_u8__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s16_to_s16(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;
    mal_copy_memory(dst, src, count * sizeof(mal_int16));
}


void mal_pcm_s16_to_s24__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_s24 = (mal_uint8*)dst;
    const mal_int16* src_s16 = (const mal_int16*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        dst_s24[i*3+0] = 0;
        dst_s24[i*3+1] = (mal_uint8)(src_s16[i] & 0xFF);
        dst_s24[i*3+2] = (mal_uint8)(src_s16[i] >> 8);
    }
}

void mal_pcm_s16_to_s24__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_s24__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s16_to_s24__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_s24__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s16_to_s24(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s16_to_s24__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s16_to_s24__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s16_to_s24__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s16_to_s32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int32* dst_s32 = (mal_int32*)dst;
    const mal_int16* src_s16 = (const mal_int16*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        dst_s32[i] = src_s16[i] << 16;
    }
}

void mal_pcm_s16_to_s32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_s32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s16_to_s32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_s32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s16_to_s32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s16_to_s32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s16_to_s32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s16_to_s32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s16_to_f32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    float* dst_f32 = (float*)dst;
    const mal_int16* src_s16 = (const mal_int16*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = (float)src_s16[i];

#if 0
        // The accurate way.
        x = x + 32768.0f;                   // -32768..32767 to 0..65535
        x = x * 0.00003051804379339284f;    // 0..65536 to 0..2
        x = x - 1;                          // 0..2 to -1..1
#else
        // The fast way.
        x = x * 0.000030517578125f;         // -32768..32767 to -1..0.999969482421875
#endif

        dst_f32[i] = x;
    }
}

void mal_pcm_s16_to_f32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_f32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s16_to_f32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s16_to_f32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s16_to_f32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s16_to_f32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s16_to_f32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s16_to_f32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_interleave_s16__reference(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_int16* dst_s16 = (mal_int16*)dst;
    const mal_int16** src_s16 = (const mal_int16**)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_s16[iFrame*channels + iChannel] = src_s16[iChannel][iFrame];
        }
    }
}

void mal_pcm_interleave_s16__optimized(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_interleave_s16__reference(dst, src, frameCount, channels);
}

void mal_pcm_interleave_s16(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_interleave_s16__reference(dst, src, frameCount, channels); 
#else
    mal_pcm_interleave_s16__optimized(dst, src, frameCount, channels);
#endif
}


void mal_pcm_deinterleave_s16__reference(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_int16** dst_s16 = (mal_int16**)dst;
    const mal_int16* src_s16 = (const mal_int16*)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_s16[iChannel][iFrame] = src_s16[iFrame*channels + iChannel];
        }
    }
}

void mal_pcm_deinterleave_s16__optimized(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_deinterleave_s16__reference(dst, src, frameCount, channels);
}

void mal_pcm_deinterleave_s16(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_deinterleave_s16__reference(dst, src, frameCount, channels); 
#else
    mal_pcm_deinterleave_s16__optimized(dst, src, frameCount, channels);
#endif
}


// s24
void mal_pcm_s24_to_u8__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const mal_uint8* src_s24 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int8 x = (mal_int8)src_s24[i*3 + 2] + 128;
        dst_u8[i] = (mal_uint8)x;
    }
}

void mal_pcm_s24_to_u8__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_u8__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s24_to_u8__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_u8__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s24_to_u8(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s24_to_u8__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s24_to_u8__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s24_to_u8__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s24_to_s16__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int16* dst_s16 = (mal_int16*)dst;
    const mal_uint8* src_s24 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_uint16 dst_lo = ((mal_uint16)src_s24[i*3 + 1]);
        mal_uint16 dst_hi = ((mal_uint16)src_s24[i*3 + 2]) << 8;
        dst_s16[i] = (mal_int16)dst_lo | dst_hi;
    }
}

void mal_pcm_s24_to_s16__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_s16__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s24_to_s16__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_s16__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s24_to_s16(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s24_to_s16__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s24_to_s16__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s24_to_s16__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s24_to_s24(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_copy_memory(dst, src, count * 3);
}


void mal_pcm_s24_to_s32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int32* dst_s32 = (mal_int32*)dst;
    const mal_uint8* src_s24 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        dst_s32[i] = (mal_int32)(((mal_uint32)(src_s24[i*3+0]) << 8) | ((mal_uint32)(src_s24[i*3+1]) << 16) | ((mal_uint32)(src_s24[i*3+2])) << 24);
    }
}

void mal_pcm_s24_to_s32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_s32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s24_to_s32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_s32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s24_to_s32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s24_to_s32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s24_to_s32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s24_to_s32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s24_to_f32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    float* dst_f32 = (float*)dst;
    const mal_uint8* src_s24 = (const mal_uint8*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = (float)(((mal_int32)(((mal_uint32)(src_s24[i*3+0]) << 8) | ((mal_uint32)(src_s24[i*3+1]) << 16) | ((mal_uint32)(src_s24[i*3+2])) << 24)) >> 8);

#if 0
        // The accurate way.
        x = x + 8388608.0f;                 // -8388608..8388607 to 0..16777215
        x = x * 0.00000011920929665621f;    // 0..16777215 to 0..2
        x = x - 1;                          // 0..2 to -1..1
#else
        // The fast way.
        x = x * 0.00000011920928955078125f; // -8388608..8388607 to -1..0.999969482421875
#endif

        dst_f32[i] = x;
    }
}

void mal_pcm_s24_to_f32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_f32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s24_to_f32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s24_to_f32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s24_to_f32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s24_to_f32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s24_to_f32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s24_to_f32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_interleave_s24__reference(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_uint8* dst8 = (mal_uint8*)dst;
    const mal_uint8** src8 = (const mal_uint8**)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst8[iFrame*3*channels + iChannel*3 + 0] = src8[iChannel][iFrame*3 + 0];
            dst8[iFrame*3*channels + iChannel*3 + 1] = src8[iChannel][iFrame*3 + 1];
            dst8[iFrame*3*channels + iChannel*3 + 2] = src8[iChannel][iFrame*3 + 2];
        }
    }
}

void mal_pcm_interleave_s24__optimized(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_interleave_s24__reference(dst, src, frameCount, channels);
}

void mal_pcm_interleave_s24(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_interleave_s24__reference(dst, src, frameCount, channels);
#else
    mal_pcm_interleave_s24__optimized(dst, src, frameCount, channels);
#endif
}


void mal_pcm_deinterleave_s24__reference(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_uint8** dst8 = (mal_uint8**)dst;
    const mal_uint8* src8 = (const mal_uint8*)src;

    mal_uint32 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst8[iChannel][iFrame*3 + 0] = src8[iFrame*3*channels + iChannel*3 + 0];
            dst8[iChannel][iFrame*3 + 1] = src8[iFrame*3*channels + iChannel*3 + 1];
            dst8[iChannel][iFrame*3 + 2] = src8[iFrame*3*channels + iChannel*3 + 2];
        }
    }
}

void mal_pcm_deinterleave_s24__optimized(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_deinterleave_s24__reference(dst, src, frameCount, channels);
}

void mal_pcm_deinterleave_s24(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_deinterleave_s24__reference(dst, src, frameCount, channels);
#else
    mal_pcm_deinterleave_s24__optimized(dst, src, frameCount, channels);
#endif
}



// s32
void mal_pcm_s32_to_u8__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const mal_int32* src_s32 = (const mal_int32*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int32 x = src_s32[i];
        x = x >> 24;
        x = x + 128;
        dst_u8[i] = (mal_uint8)x;
    }
}

void mal_pcm_s32_to_u8__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_u8__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s32_to_u8__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_u8__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s32_to_u8(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s32_to_u8__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s32_to_u8__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s32_to_u8__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s32_to_s16__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int16* dst_s16 = (mal_int16*)dst;
    const mal_int32* src_s32 = (const mal_int32*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_int32 x = src_s32[i];
        x = x >> 16;
        dst_s16[i] = (mal_int16)x;
    }
}

void mal_pcm_s32_to_s16__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_s16__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s32_to_s16__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_s16__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s32_to_s16(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s32_to_s16__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s32_to_s16__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s32_to_s16__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s32_to_s24__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_s24 = (mal_uint8*)dst;
    const mal_int32* src_s32 = (const mal_int32*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        mal_uint32 x = (mal_uint32)src_s32[i];
        dst_s24[i*3+0] = (mal_uint8)((x & 0x0000FF00) >>  8);
        dst_s24[i*3+1] = (mal_uint8)((x & 0x00FF0000) >> 16);
        dst_s24[i*3+2] = (mal_uint8)((x & 0xFF000000) >> 24);
    }
}

void mal_pcm_s32_to_s24__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_s24__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s32_to_s24__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_s24__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s32_to_s24(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s32_to_s24__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s32_to_s24__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s32_to_s24__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_s32_to_s32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_copy_memory(dst, src, count * sizeof(mal_int32));
}


void mal_pcm_s32_to_f32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    float* dst_f32 = (float*)dst;
    const mal_int32* src_s32 = (const mal_int32*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        double x = src_s32[i];

#if 0
        x = x + 2147483648.0;
        x = x * 0.0000000004656612873077392578125;
        x = x - 1;
#else
        x = x / 2147483648.0;
#endif

        dst_f32[i] = (float)x;
    }
}

void mal_pcm_s32_to_f32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_f32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_s32_to_f32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_s32_to_f32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_s32_to_f32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_s32_to_f32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_s32_to_f32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_s32_to_f32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_interleave_s32__reference(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_int32* dst_s32 = (mal_int32*)dst;
    const mal_int32** src_s32 = (const mal_int32**)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_s32[iFrame*channels + iChannel] = src_s32[iChannel][iFrame];
        }
    }
}

void mal_pcm_interleave_s32__optimized(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_interleave_s32__reference(dst, src, frameCount, channels);
}

void mal_pcm_interleave_s32(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_interleave_s32__reference(dst, src, frameCount, channels);
#else
    mal_pcm_interleave_s32__optimized(dst, src, frameCount, channels);
#endif
}


void mal_pcm_deinterleave_s32__reference(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_int32** dst_s32 = (mal_int32**)dst;
    const mal_int32* src_s32 = (const mal_int32*)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_s32[iChannel][iFrame] = src_s32[iFrame*channels + iChannel];
        }
    }
}

void mal_pcm_deinterleave_s32__optimized(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_deinterleave_s32__reference(dst, src, frameCount, channels);
}

void mal_pcm_deinterleave_s32(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_deinterleave_s32__reference(dst, src, frameCount, channels);
#else
    mal_pcm_deinterleave_s32__optimized(dst, src, frameCount, channels);
#endif
}


// f32
void mal_pcm_f32_to_u8__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_u8 = (mal_uint8*)dst;
    const float* src_f32 = (const float*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = src_f32[i];
        x = ((x < -1) ? -1 : ((x > 1) ? 1 : x));    // clip
        x = x + 1;                                  // -1..1 to 0..2
        x = x * 127.5f;                             // 0..2 to 0..255

        dst_u8[i] = (mal_uint8)x;
    }
}

void mal_pcm_f32_to_u8__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_u8__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_f32_to_u8__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_u8__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_f32_to_u8(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_f32_to_u8__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_f32_to_u8__sse(dst, src, count, ditherMode);
#else
    mal_pcm_f32_to_u8__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_f32_to_s16__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int16* dst_s16 = (mal_int16*)dst;
    const float* src_f32 = (const float*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = src_f32[i];
        x = ((x < -1) ? -1 : ((x > 1) ? 1 : x));    // clip

#if 0
        // The accurate way.
        x = x + 1;                                  // -1..1 to 0..2
        x = x * 32767.5f;                           // 0..2 to 0..65535
        x = x - 32768.0f;                           // 0...65535 to -32768..32767
#else
        // The fast way.
        x = x * 32767.0f;                           // -1..1 to -32767..32767
#endif
        
        dst_s16[i] = (mal_int16)x;
    }
}

void mal_pcm_f32_to_s16__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s16__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_f32_to_s16__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s16__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_f32_to_s16(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_f32_to_s16__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_f32_to_s16__sse(dst, src, count, ditherMode);
#else
    mal_pcm_f32_to_s16__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_f32_to_s24__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_uint8* dst_s24 = (mal_uint8*)dst;
    const float* src_f32 = (const float*)src;

    mal_uint64 i;
    for (i = 0; i < count; i += 1) {
        float x = src_f32[i];
        x = ((x < -1) ? -1 : ((x > 1) ? 1 : x));    // clip

#if 0
        // The accurate way.
        x = x + 1;                                  // -1..1 to 0..2
        x = x * 8388607.5f;                         // 0..2 to 0..16777215
        x = x - 8388608.0f;                         // 0..16777215 to -8388608..8388607
#else
        // The fast way.
        x = x * 8388607.0f;                         // -1..1 to -8388607..8388607
#endif

        mal_int32 r = (mal_int32)x;
        dst_s24[(i*3)+0] = (mal_uint8)((r & 0x0000FF) >>  0);
        dst_s24[(i*3)+1] = (mal_uint8)((r & 0x00FF00) >>  8);
        dst_s24[(i*3)+2] = (mal_uint8)((r & 0xFF0000) >> 16);
    }
}

void mal_pcm_f32_to_s24__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s24__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_f32_to_s24__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s24__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_f32_to_s24(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_f32_to_s24__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_f32_to_s24__sse(dst, src, count, ditherMode);
#else
    mal_pcm_f32_to_s24__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_f32_to_s32__reference(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_int32* dst_s32 = (mal_int32*)dst;
    const float* src_f32 = (const float*)src;

    mal_uint32 i;
    for (i = 0; i < count; i += 1) {
        double x = src_f32[i];
        x = ((x < -1) ? -1 : ((x > 1) ? 1 : x));    // clip

#if 0
        // The accurate way.
        x = x + 1;                                  // -1..1 to 0..2
        x = x * 2147483647.5;                       // 0..2 to 0..4294967295
        x = x - 2147483648.0;                       // 0...4294967295 to -2147483648..2147483647
#else
        // The fast way.
        x = x * 2147483647.0;                       // -1..1 to -2147483647..2147483647
#endif

        dst_s32[i] = (mal_int32)x;
    }
}

void mal_pcm_f32_to_s32__optimized(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s32__reference(dst, src, count, ditherMode);
}

#ifdef MAL_USE_SSE
void mal_pcm_f32_to_s32__sse(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    mal_pcm_f32_to_s32__optimized(dst, src, count, ditherMode);
}
#endif

void mal_pcm_f32_to_s32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_f32_to_s32__reference(dst, src, count, ditherMode);
#else
#ifdef MAL_USE_SSE
    mal_pcm_f32_to_s32__sse(dst, src, count, ditherMode);
#else
    mal_pcm_f32_to_s32__optimized(dst, src, count, ditherMode);
#endif
#endif
}


void mal_pcm_f32_to_f32(void* dst, const void* src, mal_uint64 count, mal_dither_mode ditherMode)
{
    (void)ditherMode;

    mal_copy_memory(dst, src, count * sizeof(float));
}


void mal_pcm_interleave_f32__reference(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    float* dst_f32 = (float*)dst;
    const float** src_f32 = (const float**)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_f32[iFrame*channels + iChannel] = src_f32[iChannel][iFrame];
        }
    }
}

void mal_pcm_interleave_f32__optimized(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_interleave_f32__reference(dst, src, frameCount, channels);
}

void mal_pcm_interleave_f32(void* dst, const void** src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_interleave_f32__reference(dst, src, frameCount, channels);
#else
    mal_pcm_interleave_f32__optimized(dst, src, frameCount, channels);
#endif
}


void mal_pcm_deinterleave_f32__reference(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    float** dst_f32 = (float**)dst;
    const float* src_f32 = (const float*)src;

    mal_uint64 iFrame;
    for (iFrame = 0; iFrame < frameCount; iFrame += 1) {
        mal_uint32 iChannel;
        for (iChannel = 0; iChannel < channels; iChannel += 1) {
            dst_f32[iChannel][iFrame] = src_f32[iFrame*channels + iChannel];
        }
    }
}

void mal_pcm_deinterleave_f32__optimized(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
    mal_pcm_deinterleave_f32__reference(dst, src, frameCount, channels);
}

void mal_pcm_deinterleave_f32(void** dst, const void* src, mal_uint64 frameCount, mal_uint32 channels)
{
#ifdef MAL_USE_REFERENCE_CONVERSION_APIS
    mal_pcm_deinterleave_f32__reference(dst, src, frameCount, channels);
#else
    mal_pcm_deinterleave_f32__optimized(dst, src, frameCount, channels);
#endif
}



mal_result mal_format_converter_init__common(const mal_format_converter_config* pConfig, void* pUserData, mal_format_converter* pConverter)
{
    if (pConverter == NULL) {
        return MAL_INVALID_ARGS;
    }
    mal_zero_object(pConverter);

    if (pConfig == NULL) {
        return MAL_INVALID_ARGS;
    }

    pConverter->config = *pConfig;
    pConverter->pUserData = pUserData;

    switch (pConfig->formatIn)
    {
        case mal_format_u8:
        {
            if (pConfig->formatOut == mal_format_u8) {
                pConverter->onConvertPCM = mal_pcm_u8_to_u8;
            } else if (pConfig->formatOut == mal_format_s16) {
                pConverter->onConvertPCM = mal_pcm_u8_to_s16;
            } else if (pConfig->formatOut == mal_format_s24) {
                pConverter->onConvertPCM = mal_pcm_u8_to_s24;
            } else if (pConfig->formatOut == mal_format_s32) {
                pConverter->onConvertPCM = mal_pcm_u8_to_s32;
            } else if (pConfig->formatOut == mal_format_f32) {
                pConverter->onConvertPCM = mal_pcm_u8_to_f32;
            }
        } break;

        case mal_format_s16:
        {
            if (pConfig->formatOut == mal_format_u8) {
                pConverter->onConvertPCM = mal_pcm_s16_to_u8;
            } else if (pConfig->formatOut == mal_format_s16) {
                pConverter->onConvertPCM = mal_pcm_s16_to_s16;
            } else if (pConfig->formatOut == mal_format_s24) {
                pConverter->onConvertPCM = mal_pcm_s16_to_s24;
            } else if (pConfig->formatOut == mal_format_s32) {
                pConverter->onConvertPCM = mal_pcm_s16_to_s32;
            } else if (pConfig->formatOut == mal_format_f32) {
                pConverter->onConvertPCM = mal_pcm_s16_to_f32;
            }
        } break;

        case mal_format_s24:
        {
            if (pConfig->formatOut == mal_format_u8) {
                pConverter->onConvertPCM = mal_pcm_s24_to_u8;
            } else if (pConfig->formatOut == mal_format_s16) {
                pConverter->onConvertPCM = mal_pcm_s24_to_s16;
            } else if (pConfig->formatOut == mal_format_s24) {
                pConverter->onConvertPCM = mal_pcm_s24_to_s24;
            } else if (pConfig->formatOut == mal_format_s32) {
                pConverter->onConvertPCM = mal_pcm_s24_to_s32;
            } else if (pConfig->formatOut == mal_format_f32) {
                pConverter->onConvertPCM = mal_pcm_s24_to_f32;
            }
        } break;

        case mal_format_s32:
        {
            if (pConfig->formatOut == mal_format_u8) {
                pConverter->onConvertPCM = mal_pcm_s32_to_u8;
            } else if (pConfig->formatOut == mal_format_s16) {
                pConverter->onConvertPCM = mal_pcm_s32_to_s16;
            } else if (pConfig->formatOut == mal_format_s24) {
                pConverter->onConvertPCM = mal_pcm_s32_to_s24;
            } else if (pConfig->formatOut == mal_format_s32) {
                pConverter->onConvertPCM = mal_pcm_s32_to_s32;
            } else if (pConfig->formatOut == mal_format_f32) {
                pConverter->onConvertPCM = mal_pcm_s32_to_f32;
            }
        } break;

        case mal_format_f32:
        default:
        {
            if (pConfig->formatOut == mal_format_u8) {
                pConverter->onConvertPCM = mal_pcm_f32_to_u8;
            } else if (pConfig->formatOut == mal_format_s16) {
                pConverter->onConvertPCM = mal_pcm_f32_to_s16;
            } else if (pConfig->formatOut == mal_format_s24) {
                pConverter->onConvertPCM = mal_pcm_f32_to_s24;
            } else if (pConfig->formatOut == mal_format_s32) {
                pConverter->onConvertPCM = mal_pcm_f32_to_s32;
            } else if (pConfig->formatOut == mal_format_f32) {
                pConverter->onConvertPCM = mal_pcm_f32_to_f32;
            }
        } break;
    }


    switch (pConfig->formatOut)
    {
        case mal_format_u8:
        {
            pConverter->onInterleavePCM   = mal_pcm_interleave_u8;
            pConverter->onDeinterleavePCM = mal_pcm_deinterleave_u8;
        } break;
        case mal_format_s16:
        {
            pConverter->onInterleavePCM   = mal_pcm_interleave_s16;
            pConverter->onDeinterleavePCM = mal_pcm_deinterleave_s16;
        } break;
        case mal_format_s24:
        {
            pConverter->onInterleavePCM   = mal_pcm_interleave_s24;
            pConverter->onDeinterleavePCM = mal_pcm_deinterleave_s24;
        } break;
        case mal_format_s32:
        {
            pConverter->onInterleavePCM   = mal_pcm_interleave_s32;
            pConverter->onDeinterleavePCM = mal_pcm_deinterleave_s32;
        } break;
        case mal_format_f32:
        default:
        {
            pConverter->onInterleavePCM   = mal_pcm_interleave_f32;
            pConverter->onDeinterleavePCM = mal_pcm_deinterleave_f32;
        } break;
    }

    return MAL_SUCCESS;
}


mal_result mal_format_converter_init(const mal_format_converter_config* pConfig, mal_format_converter_read_proc onRead, void* pUserData, mal_format_converter* pConverter)
{
    mal_result result = mal_format_converter_init__common(pConfig, pUserData, pConverter);
    if (result != MAL_SUCCESS) {
        return result;
    }

    pConverter->onRead = onRead;

    return MAL_SUCCESS;
}

mal_result mal_format_converter_init_separated(const mal_format_converter_config* pConfig, mal_format_converter_read_separated_proc onRead, void* pUserData, mal_format_converter* pConverter)
{
    mal_result result = mal_format_converter_init__common(pConfig, pUserData, pConverter);
    if (result != MAL_SUCCESS) {
        return result;
    }

    pConverter->onReadSeparated = onRead;

    return MAL_SUCCESS;
}

mal_uint64 mal_format_converter_read_frames(mal_format_converter* pConverter, mal_uint64 frameCount, void* pFramesOut)
{
    if (pConverter == NULL || pFramesOut == NULL) {
        return 0;
    }

    mal_uint64 totalFramesRead = 0;
    mal_uint32 sampleSizeIn    = mal_get_sample_size_in_bytes(pConverter->config.formatIn);
    //mal_uint32 sampleSizeOut   = mal_get_sample_size_in_bytes(pConverter->config.formatOut);
    mal_uint32 frameSizeIn     = sampleSizeIn  * pConverter->config.channels;
    //mal_uint32 frameSizeOut    = sampleSizeOut * pConverter->config.channels;
    mal_uint8* pNextFramesOut  = (mal_uint8*)pFramesOut;

    if (pConverter->onRead != NULL) {
        // Input data is interleaved.
        if (pConverter->config.formatIn == pConverter->config.formatOut) {
            // Pass through.
            while (totalFramesRead < frameCount) {
                mal_uint64 framesRemaining = (frameCount - totalFramesRead);
                mal_uint64 framesToReadRightNow = framesRemaining;
                if (framesToReadRightNow > 0xFFFFFFFF) {
                    framesToReadRightNow = 0xFFFFFFFF;
                }

                mal_uint32 framesJustRead = (mal_uint32)pConverter->onRead(pConverter, (mal_uint32)framesToReadRightNow, pNextFramesOut, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                totalFramesRead += framesJustRead;
                pNextFramesOut  += framesJustRead * frameSizeIn;
            }
        } else {
            // Conversion required.
            mal_uint8 temp[MAL_MAX_CHANNELS * MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES * 128];
            mal_assert(sizeof(temp) <= 0xFFFFFFFF);

            mal_uint32 maxFramesToReadAtATime = sizeof(temp) / sampleSizeIn / pConverter->config.channels;

            while (totalFramesRead < frameCount) {
                mal_uint64 framesRemaining = (frameCount - totalFramesRead);
                mal_uint64 framesToReadRightNow = framesRemaining;
                if (framesToReadRightNow > maxFramesToReadAtATime) {
                    framesToReadRightNow = maxFramesToReadAtATime;
                }

                mal_uint32 framesJustRead = (mal_uint32)pConverter->onRead(pConverter, (mal_uint32)framesToReadRightNow, temp, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                pConverter->onConvertPCM(pNextFramesOut, temp, framesJustRead*pConverter->config.channels, pConverter->config.ditherMode);

                totalFramesRead += framesJustRead;
                pNextFramesOut  += framesJustRead * frameSizeIn;
            }
        }
    } else {
        // Input data is separated. If a conversion is required we need to do an intermediary step.
        mal_uint8 tempSamplesOfOutFormat[MAL_MAX_CHANNELS][MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES * 128];
        mal_assert(sizeof(tempSamplesOfOutFormat[0]) <= 0xFFFFFFFFF);

        void* ppTempSampleOfOutFormat[MAL_MAX_CHANNELS];
        for (mal_uint32 i = 0; i < pConverter->config.channels; ++i) {
            ppTempSampleOfOutFormat[i] = &tempSamplesOfOutFormat[i];
        }

        mal_uint32 maxFramesToReadAtATime = sizeof(tempSamplesOfOutFormat[0]) / sampleSizeIn;

        while (totalFramesRead < frameCount) {
            mal_uint64 framesRemaining = (frameCount - totalFramesRead);
            mal_uint64 framesToReadRightNow = framesRemaining;
            if (framesToReadRightNow > maxFramesToReadAtATime) {
                framesToReadRightNow = maxFramesToReadAtATime;
            }

            mal_uint32 framesJustRead = 0;

            if (pConverter->config.formatIn == pConverter->config.formatOut) {
                // Only interleaving.
                framesJustRead = (mal_uint32)pConverter->onReadSeparated(pConverter, (mal_uint32)framesToReadRightNow, ppTempSampleOfOutFormat, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }
            } else {
                // Interleaving + Conversion. Convert first, then interleave.
                mal_uint8 tempSamplesOfInFormat[MAL_MAX_CHANNELS][MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES * 128];

                void* ppTempSampleOfInFormat[MAL_MAX_CHANNELS];
                for (mal_uint32 i = 0; i < pConverter->config.channels; ++i) {
                    ppTempSampleOfInFormat[i] = &tempSamplesOfInFormat[i];
                }


                framesJustRead = (mal_uint32)pConverter->onReadSeparated(pConverter, (mal_uint32)framesToReadRightNow, ppTempSampleOfInFormat, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                for (mal_uint32 iChannel = 0; iChannel < pConverter->config.channels; iChannel += 1) {
                    pConverter->onConvertPCM(tempSamplesOfOutFormat[iChannel], tempSamplesOfInFormat[iChannel], framesJustRead, pConverter->config.ditherMode);
                }
            }

            pConverter->onInterleavePCM(pNextFramesOut, ppTempSampleOfOutFormat, framesJustRead, pConverter->config.channels);

            totalFramesRead += framesJustRead;
            pNextFramesOut  += framesJustRead * frameSizeIn;
        }
    }

    return totalFramesRead;
}

mal_uint64 mal_format_converter_read_frames_separated(mal_format_converter* pConverter, mal_uint64 frameCount, void** ppSamplesOut)
{
    if (pConverter == NULL || ppSamplesOut == NULL) {
        return 0;
    }

    mal_uint64 totalFramesRead = 0;
    mal_uint32 sampleSizeIn = mal_get_sample_size_in_bytes(pConverter->config.formatIn);
    //mal_uint32 sampleSizeOut = mal_get_sample_size_in_bytes(pConverter->config.formatOut);

    mal_uint8* ppNextSamplesOut[MAL_MAX_CHANNELS];
    mal_copy_memory(ppNextSamplesOut, ppSamplesOut, sizeof(void*) * pConverter->config.channels);

    if (pConverter->onRead != NULL) {
        // Input data is interleaved.
        mal_uint8 tempSamplesOfOutFormat[MAL_MAX_CHANNELS * MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES * 128];
        mal_assert(sizeof(tempSamplesOfOutFormat) <= 0xFFFFFFFF);

        mal_uint32 maxFramesToReadAtATime = sizeof(tempSamplesOfOutFormat) / sampleSizeIn / pConverter->config.channels;

        while (totalFramesRead < frameCount) {
            mal_uint64 framesRemaining = (frameCount - totalFramesRead);
            mal_uint64 framesToReadRightNow = framesRemaining;
            if (framesToReadRightNow > maxFramesToReadAtATime) {
                framesToReadRightNow = maxFramesToReadAtATime;
            }

            mal_uint32 framesJustRead = 0;

            if (pConverter->config.formatIn == pConverter->config.formatOut) {
                // Only de-interleaving.
                framesJustRead = (mal_uint32)pConverter->onRead(pConverter, (mal_uint32)framesToReadRightNow, tempSamplesOfOutFormat, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }
            } else {
                // De-interleaving + Conversion. Convert first, then de-interleave.
                mal_uint8 tempSamplesOfInFormat[sizeof(tempSamplesOfOutFormat)];

                framesJustRead = (mal_uint32)pConverter->onRead(pConverter, (mal_uint32)framesToReadRightNow, tempSamplesOfInFormat, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                pConverter->onConvertPCM(tempSamplesOfOutFormat, tempSamplesOfInFormat, framesJustRead * pConverter->config.channels, pConverter->config.ditherMode);
            }

            pConverter->onDeinterleavePCM((void**)ppNextSamplesOut, tempSamplesOfOutFormat, framesJustRead, pConverter->config.channels);

            totalFramesRead += framesJustRead;
            for (mal_uint32 iChannel = 0; iChannel < pConverter->config.channels; ++iChannel) {
                ppNextSamplesOut[iChannel] += framesJustRead * sampleSizeIn;
            }
        }
    } else {
        // Input data is separated.
        if (pConverter->config.formatIn == pConverter->config.formatOut) {
            // Pass through.
            while (totalFramesRead < frameCount) {
                mal_uint64 framesRemaining = (frameCount - totalFramesRead);
                mal_uint64 framesToReadRightNow = framesRemaining;
                if (framesToReadRightNow > 0xFFFFFFFF) {
                    framesToReadRightNow = 0xFFFFFFFF;
                }

                mal_uint32 framesJustRead = (mal_uint32)pConverter->onReadSeparated(pConverter, (mal_uint32)framesToReadRightNow, ppNextSamplesOut, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                totalFramesRead += framesJustRead;
                for (mal_uint32 iChannel = 0; iChannel < pConverter->config.channels; ++iChannel) {
                    ppNextSamplesOut[iChannel] += framesJustRead * sampleSizeIn;
                }
            }
        } else {
            // Conversion required.
            mal_uint8 temp[MAL_MAX_CHANNELS][MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES * 128];
            mal_assert(sizeof(temp[0]) <= 0xFFFFFFFF);

            mal_uint32 maxFramesToReadAtATime = sizeof(temp[0]) / sampleSizeIn;

            while (totalFramesRead < frameCount) {
                mal_uint64 framesRemaining = (frameCount - totalFramesRead);
                mal_uint64 framesToReadRightNow = framesRemaining;
                if (framesToReadRightNow > maxFramesToReadAtATime) {
                    framesToReadRightNow = maxFramesToReadAtATime;
                }

                mal_uint32 framesJustRead = (mal_uint32)pConverter->onReadSeparated(pConverter, (mal_uint32)framesToReadRightNow, ppNextSamplesOut, pConverter->pUserData);
                if (framesJustRead == 0) {
                    break;
                }

                for (mal_uint32 iChannel = 0; iChannel < pConverter->config.channels; iChannel += 1) {
                    pConverter->onConvertPCM(ppNextSamplesOut[iChannel], temp[iChannel], framesJustRead, pConverter->config.ditherMode);
                    ppNextSamplesOut[iChannel] += framesJustRead * sampleSizeIn;
                }

                totalFramesRead += framesJustRead;
            }
        }
    }

    return totalFramesRead;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// SRC
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void mal_src_cache_init(mal_src* pSRC, mal_src_cache* pCache)
{
    mal_assert(pSRC != NULL);
    mal_assert(pCache != NULL);

    pCache->pSRC = pSRC;
    pCache->cachedFrameCount = 0;
    pCache->iNextFrame = 0;
}

mal_uint32 mal_src_cache_read_frames(mal_src_cache* pCache, mal_uint32 frameCount, float* pFramesOut)
{
    mal_assert(pCache != NULL);
    mal_assert(pCache->pSRC != NULL);
    mal_assert(pCache->pSRC->onRead != NULL);
    mal_assert(frameCount > 0);
    mal_assert(pFramesOut != NULL);

    mal_uint32 channels = pCache->pSRC->config.channels;

    mal_uint32 totalFramesRead = 0;
    while (frameCount > 0) {
        // If there's anything in memory go ahead and copy that over first.
        mal_uint32 framesRemainingInMemory = pCache->cachedFrameCount - pCache->iNextFrame;
        mal_uint32 framesToReadFromMemory = frameCount;
        if (framesToReadFromMemory > framesRemainingInMemory) {
            framesToReadFromMemory = framesRemainingInMemory;
        }

        mal_copy_memory(pFramesOut, pCache->pCachedFrames + pCache->iNextFrame*channels, framesToReadFromMemory * channels * sizeof(float));
        pCache->iNextFrame += framesToReadFromMemory;

        totalFramesRead += framesToReadFromMemory;
        frameCount -= framesToReadFromMemory;
        if (frameCount == 0) {
            break;
        }


        // At this point there are still more frames to read from the client, so we'll need to reload the cache with fresh data.
        mal_assert(frameCount > 0);
        pFramesOut += framesToReadFromMemory * channels;

        pCache->iNextFrame = 0;
        pCache->cachedFrameCount = 0;
        if (pCache->pSRC->config.formatIn == mal_format_f32) {
            // No need for a conversion - read straight into the cache.
            mal_uint32 framesToReadFromClient = mal_countof(pCache->pCachedFrames) / pCache->pSRC->config.channels;
            if (framesToReadFromClient > pCache->pSRC->config.cacheSizeInFrames) {
                framesToReadFromClient = pCache->pSRC->config.cacheSizeInFrames;
            }

            pCache->cachedFrameCount = pCache->pSRC->onRead(pCache->pSRC, framesToReadFromClient, pCache->pCachedFrames, pCache->pSRC->pUserData);
        } else {
            // A format conversion is required which means we need to use an intermediary buffer.
            mal_uint8 pIntermediaryBuffer[sizeof(pCache->pCachedFrames)];
            mal_uint32 framesToReadFromClient = mal_min(mal_buffer_frame_capacity(pIntermediaryBuffer, channels, pCache->pSRC->config.formatIn), mal_buffer_frame_capacity(pCache->pCachedFrames, channels, mal_format_f32));
            if (framesToReadFromClient > pCache->pSRC->config.cacheSizeInFrames) {
                framesToReadFromClient = pCache->pSRC->config.cacheSizeInFrames;
            }

            pCache->cachedFrameCount = pCache->pSRC->onRead(pCache->pSRC, framesToReadFromClient, pIntermediaryBuffer, pCache->pSRC->pUserData);

            // Convert to f32.
            mal_pcm_convert(pCache->pCachedFrames, mal_format_f32, pIntermediaryBuffer, pCache->pSRC->config.formatIn, pCache->cachedFrameCount * channels, mal_dither_mode_none);
        }


        // Get out of this loop if nothing was able to be retrieved.
        if (pCache->cachedFrameCount == 0) {
            break;
        }
    }

    return totalFramesRead;
}


mal_uint64 mal_src_read_frames_passthrough(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush);
mal_uint64 mal_src_read_frames_linear(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush);

mal_result mal_src_init(mal_src_config* pConfig, mal_src_read_proc onRead, void* pUserData, mal_src* pSRC)
{
    if (pSRC == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pSRC);

    if (pConfig == NULL || onRead == NULL) return MAL_INVALID_ARGS;
    if (pConfig->channels == 0 || pConfig->channels > MAL_MAX_CHANNELS) return MAL_INVALID_ARGS;

    pSRC->config = *pConfig;
    pSRC->onRead = onRead;
    pSRC->pUserData = pUserData;

    if (pSRC->config.cacheSizeInFrames > MAL_SRC_CACHE_SIZE_IN_FRAMES || pSRC->config.cacheSizeInFrames == 0) {
        pSRC->config.cacheSizeInFrames = MAL_SRC_CACHE_SIZE_IN_FRAMES;
    }

    mal_src_cache_init(pSRC, &pSRC->cache);
    return MAL_SUCCESS;
}

mal_result mal_src_set_input_sample_rate(mal_src* pSRC, mal_uint32 sampleRateIn)
{
    if (pSRC == NULL) return MAL_INVALID_ARGS;

    // Must have a sample rate of > 0.
    if (sampleRateIn == 0) {
        return MAL_INVALID_ARGS;
    }

    pSRC->config.sampleRateIn = sampleRateIn;
    return MAL_SUCCESS;
}

mal_result mal_src_set_output_sample_rate(mal_src* pSRC, mal_uint32 sampleRateOut)
{
    if (pSRC == NULL) return MAL_INVALID_ARGS;

    // Must have a sample rate of > 0.
    if (sampleRateOut == 0) {
        return MAL_INVALID_ARGS;
    }

    pSRC->config.sampleRateOut = sampleRateOut;
    return MAL_SUCCESS;
}

mal_uint64 mal_src_read_frames(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut)
{
    return mal_src_read_frames_ex(pSRC, frameCount, pFramesOut, MAL_FALSE);
}

mal_uint64 mal_src_read_frames_ex(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush)
{
    if (pSRC == NULL || frameCount == 0 || pFramesOut == NULL) return 0;

    mal_src_algorithm algorithm = pSRC->config.algorithm;

    // Always use passthrough if the sample rates are the same.
    if (pSRC->config.sampleRateIn == pSRC->config.sampleRateOut) {
        algorithm = mal_src_algorithm_none;
    }

    // Could just use a function pointer instead of a switch for this...
    switch (algorithm)
    {
        case mal_src_algorithm_none:   return mal_src_read_frames_passthrough(pSRC, frameCount, pFramesOut, flush);
        case mal_src_algorithm_linear: return mal_src_read_frames_linear(pSRC, frameCount, pFramesOut, flush);
        default: return 0;
    }
}

mal_uint64 mal_src_read_frames_passthrough(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush)
{
    mal_assert(pSRC != NULL);
    mal_assert(frameCount > 0);
    mal_assert(pFramesOut != NULL);

    (void)flush;    // Passthrough need not care about flushing.

    // Fast path. No need for data conversion - just pass right through.
    if (pSRC->config.formatIn == pSRC->config.formatOut) {
        if (frameCount <= UINT32_MAX) {
            return pSRC->onRead(pSRC, (mal_uint32)frameCount, pFramesOut, pSRC->pUserData);
        } else {
            mal_uint64 totalFramesRead = 0;
            while (frameCount > 0) {
                mal_uint32 framesToReadRightNow = UINT32_MAX;
                if (framesToReadRightNow > frameCount) {
                    framesToReadRightNow = (mal_uint32)frameCount;
                }

                mal_uint32 framesRead = pSRC->onRead(pSRC, framesToReadRightNow, pFramesOut, pSRC->pUserData);
                if (framesRead == 0) {
                    break;
                }

                pFramesOut  = (mal_uint8*)pFramesOut + (framesRead * pSRC->config.channels * mal_get_sample_size_in_bytes(pSRC->config.formatOut));
                frameCount -= framesRead;
                totalFramesRead += framesRead;
            }

            return totalFramesRead;
        }
    }

    // Slower path. Need to do a format conversion.
    mal_uint64 totalFramesRead = 0;
    while (frameCount > 0) {
        mal_uint8 pStagingBuffer[MAL_MAX_CHANNELS * 2048];
        mal_uint32 stagingBufferSizeInFrames = sizeof(pStagingBuffer) / mal_get_sample_size_in_bytes(pSRC->config.formatIn) / pSRC->config.channels;
        mal_uint32 framesToRead = stagingBufferSizeInFrames;
        if (framesToRead > frameCount) {
            framesToRead = (mal_uint32)frameCount;
        }

        mal_uint32 framesRead = pSRC->onRead(pSRC, framesToRead, pStagingBuffer, pSRC->pUserData);
        if (framesRead == 0) {
            break;
        }

        mal_pcm_convert(pFramesOut, pSRC->config.formatOut, pStagingBuffer, pSRC->config.formatIn, framesRead * pSRC->config.channels, mal_dither_mode_none);

        pFramesOut  = (mal_uint8*)pFramesOut + (framesRead * pSRC->config.channels * mal_get_sample_size_in_bytes(pSRC->config.formatOut));
        frameCount -= framesRead;
        totalFramesRead += framesRead;
    }

    return totalFramesRead;
}

mal_uint64 mal_src_read_frames_linear(mal_src* pSRC, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush)
{
    mal_assert(pSRC != NULL);
    mal_assert(frameCount > 0);
    mal_assert(pFramesOut != NULL);

    // For linear SRC, the bin is only 2 frames: 1 prior, 1 future.

    // Load the bin if necessary.
    if (!pSRC->linear.isPrevFramesLoaded) {
        mal_uint32 framesRead = mal_src_cache_read_frames(&pSRC->cache, 1, pSRC->bin);
        if (framesRead == 0) {
            return 0;
        }
        pSRC->linear.isPrevFramesLoaded = MAL_TRUE;
    }
    if (!pSRC->linear.isNextFramesLoaded) {
        mal_uint32 framesRead = mal_src_cache_read_frames(&pSRC->cache, 1, pSRC->bin + pSRC->config.channels);
        if (framesRead == 0) {
            return 0;
        }
        pSRC->linear.isNextFramesLoaded = MAL_TRUE;
    }

    float factor = (float)pSRC->config.sampleRateIn / pSRC->config.sampleRateOut;

    mal_uint64 totalFramesRead = 0;
    while (frameCount > 0) {
        // The bin is where the previous and next frames are located.
        float* pPrevFrame = pSRC->bin;
        float* pNextFrame = pSRC->bin + pSRC->config.channels;

        float pFrame[MAL_MAX_CHANNELS];
        mal_blend_f32(pFrame, pPrevFrame, pNextFrame, pSRC->linear.alpha, pSRC->config.channels);

        pSRC->linear.alpha += factor;

        // The new alpha value is how we determine whether or not we need to read fresh frames.
        mal_uint32 framesToReadFromClient = (mal_uint32)pSRC->linear.alpha;
        pSRC->linear.alpha = pSRC->linear.alpha - framesToReadFromClient;

        for (mal_uint32 i = 0; i < framesToReadFromClient; ++i) {
            for (mal_uint32 j = 0; j < pSRC->config.channels; ++j) {
                pPrevFrame[j] = pNextFrame[j];
            }

            mal_uint32 framesRead = mal_src_cache_read_frames(&pSRC->cache, 1, pNextFrame);
            if (framesRead == 0) {
                for (mal_uint32 j = 0; j < pSRC->config.channels; ++j) {
                    pNextFrame[j] = 0;
                }

                if (pSRC->linear.isNextFramesLoaded) {
                    pSRC->linear.isNextFramesLoaded = MAL_FALSE;
                } else {
                    if (flush) {
                        pSRC->linear.isPrevFramesLoaded = MAL_FALSE;
                    }
                }

                break;
            }
        }

        mal_pcm_convert(pFramesOut, pSRC->config.formatOut, pFrame, mal_format_f32, 1 * pSRC->config.channels, mal_dither_mode_none);

        pFramesOut  = (mal_uint8*)pFramesOut + (1 * pSRC->config.channels * mal_get_sample_size_in_bytes(pSRC->config.formatOut));
        frameCount -= 1;
        totalFramesRead += 1;

        // If there's no frames available we need to get out of this loop.
        if (!pSRC->linear.isNextFramesLoaded && (!flush || !pSRC->linear.isPrevFramesLoaded)) {
            break;
        }
    }

    return totalFramesRead;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// FORMAT CONVERSION
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mal_pcm_convert(void* pOut, mal_format formatOut, const void* pIn, mal_format formatIn, mal_uint64 sampleCount, mal_dither_mode ditherMode)
{
    if (formatOut == formatIn) {
        mal_copy_memory(pOut, pIn, sampleCount * mal_get_sample_size_in_bytes(formatOut));
        return;
    }

    switch (formatIn)
    {
        case mal_format_u8:
        {
            switch (formatOut)
            {
                case mal_format_s16: mal_pcm_u8_to_s16(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s24: mal_pcm_u8_to_s24(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s32: mal_pcm_u8_to_s32(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_f32: mal_pcm_u8_to_f32(pOut, pIn, sampleCount, ditherMode); return;
                default: break;
            }
        } break;

        case mal_format_s16:
        {
            switch (formatOut)
            {
                case mal_format_u8:  mal_pcm_s16_to_u8( pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s24: mal_pcm_s16_to_s24(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s32: mal_pcm_s16_to_s32(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_f32: mal_pcm_s16_to_f32(pOut, pIn, sampleCount, ditherMode); return;
                default: break;
            }
        } break;

        case mal_format_s24:
        {
            switch (formatOut)
            {
                case mal_format_u8:  mal_pcm_s24_to_u8( pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s16: mal_pcm_s24_to_s16(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s32: mal_pcm_s24_to_s32(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_f32: mal_pcm_s24_to_f32(pOut, pIn, sampleCount, ditherMode); return;
                default: break;
            }
        } break;

        case mal_format_s32:
        {
            switch (formatOut)
            {
                case mal_format_u8:  mal_pcm_s32_to_u8( pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s16: mal_pcm_s32_to_s16(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s24: mal_pcm_s32_to_s24(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_f32: mal_pcm_s32_to_f32(pOut, pIn, sampleCount, ditherMode); return;
                default: break;
            }
        } break;

        case mal_format_f32:
        {
            switch (formatOut)
            {
                case mal_format_u8:  mal_pcm_f32_to_u8( pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s16: mal_pcm_f32_to_s16(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s24: mal_pcm_f32_to_s24(pOut, pIn, sampleCount, ditherMode); return;
                case mal_format_s32: mal_pcm_f32_to_s32(pOut, pIn, sampleCount, ditherMode); return;
                default: break;
            }
        } break;

        default: break;
    }
}


void mal_rearrange_channels_u8(mal_uint8* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    mal_channel temp[MAL_MAX_CHANNELS];
    mal_copy_memory(temp, pFrame, sizeof(temp[0]) * channels);

    switch (channels) {
        case 32: pFrame[31] = temp[channelMap[31]];
        case 31: pFrame[30] = temp[channelMap[30]];
        case 30: pFrame[29] = temp[channelMap[29]];
        case 29: pFrame[28] = temp[channelMap[28]];
        case 28: pFrame[27] = temp[channelMap[27]];
        case 27: pFrame[26] = temp[channelMap[26]];
        case 26: pFrame[25] = temp[channelMap[25]];
        case 25: pFrame[24] = temp[channelMap[24]];
        case 24: pFrame[23] = temp[channelMap[23]];
        case 23: pFrame[22] = temp[channelMap[22]];
        case 22: pFrame[21] = temp[channelMap[21]];
        case 21: pFrame[20] = temp[channelMap[20]];
        case 20: pFrame[19] = temp[channelMap[19]];
        case 19: pFrame[18] = temp[channelMap[18]];
        case 18: pFrame[17] = temp[channelMap[17]];
        case 17: pFrame[16] = temp[channelMap[16]];
        case 16: pFrame[15] = temp[channelMap[15]];
        case 15: pFrame[14] = temp[channelMap[14]];
        case 14: pFrame[13] = temp[channelMap[13]];
        case 13: pFrame[12] = temp[channelMap[12]];
        case 12: pFrame[11] = temp[channelMap[11]];
        case 11: pFrame[10] = temp[channelMap[10]];
        case 10: pFrame[ 9] = temp[channelMap[ 9]];
        case  9: pFrame[ 8] = temp[channelMap[ 8]];
        case  8: pFrame[ 7] = temp[channelMap[ 7]];
        case  7: pFrame[ 6] = temp[channelMap[ 6]];
        case  6: pFrame[ 5] = temp[channelMap[ 5]];
        case  5: pFrame[ 4] = temp[channelMap[ 4]];
        case  4: pFrame[ 3] = temp[channelMap[ 3]];
        case  3: pFrame[ 2] = temp[channelMap[ 2]];
        case  2: pFrame[ 1] = temp[channelMap[ 1]];
        case  1: pFrame[ 0] = temp[channelMap[ 0]];
    }
}

void mal_rearrange_channels_s16(mal_int16* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    mal_int16 temp[MAL_MAX_CHANNELS];
    mal_copy_memory(temp, pFrame, sizeof(temp[0]) * channels);

    switch (channels) {
        case 32: pFrame[31] = temp[channelMap[31]];
        case 31: pFrame[30] = temp[channelMap[30]];
        case 30: pFrame[29] = temp[channelMap[29]];
        case 29: pFrame[28] = temp[channelMap[28]];
        case 28: pFrame[27] = temp[channelMap[27]];
        case 27: pFrame[26] = temp[channelMap[26]];
        case 26: pFrame[25] = temp[channelMap[25]];
        case 25: pFrame[24] = temp[channelMap[24]];
        case 24: pFrame[23] = temp[channelMap[23]];
        case 23: pFrame[22] = temp[channelMap[22]];
        case 22: pFrame[21] = temp[channelMap[21]];
        case 21: pFrame[20] = temp[channelMap[20]];
        case 20: pFrame[19] = temp[channelMap[19]];
        case 19: pFrame[18] = temp[channelMap[18]];
        case 18: pFrame[17] = temp[channelMap[17]];
        case 17: pFrame[16] = temp[channelMap[16]];
        case 16: pFrame[15] = temp[channelMap[15]];
        case 15: pFrame[14] = temp[channelMap[14]];
        case 14: pFrame[13] = temp[channelMap[13]];
        case 13: pFrame[12] = temp[channelMap[12]];
        case 12: pFrame[11] = temp[channelMap[11]];
        case 11: pFrame[10] = temp[channelMap[10]];
        case 10: pFrame[ 9] = temp[channelMap[ 9]];
        case  9: pFrame[ 8] = temp[channelMap[ 8]];
        case  8: pFrame[ 7] = temp[channelMap[ 7]];
        case  7: pFrame[ 6] = temp[channelMap[ 6]];
        case  6: pFrame[ 5] = temp[channelMap[ 5]];
        case  5: pFrame[ 4] = temp[channelMap[ 4]];
        case  4: pFrame[ 3] = temp[channelMap[ 3]];
        case  3: pFrame[ 2] = temp[channelMap[ 2]];
        case  2: pFrame[ 1] = temp[channelMap[ 1]];
        case  1: pFrame[ 0] = temp[channelMap[ 0]];
    }
}

void mal_rearrange_channels_s32(mal_int32* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    mal_int32 temp[MAL_MAX_CHANNELS];
    mal_copy_memory(temp, pFrame, sizeof(temp[0]) * channels);

    switch (channels) {
        case 32: pFrame[31] = temp[channelMap[31]];
        case 31: pFrame[30] = temp[channelMap[30]];
        case 30: pFrame[29] = temp[channelMap[29]];
        case 29: pFrame[28] = temp[channelMap[28]];
        case 28: pFrame[27] = temp[channelMap[27]];
        case 27: pFrame[26] = temp[channelMap[26]];
        case 26: pFrame[25] = temp[channelMap[25]];
        case 25: pFrame[24] = temp[channelMap[24]];
        case 24: pFrame[23] = temp[channelMap[23]];
        case 23: pFrame[22] = temp[channelMap[22]];
        case 22: pFrame[21] = temp[channelMap[21]];
        case 21: pFrame[20] = temp[channelMap[20]];
        case 20: pFrame[19] = temp[channelMap[19]];
        case 19: pFrame[18] = temp[channelMap[18]];
        case 18: pFrame[17] = temp[channelMap[17]];
        case 17: pFrame[16] = temp[channelMap[16]];
        case 16: pFrame[15] = temp[channelMap[15]];
        case 15: pFrame[14] = temp[channelMap[14]];
        case 14: pFrame[13] = temp[channelMap[13]];
        case 13: pFrame[12] = temp[channelMap[12]];
        case 12: pFrame[11] = temp[channelMap[11]];
        case 11: pFrame[10] = temp[channelMap[10]];
        case 10: pFrame[ 9] = temp[channelMap[ 9]];
        case  9: pFrame[ 8] = temp[channelMap[ 8]];
        case  8: pFrame[ 7] = temp[channelMap[ 7]];
        case  7: pFrame[ 6] = temp[channelMap[ 6]];
        case  6: pFrame[ 5] = temp[channelMap[ 5]];
        case  5: pFrame[ 4] = temp[channelMap[ 4]];
        case  4: pFrame[ 3] = temp[channelMap[ 3]];
        case  3: pFrame[ 2] = temp[channelMap[ 2]];
        case  2: pFrame[ 1] = temp[channelMap[ 1]];
        case  1: pFrame[ 0] = temp[channelMap[ 0]];
    }
}

void mal_rearrange_channels_f32(float* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS])
{
    float temp[MAL_MAX_CHANNELS];
    mal_copy_memory(temp, pFrame, sizeof(temp[0]) * channels);

    switch (channels) {
        case 32: pFrame[31] = temp[channelMap[31]];
        case 31: pFrame[30] = temp[channelMap[30]];
        case 30: pFrame[29] = temp[channelMap[29]];
        case 29: pFrame[28] = temp[channelMap[28]];
        case 28: pFrame[27] = temp[channelMap[27]];
        case 27: pFrame[26] = temp[channelMap[26]];
        case 26: pFrame[25] = temp[channelMap[25]];
        case 25: pFrame[24] = temp[channelMap[24]];
        case 24: pFrame[23] = temp[channelMap[23]];
        case 23: pFrame[22] = temp[channelMap[22]];
        case 22: pFrame[21] = temp[channelMap[21]];
        case 21: pFrame[20] = temp[channelMap[20]];
        case 20: pFrame[19] = temp[channelMap[19]];
        case 19: pFrame[18] = temp[channelMap[18]];
        case 18: pFrame[17] = temp[channelMap[17]];
        case 17: pFrame[16] = temp[channelMap[16]];
        case 16: pFrame[15] = temp[channelMap[15]];
        case 15: pFrame[14] = temp[channelMap[14]];
        case 14: pFrame[13] = temp[channelMap[13]];
        case 13: pFrame[12] = temp[channelMap[12]];
        case 12: pFrame[11] = temp[channelMap[11]];
        case 11: pFrame[10] = temp[channelMap[10]];
        case 10: pFrame[ 9] = temp[channelMap[ 9]];
        case  9: pFrame[ 8] = temp[channelMap[ 8]];
        case  8: pFrame[ 7] = temp[channelMap[ 7]];
        case  7: pFrame[ 6] = temp[channelMap[ 6]];
        case  6: pFrame[ 5] = temp[channelMap[ 5]];
        case  5: pFrame[ 4] = temp[channelMap[ 4]];
        case  4: pFrame[ 3] = temp[channelMap[ 3]];
        case  3: pFrame[ 2] = temp[channelMap[ 2]];
        case  2: pFrame[ 1] = temp[channelMap[ 1]];
        case  1: pFrame[ 0] = temp[channelMap[ 0]];
    }
}

void mal_rearrange_channels_generic(void* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS], mal_format format)
{
    mal_uint32 sampleSizeInBytes = mal_get_sample_size_in_bytes(format);

    mal_uint8 temp[MAL_MAX_CHANNELS * MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES];   // Product of MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES to ensure it's large enough for all formats.
    mal_copy_memory(temp, pFrame, sampleSizeInBytes * channels);

    switch (channels) {
        case 32: mal_copy_memory((mal_uint8*)pFrame + (31 * sampleSizeInBytes), &temp[channelMap[31] * sampleSizeInBytes], sampleSizeInBytes);
        case 31: mal_copy_memory((mal_uint8*)pFrame + (30 * sampleSizeInBytes), &temp[channelMap[30] * sampleSizeInBytes], sampleSizeInBytes);
        case 30: mal_copy_memory((mal_uint8*)pFrame + (29 * sampleSizeInBytes), &temp[channelMap[29] * sampleSizeInBytes], sampleSizeInBytes);
        case 29: mal_copy_memory((mal_uint8*)pFrame + (28 * sampleSizeInBytes), &temp[channelMap[28] * sampleSizeInBytes], sampleSizeInBytes);
        case 28: mal_copy_memory((mal_uint8*)pFrame + (27 * sampleSizeInBytes), &temp[channelMap[27] * sampleSizeInBytes], sampleSizeInBytes);
        case 27: mal_copy_memory((mal_uint8*)pFrame + (26 * sampleSizeInBytes), &temp[channelMap[26] * sampleSizeInBytes], sampleSizeInBytes);
        case 26: mal_copy_memory((mal_uint8*)pFrame + (25 * sampleSizeInBytes), &temp[channelMap[25] * sampleSizeInBytes], sampleSizeInBytes);
        case 25: mal_copy_memory((mal_uint8*)pFrame + (24 * sampleSizeInBytes), &temp[channelMap[24] * sampleSizeInBytes], sampleSizeInBytes);
        case 24: mal_copy_memory((mal_uint8*)pFrame + (23 * sampleSizeInBytes), &temp[channelMap[23] * sampleSizeInBytes], sampleSizeInBytes);
        case 23: mal_copy_memory((mal_uint8*)pFrame + (22 * sampleSizeInBytes), &temp[channelMap[22] * sampleSizeInBytes], sampleSizeInBytes);
        case 22: mal_copy_memory((mal_uint8*)pFrame + (21 * sampleSizeInBytes), &temp[channelMap[21] * sampleSizeInBytes], sampleSizeInBytes);
        case 21: mal_copy_memory((mal_uint8*)pFrame + (20 * sampleSizeInBytes), &temp[channelMap[20] * sampleSizeInBytes], sampleSizeInBytes);
        case 20: mal_copy_memory((mal_uint8*)pFrame + (19 * sampleSizeInBytes), &temp[channelMap[19] * sampleSizeInBytes], sampleSizeInBytes);
        case 19: mal_copy_memory((mal_uint8*)pFrame + (18 * sampleSizeInBytes), &temp[channelMap[18] * sampleSizeInBytes], sampleSizeInBytes);
        case 18: mal_copy_memory((mal_uint8*)pFrame + (17 * sampleSizeInBytes), &temp[channelMap[17] * sampleSizeInBytes], sampleSizeInBytes);
        case 17: mal_copy_memory((mal_uint8*)pFrame + (16 * sampleSizeInBytes), &temp[channelMap[16] * sampleSizeInBytes], sampleSizeInBytes);
        case 16: mal_copy_memory((mal_uint8*)pFrame + (15 * sampleSizeInBytes), &temp[channelMap[15] * sampleSizeInBytes], sampleSizeInBytes);
        case 15: mal_copy_memory((mal_uint8*)pFrame + (14 * sampleSizeInBytes), &temp[channelMap[14] * sampleSizeInBytes], sampleSizeInBytes);
        case 14: mal_copy_memory((mal_uint8*)pFrame + (13 * sampleSizeInBytes), &temp[channelMap[13] * sampleSizeInBytes], sampleSizeInBytes);
        case 13: mal_copy_memory((mal_uint8*)pFrame + (12 * sampleSizeInBytes), &temp[channelMap[12] * sampleSizeInBytes], sampleSizeInBytes);
        case 12: mal_copy_memory((mal_uint8*)pFrame + (11 * sampleSizeInBytes), &temp[channelMap[11] * sampleSizeInBytes], sampleSizeInBytes);
        case 11: mal_copy_memory((mal_uint8*)pFrame + (10 * sampleSizeInBytes), &temp[channelMap[10] * sampleSizeInBytes], sampleSizeInBytes);
        case 10: mal_copy_memory((mal_uint8*)pFrame + ( 9 * sampleSizeInBytes), &temp[channelMap[ 9] * sampleSizeInBytes], sampleSizeInBytes);
        case  9: mal_copy_memory((mal_uint8*)pFrame + ( 8 * sampleSizeInBytes), &temp[channelMap[ 8] * sampleSizeInBytes], sampleSizeInBytes);
        case  8: mal_copy_memory((mal_uint8*)pFrame + ( 7 * sampleSizeInBytes), &temp[channelMap[ 7] * sampleSizeInBytes], sampleSizeInBytes);
        case  7: mal_copy_memory((mal_uint8*)pFrame + ( 6 * sampleSizeInBytes), &temp[channelMap[ 6] * sampleSizeInBytes], sampleSizeInBytes);
        case  6: mal_copy_memory((mal_uint8*)pFrame + ( 5 * sampleSizeInBytes), &temp[channelMap[ 5] * sampleSizeInBytes], sampleSizeInBytes);
        case  5: mal_copy_memory((mal_uint8*)pFrame + ( 4 * sampleSizeInBytes), &temp[channelMap[ 4] * sampleSizeInBytes], sampleSizeInBytes);
        case  4: mal_copy_memory((mal_uint8*)pFrame + ( 3 * sampleSizeInBytes), &temp[channelMap[ 3] * sampleSizeInBytes], sampleSizeInBytes);
        case  3: mal_copy_memory((mal_uint8*)pFrame + ( 2 * sampleSizeInBytes), &temp[channelMap[ 2] * sampleSizeInBytes], sampleSizeInBytes);
        case  2: mal_copy_memory((mal_uint8*)pFrame + ( 1 * sampleSizeInBytes), &temp[channelMap[ 1] * sampleSizeInBytes], sampleSizeInBytes);
        case  1: mal_copy_memory((mal_uint8*)pFrame + ( 0 * sampleSizeInBytes), &temp[channelMap[ 0] * sampleSizeInBytes], sampleSizeInBytes);
    }
}

void mal_rearrange_channels(void* pFrame, mal_uint32 channels, mal_channel channelMap[MAL_MAX_CHANNELS], mal_format format)
{
    switch (format)
    {
    case mal_format_u8:  mal_rearrange_channels_u8( (mal_uint8*)pFrame, channels, channelMap); break;
    case mal_format_s16: mal_rearrange_channels_s16((mal_int16*)pFrame, channels, channelMap); break;
    case mal_format_s32: mal_rearrange_channels_s32((mal_int32*)pFrame, channels, channelMap); break;
    case mal_format_f32: mal_rearrange_channels_f32(    (float*)pFrame, channels, channelMap); break;
    default:             mal_rearrange_channels_generic(pFrame, channels, channelMap, format); break;
    }
}

void mal_dsp_mix_channels__dec(float* pFramesOut, mal_uint32 channelsOut, const mal_channel channelMapOut[MAL_MAX_CHANNELS], const float* pFramesIn, mal_uint32 channelsIn, const mal_channel channelMapIn[MAL_MAX_CHANNELS], mal_uint32 frameCount, mal_channel_mix_mode mode)
{
    mal_assert(pFramesOut != NULL);
    mal_assert(channelsOut > 0);
    mal_assert(pFramesIn != NULL);
    mal_assert(channelsIn > 0);
    mal_assert(channelsOut < channelsIn);

    (void)channelMapOut;
    (void)channelMapIn;

    if (mode == mal_channel_mix_mode_basic) {
        // Basic mode is where we just drop excess channels.
        for (mal_uint32 iFrame = 0; iFrame < frameCount; ++iFrame) {
            switch (channelsOut) {
                case 32: pFramesOut[iFrame*channelsOut+31] = pFramesIn[iFrame*channelsIn+31];
                case 31: pFramesOut[iFrame*channelsOut+30] = pFramesIn[iFrame*channelsIn+30];
                case 30: pFramesOut[iFrame*channelsOut+29] = pFramesIn[iFrame*channelsIn+29];
                case 29: pFramesOut[iFrame*channelsOut+28] = pFramesIn[iFrame*channelsIn+28];
                case 28: pFramesOut[iFrame*channelsOut+27] = pFramesIn[iFrame*channelsIn+27];
                case 27: pFramesOut[iFrame*channelsOut+26] = pFramesIn[iFrame*channelsIn+26];
                case 26: pFramesOut[iFrame*channelsOut+25] = pFramesIn[iFrame*channelsIn+25];
                case 25: pFramesOut[iFrame*channelsOut+24] = pFramesIn[iFrame*channelsIn+24];
                case 24: pFramesOut[iFrame*channelsOut+23] = pFramesIn[iFrame*channelsIn+23];
                case 23: pFramesOut[iFrame*channelsOut+22] = pFramesIn[iFrame*channelsIn+22];
                case 22: pFramesOut[iFrame*channelsOut+21] = pFramesIn[iFrame*channelsIn+21];
                case 21: pFramesOut[iFrame*channelsOut+20] = pFramesIn[iFrame*channelsIn+20];
                case 20: pFramesOut[iFrame*channelsOut+19] = pFramesIn[iFrame*channelsIn+19];
                case 19: pFramesOut[iFrame*channelsOut+18] = pFramesIn[iFrame*channelsIn+18];
                case 18: pFramesOut[iFrame*channelsOut+17] = pFramesIn[iFrame*channelsIn+17];
                case 17: pFramesOut[iFrame*channelsOut+16] = pFramesIn[iFrame*channelsIn+16];
                case 16: pFramesOut[iFrame*channelsOut+15] = pFramesIn[iFrame*channelsIn+15];
                case 15: pFramesOut[iFrame*channelsOut+14] = pFramesIn[iFrame*channelsIn+14];
                case 14: pFramesOut[iFrame*channelsOut+13] = pFramesIn[iFrame*channelsIn+13];
                case 13: pFramesOut[iFrame*channelsOut+12] = pFramesIn[iFrame*channelsIn+12];
                case 12: pFramesOut[iFrame*channelsOut+11] = pFramesIn[iFrame*channelsIn+11];
                case 11: pFramesOut[iFrame*channelsOut+10] = pFramesIn[iFrame*channelsIn+10];
                case 10: pFramesOut[iFrame*channelsOut+ 9] = pFramesIn[iFrame*channelsIn+ 9];
                case  9: pFramesOut[iFrame*channelsOut+ 8] = pFramesIn[iFrame*channelsIn+ 8];
                case  8: pFramesOut[iFrame*channelsOut+ 7] = pFramesIn[iFrame*channelsIn+ 7];
                case  7: pFramesOut[iFrame*channelsOut+ 6] = pFramesIn[iFrame*channelsIn+ 6];
                case  6: pFramesOut[iFrame*channelsOut+ 5] = pFramesIn[iFrame*channelsIn+ 5];
                case  5: pFramesOut[iFrame*channelsOut+ 4] = pFramesIn[iFrame*channelsIn+ 4];
                case  4: pFramesOut[iFrame*channelsOut+ 3] = pFramesIn[iFrame*channelsIn+ 3];
                case  3: pFramesOut[iFrame*channelsOut+ 2] = pFramesIn[iFrame*channelsIn+ 2];
                case  2: pFramesOut[iFrame*channelsOut+ 1] = pFramesIn[iFrame*channelsIn+ 1];
                case  1: pFramesOut[iFrame*channelsOut+ 0] = pFramesIn[iFrame*channelsIn+ 0];
            }
        }
    } else {
        // Blend mode is where we just use simple averaging to blend based on spacial locality.
        if (channelsOut == 1) {
            for (mal_uint32 iFrame = 0; iFrame < frameCount; ++iFrame) {
                float total = 0;
                switch (channelsIn) {
                    case 32: total += pFramesIn[iFrame*channelsIn+31];
                    case 31: total += pFramesIn[iFrame*channelsIn+30];
                    case 30: total += pFramesIn[iFrame*channelsIn+29];
                    case 29: total += pFramesIn[iFrame*channelsIn+28];
                    case 28: total += pFramesIn[iFrame*channelsIn+27];
                    case 27: total += pFramesIn[iFrame*channelsIn+26];
                    case 26: total += pFramesIn[iFrame*channelsIn+25];
                    case 25: total += pFramesIn[iFrame*channelsIn+24];
                    case 24: total += pFramesIn[iFrame*channelsIn+23];
                    case 23: total += pFramesIn[iFrame*channelsIn+22];
                    case 22: total += pFramesIn[iFrame*channelsIn+21];
                    case 21: total += pFramesIn[iFrame*channelsIn+20];
                    case 20: total += pFramesIn[iFrame*channelsIn+19];
                    case 19: total += pFramesIn[iFrame*channelsIn+18];
                    case 18: total += pFramesIn[iFrame*channelsIn+17];
                    case 17: total += pFramesIn[iFrame*channelsIn+16];
                    case 16: total += pFramesIn[iFrame*channelsIn+15];
                    case 15: total += pFramesIn[iFrame*channelsIn+14];
                    case 14: total += pFramesIn[iFrame*channelsIn+13];
                    case 13: total += pFramesIn[iFrame*channelsIn+12];
                    case 12: total += pFramesIn[iFrame*channelsIn+11];
                    case 11: total += pFramesIn[iFrame*channelsIn+10];
                    case 10: total += pFramesIn[iFrame*channelsIn+ 9];
                    case  9: total += pFramesIn[iFrame*channelsIn+ 8];
                    case  8: total += pFramesIn[iFrame*channelsIn+ 7];
                    case  7: total += pFramesIn[iFrame*channelsIn+ 6];
                    case  6: total += pFramesIn[iFrame*channelsIn+ 5];
                    case  5: total += pFramesIn[iFrame*channelsIn+ 4];
                    case  4: total += pFramesIn[iFrame*channelsIn+ 3];
                    case  3: total += pFramesIn[iFrame*channelsIn+ 2];
                    case  2: total += pFramesIn[iFrame*channelsIn+ 1];
                    case  1: total += pFramesIn[iFrame*channelsIn+ 0];
                }

                pFramesOut[iFrame+0] = total / channelsIn;
            }
        } else if (channelsOut == 2) {
            // TODO: Implement proper stereo blending.
            mal_dsp_mix_channels__dec(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mal_channel_mix_mode_basic);
        } else {
            // Fall back to basic mode.
            mal_dsp_mix_channels__dec(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mal_channel_mix_mode_basic);
        }
    }
}

void mal_dsp_mix_channels__inc(float* pFramesOut, mal_uint32 channelsOut, const mal_channel channelMapOut[MAL_MAX_CHANNELS], const float* pFramesIn, mal_uint32 channelsIn, const mal_channel channelMapIn[MAL_MAX_CHANNELS], mal_uint32 frameCount, mal_channel_mix_mode mode)
{
    mal_assert(pFramesOut != NULL);
    mal_assert(channelsOut > 0);
    mal_assert(pFramesIn != NULL);
    mal_assert(channelsIn > 0);
    mal_assert(channelsOut > channelsIn);

    (void)channelMapOut;
    (void)channelMapIn;

    if (mode == mal_channel_mix_mode_basic) {
        // Basic mode is where we just zero out extra channels.
        for (mal_uint32 iFrame = 0; iFrame < frameCount; ++iFrame) {
            switch (channelsIn) {
                case 32: pFramesOut[iFrame*channelsOut+31] = pFramesIn[iFrame*channelsIn+31];
                case 31: pFramesOut[iFrame*channelsOut+30] = pFramesIn[iFrame*channelsIn+30];
                case 30: pFramesOut[iFrame*channelsOut+29] = pFramesIn[iFrame*channelsIn+29];
                case 29: pFramesOut[iFrame*channelsOut+28] = pFramesIn[iFrame*channelsIn+28];
                case 28: pFramesOut[iFrame*channelsOut+27] = pFramesIn[iFrame*channelsIn+27];
                case 27: pFramesOut[iFrame*channelsOut+26] = pFramesIn[iFrame*channelsIn+26];
                case 26: pFramesOut[iFrame*channelsOut+25] = pFramesIn[iFrame*channelsIn+25];
                case 25: pFramesOut[iFrame*channelsOut+24] = pFramesIn[iFrame*channelsIn+24];
                case 24: pFramesOut[iFrame*channelsOut+23] = pFramesIn[iFrame*channelsIn+23];
                case 23: pFramesOut[iFrame*channelsOut+22] = pFramesIn[iFrame*channelsIn+22];
                case 22: pFramesOut[iFrame*channelsOut+21] = pFramesIn[iFrame*channelsIn+21];
                case 21: pFramesOut[iFrame*channelsOut+20] = pFramesIn[iFrame*channelsIn+20];
                case 20: pFramesOut[iFrame*channelsOut+19] = pFramesIn[iFrame*channelsIn+19];
                case 19: pFramesOut[iFrame*channelsOut+18] = pFramesIn[iFrame*channelsIn+18];
                case 18: pFramesOut[iFrame*channelsOut+17] = pFramesIn[iFrame*channelsIn+17];
                case 17: pFramesOut[iFrame*channelsOut+16] = pFramesIn[iFrame*channelsIn+16];
                case 16: pFramesOut[iFrame*channelsOut+15] = pFramesIn[iFrame*channelsIn+15];
                case 15: pFramesOut[iFrame*channelsOut+14] = pFramesIn[iFrame*channelsIn+14];
                case 14: pFramesOut[iFrame*channelsOut+13] = pFramesIn[iFrame*channelsIn+13];
                case 13: pFramesOut[iFrame*channelsOut+12] = pFramesIn[iFrame*channelsIn+12];
                case 12: pFramesOut[iFrame*channelsOut+11] = pFramesIn[iFrame*channelsIn+11];
                case 11: pFramesOut[iFrame*channelsOut+10] = pFramesIn[iFrame*channelsIn+10];
                case 10: pFramesOut[iFrame*channelsOut+ 9] = pFramesIn[iFrame*channelsIn+ 9];
                case  9: pFramesOut[iFrame*channelsOut+ 8] = pFramesIn[iFrame*channelsIn+ 8];
                case  8: pFramesOut[iFrame*channelsOut+ 7] = pFramesIn[iFrame*channelsIn+ 7];
                case  7: pFramesOut[iFrame*channelsOut+ 6] = pFramesIn[iFrame*channelsIn+ 6];
                case  6: pFramesOut[iFrame*channelsOut+ 5] = pFramesIn[iFrame*channelsIn+ 5];
                case  5: pFramesOut[iFrame*channelsOut+ 4] = pFramesIn[iFrame*channelsIn+ 4];
                case  4: pFramesOut[iFrame*channelsOut+ 3] = pFramesIn[iFrame*channelsIn+ 3];
                case  3: pFramesOut[iFrame*channelsOut+ 2] = pFramesIn[iFrame*channelsIn+ 2];
                case  2: pFramesOut[iFrame*channelsOut+ 1] = pFramesIn[iFrame*channelsIn+ 1];
                case  1: pFramesOut[iFrame*channelsOut+ 0] = pFramesIn[iFrame*channelsIn+ 0];
            }

            // Zero out extra channels.
            switch (channelsOut - channelsIn) {
                case 32: pFramesOut[iFrame*channelsOut+31 + channelsIn] = 0;
                case 31: pFramesOut[iFrame*channelsOut+30 + channelsIn] = 0;
                case 30: pFramesOut[iFrame*channelsOut+29 + channelsIn] = 0;
                case 29: pFramesOut[iFrame*channelsOut+28 + channelsIn] = 0;
                case 28: pFramesOut[iFrame*channelsOut+27 + channelsIn] = 0;
                case 27: pFramesOut[iFrame*channelsOut+26 + channelsIn] = 0;
                case 26: pFramesOut[iFrame*channelsOut+25 + channelsIn] = 0;
                case 25: pFramesOut[iFrame*channelsOut+24 + channelsIn] = 0;
                case 24: pFramesOut[iFrame*channelsOut+23 + channelsIn] = 0;
                case 23: pFramesOut[iFrame*channelsOut+22 + channelsIn] = 0;
                case 22: pFramesOut[iFrame*channelsOut+21 + channelsIn] = 0;
                case 21: pFramesOut[iFrame*channelsOut+20 + channelsIn] = 0;
                case 20: pFramesOut[iFrame*channelsOut+19 + channelsIn] = 0;
                case 19: pFramesOut[iFrame*channelsOut+18 + channelsIn] = 0;
                case 18: pFramesOut[iFrame*channelsOut+17 + channelsIn] = 0;
                case 17: pFramesOut[iFrame*channelsOut+16 + channelsIn] = 0;
                case 16: pFramesOut[iFrame*channelsOut+15 + channelsIn] = 0;
                case 15: pFramesOut[iFrame*channelsOut+14 + channelsIn] = 0;
                case 14: pFramesOut[iFrame*channelsOut+13 + channelsIn] = 0;
                case 13: pFramesOut[iFrame*channelsOut+12 + channelsIn] = 0;
                case 12: pFramesOut[iFrame*channelsOut+11 + channelsIn] = 0;
                case 11: pFramesOut[iFrame*channelsOut+10 + channelsIn] = 0;
                case 10: pFramesOut[iFrame*channelsOut+ 9 + channelsIn] = 0;
                case  9: pFramesOut[iFrame*channelsOut+ 8 + channelsIn] = 0;
                case  8: pFramesOut[iFrame*channelsOut+ 7 + channelsIn] = 0;
                case  7: pFramesOut[iFrame*channelsOut+ 6 + channelsIn] = 0;
                case  6: pFramesOut[iFrame*channelsOut+ 5 + channelsIn] = 0;
                case  5: pFramesOut[iFrame*channelsOut+ 4 + channelsIn] = 0;
                case  4: pFramesOut[iFrame*channelsOut+ 3 + channelsIn] = 0;
                case  3: pFramesOut[iFrame*channelsOut+ 2 + channelsIn] = 0;
                case  2: pFramesOut[iFrame*channelsOut+ 1 + channelsIn] = 0;
                case  1: pFramesOut[iFrame*channelsOut+ 0 + channelsIn] = 0;
            }
        }
    } else {
        // Using blended mixing mode. Basically this is just the mode where audio is distributed across all channels
        // based on spacial locality.
        if (channelsIn == 1) {
            for (mal_uint32 iFrame = 0; iFrame < frameCount; ++iFrame) {
                switch (channelsOut) {
                    case 32: pFramesOut[iFrame*channelsOut+31] = pFramesIn[iFrame*channelsIn+0];
                    case 31: pFramesOut[iFrame*channelsOut+30] = pFramesIn[iFrame*channelsIn+0];
                    case 30: pFramesOut[iFrame*channelsOut+29] = pFramesIn[iFrame*channelsIn+0];
                    case 29: pFramesOut[iFrame*channelsOut+28] = pFramesIn[iFrame*channelsIn+0];
                    case 28: pFramesOut[iFrame*channelsOut+27] = pFramesIn[iFrame*channelsIn+0];
                    case 27: pFramesOut[iFrame*channelsOut+26] = pFramesIn[iFrame*channelsIn+0];
                    case 26: pFramesOut[iFrame*channelsOut+25] = pFramesIn[iFrame*channelsIn+0];
                    case 25: pFramesOut[iFrame*channelsOut+24] = pFramesIn[iFrame*channelsIn+0];
                    case 24: pFramesOut[iFrame*channelsOut+23] = pFramesIn[iFrame*channelsIn+0];
                    case 23: pFramesOut[iFrame*channelsOut+22] = pFramesIn[iFrame*channelsIn+0];
                    case 22: pFramesOut[iFrame*channelsOut+21] = pFramesIn[iFrame*channelsIn+0];
                    case 21: pFramesOut[iFrame*channelsOut+20] = pFramesIn[iFrame*channelsIn+0];
                    case 20: pFramesOut[iFrame*channelsOut+19] = pFramesIn[iFrame*channelsIn+0];
                    case 19: pFramesOut[iFrame*channelsOut+18] = pFramesIn[iFrame*channelsIn+0];
                    case 18: pFramesOut[iFrame*channelsOut+17] = pFramesIn[iFrame*channelsIn+0];
                    case 17: pFramesOut[iFrame*channelsOut+16] = pFramesIn[iFrame*channelsIn+0];
                    case 16: pFramesOut[iFrame*channelsOut+15] = pFramesIn[iFrame*channelsIn+0];
                    case 15: pFramesOut[iFrame*channelsOut+14] = pFramesIn[iFrame*channelsIn+0];
                    case 14: pFramesOut[iFrame*channelsOut+13] = pFramesIn[iFrame*channelsIn+0];
                    case 13: pFramesOut[iFrame*channelsOut+12] = pFramesIn[iFrame*channelsIn+0];
                    case 12: pFramesOut[iFrame*channelsOut+11] = pFramesIn[iFrame*channelsIn+0];
                    case 11: pFramesOut[iFrame*channelsOut+10] = pFramesIn[iFrame*channelsIn+0];
                    case 10: pFramesOut[iFrame*channelsOut+ 9] = pFramesIn[iFrame*channelsIn+0];
                    case  9: pFramesOut[iFrame*channelsOut+ 8] = pFramesIn[iFrame*channelsIn+0];
                    case  8: pFramesOut[iFrame*channelsOut+ 7] = pFramesIn[iFrame*channelsIn+0];
                    case  7: pFramesOut[iFrame*channelsOut+ 6] = pFramesIn[iFrame*channelsIn+0];
                    case  6: pFramesOut[iFrame*channelsOut+ 5] = pFramesIn[iFrame*channelsIn+0];
                    case  5: pFramesOut[iFrame*channelsOut+ 4] = pFramesIn[iFrame*channelsIn+0];
                    case  4: pFramesOut[iFrame*channelsOut+ 3] = pFramesIn[iFrame*channelsIn+0];
                    case  3: pFramesOut[iFrame*channelsOut+ 2] = pFramesIn[iFrame*channelsIn+0];
                    case  2: pFramesOut[iFrame*channelsOut+ 1] = pFramesIn[iFrame*channelsIn+0];
                    case  1: pFramesOut[iFrame*channelsOut+ 0] = pFramesIn[iFrame*channelsIn+0];
                }
            }
        } else if (channelsIn == 2) {
            // TODO: Implement an optimized stereo conversion.
            mal_dsp_mix_channels__inc(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mal_channel_mix_mode_basic);
        } else {
            // Fall back to basic mixing mode.
            mal_dsp_mix_channels__inc(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mal_channel_mix_mode_basic);
        }
    }
}

void mal_dsp_mix_channels(float* pFramesOut, mal_uint32 channelsOut, const mal_channel channelMapOut[MAL_MAX_CHANNELS], const float* pFramesIn, mal_uint32 channelsIn, const mal_channel channelMapIn[MAL_MAX_CHANNELS], mal_uint32 frameCount, mal_channel_mix_mode mode)
{
    if (channelsIn < channelsOut) {
        // Increasing the channel count.
        mal_dsp_mix_channels__inc(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mode);
    } else {
        // Decreasing the channel count.
        mal_dsp_mix_channels__dec(pFramesOut, channelsOut, channelMapOut, pFramesIn, channelsIn, channelMapIn, frameCount, mode);
    }
}


mal_uint32 mal_dsp__src_on_read(mal_src* pSRC, mal_uint32 frameCount, void* pFramesOut, void* pUserData)
{
    (void)pSRC;

    mal_dsp* pDSP = (mal_dsp*)pUserData;
    mal_assert(pDSP != NULL);

    return pDSP->onRead(pDSP, frameCount, pFramesOut, pDSP->pUserDataForOnRead);
}

mal_result mal_dsp_init(mal_dsp_config* pConfig, mal_dsp_read_proc onRead, void* pUserData, mal_dsp* pDSP)
{
    if (pDSP == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pDSP);
    pDSP->config = *pConfig;
    pDSP->onRead = onRead;
    pDSP->pUserDataForOnRead = pUserData;

    if (pDSP->config.cacheSizeInFrames > MAL_SRC_CACHE_SIZE_IN_FRAMES || pDSP->config.cacheSizeInFrames == 0) {
        pDSP->config.cacheSizeInFrames = MAL_SRC_CACHE_SIZE_IN_FRAMES;
    }

    if (pConfig->sampleRateIn != pConfig->sampleRateOut) {
        pDSP->isSRCRequired = MAL_TRUE;

        mal_src_config srcConfig;
        srcConfig.sampleRateIn = pConfig->sampleRateIn;
        srcConfig.sampleRateOut = pConfig->sampleRateOut;
        srcConfig.formatIn = pConfig->formatIn;
        srcConfig.formatOut = mal_format_f32;
        srcConfig.channels = pConfig->channelsIn;
        srcConfig.algorithm = mal_src_algorithm_linear;
        srcConfig.cacheSizeInFrames = pConfig->cacheSizeInFrames;
        mal_result result = mal_src_init(&srcConfig, mal_dsp__src_on_read, pDSP, &pDSP->src);
        if (result != MAL_SUCCESS) {
            return result;
        }
    }



    pDSP->isChannelMappingRequired = MAL_FALSE;
    if (pConfig->channelMapIn[0] != MAL_CHANNEL_NONE && pConfig->channelMapOut[0] != MAL_CHANNEL_NONE) {    // <-- Channel mapping will be ignored if the first channel map is MAL_CHANNEL_NONE.
        // When using channel mapping we need to figure out a shuffling table. The first thing to do is convert the input channel map
        // so that it contains the same number of channels as the output channel count.
        mal_uint32 iChannel;
        mal_uint32 channelsMin = mal_min(pConfig->channelsIn, pConfig->channelsOut);
        for (iChannel = 0; iChannel < channelsMin; ++iChannel) {
            pDSP->channelMapInPostMix[iChannel] = pConfig->channelMapIn[iChannel];
        }

        // Any excess channels need to be filled with the relevant channels from the output channel map. Currently we're justing filling it with
        // the first channels that are not present in the input channel map.
        if (pConfig->channelsOut > pConfig->channelsIn) {
            for (iChannel = pConfig->channelsIn; iChannel < pConfig->channelsOut; ++iChannel) {
                mal_uint8 newChannel = MAL_CHANNEL_NONE;
                for (mal_uint32 iChannelOut = 0; iChannelOut < pConfig->channelsOut; ++iChannelOut) {
                    mal_bool32 exists = MAL_FALSE;
                    for (mal_uint32 iChannelIn = 0; iChannelIn < pConfig->channelsIn; ++iChannelIn) {
                        if (pConfig->channelMapOut[iChannelOut] == pConfig->channelMapIn[iChannelIn]) {
                            exists = MAL_TRUE;
                            break;
                        }
                    }

                    if (!exists) {
                        newChannel = pConfig->channelMapOut[iChannelOut];
                        break;
                    }
                }

                pDSP->channelMapInPostMix[iChannel] = newChannel;
            }
        }

        // We only need to do a channel mapping if the map after mixing is different to the final output map.
        for (iChannel = 0; iChannel < pConfig->channelsOut; ++iChannel) {
            if (pDSP->channelMapInPostMix[iChannel] != pConfig->channelMapOut[iChannel]) {
                pDSP->isChannelMappingRequired = MAL_TRUE;
                break;
            }
        }

        // Now we need to create the shuffling table.
        if (pDSP->isChannelMappingRequired) {
            for (mal_uint32 iChannelIn = 0; iChannelIn < pConfig->channelsOut; ++iChannelIn) {
                for (mal_uint32 iChannelOut = 0; iChannelOut < pConfig->channelsOut; ++iChannelOut) {
                    if (pDSP->channelMapInPostMix[iChannelOut] == pConfig->channelMapOut[iChannelIn]) {
                        pDSP->channelShuffleTable[iChannelOut] = (mal_uint8)iChannelIn;
                    }
                }
            }
        }
    }

    if (pConfig->formatIn == pConfig->formatOut && pConfig->channelsIn == pConfig->channelsOut && pConfig->sampleRateIn == pConfig->sampleRateOut && !pDSP->isChannelMappingRequired) {
        pDSP->isPassthrough = MAL_TRUE;
    } else {
        pDSP->isPassthrough = MAL_FALSE;
    }

    return MAL_SUCCESS;
}


mal_result mal_dsp_refresh_sample_rate(mal_dsp* pDSP)
{
    // If we already have an SRC pipeline initialized we do _not_ want to re-create it. Instead we adjust it. If we didn't previously
    // have an SRC pipeline in place we'll need to initialize it.
    if (pDSP->isSRCRequired) {
        if (pDSP->config.sampleRateIn != pDSP->config.sampleRateOut) {
            mal_src_set_input_sample_rate(&pDSP->src, pDSP->config.sampleRateIn);
            mal_src_set_output_sample_rate(&pDSP->src, pDSP->config.sampleRateOut);
        } else {
            pDSP->isSRCRequired = MAL_FALSE;
        }
    } else {
        // We may need a new SRC pipeline.
        if (pDSP->config.sampleRateIn != pDSP->config.sampleRateOut) {
            pDSP->isSRCRequired = MAL_TRUE;

            mal_src_config srcConfig;
            srcConfig.sampleRateIn      = pDSP->config.sampleRateIn;
            srcConfig.sampleRateOut     = pDSP->config.sampleRateOut;
            srcConfig.formatIn          = pDSP->config.formatIn;
            srcConfig.formatOut         = mal_format_f32;
            srcConfig.channels          = pDSP->config.channelsIn;
            srcConfig.algorithm         = mal_src_algorithm_linear;
            srcConfig.cacheSizeInFrames = pDSP->config.cacheSizeInFrames;
            mal_result result = mal_src_init(&srcConfig, mal_dsp__src_on_read, pDSP, &pDSP->src);
            if (result != MAL_SUCCESS) {
                return result;
            }
        } else {
            pDSP->isSRCRequired = MAL_FALSE;
        }
    }

    // Update whether or not the pipeline is a passthrough.
    if (pDSP->config.formatIn == pDSP->config.formatOut && pDSP->config.channelsIn == pDSP->config.channelsOut && pDSP->config.sampleRateIn == pDSP->config.sampleRateOut && !pDSP->isChannelMappingRequired) {
        pDSP->isPassthrough = MAL_TRUE;
    } else {
        pDSP->isPassthrough = MAL_FALSE;
    }

    return MAL_SUCCESS;
}

mal_result mal_dsp_set_input_sample_rate(mal_dsp* pDSP, mal_uint32 sampleRateIn)
{
    if (pDSP == NULL) return MAL_INVALID_ARGS;

    // Must have a sample rate of > 0.
    if (sampleRateIn == 0) {
        return MAL_INVALID_ARGS;
    }

    pDSP->config.sampleRateIn = sampleRateIn;
    return mal_dsp_refresh_sample_rate(pDSP);
}

mal_result mal_dsp_set_output_sample_rate(mal_dsp* pDSP, mal_uint32 sampleRateOut)
{
    if (pDSP == NULL) return MAL_INVALID_ARGS;

    // Must have a sample rate of > 0.
    if (sampleRateOut == 0) {
        return MAL_INVALID_ARGS;
    }

    pDSP->config.sampleRateOut = sampleRateOut;
    return mal_dsp_refresh_sample_rate(pDSP);
}

mal_uint64 mal_dsp_read_frames(mal_dsp* pDSP, mal_uint64 frameCount, void* pFramesOut)
{
    return mal_dsp_read_frames_ex(pDSP, frameCount, pFramesOut, MAL_FALSE);
}

mal_uint64 mal_dsp_read_frames_ex(mal_dsp* pDSP, mal_uint64 frameCount, void* pFramesOut, mal_bool32 flush)
{
    if (pDSP == NULL || pFramesOut == NULL) return 0;

    // Fast path.
    if (pDSP->isPassthrough) {
        if (frameCount <= UINT32_MAX) {
            return (mal_uint32)pDSP->onRead(pDSP, (mal_uint32)frameCount, pFramesOut, pDSP->pUserDataForOnRead);
        } else {
            mal_uint64 totalFramesRead = 0;
            while (frameCount > 0) {
                mal_uint32 framesToReadRightNow = UINT32_MAX;
                if (framesToReadRightNow > frameCount) {
                    framesToReadRightNow = (mal_uint32)frameCount;
                }

                mal_uint32 framesRead = pDSP->onRead(pDSP, framesToReadRightNow, pFramesOut, pDSP->pUserDataForOnRead);
                if (framesRead == 0) {
                    break;
                }

                pFramesOut  = (mal_uint8*)pFramesOut + (framesRead * pDSP->config.channelsOut * mal_get_sample_size_in_bytes(pDSP->config.formatOut));
                frameCount -= framesRead;
                totalFramesRead += framesRead;
            }

            return totalFramesRead;
        }
    }


    // Slower path - where the real work is done.
    mal_uint8 pFrames[2][MAL_MAX_CHANNELS * 512 * MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES];
    mal_format pFramesFormat[2];

    mal_uint64 totalFramesRead = 0;
    while (frameCount > 0) {
        mal_uint32 iFrames = 0; // <-- Used as an index into pFrames and cycles between 0 and 1.

        mal_uint32 framesToRead = mal_countof(pFrames[0]) / (mal_max(pDSP->config.channelsIn, pDSP->config.channelsOut) * MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES);
        if (framesToRead > frameCount) {
            framesToRead = (mal_uint32)frameCount;
        }

        // The initial filling of sample data depends on whether or not we are using SRC.
        mal_uint32 framesRead = 0;
        if (pDSP->isSRCRequired) {
            framesRead = (mal_uint32)mal_src_read_frames_ex(&pDSP->src, framesToRead, pFrames[iFrames], flush);
            pFramesFormat[iFrames] = pDSP->src.config.formatOut;  // Should always be f32.
        } else {
            framesRead = pDSP->onRead(pDSP, framesToRead, pFrames[iFrames], pDSP->pUserDataForOnRead);
            pFramesFormat[iFrames] = pDSP->config.formatIn;
        }

        if (framesRead == 0) {
            break;
        }


        // Channel mixing. The input format must be in f32 which may require a conversion.
        if (pDSP->config.channelsIn != pDSP->config.channelsOut) {
            if (pFramesFormat[iFrames] != mal_format_f32) {
                mal_pcm_convert(pFrames[(iFrames + 1) % 2], mal_format_f32, pFrames[iFrames], pDSP->config.formatIn, framesRead * pDSP->config.channelsIn, mal_dither_mode_none);
                iFrames = (iFrames + 1) % 2;
                pFramesFormat[iFrames] = mal_format_f32;
            }

            mal_dsp_mix_channels((float*)(pFrames[(iFrames + 1) % 2]), pDSP->config.channelsOut, pDSP->config.channelMapOut, (const float*)(pFrames[iFrames]), pDSP->config.channelsIn, pDSP->config.channelMapIn, framesRead, mal_channel_mix_mode_blend);
            iFrames = (iFrames + 1) % 2;
            pFramesFormat[iFrames] = mal_format_f32;
        }


        // Channel mapping.
        if (pDSP->isChannelMappingRequired) {
            for (mal_uint32 i = 0; i < framesRead; ++i) {
                mal_rearrange_channels(pFrames[iFrames] + (i * pDSP->config.channelsOut * mal_get_sample_size_in_bytes(pFramesFormat[iFrames])), pDSP->config.channelsOut, pDSP->channelShuffleTable, pFramesFormat[iFrames]);
            }
        }


        // Final conversion to output format.
        mal_pcm_convert(pFramesOut, pDSP->config.formatOut, pFrames[iFrames], pFramesFormat[iFrames], framesRead * pDSP->config.channelsOut, mal_dither_mode_none);

        pFramesOut  = (mal_uint8*)pFramesOut + (framesRead * pDSP->config.channelsOut * mal_get_sample_size_in_bytes(pDSP->config.formatOut));
        frameCount -= framesRead;
        totalFramesRead += framesRead;
    }

    return totalFramesRead;
}


mal_uint64 mal_calculate_frame_count_after_src(mal_uint32 sampleRateOut, mal_uint32 sampleRateIn, mal_uint64 frameCountIn)
{
    double srcRatio = (double)sampleRateOut / sampleRateIn;
    double frameCountOutF = frameCountIn * srcRatio;

    mal_uint64 frameCountOut = (mal_uint64)frameCountOutF;

    // If the output frame count is fractional, make sure we add an extra frame to ensure there's enough room for that last sample.
    if ((frameCountOutF - frameCountOut) > 0.0) {
        frameCountOut += 1;
    }

    return frameCountOut;
}

typedef struct
{
    const void* pDataIn;
    mal_format formatIn;
    mal_uint32 channelsIn;
    mal_uint64 totalFrameCount;
    mal_uint64 iNextFrame;
} mal_convert_frames__data;

mal_uint32 mal_convert_frames__on_read(mal_dsp* pDSP, mal_uint32 frameCount, void* pFramesOut, void* pUserData)
{
    (void)pDSP;

    mal_convert_frames__data* pData = (mal_convert_frames__data*)pUserData;
    mal_assert(pData != NULL);
    mal_assert(pData->totalFrameCount >= pData->iNextFrame);

    mal_uint32 framesToRead = frameCount;
    mal_uint64 framesRemaining = (pData->totalFrameCount - pData->iNextFrame);
    if (framesToRead > framesRemaining) {
        framesToRead = (mal_uint32)framesRemaining;
    }

    mal_uint32 frameSizeInBytes = mal_get_sample_size_in_bytes(pData->formatIn) * pData->channelsIn;
    mal_copy_memory(pFramesOut, (const mal_uint8*)pData->pDataIn + (frameSizeInBytes * pData->iNextFrame), frameSizeInBytes * framesToRead);

    pData->iNextFrame += framesToRead;
    return framesToRead;
}

mal_uint64 mal_convert_frames(void* pOut, mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut, const void* pIn, mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_uint64 frameCountIn)
{
    if (frameCountIn == 0) {
        return 0;
    }

    mal_uint64 frameCountOut = mal_calculate_frame_count_after_src(sampleRateOut, sampleRateIn, frameCountIn);
    if (pOut == NULL) {
        return frameCountOut;
    }

    mal_convert_frames__data data;
    data.pDataIn = pIn;
    data.formatIn = formatIn;
    data.channelsIn = channelsIn;
    data.totalFrameCount = frameCountIn;
    data.iNextFrame = 0;

    mal_dsp_config config;
    mal_zero_object(&config);
    config.formatIn = formatIn;
    config.channelsIn = channelsIn;
    config.sampleRateIn = sampleRateIn;
    config.formatOut = formatOut;
    config.channelsOut = channelsOut;
    config.sampleRateOut = sampleRateOut;

    mal_dsp dsp;
    if (mal_dsp_init(&config, mal_convert_frames__on_read, &data, &dsp) != MAL_SUCCESS) {
        return 0;
    }

    return mal_dsp_read_frames_ex(&dsp, frameCountOut, pOut, MAL_TRUE);
}

mal_dsp_config mal_dsp_config_init(mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut)
{
    return mal_dsp_config_init_ex(formatIn, channelsIn, sampleRateIn, NULL, formatOut, channelsOut, sampleRateOut, NULL);
}

mal_dsp_config mal_dsp_config_init_ex(mal_format formatIn, mal_uint32 channelsIn, mal_uint32 sampleRateIn, mal_channel channelMapIn[MAL_MAX_CHANNELS], mal_format formatOut, mal_uint32 channelsOut, mal_uint32 sampleRateOut,  mal_channel channelMapOut[MAL_MAX_CHANNELS])
{
    mal_dsp_config config;
    mal_zero_object(&config);
    config.formatIn = formatIn;
    config.channelsIn = channelsIn;
    config.sampleRateIn = sampleRateIn;
    config.formatOut = formatOut;
    config.channelsOut = channelsOut;
    config.sampleRateOut = sampleRateOut;
    if (channelMapIn != NULL) {
        mal_copy_memory(config.channelMapIn, channelMapIn, sizeof(config.channelMapIn));
    }
    if (channelMapOut != NULL) {
        mal_copy_memory(config.channelMapOut, channelMapOut, sizeof(config.channelMapOut));
    }

    return config;
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Miscellaneous Helpers
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* mal_get_backend_name(mal_backend backend)
{
    switch (backend)
    {
        case mal_backend_null:       return "Null";
        case mal_backend_wasapi:     return "WASAPI";
        case mal_backend_dsound:     return "DirectSound";
        case mal_backend_winmm:      return "WinMM";
        case mal_backend_alsa:       return "ALSA";
        case mal_backend_pulseaudio: return "PulseAudio";
        case mal_backend_jack:       return "JACK";
        //case mal_backend_coreaudio:  return "Core Audio";
        case mal_backend_oss:        return "OSS";
        case mal_backend_opensl:     return "OpenSL|ES";
        case mal_backend_openal:     return "OpenAL";
        case mal_backend_sdl:        return "SDL";
        default:                     return "Unknown";
    }
}

const char* mal_get_format_name(mal_format format)
{
    switch (format)
    {
        case mal_format_unknown: return "Unknown";
        case mal_format_u8:      return "8-bit Unsigned Integer";
        case mal_format_s16:     return "16-bit Signed Integer";
        case mal_format_s24:     return "24-bit Signed Integer (Tightly Packed)";
        case mal_format_s32:     return "32-bit Signed Integer";
        case mal_format_f32:     return "32-bit IEEE Floating Point";
        default:                 return "Invalid";
    }
}

void mal_blend_f32(float* pOut, float* pInA, float* pInB, float factor, mal_uint32 channels)
{
    for (mal_uint32 i = 0; i < channels; ++i) {
        pOut[i] = mal_mix_f32(pInA[i], pInB[i], factor);
    }
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// DECODING
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef MAL_NO_DECODING

mal_decoder_config mal_decoder_config_init(mal_format outputFormat, mal_uint32 outputChannels, mal_uint32 outputSampleRate)
{
    mal_decoder_config config;
    mal_zero_object(&config);
    config.outputFormat = outputFormat;
    config.outputChannels = outputChannels;
    config.outputSampleRate = outputSampleRate;

    return config;
}

mal_decoder_config mal_decoder_config_init_copy(const mal_decoder_config* pConfig)
{
    mal_decoder_config config;
    if (pConfig != NULL) {
        config = *pConfig;
    } else {
        mal_zero_object(&config);
    }

    return config;
}

mal_result mal_decoder__init_dsp(mal_decoder* pDecoder, const mal_decoder_config* pConfig, mal_dsp_read_proc onRead)
{
    mal_assert(pDecoder != NULL);

    // Output format.
    if (pConfig->outputFormat == mal_format_unknown) {
        pDecoder->outputFormat = pDecoder->internalFormat;
    } else {
        pDecoder->outputFormat = pConfig->outputFormat;
    }

    if (pConfig->outputChannels == 0) {
        pDecoder->outputChannels = pDecoder->internalChannels;
    } else {
        pDecoder->outputChannels = pConfig->outputChannels;
    }

    if (pConfig->outputSampleRate == 0) {
        pDecoder->outputSampleRate = pDecoder->internalSampleRate;
    } else {
        pDecoder->outputSampleRate = pConfig->outputSampleRate;
    }

    mal_copy_memory(pDecoder->outputChannelMap, pConfig->outputChannelMap, sizeof(pConfig->outputChannelMap));


    // DSP.
    mal_dsp_config dspConfig = mal_dsp_config_init_ex(
        pDecoder->internalFormat, pDecoder->internalChannels, pDecoder->internalSampleRate, pDecoder->internalChannelMap,
        pDecoder->outputFormat,   pDecoder->outputChannels,   pDecoder->outputSampleRate,   pDecoder->outputChannelMap);
    return mal_dsp_init(&dspConfig, onRead, pDecoder, &pDecoder->dsp);
}

// WAV
#ifdef dr_wav_h
#define MAL_HAS_WAV

size_t mal_decoder_internal_on_read__wav(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);

    return pDecoder->onRead(pDecoder, pBufferOut, bytesToRead);
}

drwav_bool32 mal_decoder_internal_on_seek__wav(void* pUserData, int offset, drwav_seek_origin origin)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    return pDecoder->onSeek(pDecoder, offset, (origin == drwav_seek_origin_start) ? mal_seek_origin_start : mal_seek_origin_current);
}

mal_result mal_decoder_internal_on_seek_to_frame__wav(mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    drwav* pWav = (drwav*)pDecoder->pInternalDecoder;
    mal_assert(pWav != NULL);

    drwav_bool32 result = drwav_seek_to_sample(pWav, frameIndex*pWav->channels);
    if (result) {
        return MAL_SUCCESS;
    } else {
        return MAL_ERROR;
    }
}

mal_result mal_decoder_internal_on_uninit__wav(mal_decoder* pDecoder)
{
    drwav_close((drwav*)pDecoder->pInternalDecoder);
    return MAL_SUCCESS;
}

mal_uint32 mal_decoder_internal_on_read_frames__wav(mal_dsp* pDSP, mal_uint32 frameCount, void* pSamplesOut, void* pUserData)
{
    (void)pDSP;

    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);

    drwav* pWav = (drwav*)pDecoder->pInternalDecoder;
    mal_assert(pWav != NULL);

    switch (pDecoder->internalFormat) {
        case mal_format_s16: return (mal_uint32)drwav_read_s16(pWav, frameCount*pDecoder->internalChannels, (drwav_int16*)pSamplesOut) / pDecoder->internalChannels;
        case mal_format_s32: return (mal_uint32)drwav_read_s32(pWav, frameCount*pDecoder->internalChannels, (drwav_int32*)pSamplesOut) / pDecoder->internalChannels;
        case mal_format_f32: return (mal_uint32)drwav_read_f32(pWav, frameCount*pDecoder->internalChannels,       (float*)pSamplesOut) / pDecoder->internalChannels;
        default: break;
    }

    // Should never get here. If we do, it means the internal format was not set correctly at initialization time.
    mal_assert(MAL_FALSE);
    return 0;
}

mal_result mal_decoder_init_wav__internal(const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_assert(pConfig != NULL);
    mal_assert(pDecoder != NULL);

    // Try opening the decoder first.
    drwav* pWav = drwav_open(mal_decoder_internal_on_read__wav, mal_decoder_internal_on_seek__wav, pDecoder);
    if (pWav == NULL) {
        return MAL_ERROR;
    }

    // If we get here it means we successfully initialized the WAV decoder. We can now initialize the rest of the mal_decoder.
    pDecoder->onSeekToFrame = mal_decoder_internal_on_seek_to_frame__wav;
    pDecoder->onUninit = mal_decoder_internal_on_uninit__wav;
    pDecoder->pInternalDecoder = pWav;

    // Try to be as optimal as possible for the internal format. If mini_al does not support a format we will fall back to f32.
    pDecoder->internalFormat = mal_format_unknown;
    switch (pWav->translatedFormatTag) {
        case DR_WAVE_FORMAT_PCM:
        {
            if (pWav->bitsPerSample == 8) {
                pDecoder->internalFormat = mal_format_s16;
            } else if (pWav->bitsPerSample == 16) {
                pDecoder->internalFormat = mal_format_s16;
            } else if (pWav->bitsPerSample == 32) {
                pDecoder->internalFormat = mal_format_s32;
            }
        } break;

        case DR_WAVE_FORMAT_IEEE_FLOAT:
        {
            if (pWav->bitsPerSample == 32) {
                pDecoder->internalFormat = mal_format_f32;
            }
        } break;

        case DR_WAVE_FORMAT_ALAW:
        case DR_WAVE_FORMAT_MULAW:
        case DR_WAVE_FORMAT_ADPCM:
        case DR_WAVE_FORMAT_DVI_ADPCM:
        {
            pDecoder->internalFormat = mal_format_s16;
        } break;
    }

    if (pDecoder->internalFormat == mal_format_unknown) {
        pDecoder->internalFormat = mal_format_f32;
    }

    pDecoder->internalChannels = pWav->channels;
    pDecoder->internalSampleRate = pWav->sampleRate;
    mal_get_standard_channel_map(mal_standard_channel_map_microsoft, pDecoder->internalChannels, pDecoder->internalChannelMap);

    mal_result result = mal_decoder__init_dsp(pDecoder, pConfig, mal_decoder_internal_on_read_frames__wav);
    if (result != MAL_SUCCESS) {
        drwav_close(pWav);
        return result;
    }

    return MAL_SUCCESS;
}
#endif

// FLAC
#ifdef dr_flac_h
#define MAL_HAS_FLAC

size_t mal_decoder_internal_on_read__flac(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);

    return pDecoder->onRead(pDecoder, pBufferOut, bytesToRead);
}

drflac_bool32 mal_decoder_internal_on_seek__flac(void* pUserData, int offset, drflac_seek_origin origin)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    return pDecoder->onSeek(pDecoder, offset, (origin == drflac_seek_origin_start) ? mal_seek_origin_start : mal_seek_origin_current);
}

mal_result mal_decoder_internal_on_seek_to_frame__flac(mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    drflac* pFlac = (drflac*)pDecoder->pInternalDecoder;
    mal_assert(pFlac != NULL);

    drflac_bool32 result = drflac_seek_to_sample(pFlac, frameIndex*pFlac->channels);
    if (result) {
        return MAL_SUCCESS;
    } else {
        return MAL_ERROR;
    }
}

mal_result mal_decoder_internal_on_uninit__flac(mal_decoder* pDecoder)
{
    drflac_close((drflac*)pDecoder->pInternalDecoder);
    return MAL_SUCCESS;
}

mal_uint32 mal_decoder_internal_on_read_frames__flac(mal_dsp* pDSP, mal_uint32 frameCount, void* pSamplesOut, void* pUserData)
{
    (void)pDSP;

    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->internalFormat == mal_format_s32);

    drflac* pFlac = (drflac*)pDecoder->pInternalDecoder;
    mal_assert(pFlac != NULL);

    return (mal_uint32)drflac_read_s32(pFlac, frameCount*pDecoder->internalChannels, (drflac_int32*)pSamplesOut) / pDecoder->internalChannels;
}

mal_result mal_decoder_init_flac__internal(const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_assert(pConfig != NULL);
    mal_assert(pDecoder != NULL);

    // Try opening the decoder first.
    drflac* pFlac = drflac_open(mal_decoder_internal_on_read__flac, mal_decoder_internal_on_seek__flac, pDecoder);
    if (pFlac == NULL) {
        return MAL_ERROR;
    }

    // If we get here it means we successfully initialized the FLAC decoder. We can now initialize the rest of the mal_decoder.
    pDecoder->onSeekToFrame = mal_decoder_internal_on_seek_to_frame__flac;
    pDecoder->onUninit = mal_decoder_internal_on_uninit__flac;
    pDecoder->pInternalDecoder = pFlac;

    // The internal format is always s32.
    pDecoder->internalFormat = mal_format_s32;
    pDecoder->internalChannels = pFlac->channels;
    pDecoder->internalSampleRate = pFlac->sampleRate;
    mal_get_standard_channel_map(mal_standard_channel_map_flac, pDecoder->internalChannels, pDecoder->internalChannelMap);

    mal_result result = mal_decoder__init_dsp(pDecoder, pConfig, mal_decoder_internal_on_read_frames__flac);
    if (result != MAL_SUCCESS) {
        drflac_close(pFlac);
        return result;
    }

    return MAL_SUCCESS;
}
#endif

// Vorbis
#ifdef STB_VORBIS_INCLUDE_STB_VORBIS_H
#define MAL_HAS_VORBIS

// The size in bytes of each chunk of data to read from the Vorbis stream.
#define MAL_VORBIS_DATA_CHUNK_SIZE  4096

typedef struct
{
    stb_vorbis* pInternalVorbis;
    mal_uint8* pData;
    size_t dataSize;
    size_t dataCapacity;
    mal_uint32 framesConsumed;  // The number of frames consumed in ppPacketData.
    mal_uint32 framesRemaining; // The number of frames remaining in ppPacketData.
    float** ppPacketData;
} mal_vorbis_decoder;

mal_uint32 mal_vorbis_decoder_read(mal_vorbis_decoder* pVorbis, mal_decoder* pDecoder, mal_uint32 frameCount, void* pSamplesOut)
{
    mal_assert(pVorbis != NULL);
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    float* pSamplesOutF = (float*)pSamplesOut;

    mal_uint32 totalFramesRead = 0;
    while (frameCount > 0) {
        // Read from the in-memory buffer first.
        while (pVorbis->framesRemaining > 0 && frameCount > 0) {
            for (mal_uint32 iChannel = 0; iChannel < pDecoder->internalChannels; ++iChannel) {
                pSamplesOutF[0] = pVorbis->ppPacketData[iChannel][pVorbis->framesConsumed];
                pSamplesOutF += 1;
            }

            pVorbis->framesConsumed += 1;
            pVorbis->framesRemaining -= 1;
            frameCount -= 1;
            totalFramesRead += 1;
        }

        if (frameCount == 0) {
            break;
        }

        mal_assert(pVorbis->framesRemaining == 0);

        // We've run out of cached frames, so decode the next packet and continue iteration.
        do
        {
            if (pVorbis->dataSize > INT_MAX) {
                break;  // Too big.
            }

            int samplesRead = 0;
            int consumedDataSize = stb_vorbis_decode_frame_pushdata(pVorbis->pInternalVorbis, pVorbis->pData, (int)pVorbis->dataSize, NULL, (float***)&pVorbis->ppPacketData, &samplesRead);
            if (consumedDataSize != 0) {
                size_t leftoverDataSize = (pVorbis->dataSize - (size_t)consumedDataSize);
                for (size_t i = 0; i < leftoverDataSize; ++i) {
                    pVorbis->pData[i] = pVorbis->pData[i + consumedDataSize];
                }

                pVorbis->dataSize = leftoverDataSize;
                pVorbis->framesConsumed = 0;
                pVorbis->framesRemaining = samplesRead;
                break;
            } else {
                // Need more data. If there's any room in the existing buffer allocation fill that first. Otherwise expand.
                if (pVorbis->dataCapacity == pVorbis->dataSize) {
                    // No room. Expand.
                    pVorbis->dataCapacity += MAL_VORBIS_DATA_CHUNK_SIZE;
                    mal_uint8* pNewData = (mal_uint8*)mal_realloc(pVorbis->pData, pVorbis->dataCapacity);
                    if (pNewData == NULL) {
                        return totalFramesRead; // Out of memory.
                    }

                    pVorbis->pData = pNewData;
                }

                // Fill in a chunk.
                size_t bytesRead = pDecoder->onRead(pDecoder, pVorbis->pData + pVorbis->dataSize, (pVorbis->dataCapacity - pVorbis->dataSize));
                if (bytesRead == 0) {
                    return totalFramesRead; // Error reading more data.
                }

                pVorbis->dataSize += bytesRead;
            }
        } while (MAL_TRUE);
    }

    return totalFramesRead;
}

mal_result mal_vorbis_decoder_seek_to_frame(mal_vorbis_decoder* pVorbis, mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    mal_assert(pVorbis != NULL);
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    // This is terribly inefficient because stb_vorbis does not have a good seeking solution with it's push API. Currently this just performs
    // a full decode right from the start of the stream. Later on I'll need to write a layer that goes through all of the Ogg pages until we
    // find the one containing the sample we need. Then we know exactly where to seek for stb_vorbis.
    if (!pDecoder->onSeek(pDecoder, 0, mal_seek_origin_start)) {
        return MAL_ERROR;
    }

    stb_vorbis_flush_pushdata(pVorbis->pInternalVorbis);
    pVorbis->framesConsumed = 0;
    pVorbis->framesRemaining = 0;
    pVorbis->dataSize = 0;

    float buffer[4096];
    while (frameIndex > 0) {
        mal_uint32 framesToRead = mal_countof(buffer)/pDecoder->internalChannels;
        if (framesToRead > frameIndex) {
            framesToRead = (mal_uint32)frameIndex;
        }

        mal_uint32 framesRead = mal_vorbis_decoder_read(pVorbis, pDecoder, framesToRead, buffer);
        if (framesRead == 0) {
            return MAL_ERROR;
        }

        frameIndex -= framesRead;
    }

    return MAL_SUCCESS;
}


mal_result mal_decoder_internal_on_seek_to_frame__vorbis(mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    mal_vorbis_decoder* pVorbis = (mal_vorbis_decoder*)pDecoder->pInternalDecoder;
    mal_assert(pVorbis != NULL);

    return mal_vorbis_decoder_seek_to_frame(pVorbis, pDecoder, frameIndex);
}

mal_result mal_decoder_internal_on_uninit__vorbis(mal_decoder* pDecoder)
{
    mal_vorbis_decoder* pVorbis = (mal_vorbis_decoder*)pDecoder->pInternalDecoder;
    mal_assert(pVorbis != NULL);

    stb_vorbis_close(pVorbis->pInternalVorbis);
    mal_free(pVorbis->pData);
    mal_free(pVorbis);

    return MAL_SUCCESS;
}

mal_uint32 mal_decoder_internal_on_read_frames__vorbis(mal_dsp* pDSP, mal_uint32 frameCount, void* pSamplesOut, void* pUserData)
{
    (void)pDSP;

    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->internalFormat == mal_format_f32);
    mal_assert(pDecoder->onRead != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    mal_vorbis_decoder* pVorbis = (mal_vorbis_decoder*)pDecoder->pInternalDecoder;
    mal_assert(pVorbis != NULL);

    return mal_vorbis_decoder_read(pVorbis, pDecoder, frameCount, pSamplesOut);
}

mal_result mal_decoder_init_vorbis__internal(const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_assert(pConfig != NULL);
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    stb_vorbis* pInternalVorbis = NULL;

    // We grow the buffer in chunks.
    size_t dataSize = 0;
    size_t dataCapacity = 0;
    mal_uint8* pData = NULL;
    do
    {
        // Allocate memory for a new chunk.
        dataCapacity += MAL_VORBIS_DATA_CHUNK_SIZE;
        mal_uint8* pNewData = (mal_uint8*)mal_realloc(pData, dataCapacity);
        if (pNewData == NULL) {
            mal_free(pData);
            return MAL_OUT_OF_MEMORY;
        }

        pData = pNewData;

        // Fill in a chunk.
        size_t bytesRead = pDecoder->onRead(pDecoder, pData + dataSize, (dataCapacity - dataSize));
        if (bytesRead == 0) {
            return MAL_ERROR;
        }

        dataSize += bytesRead;
        if (dataSize > INT_MAX) {
            return MAL_ERROR;   // Too big.
        }

        int vorbisError = 0;
        int consumedDataSize = 0;
        pInternalVorbis = stb_vorbis_open_pushdata(pData, (int)dataSize, &consumedDataSize, &vorbisError, NULL);
        if (pInternalVorbis != NULL) {
            // If we get here it means we were able to open the stb_vorbis decoder. There may be some leftover bytes in our buffer, so
            // we need to move those bytes down to the front of the buffer since they'll be needed for future decoding.
            size_t leftoverDataSize = (dataSize - (size_t)consumedDataSize);
            for (size_t i = 0; i < leftoverDataSize; ++i) {
                pData[i] = pData[i + consumedDataSize];
            }

            dataSize = leftoverDataSize;
            break;  // Success.
        } else {
            if (vorbisError == VORBIS_need_more_data) {
                continue;
            } else {
                return MAL_ERROR;   // Failed to open the stb_vorbis decoder.
            }
        }
    } while (MAL_TRUE);


    // If we get here it means we successfully opened the Vorbis decoder.
    stb_vorbis_info vorbisInfo = stb_vorbis_get_info(pInternalVorbis);

    // Don't allow more than MAL_MAX_CHANNELS channels.
    if (vorbisInfo.channels > MAL_MAX_CHANNELS) {
        stb_vorbis_close(pInternalVorbis);
        mal_free(pData);
        return MAL_ERROR;   // Too many channels.
    }

    size_t vorbisDataSize = sizeof(mal_vorbis_decoder) + sizeof(float)*vorbisInfo.max_frame_size;
    mal_vorbis_decoder* pVorbis = (mal_vorbis_decoder*)mal_malloc(vorbisDataSize);
    if (pVorbis == NULL) {
        stb_vorbis_close(pInternalVorbis);
        mal_free(pData);
        return MAL_OUT_OF_MEMORY;
    }

    mal_zero_memory(pVorbis, vorbisDataSize);
    pVorbis->pInternalVorbis = pInternalVorbis;
    pVorbis->pData = pData;
    pVorbis->dataSize = dataSize;
    pVorbis->dataCapacity = dataCapacity;

    pDecoder->onSeekToFrame = mal_decoder_internal_on_seek_to_frame__vorbis;
    pDecoder->onUninit = mal_decoder_internal_on_uninit__vorbis;
    pDecoder->pInternalDecoder = pVorbis;

    // The internal format is always f32.
    pDecoder->internalFormat = mal_format_f32;
    pDecoder->internalChannels = vorbisInfo.channels;
    pDecoder->internalSampleRate = vorbisInfo.sample_rate;
    mal_get_standard_channel_map(mal_standard_channel_map_vorbis, pDecoder->internalChannels, pDecoder->internalChannelMap);

    mal_result result = mal_decoder__init_dsp(pDecoder, pConfig, mal_decoder_internal_on_read_frames__vorbis);
    if (result != MAL_SUCCESS) {
        stb_vorbis_close(pVorbis->pInternalVorbis);
        mal_free(pVorbis->pData);
        mal_free(pVorbis);
        return result;
    }

    return MAL_SUCCESS;
}
#endif

// MP3
#ifdef dr_mp3_h
#define MAL_HAS_MP3

size_t mal_decoder_internal_on_read__mp3(void* pUserData, void* pBufferOut, size_t bytesToRead)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onRead != NULL);

    return pDecoder->onRead(pDecoder, pBufferOut, bytesToRead);
}

drmp3_bool32 mal_decoder_internal_on_seek__mp3(void* pUserData, int offset, drmp3_seek_origin origin)
{
    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->onSeek != NULL);

    return pDecoder->onSeek(pDecoder, offset, (origin == drmp3_seek_origin_start) ? mal_seek_origin_start : mal_seek_origin_current);
}

mal_result mal_decoder_internal_on_seek_to_frame__mp3(mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    drmp3* pMP3 = (drmp3*)pDecoder->pInternalDecoder;
    mal_assert(pMP3 != NULL);

    drmp3_bool32 result = drmp3_seek_to_frame(pMP3, frameIndex);
    if (result) {
        return MAL_SUCCESS;
    } else {
        return MAL_ERROR;
    }
}

mal_result mal_decoder_internal_on_uninit__mp3(mal_decoder* pDecoder)
{
    drmp3_uninit((drmp3*)pDecoder->pInternalDecoder);
    mal_free(pDecoder->pInternalDecoder);
    return MAL_SUCCESS;
}

mal_uint32 mal_decoder_internal_on_read_frames__mp3(mal_dsp* pDSP, mal_uint32 frameCount, void* pSamplesOut, void* pUserData)
{
    (void)pDSP;

    mal_decoder* pDecoder = (mal_decoder*)pUserData;
    mal_assert(pDecoder != NULL);
    mal_assert(pDecoder->internalFormat == mal_format_f32);

    drmp3* pMP3 = (drmp3*)pDecoder->pInternalDecoder;
    mal_assert(pMP3 != NULL);

    return (mal_uint32)drmp3_read_f32(pMP3, frameCount, (float*)pSamplesOut);
}

mal_result mal_decoder_init_mp3__internal(const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_assert(pConfig != NULL);
    mal_assert(pDecoder != NULL);

    drmp3* pMP3 = (drmp3*)mal_malloc(sizeof(*pMP3));
    if (pMP3 == NULL) {
        return MAL_OUT_OF_MEMORY;
    }

    // Try opening the decoder first. MP3 can have variable sample rates (it's per frame/packet). We therefore need
    // to use some smarts to determine the most appropriate internal sample rate. These are the rules we're going
    // to use:
    //
    // Sample Rates
    // 1) If an output sample rate is specified in pConfig we just use that. Otherwise;
    // 2) Fall back to 44100.
    //
    // The internal channel count is always stereo, and the internal format is always f32.
    drmp3_config mp3Config;
    mal_zero_object(&mp3Config);
    mp3Config.outputChannels = 2;
    mp3Config.outputSampleRate = (pConfig->outputSampleRate != 0) ? pConfig->outputSampleRate : 44100;
    if (!drmp3_init(pMP3, mal_decoder_internal_on_read__mp3, mal_decoder_internal_on_seek__mp3, pDecoder, &mp3Config)) {
        return MAL_ERROR;
    }

    // If we get here it means we successfully initialized the MP3 decoder. We can now initialize the rest of the mal_decoder.
    pDecoder->onSeekToFrame = mal_decoder_internal_on_seek_to_frame__mp3;
    pDecoder->onUninit = mal_decoder_internal_on_uninit__mp3;
    pDecoder->pInternalDecoder = pMP3;

    // Internal format.
    pDecoder->internalFormat = mal_format_f32;
    pDecoder->internalChannels = pMP3->channels;
    pDecoder->internalSampleRate = pMP3->sampleRate;
    mal_get_standard_channel_map(mal_standard_channel_map_default, pDecoder->internalChannels, pDecoder->internalChannelMap);

    mal_result result = mal_decoder__init_dsp(pDecoder, pConfig, mal_decoder_internal_on_read_frames__mp3);
    if (result != MAL_SUCCESS) {
        mal_free(pMP3);
        return result;
    }

    return MAL_SUCCESS;
}
#endif

mal_result mal_decoder__preinit(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_assert(pConfig != NULL);

    if (pDecoder == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pDecoder);

    if (onRead == NULL || onSeek == NULL) {
        return MAL_INVALID_ARGS;
    }

    pDecoder->onRead = onRead;
    pDecoder->onSeek = onSeek;
    pDecoder->pUserData = pUserData;

    (void)pConfig;
    return MAL_SUCCESS;
}

mal_result mal_decoder_init_wav(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_decoder_config config = mal_decoder_config_init_copy(pConfig);

    mal_result result = mal_decoder__preinit(onRead, onSeek, pUserData, &config, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

#ifdef MAL_HAS_WAV
    return mal_decoder_init_wav__internal(&config, pDecoder);
#else
    return MAL_NO_BACKEND;
#endif
}

mal_result mal_decoder_init_flac(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_decoder_config config = mal_decoder_config_init_copy(pConfig);

    mal_result result = mal_decoder__preinit(onRead, onSeek, pUserData, &config, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

#ifdef MAL_HAS_FLAC
    return mal_decoder_init_flac__internal(&config, pDecoder);
#else
    return MAL_NO_BACKEND;
#endif
}

mal_result mal_decoder_init_vorbis(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_decoder_config config = mal_decoder_config_init_copy(pConfig);

    mal_result result = mal_decoder__preinit(onRead, onSeek, pUserData, &config, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

#ifdef MAL_HAS_VORBIS
    return mal_decoder_init_vorbis__internal(&config, pDecoder);
#else
    return MAL_NO_BACKEND;
#endif
}

mal_result mal_decoder_init_mp3(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_decoder_config config = mal_decoder_config_init_copy(pConfig);

    mal_result result = mal_decoder__preinit(onRead, onSeek, pUserData, &config, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

#ifdef MAL_HAS_MP3
    return mal_decoder_init_mp3__internal(&config, pDecoder);
#else
    return MAL_NO_BACKEND;
#endif
}

mal_result mal_decoder_init(mal_decoder_read_proc onRead, mal_decoder_seek_proc onSeek, void* pUserData, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_decoder_config config = mal_decoder_config_init_copy(pConfig);

    mal_result result = mal_decoder__preinit(onRead, onSeek, pUserData, &config, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    // We use trial and error to open a decoder.
    result = MAL_NO_BACKEND;

#ifdef MAL_HAS_WAV
    if (result != MAL_SUCCESS) {
        result = mal_decoder_init_wav__internal(&config, pDecoder);
        if (result != MAL_SUCCESS) {
            onSeek(pDecoder, 0, mal_seek_origin_start);
        }
    }
#endif
#ifdef MAL_HAS_FLAC
    if (result != MAL_SUCCESS) {
        result = mal_decoder_init_flac__internal(&config, pDecoder);
        if (result != MAL_SUCCESS) {
            onSeek(pDecoder, 0, mal_seek_origin_start);
        }
    }
#endif
#ifdef MAL_HAS_MP3
    if (result != MAL_SUCCESS) {
        result = mal_decoder_init_mp3__internal(&config, pDecoder);
        if (result != MAL_SUCCESS) {
            onSeek(pDecoder, 0, mal_seek_origin_start);
        }
    }
#endif
#ifdef MAL_HAS_VORBIS
    if (result != MAL_SUCCESS) {
        result = mal_decoder_init_vorbis__internal(&config, pDecoder);
        if (result != MAL_SUCCESS) {
            onSeek(pDecoder, 0, mal_seek_origin_start);
        }
    }
#endif

    if (result != MAL_SUCCESS) {
        return result;
    }

    return result;
}


size_t mal_decoder__on_read_memory(mal_decoder* pDecoder, void* pBufferOut, size_t bytesToRead)
{
    mal_assert(pDecoder->memory.dataSize >= pDecoder->memory.currentReadPos);

    size_t bytesRemaining = pDecoder->memory.dataSize - pDecoder->memory.currentReadPos;
    if (bytesToRead > bytesRemaining) {
        bytesToRead = bytesRemaining;
    }

    if (bytesToRead > 0) {
        mal_copy_memory(pBufferOut, pDecoder->memory.pData + pDecoder->memory.currentReadPos, bytesToRead);
        pDecoder->memory.currentReadPos += bytesToRead;
    }

    return bytesToRead;
}

mal_bool32 mal_decoder__on_seek_memory(mal_decoder* pDecoder, int byteOffset, mal_seek_origin origin)
{
    if (origin == mal_seek_origin_current) {
        if (byteOffset > 0) {
            if (pDecoder->memory.currentReadPos + byteOffset > pDecoder->memory.dataSize) {
                byteOffset = (int)(pDecoder->memory.dataSize - pDecoder->memory.currentReadPos);  // Trying to seek too far forward.
            }
        } else {
            if (pDecoder->memory.currentReadPos < (size_t)-byteOffset) {
                byteOffset = -(int)pDecoder->memory.currentReadPos;  // Trying to seek too far backwards.
            }
        }

        // This will never underflow thanks to the clamps above.
        pDecoder->memory.currentReadPos += byteOffset;
    } else {
        if ((mal_uint32)byteOffset <= pDecoder->memory.dataSize) {
            pDecoder->memory.currentReadPos = byteOffset;
        } else {
            pDecoder->memory.currentReadPos = pDecoder->memory.dataSize;  // Trying to seek too far forward.
        }
    }

    return MAL_TRUE;
}

mal_result mal_decoder__preinit_memory(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    if (pDecoder == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pDecoder);

    if (pData == NULL || dataSize == 0) {
        return MAL_INVALID_ARGS;
    }

    pDecoder->memory.pData = (const mal_uint8*)pData;
    pDecoder->memory.dataSize = dataSize;
    pDecoder->memory.currentReadPos = 0;

    (void)pConfig;
    return MAL_SUCCESS;
}

mal_result mal_decoder_init_memory(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_memory(pData, dataSize, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init(mal_decoder__on_read_memory, mal_decoder__on_seek_memory, NULL, pConfig, pDecoder);
}

mal_result mal_decoder_init_memory_wav(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_memory(pData, dataSize, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_wav(mal_decoder__on_read_memory, mal_decoder__on_seek_memory, NULL, pConfig, pDecoder);
}

mal_result mal_decoder_init_memory_flac(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_memory(pData, dataSize, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_flac(mal_decoder__on_read_memory, mal_decoder__on_seek_memory, NULL, pConfig, pDecoder);
}

mal_result mal_decoder_init_memory_vorbis(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_memory(pData, dataSize, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_vorbis(mal_decoder__on_read_memory, mal_decoder__on_seek_memory, NULL, pConfig, pDecoder);
}

mal_result mal_decoder_init_memory_mp3(const void* pData, size_t dataSize, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_memory(pData, dataSize, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_mp3(mal_decoder__on_read_memory, mal_decoder__on_seek_memory, NULL, pConfig, pDecoder);
}

#ifndef MAL_NO_STDIO
#include <stdio.h>
#ifndef _MSC_VER
#include <strings.h>    // For strcasecmp().
#endif

const char* mal_path_file_name(const char* path)
{
    if (path == NULL) {
        return NULL;
    }

    const char* fileName = path;

    // We just loop through the path until we find the last slash.
    while (path[0] != '\0') {
        if (path[0] == '/' || path[0] == '\\') {
            fileName = path;
        }

        path += 1;
    }

    // At this point the file name is sitting on a slash, so just move forward.
    while (fileName[0] != '\0' && (fileName[0] == '/' || fileName[0] == '\\')) {
        fileName += 1;
    }

    return fileName;
}

const char* mal_path_extension(const char* path)
{
    if (path == NULL) {
        path = "";
    }

    const char* extension = mal_path_file_name(path);
    const char* lastOccurance = NULL;

    // Just find the last '.' and return.
    while (extension[0] != '\0') {
        if (extension[0] == '.') {
            extension += 1;
            lastOccurance = extension;
        }

        extension += 1;
    }

    return (lastOccurance != NULL) ? lastOccurance : extension;
}

mal_bool32 mal_path_extension_equal(const char* path, const char* extension)
{
    if (path == NULL || extension == NULL) {
        return MAL_FALSE;
    }

    const char* ext1 = extension;
    const char* ext2 = mal_path_extension(path);

#ifdef _MSC_VER
    return _stricmp(ext1, ext2) == 0;
#else
    return strcasecmp(ext1, ext2) == 0;
#endif
}

size_t mal_decoder__on_read_stdio(mal_decoder* pDecoder, void* pBufferOut, size_t bytesToRead)
{
    return fread(pBufferOut, 1, bytesToRead, (FILE*)pDecoder->pUserData);
}

mal_bool32 mal_decoder__on_seek_stdio(mal_decoder* pDecoder, int byteOffset, mal_seek_origin origin)
{
    return fseek((FILE*)pDecoder->pUserData, byteOffset, (origin == mal_seek_origin_current) ? SEEK_CUR : SEEK_SET) == 0;
}

mal_result mal_decoder__preinit_file(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    if (pDecoder == NULL) return MAL_INVALID_ARGS;
    mal_zero_object(pDecoder);

    if (pFilePath == NULL || pFilePath[0] == '\0') {
        return MAL_INVALID_ARGS;
    }

    FILE* pFile;
#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (fopen_s(&pFile, pFilePath, "rb") != 0) {
        return MAL_ERROR;
    }
#else
    pFile = fopen(pFilePath, "rb");
    if (pFile == NULL) {
        return MAL_ERROR;
    }
#endif

    // We need to manually set the user data so the calls to mal_decoder__on_seek_stdio() succeed.
    pDecoder->pUserData = pFile;

    (void)pConfig;
    return MAL_SUCCESS;
}

mal_result mal_decoder_init_file(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_file(pFilePath, pConfig, pDecoder);    // This sets pDecoder->pUserData to a FILE*.
    if (result != MAL_SUCCESS) {
        return result;
    }

    // WAV
    if (mal_path_extension_equal(pFilePath, "wav")) {
        result =  mal_decoder_init_wav(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
        if (result == MAL_SUCCESS) {
            return MAL_SUCCESS;
        }

        mal_decoder__on_seek_stdio(pDecoder, 0, mal_seek_origin_start);
    }

    // FLAC
    if (mal_path_extension_equal(pFilePath, "flac")) {
        result =  mal_decoder_init_flac(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
        if (result == MAL_SUCCESS) {
            return MAL_SUCCESS;
        }

        mal_decoder__on_seek_stdio(pDecoder, 0, mal_seek_origin_start);
    }

    // MP3
    if (mal_path_extension_equal(pFilePath, "mp3")) {
        result =  mal_decoder_init_mp3(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
        if (result == MAL_SUCCESS) {
            return MAL_SUCCESS;
        }

        mal_decoder__on_seek_stdio(pDecoder, 0, mal_seek_origin_start);
    }

    // Trial and error.
    return mal_decoder_init(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
}

mal_result mal_decoder_init_file_wav(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_file(pFilePath, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_wav(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
}

mal_result mal_decoder_init_file_flac(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_file(pFilePath, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_flac(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
}

mal_result mal_decoder_init_file_vorbis(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_file(pFilePath, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_vorbis(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
}

mal_result mal_decoder_init_file_mp3(const char* pFilePath, const mal_decoder_config* pConfig, mal_decoder* pDecoder)
{
    mal_result result = mal_decoder__preinit_file(pFilePath, pConfig, pDecoder);
    if (result != MAL_SUCCESS) {
        return result;
    }

    return mal_decoder_init_mp3(mal_decoder__on_read_stdio, mal_decoder__on_seek_stdio, pDecoder->pUserData, pConfig, pDecoder);
}
#endif

mal_result mal_decoder_uninit(mal_decoder* pDecoder)
{
    if (pDecoder == NULL) return MAL_INVALID_ARGS;

    if (pDecoder->onUninit) {
        pDecoder->onUninit(pDecoder);
    }

#ifndef MAL_NO_STDIO
    // If we have a file handle, close it.
    if (pDecoder->onRead == mal_decoder__on_read_stdio) {
        fclose((FILE*)pDecoder->pUserData);
    }
#endif

    return MAL_SUCCESS;
}

mal_uint64 mal_decoder_read(mal_decoder* pDecoder, mal_uint64 frameCount, void* pFramesOut)
{
    if (pDecoder == NULL) return 0;

    return mal_dsp_read_frames_ex(&pDecoder->dsp, frameCount, pFramesOut, MAL_TRUE);
}

mal_result mal_decoder_seek_to_frame(mal_decoder* pDecoder, mal_uint64 frameIndex)
{
    if (pDecoder == NULL) return 0;

    if (pDecoder->onSeekToFrame) {
        return pDecoder->onSeekToFrame(pDecoder, frameIndex);
    }

    // Should never get here, but if we do it means onSeekToFrame was not set by the backend.
    return MAL_INVALID_ARGS;
}
#endif  // MAL_NO_DECODING

#endif  // MAL_IMPLEMENTATION


// REVISION HISTORY
// ================
//
// v0.x - 2018-xx-xx
//   - API CHANGE: Replace device enumeration APIs. mal_enumerate_devices() has been replaced with
//     mal_context_get_devices(). An additional low-level device enumration API has been introduced called
//     mal_context_enumerate_devices() which uses a callback to report devices.
//   - API CHANGE: Replace mal_device_config.preferExclusiveMode with mal_device_config.shareMode.
//     - This new config can be set to mal_share_mode_shared (default) or mal_share_mode_exclusive.
//   - API CHANGE: Remove excludeNullDevice from mal_context_config.alsa.
//   - API CHANGE: Rename MAL_MAX_SAMPLE_SIZE_IN_BYTES to MAL_MAX_PCM_SAMPLE_SIZE_IN_BYTES.
//   - API CHANGE: Change the default channel mapping to the standard Microsoft mapping.
//   - API CHANGE: Remove backend-specific result codes.
//   - Add support for PulseAudio.
//     - This is the highest priority backend on Linux (higher priority than ALSA) since it is commonly
//       installed by default on many of the popular distros and offer's more seamless integration on
//       platforms where PulseAudio is used. In addition, if PulseAudio is installed and running (which
//       is extremely common), it's better to just use PulseAudio directly rather than going through the
//       "pulse" ALSA plugin (which is what the "default" ALSA device is likely set to).
//   - Add support for JACK.
//   - Remove dependency on asound.h for the ALSA backend. This means the ALSA development packages are no
//     longer required to build mini_al.
//   - Remove dependency on dsound.h for the DirectSound backend. This fixes build issues with some
//     distributions of MinGW.
//   - Remove dependency on audioclient.h for the WASAPI backend. This fixes build issues with some
//     distributions of MinGW.
//   - Add support for configuring the priority of the worker thread.
//   - Introduce the notion of standard channel maps. Use mal_get_standard_channel_map().
//   - Introduce the notion of default device configurations. A default config uses the same configuration
//     as the backend's internal device, and as such results in a pass-through data transmission pipeline.
//   - Add support for passing in NULL for the device config in mal_device_init(), which uses a default
//     config. This requires manually calling mal_device_set_send/recv_callback().
//   - Make mal_device_init_ex() more robust.
//   - Make some APIs more const-correct.
//   - Fix errors with OpenAL detection.
//   - Fix some memory leaks.
//   - Miscellaneous bug fixes.
//   - Documentation updates.
//
// v0.7 - 2018-02-25
//   - API CHANGE: Change mal_src_read_frames() and mal_dsp_read_frames() to use 64-bit sample counts.
//   - Add decoder APIs for loading WAV, FLAC, Vorbis and MP3 files.
//   - Allow opening of devices without a context.
//     - In this case the context is created and managed internally by the device.
//   - Change the default channel mapping to the same as that used by FLAC.
//   - Fix build errors with macOS.
//
// v0.6c - 2018-02-12
//   - Fix build errors with BSD/OSS.
//
// v0.6b - 2018-02-03
//   - Fix some warnings when compiling with Visual C++.
//
// v0.6a - 2018-01-26
//   - Fix errors with channel mixing when increasing the channel count.
//   - Improvements to the build system for the OpenAL backend.
//   - Documentation fixes.
//
// v0.6 - 2017-12-08
//   - API CHANGE: Expose and improve mutex APIs. If you were using the mutex APIs before this version you'll
//     need to update.
//   - API CHANGE: SRC and DSP callbacks now take a pointer to a mal_src and mal_dsp object respectively.
//   - API CHANGE: Improvements to event and thread APIs. These changes make these APIs more consistent.
//   - Add support for SDL and Emscripten.
//   - Simplify the build system further for when development packages for various backends are not installed.
//     With this change, when the compiler supports __has_include, backends without the relevant development
//     packages installed will be ignored. This fixes the build for old versions of MinGW.
//   - Fixes to the Android build.
//   - Add mal_convert_frames(). This is a high-level helper API for performing a one-time, bulk conversion of
//     audio data to a different format.
//   - Improvements to f32 -> u8/s16/s24/s32 conversion routines.
//   - Fix a bug where the wrong value is returned from mal_device_start() for the OpenSL backend.
//   - Fixes and improvements for Raspberry Pi.
//   - Warning fixes.
//
// v0.5 - 2017-11-11
//   - API CHANGE: The mal_context_init() function now takes a pointer to a mal_context_config object for
//     configuring the context. The works in the same kind of way as the device config. The rationale for this
//     change is to give applications better control over context-level properties, add support for backend-
//     specific configurations, and support extensibility without breaking the API.
//   - API CHANGE: The alsa.preferPlugHW device config variable has been removed since it's not really useful for
//     anything anymore.
//   - ALSA: By default, device enumeration will now only enumerate over unique card/device pairs. Applications
//     can enable verbose device enumeration by setting the alsa.useVerboseDeviceEnumeration context config
//     variable.
//   - ALSA: When opening a device in shared mode (the default), the dmix/dsnoop plugin will be prioritized. If
//     this fails it will fall back to the hw plugin. With this change the preferExclusiveMode config is now
//     honored. Note that this does not happen when alsa.useVerboseDeviceEnumeration is set to true (see above)
//     which is by design.
//   - ALSA: Add support for excluding the "null" device using the alsa.excludeNullDevice context config variable.
//   - ALSA: Fix a bug with channel mapping which causes an assertion to fail.
//   - Fix errors with enumeration when pInfo is set to NULL.
//   - OSS: Fix a bug when starting a device when the client sends 0 samples for the initial buffer fill.
//
// v0.4 - 2017-11-05
//   - API CHANGE: The log callback is now per-context rather than per-device and as is thus now passed to
//     mal_context_init(). The rationale for this change is that it allows applications to capture diagnostic
//     messages at the context level. Previously this was only available at the device level.
//   - API CHANGE: The device config passed to mal_device_init() is now const.
//   - Added support for OSS which enables support on BSD platforms.
//   - Added support for WinMM (waveOut/waveIn).
//   - Added support for UWP (Universal Windows Platform) applications. Currently C++ only.
//   - Added support for exclusive mode for selected backends. Currently supported on WASAPI.
//   - POSIX builds no longer require explicit linking to libpthread (-lpthread).
//   - ALSA: Explicit linking to libasound (-lasound) is no longer required.
//   - ALSA: Latency improvements.
//   - ALSA: Use MMAP mode where available. This can be disabled with the alsa.noMMap config.
//   - ALSA: Use "hw" devices instead of "plughw" devices by default. This can be disabled with the
//     alsa.preferPlugHW config.
//   - WASAPI is now the highest priority backend on Windows platforms.
//   - Fixed an error with sample rate conversion which was causing crackling when capturing.
//   - Improved error handling.
//   - Improved compiler support.
//   - Miscellaneous bug fixes.
//
// v0.3 - 2017-06-19
//   - API CHANGE: Introduced the notion of a context. The context is the highest level object and is required for
//     enumerating and creating devices. Now, applications must first create a context, and then use that to
//     enumerate and create devices. The reason for this change is to ensure device enumeration and creation is
//     tied to the same backend. In addition, some backends are better suited to this design.
//   - API CHANGE: Removed the rewinding APIs because they're too inconsistent across the different backends, hard
//     to test and maintain, and just generally unreliable.
//   - Added helper APIs for initializing mal_device_config objects.
//   - Null Backend: Fixed a crash when recording.
//   - Fixed build for UWP.
//   - Added support for f32 formats to the OpenSL|ES backend.
//   - Added initial implementation of the WASAPI backend.
//   - Added initial implementation of the OpenAL backend.
//   - Added support for low quality linear sample rate conversion.
//   - Added early support for basic channel mapping.
//
// v0.2 - 2016-10-28
//   - API CHANGE: Add user data pointer as the last parameter to mal_device_init(). The rationale for this
//     change is to ensure the logging callback has access to the user data during initialization.
//   - API CHANGE: Have device configuration properties be passed to mal_device_init() via a structure. Rationale:
//     1) The number of parameters is just getting too much.
//     2) It makes it a bit easier to add new configuration properties in the future. In particular, there's a
//        chance there will be support added for backend-specific properties.
//   - Dropped support for f64, A-law and Mu-law formats since they just aren't common enough to justify the
//     added maintenance cost.
//   - DirectSound: Increased the default buffer size for capture devices.
//   - Added initial implementation of the OpenSL|ES backend.
//
// v0.1 - 2016-10-21
//   - Initial versioned release.


/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/
