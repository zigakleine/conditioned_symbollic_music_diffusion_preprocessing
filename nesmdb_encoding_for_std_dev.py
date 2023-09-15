import pickle
import os
import numpy as np
import json
import re
import time
from singletrack_VAE import db_processing, singletrack_vae, check_gpus

check_gpus()

current_dir = os.getcwd()
model_rel_path = "cat-mel_2bar_big.tar"
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_single_lib.so"
db_type = "nesmdb_singletrack"

batch_size = 64

model_path = os.path.join(current_dir, model_rel_path)
nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)

db_proc = db_processing(nesmdb_shared_library_path, db_type)
vae = singletrack_vae(model_path, batch_size)

nesmdb_folder = "nesmdb_flat"
nesmdb_flat_abs_path = os.path.join(current_dir, nesmdb_folder)

z_list = []

count = 1
all_songs = len(os.listdir(nesmdb_flat_abs_path))

for filename in os.listdir(nesmdb_flat_abs_path):
    song_path = os.path.join(nesmdb_flat_abs_path, filename)

    if os.path.isfile(song_path) and filename.split(".")[1] == "mid":

        time_start = time.time()

        song_data = db_proc.song_from_midi_nesmdb(song_path, 0, True)
        z = vae.encode_sequence(song_data)
        z_list.append(z)
        time_diff = time.time() - time_start

        print(f"{count}/{all_songs} time-{time_diff}")
        count += 1


z_stacked = np.vstack(z_list)
print("stacked")

file = open("./nesmdb_single_stacked.pkl", 'wb')
pickle.dump(z_stacked, file)
file.close()


