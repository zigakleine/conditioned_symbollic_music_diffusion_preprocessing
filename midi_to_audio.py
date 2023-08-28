from nesmdb.convert import midi_to_wav

import numpy as np
from scipy.io import wavfile


with open('nesdbmario/322_SuperMarioBros__00_01RunningAbout.mid', 'rb') as f:
  mid = f.read()
# Quantizes MIDI to 100Hz before rendering
# Can set to None to avoid quantization but will take more time/memory
wav = midi_to_wav(mid, midi_to_wav_rate=100)


# Replace this with your actual NumPy array containing audio samples
audio_samples = wav

# Set the sampling rate (44.1kHz in your case)
sampling_rate = 44100

# Define the output .wav file name
output_file = "output_audio.wav"

# Ensure that the audio samples are in the appropriate data type (int16 for 16-bit PCM)
audio_samples_int = np.int16(audio_samples * 32767)  # Scale to fit in 16 bits

# Write the audio samples to the .wav file
wavfile.write(output_file, sampling_rate, audio_samples_int)