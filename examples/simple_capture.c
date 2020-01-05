/* This example simply captures data from your default microphone until you press Enter. The output is saved to the file specified on the command line. */

#define MINIAUDIO_IMPLEMENTATION
#include "../miniaudio.h"

#define DR_WAV_IMPLEMENTATION
#include "../extras/dr_wav.h"

#include <stdlib.h>
#include <stdio.h>

void log_callback(ma_context* pContext, ma_device* pDevice, ma_uint32 logLevel, const char* message)
{
    (void)pContext;
    (void)pDevice;
    fprintf(stderr, "miniaudio: [%s] %s\n", ma_log_level_to_string(logLevel), message);
}

void data_callback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    drwav* pWav = (drwav*)pDevice->pUserData;
    ma_assert(pWav != NULL);

    drwav_write_pcm_frames(pWav, frameCount, pInput);

    fprintf(stderr, ".");

    (void)pOutput;
}

int main(int argc, char** argv)
{
    ma_result result;
    drwav_data_format wavFormat;
    drwav wav;
    ma_device_config deviceConfig;
    ma_device device;

    if (argc < 2) {
        printf("No input file.\n");
        return -1;
    }

    wavFormat.container     = drwav_container_riff;
    wavFormat.format        = DR_WAVE_FORMAT_IEEE_FLOAT;
    wavFormat.channels      = 2;
    wavFormat.sampleRate    = 44100;
    wavFormat.bitsPerSample = 32;

    if (drwav_init_file_write(&wav, argv[1], &wavFormat, NULL) == DRWAV_FALSE) {
        printf("Failed to initialize output file.\n");
        return -1;
    }

    {
    ma_context_config contextConfig;
    ma_context context;

    contextConfig = ma_context_config_init();
    contextConfig.logCallback = log_callback;

    if (ma_context_init(NULL, 0, &contextConfig, &context) != MA_SUCCESS) {
        printf("Failed to initialize context.");
        return -2;
    }
    }

    deviceConfig = ma_device_config_init(ma_device_type_capture);
    deviceConfig.capture.format   = ma_format_f32;
    deviceConfig.capture.channels = wavFormat.channels;
    deviceConfig.sampleRate       = wavFormat.sampleRate;
    deviceConfig.dataCallback     = data_callback;
    deviceConfig.pUserData        = &wav;
    deviceConfig.performanceProfile = ma_performance_profile_conservative;
    deviceConfig.periods = 3;
    deviceConfig.bufferSizeInMilliseconds = 20 * deviceConfig.periods;

    result = ma_device_init(NULL, &deviceConfig, &device);
    if (result != MA_SUCCESS) {
        printf("Failed to initialize capture device.\n");
        return -2;
    }

    result = ma_device_start(&device);
    if (result != MA_SUCCESS) {
        ma_device_uninit(&device);
        printf("Failed to start device.\n");
        return -3;
    }

    printf("Press Enter to stop recording...\n");
    getchar();
    
    ma_device_uninit(&device);
    drwav_uninit(&wav);

    return 0;
}
