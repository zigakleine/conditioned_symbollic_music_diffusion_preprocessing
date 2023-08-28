import pickle
import os
from selenium import webdriver
import json
import re
import time


game_systems_translate = dict()

game_systems_translate["NES"] = "nintendo_nes"
game_systems_translate["NES,_Hyundai_Comboy"] = "nintendo_nes"
game_systems_translate["NES,_Tengen"] = "nintendo_nes"
game_systems_translate["Dendy"] = "nintendo_nes"

game_systems_translate["Arcade"] = "nintendo_arcade_systems"
game_systems_translate["VS_System"] = "nintendo_arcade_systems"
game_systems_translate["Nintendo_Vs._Unisystem"] = "nintendo_arcade_systems"
game_systems_translate["Vs._Unisystem"] = "nintendo_arcade_systems"
game_systems_translate["Nintendo_VS._System"] = "nintendo_arcade_systems"


game_systems_translate["Family_Computer_Disk_System"] = "nintendo_famicom_disk_system"
game_systems_translate["FDS"] = "nintendo_famicom_disk_system"
game_systems_translate["Nintendo_Famicom_Disk_System"] = "nintendo_famicom_disk_system"
game_systems_translate["Famicom_Disk_System"] = "nintendo_famicom_disk_system"
game_systems_translate["Family_Computer"] = "nintendo_famicom_disk_system"

def extract_content_inside_parentheses(input_string):
    # Define a regular expression pattern to match content inside parentheses
    pattern = r"\((.*?)\)"

    # Use re.findall() to extract all matches as a list of strings
    matches = re.findall(pattern, input_string)

    return matches
#
# game_systems = set()
# genres = dict()
# metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))
# no_genre_count = 0
# all_count = 0
#
# if "211_M82GameSelectableWorkingProductDisplay" in metadata:
#     del metadata["211_M82GameSelectableWorkingProductDisplay"]
#
# for game in metadata.keys():
#
#     # print(game)
#
#     game_name = os.path.basename(metadata[game]["download_url"])[:-4]
#
#     system_name = extract_content_inside_parentheses(game_name)
#     system_name_words = 0
#     if not len(system_name) == 0:
#         #print(system_name)
#         game_systems.add(system_name[0])
#         sys_name = system_name[0]
#         system_name_words = len(system_name[0].split("_"))
#     else:
#         system_name_words = 0
#         sys_name = ""
#
#     game_name_for_search = metadata[game]["game_name_split"].split("-")[:(-1*system_name_words)]
#     # print("---", " ".join(game_name_for_search))
#     #
#     # print("genre: ", metadata[game]["genre"])
#     # print("sysname: ", system_name)
#
#     if metadata[game]["genre"] == "Role-Playing":
#         metadata[game]["genre"] = "Role-playing"
#
#     #print('Enter genre for:', " ".join(game_name_for_search), metadata[game]["genre"])
#     # x = input()
#     # x = x.strip()
#     # if not(x == ""):
#     #    metadata[game]["genre"] = x
#
#     if metadata[game]["genre"] in genres:
#         genres[metadata[game]["genre"]] +=1
#     else:
#         genres[metadata[game]["genre"]] = 1
#
#     all_count += 1
#     print("\n")
#

# metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))
# file = open('./db_metadata/nesmdb/nesmdb_updated2808.pkl', 'wb')

# # dump information to that file
# pickle.dump(metadata, file)
#
# # close the file
# file.close()
#
# y = json.dumps(metadata, indent=4)
# # print(y)
#
# file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808.json', 'w')
# file_json.write(y)
# file_json.close()
#
# print(no_genre_count, "/", all_count)
# print(game_systems)
# print(genres)
# print(len(genres))

