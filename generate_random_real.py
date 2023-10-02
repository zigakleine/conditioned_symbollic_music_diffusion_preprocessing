import pickle
import os
import numpy as np
import json
import re
import time
from singletrack_VAE import db_processing
import uuid

current_dir = os.getcwd()
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_single_lib.so"
db_type = "nesmdb_singletrack"

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_proc = db_processing(nesmdb_shared_library_path, db_type)

real_eval_out_dir = os.path.join(current_dir, "samples_eval_real")
real_survey_out_dir = os.path.join(current_dir, "samples_survey_real")

if not os.path.exists(real_eval_out_dir):
    os.mkdir(real_eval_out_dir)

if not os.path.exists(real_survey_out_dir):
    os.mkdir(real_survey_out_dir)


db_metadata_pkl_rel_path = "db_metadata/nesmdb/nesmdb_updated2808.pkl"
db_metadata_pkl_abs_path = os.path.join(current_dir, db_metadata_pkl_rel_path)
metadata = pickle.load(open(db_metadata_pkl_abs_path, "rb"))


songs = []
for game in metadata:
    for song in metadata[game]["songs"]:
        if song["is_encodable"]:
            song_url = song["url"]
            is_looping = song["is_looping"]
            num_sequences = song["num_sequences"]
            for i in range(num_sequences):
                song_dict = {"url": song_url, "is_looping": is_looping, "index": i}
                songs.append(song_dict)


valid_songs = len(songs)
songs_to_sample = 100
song_min_measures = 32
num_tracks = 4

sample_songs_idxs = np.random.choice(valid_songs, songs_to_sample, replace=False)
sample_songs = []

song_num = 0

for i in sample_songs_idxs:
    uuid_string = str(uuid.uuid4())

    song_dict = songs[i]
    song_url = song_dict["url"]
    is_looping = song_dict["is_looping"]
    index = song_dict["index"]

    song_url_abs = os.path.join(current_dir, song_url)
    song_data = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)

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
            song_data_extended = song_data[:, :new_length_measures * 16]

    num_batches = (song_data_extended.shape[1]//16) // song_min_measures

    song_data_eval = song_data_extended[:, (index*16*32):((index+1)*16*32)]
    song_data_survey = song_data_eval[:, :15*16]

    midi_eval_output_path = os.path.join(real_eval_out_dir, "eval_real_" + str(song_num) + ".mid")
    midi_survey_output_path = os.path.join(real_survey_out_dir, uuid_string + ".mid")
    song_num += 1
    generated_midi_eval = db_proc.midi_from_song(song_data_eval)
    generated_midi_survey = db_proc.midi_from_song(song_data_survey)

    generated_midi_eval.save(midi_eval_output_path)
    generated_midi_survey.save(midi_survey_output_path)


