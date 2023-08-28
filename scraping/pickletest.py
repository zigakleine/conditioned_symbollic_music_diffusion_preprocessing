import soundfile as sf
import pickle
import numpy as np
import os
import json
import re


metadata = pickle.load(open("../nesmdb_updated.pkl", "rb"))
# print(metadata)

# f = open('./urls.txt', 'w')

# for game in metadata.keys():
#
#     print("")
#     # f.write(metadata[game]["download_url"] + "\n")
#     print(str(os.path.isdir("./nesmdb_metadata/" + os.path.basename(metadata[game]["download_url"])[:-4])) + ": " + metadata[game]["download_url"])
#
# # f.close()


# tega nismo !!! http://vgmrips.net/files/NES/Mother_(NES).zip

# print(metadata)

updated_game_metadata = dict()

# del metadata["246_Mother"]
pattern = r"\([^)]*\)"

for game in metadata.keys():
    print(game)

developers = set()
for game in metadata.keys():
    # print(game)
    game_dir = "./nesmdb_metadata/" + os.path.basename(metadata[game]["download_url"])[:-4] + "/"
    # print(game_dir)
    for file in os.listdir(game_dir):
        if file.endswith(".txt"):
            txt_file_path = os.path.join(game_dir, file)
            # print(txt_file_path)
            # print("\n")
            f = open(txt_file_path, 'r', encoding='utf-8')
            for line in f.readlines():
                # print(line)
                if line.startswith("Game developer:"):
                    # print(line[21:])
                    developer = line[21:]

                    developer = re.sub(pattern, "", developer)

                    developer = developer.strip()
                    if developer[-1] ==  ';' or developer[-1] == ',' or developer[-1] == '/':
                        developer = developer[:-1]
                        developer = developer.strip()

                    if len(developer.split(",")) > 1:
                        developer = developer.split(",")[0]
                        developer = developer.strip()

                    if len(developer.split(";")) > 1:
                        developer = developer.split(";")[0]
                        developer = developer.strip()

                    developers.add(developer)

            f.close()

print("\n\n")
for dev in developers:
    print(dev)

print(len(developers))