import pickle
import os
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
           metadata[song]["encoded_song_path"] = ""


    file = open(current_lakh_metadata_abs_path, 'wb')
    pickle.dump(metadata, file)
    file.close()

    y = json.dumps(metadata, indent=4)
    file_json = open(current_lakh_json_abs_path, 'w')
    file_json.write(y)
    file_json.close()