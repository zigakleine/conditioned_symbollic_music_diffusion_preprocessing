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
song_min_measures = 32


valid_songs_counter = 0
valid_sequences_counter = 0
all_songs_counter = 0

db_metadata_pkl_rel_path = "db_metadata/nesmdb/nesmdb_updated2808.pkl"
metadata_full_path_pkl = os.path.join(current_dir, db_metadata_pkl_rel_path)
metadata = pickle.load(open(metadata_full_path_pkl, "rb"))

for game in metadata.keys():
    songs = metadata[game]["songs"]
    for song in songs:
        all_songs_counter += 1

        is_encodable = song["is_encodable"]
        is_looping = song["is_looping"]
        song_rel_path = song["url"]
        song_full_path = os.path.join(current_dir, song_rel_path)

        if is_encodable:

            time_start = time.time()
            song_data = db_proc.song_from_midi_nesmdb(song_full_path, 0, True)

            song_measures = song_data.shape[1] // 16

            song_data_extended = []

            if song_measures < song_min_measures:
                if is_looping:
                    for i in range(song_min_measures * 16):
                        song_data_extended.append(song_data[:, i % (song_measures * 16)])
                    song_data_extended = np.vstack(song_data_extended).T
            else:
                if is_looping:
                    new_length_measures = ((song_measures // song_min_measures + 1) * song_min_measures)
                    for i in range(new_length_measures * 16):
                        song_data_extended.append(song_data[:, i % (song_measures * 16)])
                    song_data_extended = np.vstack(song_data_extended).T
                else:
                    new_length_measures = ((song_measures // song_min_measures) * song_min_measures)
                    song_data_extended = song_data[:, :(new_length_measures * 16)]

            z = vae.encode_sequence(song_data_extended)

            z_list.append(z)
            time_diff = time.time() - time_start

            print(f"{count}/{all_songs} time-{time_diff}")
            count += 1

file = open("./nesmdb_single_stacked_2.pkl", 'wb')
pickle.dump(z_list, file)
file.close()


