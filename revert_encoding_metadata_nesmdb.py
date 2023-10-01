import pickle
import json

metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))

for game in metadata.keys():

    for song in metadata[game]["songs"]:
        song["encoded_song_urls"] = []
        if "num_sequences" in song:
            del song["num_sequences"]


file = open('./db_metadata/nesmdb/nesmdb_updated2808.pkl', 'wb')
pickle.dump(metadata, file)
file.close()

y = json.dumps(metadata, indent=4)
file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808.json', 'w')
file_json.write(y)
file_json.close()