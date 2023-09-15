# import os
# import pickle
# import json
# from multitrack_VAE import db_processing
#
# # assign directory
# current_directory = os.getcwd()
#
# lakh_folder = "lmd_full"
# subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
# index = 15
# subdirectory = subdirectories[index]
#
# lakh_subdir_path = os.path.join(current_directory, lakh_folder, subdirectory)
#
# lakh_shared_library_rel_path = "ext_nseq_lakh_lib.so"
# batch_size = 32
#
# lakh_shared_library_path = os.path.join(current_directory, lakh_shared_library_rel_path)
# db_type = "lakh"
#
# db_proc = db_processing(lakh_shared_library_path, db_type)
#
# metadata = pickle.load(open("./db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl", 'rb'))
#
# all_songs = 0
# valid_sequences = 0
# valid_songs = 0
#
# for song in metadata.keys():
#
#     all_songs +=1
#
#     song_rel_path = metadata[song]["url"]
#     song_abs_path = os.path.join(current_directory, song_rel_path)
#     song_data = db_proc.song_from_midi_lakh(song_abs_path)
#
#     if song_data is None:
#         metadata[song]["encodable"] = False
#     else:
#         valid_sequences += song_data.shape[0]
#         valid_songs += 1
#
#
# print("all_songs", all_songs)
# print("valid_sequences", valid_sequences)
# print("valid_songs", valid_songs)
# file = open("./db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl", 'wb')
# pickle.dump(metadata, file)
# file.close()
#
# y = json.dumps(metadata, indent=4)
# file_json = open("db_metadata/lakh/lakh_json_2908_" + subdirectory + ".json", 'w')
# file_json.write(y)
# file_json.close()

import os
import torch
from torch.utils.data import Dataset
from torch.utils.data.dataset import T
import pickle
import json

subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
metadata_folder = "db_metadata"
database_folder = "lakh"

current_dir = os.getcwd()
all_lakh_metadata = []

for subdir_name in subdirectories:
    current_metadata_filename = "lakh_2908_" + subdir_name + ".pkl"
    current_json_filename = "lakh_json_2908_" + subdir_name + ".json"
    current_lakh_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  current_metadata_filename)
    current_lakh_json_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  current_json_filename)

    metadata = pickle.load(open(current_lakh_metadata_abs_path, "rb"))
    for song in metadata:
        if metadata[song]["encodable"]:
            #all_lakh_metadata.append(metadata[song])
            encoded_abs_path = os.path.join(current_dir, metadata[song]["encoded_song_path"])
            file_enc = pickle.load(open(encoded_abs_path, "rb"))
            metadata[song]["num_sequences"] = file_enc.shape[0]


    file = open(current_lakh_metadata_abs_path, 'wb')
    pickle.dump(metadata, file)
    file.close()

    y = json.dumps(metadata, indent=4)
    file_json = open(current_lakh_json_abs_path, 'w')
    file_json.write(y)
    file_json.close()