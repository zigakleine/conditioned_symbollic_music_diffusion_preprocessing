import pickle
import os
import numpy as np
import json
import re
import time
from multitrack_VAE import db_processing, multitrack_vae, check_gpus

song_min_measures = 32
current_dir = os.getcwd()

subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
index = 0
subdirectory = subdirectories[index]

db_metadata_pkl_rel_path = "db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl"


db_metadata_json_rel_path = "db_metadata/lakh/lakh_json_2908_" + subdirectory + ".json"


metadata_full_path_pkl = os.path.join(current_dir, db_metadata_pkl_rel_path)
metadata_full_path_json = os.path.join(current_dir, db_metadata_json_rel_path)


def save_metadata(metadata):
    file = open(metadata_full_path_pkl, 'wb')
    pickle.dump(metadata, file)
    file.close()

    y = json.dumps(metadata, indent=4)
    file_json = open(metadata_full_path_json, 'w')
    file_json.write(y)
    file_json.close()


def lakh_encode(vae, db_proc):

    output_folder = "lakh_encoded"

    all_encodings_dir = os.path.join(current_dir, output_folder)

    if not os.path.exists(all_encodings_dir):
        os.mkdir(all_encodings_dir)

    sub_folder_encodings_dir = os.path.join(current_dir, output_folder, subdirectory)

    if not os.path.exists(sub_folder_encodings_dir):
        os.mkdir(sub_folder_encodings_dir)

    metadata = pickle.load(open(metadata_full_path_pkl, "rb"))
    count = 0
    all_songs = len(metadata.keys())

    for song in metadata.keys():

        is_encodable = metadata[song]["encodable"]

        encoded_song_filename = song + "_enc.pkl"
        encoded_song_rel_path = os.path.join(output_folder, subdirectory, encoded_song_filename)
        encoded_song_abs_path = os.path.join(current_dir, encoded_song_rel_path)

        if not is_encodable:
            count += 1
            continue

        time_start = time.time()
        if os.path.isfile(encoded_song_abs_path):

            if metadata[song]["encoded_song_path"] == encoded_song_rel_path:
                # encoding already exists
                continue
            else:
                # encoding exists on disk but not in metadata
                metadata[song]["encoded_song_path"] = encoded_song_rel_path
                save_metadata(metadata)

        else:
            song_rel_path = metadata[song]["url"]
            song_abs_path = os.path.join(current_dir, song_rel_path)
            song_data = db_proc.song_from_midi_lakh(song_abs_path)

            z = vae.encode_sequence(song_data)

            file = open(encoded_song_abs_path, 'wb')
            pickle.dump(z, file)
            file.close()

            metadata[song]["encoded_song_path"] = encoded_song_rel_path
            save_metadata(metadata)

        time_diff = time.time() - time_start
        print(f"{count}/{all_songs} time-{time_diff}")
        count += 1

    return metadata


if __name__ == "__main__":

    check_gpus()

    model_rel_path = "multitrack_vae_model/model_fb256.ckpt"
    nesmdb_shared_library_rel_path = "ext_nseq_lakh_lib.so"

    batch_size = 32

    model_path = os.path.join(current_dir, model_rel_path)

    nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
    db_type = "lakh"

    db_proc = db_processing(nesmdb_shared_library_path, db_type)
    vae = multitrack_vae(model_path, batch_size)

    metadata = lakh_encode(vae, db_proc)
    save_metadata(metadata)
