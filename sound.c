#include <SDL2/SDL.h>

static SDL_AudioDeviceID output;
static int64_t sample, freq;
static SDL_AudioSpec have;
static double vol = .1;
void AiwniosSetVolume(double v) {
  if (v > 100.)
    v = 100;
  vol = v / 100;
}
double AiwniosGetVolume() {
  return vol * 100;
}
static void AudioCB(void *ul, int8_t *out, int64_t len) {
  unsigned int i, i2;
  int64_t fpb = len / have.channels / 2;
  for (i = 0; i < fpb; i++) {
    double t      = (double)++sample / have.freq;
    double amp    = -1.0 + 2.0 * round(fmod(2.0 * t * freq, 1.0));
    int64_t maxed = (amp > 0) ? 16384 : -16384;
    maxed *= vol;
    if (!freq)
      maxed = 0;
    for (i2 = 0; i2 != have.channels; i2++)
      ((int16_t*)out)[have.channels * i + i2] = maxed;
  }
}
void SndFreq(int64_t f) {
  freq = f;
}
void InitSound() {
#ifdef __linux__
  SDL_SetHint(SDL_HINT_AUDIODRIVER, "pipewire");
  if (SDL_Init(SDL_INIT_AUDIO))
    SDL_SetHintWithPriority(SDL_HINT_AUDIODRIVER, NULL, SDL_HINT_OVERRIDE);
#endif
  SDL_AudioSpec want;
  if (SDL_Init(SDL_INIT_AUDIO)) {
    // Audio failed to initailize
    return;
  }
  memset(&want, 0, sizeof(SDL_AudioSpec));
  want.freq     = 24000;
  want.format   = AUDIO_S16;
  want.channels = 2;
  want.samples  = 64;
  want.callback = (void *)&AudioCB;
  output        = SDL_OpenAudioDevice(NULL, 0, &want, &have,
                                      SDL_AUDIO_ALLOW_FREQUENCY_CHANGE |
                                          SDL_AUDIO_ALLOW_CHANNELS_CHANGE);
  SDL_PauseAudioDevice(output, 0);
}
