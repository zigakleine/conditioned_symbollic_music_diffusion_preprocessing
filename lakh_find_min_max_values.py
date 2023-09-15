
import os
import torch
from torch.utils.data import Dataset
from torch.utils.data.dataset import T
import pickle
import numpy as np
import math

subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
metadata_folder = "db_metadata"
database_folder = "lakh"

current_dir = os.getcwd()
all_lakh_metadata = dict()

for subdir_name in subdirectories:
    current_metadata_filename = "lakh_2908_" + subdir_name + ".pkl"
    current_lakh_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  current_metadata_filename)

    metadata = pickle.load(open(current_lakh_metadata_abs_path, "rb"))
    all_lakh_metadata.update(metadata)


# print(all_lakh_metadata)
current_dir = os.getcwd()

global_min = float('inf')
global_max = float('-inf')

all_songs = 0
encodable_songs = 0
sequences = 0

for song in all_lakh_metadata:

    if all_lakh_metadata[song]["encodable"]:
        encodable_songs += 1
        song_rel_url = all_lakh_metadata[song]["encoded_song_path"]
        song_abs_url = os.path.join(current_dir, song_rel_url)
        song_encoded = pickle.load(open(song_abs_url, "rb"))
        sequences += song_encoded.shape[0]

        current_max = np.amax(song_encoded)
        current_min = np.amin(song_encoded)

        if current_min < global_min:
            global_min = current_min

        if current_max > global_max:
            global_max = current_max
    all_songs += 1
    print(all_songs)


# global_max 17.200264
# global_min -16.829231
# valid_songs: 80173/178460
# sequences: 280962

print("global_max", global_max)
print("global_min", global_min)

print(f"valid_songs: {encodable_songs}/{all_songs}")
print("sequences:", sequences)

min_max = {"min": global_min, "max": global_max}

file = open('./lakh_min_max.pkl', 'wb')
pickle.dump(min_max, file)
file.close()