import pickle
import os
import numpy as np
import json
import re
import time
from singletrack_VAE import db_processing, singletrack_vae, check_gpus

instruments_dict = {"p1": 0, "p2": 1, "tr": 2, "no": 3}

run_info_dir = "nesmdb_run_info"

song_min_measures = 32
current_dir = os.getcwd()

db_metadata_pkl_1_rel_path = "db_metadata/nesmdb/nesmdb_updated2808.pkl"
db_metadata_pkl_2_rel_path = "db_metadata/nesmdb/nesmdb_updated2808_BACKUP.pkl"

db_metadata_json_1_rel_path = "db_metadata/nesmdb/nesmdb_meta_json2808.json"
db_metadata_json_2_rel_path = "db_metadata/nesmdb/nesmdb_updated2808_BACKUP.pkl"

metadata_full_path_pkl = os.path.join(current_dir, db_metadata_pkl_1_rel_path)
metadata_full_path_json = os.path.join(current_dir, db_metadata_json_1_rel_path)


def save_metadata(metadata):
    file = open(metadata_full_path_pkl, 'wb')
    pickle.dump(metadata, file)
    file.close()

    y = json.dumps(metadata, indent=4)
    file_json = open(metadata_full_path_json, 'w')
    file_json.write(y)
    file_json.close()


