import pickle
import os
import numpy as np
import json
import re
import time
from multitrack_VAE import db_processing, multitrack_vae, check_gpus

check_gpus()

current_dir = os.getcwd()
model_rel_path = "multitrack_vae_model/model_fb256.ckpt"
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_lib.so"
batch_size = 32

model_path = os.path.join(current_dir, model_rel_path)
nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_type = "nesmdb"

db_proc = db_processing(nesmdb_shared_library_path, db_type)
vae = multitrack_vae(model_path, batch_size)

nesmdb_folder = "nesmdb_flat"
nesmdb_flat_abs_path = os.path.join(current_dir, nesmdb_folder)

z_list = []

count = 1
all_songs = len(os.listdir(nesmdb_flat_abs_path))

for filename in os.listdir(nesmdb_flat_abs_path):
    song_path = os.path.join(nesmdb_flat_abs_path, filename)

    if os.path.isfile(song_path) and filename.split(".")[1] == "mid":

        time_start = time.time()

        song_data = db_proc.song_from_midi_nesmdb(song_path)
        song_data_reshaped = song_data[:, 1:, :]
        z = vae.encode_sequence(song_data_reshaped)
        z_list.append(z)
        time_diff = time.time() - time_start

        print(f"{count}/{all_songs} time-{time_diff}")
        count += 1

        if count > 50:
            break

z_stacked = np.vstack(z_list)
print("stacked")

file = open("./nesmdb_stacked.pkl", 'wb')
pickle.dump(z_stacked, file)
file.close()


