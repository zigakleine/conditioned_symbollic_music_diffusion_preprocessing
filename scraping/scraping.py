import pickle
import os
from selenium import webdriver
import json
import re
import time

options = webdriver.ChromeOptions()
options.binary_location = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
chrome_driver_binary = "/Users/zigakleine/Downloads/chromedriver-mac-x64/chromedriver"
driver = webdriver.Chrome(chrome_driver_binary, chrome_options=options)

def remove_content_inside_parentheses(input_string):
    # Define a regular expression pattern to match content inside parentheses
    pattern = r"\([^)]*\)"

    # Use re.sub() to replace matches with an empty string
    result_string = re.sub(pattern, "", input_string)

    result_string = result_string.strip()

    return result_string

def split_on_char(gamename_split, ch):
    appended_num = 0
    for (i, gs) in enumerate(gamename_split.copy()):
        append_index = i + appended_num
        if ch in gs:
            new_split = gs.split(ch)
            gamename_split.remove(gs)
            for ns in new_split:
                gamename_split.insert(append_index, ns)
                appended_num += 1
                append_index += 1
            appended_num -= 1
    return gamename_split

metadata = pickle.load(open("../nesmdb_meta.pkl", "rb"))
base_url = "https://vgmrips.net/packs/pack/"


del metadata["143_Gimmick_"]
del metadata["019_Ast_rix"]
del metadata["102_FamicomDiskSystemBIOS"]
del metadata["200_LagrangePoint"]
del metadata["349_TheLegendofZelda2_LinknoBouken"]
del metadata["352_TheSmurfs"]
del metadata["112_FantasyZone"]
del metadata["074_DonkeyKong3"]
del metadata["337_TerraCresta"]


metadata["246_Mother"]["download_url"] = "https://vgmrips.net/files/NES/Mother_(Family_Computer).zip"


