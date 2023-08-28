import numpy as np
import os
import tensorflow.compat.v1 as tf
import time

# from google.colab import files

# import magenta.music as mm
# from magenta.music.sequences_lib import concatenate_sequences
from magenta.models.music_vae import configs
from magenta.models.music_vae.trained_model import TrainedModel

from note_seq import midi_io, midi_file_to_note_sequence
from note_seq import sequences_lib

tf.disable_v2_behavior()
print('Done!')

BATCH_SIZE = 32


class multitrack_vae_old:

    def __init__(self):
        self.config = configs.CONFIG_MAP['hier-multiperf_vel_1bar_med']
        self.model = TrainedModel(
            self.config, batch_size=BATCH_SIZE,
            checkpoint_dir_or_path='./model_fb256.ckpt')
        self.model._config.data_converter._max_tensors_per_input = None

    def encode_sequence(self, full_file_path):
        sequence = midi_io.midi_to_sequence_proto(
            tf.gfile.GFile(full_file_path, 'rb').read())

        uploaded_seqs = []

        _, tensors, _, _ = self.model._config.data_converter.to_tensors(sequence)


        print(tensors)
        uploaded_seqs.extend(self.model._config.data_converter.from_tensors(tensors))

        #trim_sequences(uploaded_seqs)

        print('Parsed %d measures' % len(uploaded_seqs))
        start_time = time.time()
        z, _, _ = self.model.encode([seq for seq in uploaded_seqs])

        # self.model.encode_tensors()
        end_time = time.time()
        print("time in s:", (end_time - start_time), z)


