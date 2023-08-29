import os
import pickle
import json

# assign directory
current_directory = os.getcwd()

lakh_folder = "lmd_full"
subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
index = 0
subdirectory = subdirectories[index]

lakh_subdir_path = os.path.join(current_directory, lakh_folder, subdirectory)

metadata = dict()
# metadata = pickle.load(open("./db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl", 'rb'))

# del metadata["0302754150beb50b30cb2862d212fa7a"]

for filename in os.listdir(lakh_subdir_path):
    song_path = os.path.join(lakh_subdir_path, filename)
    if os.path.isfile(song_path) and filename.split(".")[1] == "mid":

        song = dict()
        song_name = filename.split(".")[0]
        song_path_split = song_path.split("/")
        song_rel_path = "/".join(song_path_split[5:])
        song["url"] = song_rel_path
        song["encoded_song_path"] = ""
        song["encodable"] = True

        metadata[song_name] = song


file = open("./db_metadata/lakh/lakh_2908_" + subdirectory + ".pkl", 'wb')
pickle.dump(metadata, file)
file.close()

y = json.dumps(metadata, indent=4)
file_json = open("db_metadata/lakh/lakh_json_2908_" + subdirectory + ".json", 'w')
file_json.write(y)
file_json.close()
