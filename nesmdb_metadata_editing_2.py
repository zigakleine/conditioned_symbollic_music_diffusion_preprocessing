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

game_systems = set()
genres = dict()
composers = dict()
release_years = dict()
metadata = pickle.load(open("./db_metadata/nesmdb/nesmdb_updated2808.pkl", "rb"))
no_genre_count = 0
all_count = 0

if "211_M82GameSelectableWorkingProductDisplay" in metadata:
    del metadata["211_M82GameSelectableWorkingProductDisplay"]

for game in metadata.keys():

    # print(game)

    game_name = os.path.basename(metadata[game]["download_url"])[:-4]

    system_name = extract_content_inside_parentheses(game_name)
    system_name_words = 0
    if not len(system_name) == 0:
        #print(system_name)
        game_systems.add(system_name[0])
        sys_name = system_name[0]
        system_name_words = len(system_name[0].split("_"))
    else:
        system_name_words = 0
        sys_name = ""

    game_name_for_search = metadata[game]["game_name_split"].split("-")[:(-1*system_name_words)]
    # print("---", " ".join(game_name_for_search))
    #
    # print("genre: ", metadata[game]["genre"])
    # print("sysname: ", system_name)

    # if metadata[game]["genre"] == "Role-Playing":
    #     metadata[game]["genre"] = "Role-playing"

    #print('Enter genre for:', " ".join(game_name_for_search), metadata[game]["genre"])
    # x = input()
    # x = x.strip()
    # if not(x == ""):
    #    metadata[game]["genre"] = x

    for i, composer in enumerate(metadata[game]["composers"].copy()):

        if (composer == "Hal") or (composer == "Wood"):
            metadata[game]["composers"].remove(composer)

        if (composer == "Various"):
            metadata[game]["composers"][i] = "Unknown"
        #
        # if (composer == "TM Network"):
        #     metadata[game]["composers"][i] = "Unknown"
        #
        # if (composer == "Pinch Punch"):
        #     metadata[game]["composers"][i] = "Unknown"
        #
        # if (composer == "Peru"):
        #     metadata[game]["composers"][i] = "Unknown"

        if (composer == "Dave Wise"):
            metadata[game]["composers"][i] = "David Wise"
        #
        # if (composer == "Shotaro"):
        #     metadata[game]["composers"][i] = "Unknown"
        #
        # if (composer == "Konami Kukeiha Club"):
        #     metadata[game]["composers"][i] = "Unknown"
        #
        # if (composer == "Sato"):
        #     metadata[game]["composers"][i] = "Unknown"

        if not (composer == "Unknown"):
            if composer in composers:
                composers[composer] += 1
            else:
                composers[composer] = 1

    if len(metadata[game]["composers"]) > 1:
        for i, composer in enumerate(metadata[game]["composers"].copy()):
            if (composer == "Unknown"):
                metadata[game]["composers"].remove(composer)

    if len(metadata[game]["composers"]) == 0:
        metadata[game]["composers"] = ["Unknown"]

    if metadata[game]["genre"] in genres:
        genres[metadata[game]["genre"]] += 1
    else:
        genres[metadata[game]["genre"]] = 1

    if metadata[game]["release_year"] in release_years:
        release_years[metadata[game]["release_year"]] += 1
    else:
        release_years[metadata[game]["release_year"]] = 1

    all_count += 1
    print("\n")


print(genres)
print(len(genres))

print(composers)
print(len(composers))

print(release_years)
print(len(release_years))

all_categories = dict()

genre_nums = dict()
composer_nums = dict()
release_years_nums = dict()


for i, genre in enumerate(genres):
    genre_nums[genre] = i

for i, composer in enumerate(composers):
    composer_nums[composer] = i

for i, release_year in enumerate(release_years):
    release_years_nums[release_year] = i


print(genre_nums)
print(composer_nums)
print(release_years_nums)

all_categories["genres"] = genre_nums
all_categories["composers"] = composer_nums
all_categories["release_years"] = release_years


file = open('./db_metadata/nesmdb/nesmdb_categories.pkl', 'wb')
pickle.dump(all_categories, file)
file.close()

y = json.dumps(all_categories, indent=4)
file_json = open('db_metadata/nesmdb/nesmdb_categories.json', 'w')
file_json.write(y)
file_json.close()


file = open('./db_metadata/nesmdb/nesmdb_updated2808.pkl', 'wb')
pickle.dump(metadata, file)
file.close()

y = json.dumps(metadata, indent=4)
file_json = open('db_metadata/nesmdb/nesmdb_meta_json2808.json', 'w')
file_json.write(y)
file_json.close()