import pickle
import os
import numpy as np
import json
import re
import time
from multitrack_VAE import db_processing
import uuid

current_dir = os.getcwd()
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_lib.so"
db_type = "nesmdb"

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_proc = db_processing(nesmdb_shared_library_path, db_type)

real_midis_out_dir = os.path.join(current_dir, "real_midis")
if not os.path.exists(real_midis_out_dir):
    os.mkdir(real_midis_out_dir)


db_metadata_pkl_rel_path = "db_metadata/nesmdb/nesmdb_updated2808.pkl"
db_metadata_pkl_abs_path = os.path.join(current_dir, db_metadata_pkl_rel_path)
metadata = pickle.load(open(db_metadata_pkl_abs_path, "rb"))


songs = []
for game in metadata:
    for song in metadata[game]["songs"]:
        if song["is_encodable"] and song["num_sequences"] > 1:
            song_url = song["url"]
            songs.append(song_url)
#
# for song_url in songs:
#     song_url_abs = os.path.join(current_dir, song_url)
#     song = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)
#     if len(song) < 32:
#         print("ojoj")


valid_songs = len(songs)
songs_to_sample = 100

sample_songs_idxs = np.random.choice(valid_songs, songs_to_sample, replace=False)

for i in sample_songs_idxs:
    uuid_string = str(uuid.uuid4())
    print(uuid_string)
    song_url = songs[i]
    song_url_abs = os.path.join(current_dir, song_url)
    song = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)
    song = song[:, 1:, :]
    song = song[:15]
    midi_output_path = os.path.join(real_midis_out_dir, uuid_string + ".mid")
    generated_midi = db_proc.midi_from_song(song)
    generated_midi.save(midi_output_path)