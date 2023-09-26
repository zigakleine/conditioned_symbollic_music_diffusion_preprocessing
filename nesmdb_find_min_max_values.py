
import os
import torch
from torch.utils.data import Dataset
from torch.utils.data.dataset import T
import pickle
import numpy as np
import math
import json

metadata_folder = "db_metadata"
database_folder = "nesmdb"

current_dir = os.getcwd()
encoded_dir = "/storage/local/ssd/zigakleine-workspace/"


metadata_filename = "nesmdb_updated2808.pkl"
nesmdb_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  metadata_filename)

metadata = pickle.load(open(nesmdb_metadata_abs_path, "rb"))


global_min = float('inf')
global_max = float('-inf')

all_songs = 0
encodable_songs = 0
sequences = 0

for game in metadata:

    for song in metadata[game]["songs"]:

        if song["is_encodable"]:
            encodable_songs += 1
            song_rel_urls = song["encoded_song_urls"]
            print()
            for song_rel_url in song_rel_urls:
                song_abs_url = os.path.join(encoded_dir, song_rel_url)

                # if not os.path.exists(song_abs_url):
                #     print("notafile!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
                #     continue

                song_encoded = pickle.load(open(song_abs_url, "rb"))
                sequences += song_encoded.shape[0]
                song["num_sequences"] = song_encoded.shape[0]

                current_max = np.amax(song_encoded)
                current_min = np.amin(song_encoded)

                if current_min < global_min:
                    global_min = current_min

                if current_max > global_max:
                    global_max = current_max
        all_songs += 1
        print(all_songs)



print("global_max", global_max)
print("global_min", global_min)

print(f"valid_songs: {encodable_songs}/{all_songs}")
print("sequences:", sequences)

min_max = {"min": global_min, "max": global_max}

file = open('./nesmdb_min_max.pkl', 'wb')
pickle.dump(min_max, file)
file.close()

nesmdb_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder, metadata_filename)
file2 = open(nesmdb_metadata_abs_path, 'wb')
pickle.dump(metadata, file2)
file2.close()

json_filename = "nesmdb_meta_json2808.json"
nesmdb_json_abs_path = os.path.join(current_dir, metadata_folder, database_folder, json_filename)
y = json.dumps(metadata, indent=4)
file_json = open(nesmdb_json_abs_path, 'w')
file_json.write(y)
file_json.close()