for game in metadata.keys():

    print(game)
    gamename = os.path.basename(metadata[game]["download_url"])[:-4]
    gamename_split = gamename.split("_")
    print(gamename_split)

    split_on_char(gamename_split, " ")
    split_on_char(gamename_split, "'")
    split_on_char(gamename_split, "-")
    split_on_char(gamename_split, ".")


    for i in range(len(gamename_split)):
        gamename_split[i] = gamename_split[i].replace("(", "")
        gamename_split[i] = gamename_split[i].replace(")", "")
        gamename_split[i] = gamename_split[i].replace("-", "")
        gamename_split[i] = gamename_split[i].replace("!", "")
        gamename_split[i] = gamename_split[i].replace(".", "")
        gamename_split[i] = gamename_split[i].replace("&", "")
        gamename_split[i] = gamename_split[i].replace("°", "")
        gamename_split[i] = gamename_split[i].replace(",", "")
        gamename_split[i] = gamename_split[i].lower()


    for gs in gamename_split.copy():
        if gs == "":
            gamename_split.remove(gs)



    print(gamename_split)
    game_url = "-".join(gamename_split)
    url = base_url + game_url
    print(url)
    metadata[game]["game_name_split"] = game_url
    metadata[game]["web_url"] = url

    game_num = game.split("_")[0]
    print(game_num)
    # search the game dir(s) and collect all songs that start with num
    songs = []
    game_dir = "../nesmdb_flat"
    game_dir_abs = os.path.join(os.getcwd(), game_dir)
    for file in os.listdir(game_dir_abs):
        if file.startswith(game_num):
            file_split = file.split("_")
            txt_file_path = os.path.join(game_dir_abs, file)
            print(txt_file_path)

            song = dict()
            song["number"] = int(file_split[-2]) + 1
            song["url"] = txt_file_path
            songs.append(song)

    songs = sorted(songs, key=lambda x: x['number'])
    metadata[game]["songs"] = songs

    driver.get(metadata[game]["web_url"])
    time.sleep(1)


    before_playlist = driver.find_element_by_id("beforePlaylist")
    composers = before_playlist.find_element_by_class_name("composers")
    companies = before_playlist.find_element_by_class_name("companies")

    composers_found = composers.find_elements_by_tag_name("a")
    composers_text = [remove_content_inside_parentheses(composer_found.text) for composer_found in composers_found]

    companies_children = companies.find_elements_by_xpath("./*")
    company_type = None
    publishers = []
    developers = []
    release_date = companies.text.split("\n")[-1][13:]
    for child in companies_children:
        # print(child.text)
        if child.text == "Developer:" or child.text == "Developers:":
            company_type = "dev"
            continue
        if child.text == "Publisher:" or child.text == "Publishers:":
            company_type = "pub"
            continue

        if child.tag_name == "span" and company_type is not None:
            if company_type == "dev":
                developers.append(remove_content_inside_parentheses(child.text))
            elif company_type == "pub":
                publishers.append(remove_content_inside_parentheses(child.text))

    release_date = release_date.replace("\"", "")
    release_date = release_date.strip()

    release_date_full = release_date

    release_date_split = release_date.split("/")
    release_date = release_date_split[0]
    release_date = release_date.strip()

    release_date = remove_content_inside_parentheses(release_date)
    release_date = release_date.strip()

    release_year = release_date.split("-")[0]

    metadata[game]["publishers"] = publishers
    metadata[game]["developers"] = developers
    metadata[game]["release_year"] = release_year
    metadata[game]["release_date_full"] = release_date_full
    metadata[game]["composers_2"] = composers_text



    song_table = driver.find_element_by_xpath("//*[@id='details']/table/tbody")
    song_table_children = song_table.find_elements_by_xpath("./tr")
    for (i, song_) in enumerate(song_table_children):
        if not (i + 1 == len(song_table_children)):
            print(song_.text)
            song_num = song_.find_element_by_xpath("./td[2]/small").text
            song_num = song_num.replace(".", "")

            song_name = song_.find_element_by_xpath("./td[3]/a").text
            song_duration = song_.find_element_by_xpath("./td[3]/span").text

            is_looping = False
            if "+" in song_duration:
                is_looping = True

            song_duration_split = song_duration.split("+")
            song_duration_seconds = 0

            song_duration_normal = song_duration_split[0]
            song_duration_normal = song_duration_normal.strip()
            normal_split = song_duration_normal.split(":")
            song_duration_normal_seconds = (int(normal_split[0]) * 60 + int(normal_split[1]))

            song_duration_looping_seconds = 0
            if is_looping:
                song_duration_looping = song_duration_split[1]
                song_duration_looping = song_duration_looping.strip()
                looping_split = song_duration_looping.split(":")
                song_duration_looping_seconds = (int(looping_split[0])*60 + int(looping_split[1]))

            song_duration_seconds = song_duration_normal_seconds + song_duration_looping_seconds

            if i >= len(metadata[game]["songs"]):
                print("!!!!! too many songs at ", song_num, " ", song_name)
                continue

            metadata[game]["songs"][i]["song_name"] = song_name
            metadata[game]["songs"][i]["song_duration"] = song_duration
            metadata[game]["songs"][i]["song_num"] = song_num

            metadata[game]["songs"][i]["song_duration_seconds"] = song_duration_seconds
            metadata[game]["songs"][i]["song_duration_normal_seconds"] = song_duration_normal_seconds
            metadata[game]["songs"][i]["song_duration_looping_seconds"] = song_duration_looping_seconds

            metadata[game]["songs"][i]["is_looping"] = is_looping

    time.sleep(1)

driver.quit()

file = open('../nesmdb_updated.pkl', 'wb')

# dump information to that file
pickle.dump(metadata, file)

# close the file
file.close()

y = json.dumps(metadata, indent=4)
print(y)

file_json = open('../nesmdb__meta_json.json', 'w')
file_json.write(y)
file_json.close()


