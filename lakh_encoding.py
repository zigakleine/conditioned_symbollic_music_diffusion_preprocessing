import pickle
import os
import numpy as np
import json
import re
import time
from singletrack_VAE import db_processing, singletrack_vae, check_gpus





def save_metadata(metadata):
    file = open(metadata_full_path_pkl, 'wb')
    pickle.dump(metadata, file)
    file.close()

    y = json.dumps(metadata, indent=4)
    file_json = open(metadata_full_path_json, 'w')
    file_json.write(y)
    file_json.close()


def lakh_encode(vae, db_proc, dir_to_save, fb256_mask):

    output_folder = "lakh_encoded"

    all_encodings_dir = os.path.join(dir_to_save, output_folder)

    if not os.path.exists(all_encodings_dir):
        os.mkdir(all_encodings_dir)

    sub_folder_encodings_dir = os.path.join(dir_to_save, output_folder, subdirectory)

    if not os.path.exists(sub_folder_encodings_dir):
        os.mkdir(sub_folder_encodings_dir)

    metadata = pickle.load(open(metadata_full_path_pkl, "rb"))
    count = 0
    succesful_songs = 0
    successful_sequences = 0
    all_songs = len(metadata.keys())

    for song in metadata.keys():


        encoded_song_filename = song + "_enc.pkl"
        encoded_song_rel_path = os.path.join(output_folder, subdirectory, encoded_song_filename)
        encoded_song_abs_path = os.path.join(dir_to_save, encoded_song_rel_path)

        # if not is_encodable:
        #     count += 1
        #     continue

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

            if song_data is None:
                count += 1
                print("unsuccessful")
                metadata[song]["encodable"] = False
                continue

            song_data_sequences = song_data.shape[0]

            zs = []
            for song_data_seq in song_data:
                z = vae.encode_sequence(song_data_seq)
                zs.append(z)
            z_reshaped = np.array(zs)

            file = open(encoded_song_abs_path, 'wb')
            pickle.dump(z_reshaped, file)
            file.close()
            succesful_songs += 1
            successful_sequences += song_data_sequences

            metadata[song]["encodable"] = True
            metadata[song]["encoded_song_path"] = encoded_song_rel_path
            metadata[song]["num_sequences"] = song_data_sequences
            save_metadata(metadata)

        time_diff = time.time() - time_start
        print(f"{count}/{all_songs} time-{time_diff}")
        count += 1

    print(f"successful songs-{succesful_songs}")
    print(f"successful sequences-{successful_sequences}")
    print(f"all songs-{all_songs}")

    file_info_dir = os.path.join(current_dir, run_info_dir)
    run_info_file_name = subdirectory + "_run.txt"
    file_info_file_dir = os.path.join(current_dir, run_info_dir, run_info_file_name)
    if not os.path.exists(file_info_dir):
        os.mkdir(file_info_dir)
    file = open(file_info_file_dir, 'w')

    file.write("successful songs: " + str(succesful_songs) + "\n")
    file.write("successful sequences: " + str(successful_sequences) + "\n")
    file.write("all songs: " + str(all_songs) + "\n")
    file.close()
    return metadata, successful_sequences


current_dir = os.getcwd()
dir_to_save = "/storage/local/ssd/zigakleine-workspace"
model_rel_path = "cat-mel_2bar_big.tar"
nesmdb_shared_library_rel_path = "ext_nseq_lakh_single_lib.so"

batch_size = 64

model_path = os.path.join(current_dir, model_rel_path)

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_type = "lakh_singletrack"


db_proc = db_processing(nesmdb_shared_library_path, db_type)
vae = singletrack_vae(model_path, batch_size)

run_info_dir = "lakh_run_info"

# slices_rel_path = "fb256_slices_76.pkl"
# slices_abs_path = os.path.join(current_dir, slices_rel_path)
# fb256_slices = pickle.load(open(slices_abs_path, "rb"))
# fb256_slices = np.array(fb256_slices)
#
# fb256_mask = np.zeros((512,), dtype=bool)
# fb256_mask[fb256_slices] = True

check_gpus()
all_valid_sequences_num = 0

for i in range(16):

    song_min_measures = 32

    subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]

    subdirectory = subdirectories[i]

    db_metadata_pkl_rel_path = "db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl"

    db_metadata_json_rel_path = "db_metadata/lakh/lakh_json_2908_" + subdirectory + ".json"

    metadata_full_path_pkl = os.path.join(current_dir, db_metadata_pkl_rel_path)
    metadata_full_path_json = os.path.join(current_dir, db_metadata_json_rel_path)

    metadata, successful_sequences_num = lakh_encode(vae, db_proc, dir_to_save, None)
    all_valid_sequences_num += successful_sequences_num
    save_metadata(metadata)

print("all_valid_sequences_num", all_valid_sequences_num)
