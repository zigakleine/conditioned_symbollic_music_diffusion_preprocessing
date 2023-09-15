import pickle
import os
import numpy as np
import json
import re
import time
from multitrack_VAE import db_processing, multitrack_vae, check_gpus
import uuid


current_dir = os.getcwd()
model_rel_path = "multitrack_vae_model_2/model.ckpt"
batch_size = 32
total_steps = 512
temperature = 0.2
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_lib.so"
db_type = "nesmdb"

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
model_path = os.path.join(current_dir, model_rel_path)

vae = multitrack_vae(model_path, batch_size)
db_proc = db_processing(nesmdb_shared_library_path, db_type)

vae_midis_out_dir = os.path.join(current_dir, "vae_midis")
if not os.path.exists(vae_midis_out_dir):
    os.mkdir(vae_midis_out_dir)

for i in range(100):
    uuid_string = str(uuid.uuid4())
    print(uuid_string)

    z = np.random.randn(batch_size, total_steps).astype(np.float32)
    decoded_song = vae.decode_sequence(z, total_steps, temperature)

    decoded_song = decoded_song[:15]
    midi_output_path = os.path.join(vae_midis_out_dir, uuid_string + ".mid")
    generated_midi = db_proc.midi_from_song(decoded_song)
    generated_midi.save(midi_output_path)