def nesmdb_encode(transposition, transposition_plus, instruments, vae, db_proc, dir_to_save):

    output_folder = "nesmdb_encoded"
    all_encodings_dir = os.path.join(dir_to_save, output_folder)
    #
    if not os.path.exists(all_encodings_dir):
        os.mkdir(all_encodings_dir)

    metadata = pickle.load(open(metadata_full_path_pkl, "rb"))

    valid_songs_counter = 0
    valid_sequences_counter = 0
    all_songs_counter = 0
    transposition_sign = "+" if transposition_plus else "-"

    for game in metadata.keys():

        # create a directory for current_songs encoded tensors

        current_game_dir = os.path.join(dir_to_save, output_folder, game)
        if not os.path.exists(current_game_dir):
            os.mkdir(current_game_dir)

        # iterate through all songs in game
        songs = metadata[game]["songs"]
        for song in songs:

            all_songs_counter += 1

            is_encodable = song["is_encodable"]
            encoded_song_urls = song["encoded_song_urls"]
            is_looping = song["is_looping"]
            song_rel_path = song["url"]
            song_full_path = os.path.join(current_dir, song_rel_path)

            if not is_encodable:
                continue

            instruments_str = "-".join(instruments)
            encoded_song_file_name = str(song["number"] - 1) + "*" + transposition_sign + str(
                transposition) + "*" + instruments_str + ".pkl"
            encoded_song_rel_path = os.path.join(output_folder, game, encoded_song_file_name)
            encoded_song_abs_path = os.path.join(dir_to_save, encoded_song_rel_path)

            time_start = time.time()
            # preveri če željena transpozicija/ kombinacija trackov že obstaja, če ja skipaš,
            # sicer jo narediš + dodaš url v seznam urljev
            if os.path.isfile(encoded_song_abs_path):

                if encoded_song_rel_path in encoded_song_urls:
                    # encoding already exists
                    continue
                else:
                    # encoding exists on disk but not in metadata
                    encoded_song_urls.append(encoded_song_rel_path)
                    song["encoded_song_urls"] = encoded_song_urls
                    save_metadata(metadata)

            else:

                song_data = db_proc.song_from_midi_nesmdb(song_full_path, transposition, transposition_plus)

                instruments_order_indices = [instruments_dict[ins] for ins in instruments]
                song_data = song_data[instruments_order_indices, :]

                song_measures = song_data.shape[1]//16

                song_data_extended = []

                if song_measures < song_min_measures:
                    if is_looping:
                        for i in range(song_min_measures*16):
                            song_data_extended.append(song_data[:, i % (song_measures*16)])
                        song_data_extended = np.vstack(song_data_extended).T
                else:
                    if is_looping:
                        new_length_measures = ((song_measures//song_min_measures + 1)*song_min_measures)
                        for i in range(new_length_measures*16):
                            song_data_extended.append(song_data[:, i % (song_measures*16)])
                        song_data_extended = np.vstack(song_data_extended).T
                    else:
                        new_length_measures = ((song_measures // song_min_measures) * song_min_measures)
                        song_data_extended = song_data[:, :(new_length_measures*16)]

                new_measures = song_data_extended.shape[1]//(song_min_measures*16)
                song_data_extended_split = np.split(song_data_extended, new_measures, axis=1)

                zs = []
                for song_data_ext in song_data_extended_split:
                    z = vae.encode_sequence(song_data_ext)
                    zs.append(z)
                reshaped_z = np.array(zs)

                valid_sequences_counter += reshaped_z.shape[0]
                file = open(encoded_song_abs_path, 'wb')
                pickle.dump(reshaped_z, file)
                file.close()

                # temperature = 0.0002
                # total_steps = 32
                # total_acc = 0
                # for i in range(new_measures):
                #     song_data_ = vae.decode_sequence(reshaped_z[i], total_steps, temperature)
                #     compare_encoded_decoded = (song_data_ == song_data_extended_split[i])
                #     true_count = np.count_nonzero(compare_encoded_decoded)
                #     total_elements = compare_encoded_decoded.size
                #     total_acc += true_count / total_elements
                # print("accuracy-", (total_acc/new_measures))

                encoded_song_urls.append(encoded_song_rel_path)
                song["encoded_song_urls"] = encoded_song_urls
                song["num_sequences"] = reshaped_z.shape[0]
                save_metadata(metadata)


            time_end = time.time()
            time_diff = time_end - time_start
            print(valid_songs_counter, game, str(song["number"] - 1), "time-" + str(time_diff), encoded_song_abs_path)
            valid_songs_counter += 1

    instruments_str = "-".join(instruments)
    run_info_file_name = transposition_sign + str(transposition) + "*" + instruments_str + "*run.txt"
    print(f"valid_songs-{valid_songs_counter}")
    print(f"valid_sequences-{valid_sequences_counter}")
    print(f"all_songs-{all_songs_counter}")

    file_info_abs_dir = os.path.join(current_dir, run_info_dir, run_info_file_name)

    file_info_dir = os.path.join(current_dir, run_info_dir)
    if not os.path.exists(file_info_dir):
        os.mkdir(file_info_dir)

    file = open(file_info_abs_dir, 'w')
    file.write("successful songs: " + str(valid_songs_counter) + "\n")
    file.write("successful sequences: " + str(valid_sequences_counter) + "\n")
    file.write("all songs: " + str(all_songs_counter) + "\n")
    file.close()
    return metadata, valid_sequences_counter


dir_to_save = "/storage/local/ssd/zigakleine-workspace"
# dir_to_save = os.getcwd()

desired_instruments_permutations = [["p1", "p2", "tr", "no"], ["p2", "p1", "tr", "no"]]
transpositions = ((False, 5), (False, 4), (False, 3), (False, 2), (False, 1), (True, 0), (True, 1), (True, 2), (True, 3), (True, 4), (True, 5), (True, 6))

check_gpus()

model_rel_path = "cat-mel_2bar_big.tar"
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_single_lib.so"

batch_size = 64

model_path = os.path.join(current_dir, model_rel_path)
nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_type = "nesmdb_singletrack"

db_proc = db_processing(nesmdb_shared_library_path, db_type)
vae = singletrack_vae(model_path, batch_size)

# slices_rel_path = "fb256_slices_76.pkl"
# slices_abs_path = os.path.join(current_dir, slices_rel_path)
# fb256_slices = pickle.load(open(slices_abs_path, "rb"))
# fb256_slices = np.array(fb256_slices)
#
# fb256_mask = np.zeros((512,), dtype=bool)
# fb256_mask[fb256_slices] = True
all_valid_sequences_num = 0

for desired_instruments in desired_instruments_permutations:
    for transposition_plus, transposition in transpositions:

        metadata, valid_sequences_num = nesmdb_encode(transposition, transposition_plus, desired_instruments, vae, db_proc, dir_to_save)
        all_valid_sequences_num += valid_sequences_num
        save_metadata(metadata)

print("all_valid_sequences_num", all_valid_sequences_num)
