import pickle
import os

import json
import re
import time
from multitrack_VAE import db_processing, multitrack_vae

song_min_measures = 32


def save_metadata(db_metadata_file_name, metadata):
    file = open(db_metadata_file_name, 'wb')

    # dump information to that file
    pickle.dump(metadata, file)

    # close the file
    file.close()

    y = json.dumps(metadata, indent=4)

    file_json = open('nesmdb__meta_json.json', 'w')
    file_json.write(y)
    file_json.close()

def nesmdb_encode(db_metadata_file_name, transposition, transposition_plus, instruments, vae, db_proc):

    times = []
    output_folder = "nesmdb_encoded"

    encoded_vectors_base_url = os.getcwd()

    all_encodings_dir = os.path.join(encoded_vectors_base_url, output_folder)
    if not os.path.exists(all_encodings_dir):
        os.mkdir(all_encodings_dir)

    metadata = pickle.load(open(db_metadata_file_name, "rb"))


    metadata["069_DigDug"]["songs"][8]["is_looping"] = False


    song_counter = 0
    transposition_sign = "+" if transposition_plus else "-"

    for game in metadata.keys():
        # print(game)

        # create a directory for current_songs encoded tensors
        current_game_dir = os.path.join(encoded_vectors_base_url, output_folder, game)
        if not os.path.exists(current_game_dir):
            os.mkdir(current_game_dir)

        # iterate through all songs in game
        songs = metadata[game]["songs"]
        for song in songs:

            time_start = time.time()


            instruments_str = "-".join(instruments)
            if "encoded_song_urls" in song.keys():
                encoded_song_urls = song["encoded_song_urls"]
            else:
                encoded_song_urls = []
                song["encoded_song_urls"] = encoded_song_urls

            encoded_song_file_name = str(song["number"] - 1) + "*" + transposition_sign + str(
                transposition) + "*" + instruments_str + ".pkl"

            encoded_song_url = os.path.join(encoded_vectors_base_url, output_folder, game, encoded_song_file_name)

            # preveri če željena transpozicija/ kombinacija trackov že obstaja, če ja skipaš,
            # sicer jo narediš + dodaš url v seznam urljev
            if os.path.isfile(encoded_song_url):

                if encoded_song_url in encoded_song_urls:
                    # encoding already exists
                    continue
                else:
                    # encoding exists on disk but not in metadata
                    encoded_song_urls.append(encoded_song_url)
                    song["encoded_song_urls"] = encoded_song_urls
                    save_metadata(db_metadata_file_name, metadata)

            else:

                # print("\t" + (song["song_name"] if "song_name" in song.keys() else "noname"))

                # if game == "378_WaiWaiWorld2_SOS__ParsleyJou" and song["number"] - 1 == 10:
                #     print("hehe")


                song_url = song["url"]
                is_looping = song["is_looping"]
                is_encodable = True

                song_data = db_proc.song_from_midi_nesmdb(song_url)
                song_measures = len(song_data)

                if song_measures < song_min_measures:
                    if is_looping:
                        measures_to_add = song_min_measures - song_measures
                        # for i in range(measures_to_add):
                        #     song_data.append(song_data[i%song_measures])
                    else:
                        is_encodable = False
                # else:
                #     if is_looping:
                #         measures_to_add = ((song_measures//song_min_measures + 1)*song_min_measures) - song_measures
                #         for i in range(measures_to_add):
                #             song_data.append(song_data[i % song_measures])
                #
                # if is_encodable:
                #     # z = vae.encode_sequence(song_data)
                song["is_encodable"] = is_encodable


            time_end = time.time()
            time_diff = time_end - time_start
            times.append(time_diff)
            print(game, str(song["number"] - 1), "time-" + str(time_diff), "avg-" + str(sum(times) / len(times)) )
            save_metadata(db_metadata_file_name, metadata)

    return metadata

if __name__ == "__main__":



    current_dir = os.getcwd()
    model_file_name = "model_fb256.ckpt"
    nesmdb_shared_library_name = "ext_nseq_lib.so"
    batch_size = 32

    model_path = os.path.join(current_dir, model_file_name)

    nesmdb_shared_library_path = "/Users/zigakleine/Desktop/scraping_and_cppmidi/ext_nseq_lib.so"
    db_type = "nesmdb"

    db_proc = db_processing(nesmdb_shared_library_path, db_type)
    vae = multitrack_vae(os.path.join(current_dir, model_file_name), batch_size)

    db_metadata_file_name = "nesmdb_updated.pkl"

    transposition = 0
    trasposition_plus = True
    desired_instruments = ["p1", "p2", "tr", "no"]

    metadata = nesmdb_encode(db_metadata_file_name, transposition, trasposition_plus, desired_instruments, vae, db_proc)

    save_metadata(current_dir, db_metadata_file_name, metadata)