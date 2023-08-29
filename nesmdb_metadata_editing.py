import pickle
import json
from multitrack_VAE import db_processing
import os

metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))

nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_lib.so"
batch_size = 32
current_dir = os.getcwd()

nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)
db_type = "nesmdb"

db_proc = db_processing(nesmdb_shared_library_path, db_type)

song_min_measures = 32

encodable_songs = 0
all_songs = 0

for game in metadata.keys():

    for song in metadata[game]["songs"]:

        song_path = song["url"]
        # song_path_split = song_path.split("/")
        # song_path_split_rel = song_path_split[5:]
        # song["encoded_song_urls"] = []
        # song["url"] = "/".join(song_path_split_rel)
        abs_song_path = os.path.join(current_dir, song_path)

        song_data = db_proc.song_from_midi_nesmdb(abs_song_path)
        song_measures = len(song_data)
        if "is_looping" in song.keys():
            is_looping = song["is_looping"]
        else:
            is_looping = False
            song["is_looping"] = is_looping

        is_encodable = True

        if song_measures < song_min_measures:
            if not is_looping:
                is_encodable = False
        song["is_encodable"] = is_encodable

        all_songs += 1
        if song["is_encodable"]:
            encodable_songs += 1



print("encodable_songs-", encodable_songs, "all_songs-", all_songs)


file = open('./db_metadata/nesmdb/nesmdb_updated2808.pkl', 'wb')
pickle.dump(metadata, file)
file.close()

y = json.dumps(metadata, indent=4)
file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808.json', 'w')
file_json.write(y)
file_json.close()



#
#
# metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))
#
# file = open('./db_metadata/nesmdb/nesmdb_updated2808_BACKUP.pkl', 'wb')
# pickle.dump(metadata, file)
# file.close()
#
# y = json.dumps(metadata, indent=4)
# file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808_BACKUP.json', 'w')
# file_json.write(y)
# file_json.close()