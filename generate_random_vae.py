import pickle
import os
import numpy as np
import json
import re
import time
from singletrack_VAE import db_processing, singletrack_vae, check_gpus
import uuid

def _slerp(p0, p1, t):
    """Spherical linear interpolation."""
    omega = np.arccos(np.dot(np.squeeze(p0 / np.linalg.norm(p0)),
                             np.squeeze(p1 / np.linalg.norm(p1))))
    so = np.sin(omega)
    return np.sin((1.0 - t) * omega) / so * p0 + np.sin(t * omega) / so * p1


current_dir = os.getcwd()
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_single_lib.so"
db_type = "nesmdb_singletrack"
model_rel_path = "cat-mel_2bar_big.tar"

batch_size = 64
batch_size_latents = 16
total_steps = 32
latent_space_dims = 512
temperature = 0.0002

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
model_path = os.path.join(current_dir, model_rel_path)

vae = singletrack_vae(model_path, batch_size)
db_proc = db_processing(nesmdb_shared_library_path, db_type)

vae_eval_out_dir = os.path.join(current_dir, "samples_eval_vae")
if not os.path.exists(vae_eval_out_dir):
    os.mkdir(vae_eval_out_dir)

vae_survey_out_dir = os.path.join(current_dir, "samples_survey_vae")
if not os.path.exists(vae_survey_out_dir):
    os.mkdir(vae_survey_out_dir)

sample_songs = []

for i in range(100):
    uuid_string = str(uuid.uuid4())
    song_num = i

    z = np.random.randn(batch_size_latents*4, latent_space_dims).astype(np.float32)
    song_data_eval = vae.decode_sequence(z, total_steps, temperature)
    song_data_survey = song_data_eval[:, :15*16]

    midi_eval_output_path = os.path.join(vae_eval_out_dir, "eval_vae_" + str(song_num) + ".mid")
    midi_survey_output_path = os.path.join(vae_survey_out_dir, uuid_string + ".mid")

    generated_midi_eval = db_proc.midi_from_song(song_data_eval)
    generated_midi_survey = db_proc.midi_from_song(song_data_survey)

    generated_midi_eval.save(midi_eval_output_path)
    generated_midi_survey.save(midi_survey_output_path)