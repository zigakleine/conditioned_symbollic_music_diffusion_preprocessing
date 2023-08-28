import pickle
import os
from selenium import webdriver
import json
import re
import time

from selenium.webdriver.common.keys import Keys
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

from selenium.common.exceptions import NoSuchElementException
def check_exists_by_xpath(xpath):
    try:
        driver.find_element_by_xpath(xpath)
    except NoSuchElementException:
        return False
    return True

def extract_content_inside_parentheses(input_string):
    # Define a regular expression pattern to match content inside parentheses
    pattern = r"\((.*?)\)"

    # Use re.findall() to extract all matches as a list of strings
    matches = re.findall(pattern, input_string)

    return matches



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


options = webdriver.ChromeOptions()
options.binary_location = "/Applications/Google Chrome.app/Contents/MacOS/Google Chrome"
chrome_driver_binary = "/Users/zigakleine/Downloads/chromedriver-mac-x64/chromedriver"
driver = webdriver.Chrome(chrome_driver_binary, chrome_options=options)
base_url = "https://www.gamesdatabase.org/"
search_url_1 = "https://www.gamesdatabase.org/list.aspx?DM=0&searchtext="
search_url_2 = "&searchtype=1&system="
search_url_3 = "&sort=Game"

metadata = pickle.load(open("../nesmdb_updated.pkl", "rb"))

game_systems = set()

for game in metadata.keys():
    print(game)

    game_name = os.path.basename(metadata[game]["download_url"])[:-4]

    system_name = extract_content_inside_parentheses(game_name)
    system_name_words = 0
    if not len(system_name) == 0:
        #print(system_name)
        game_systems.add(system_name[0])
        system_name_words = len(system_name[0].split("_"))
    else:
        metadata[game]["genre"] = "-"
        continue

    game_name_for_search = metadata[game]["game_name_split"].split("-")[:(-1*system_name_words)]
    print("---", " ".join(game_name_for_search))


    search_url = (search_url_1 + "%20".join(game_name_for_search) + search_url_2 + game_systems_translate[system_name[0]] + search_url_3)
    print(search_url)
    driver.get(search_url)
    time.sleep(2.5)

    # search_bar = driver.find_element_by_id("txtsearch")
    # search_button = driver.find_element_by_id("cmdSearch")
    # search_bar.send_keys(" ".join(game_name_for_search))
    # search_button.click()


    #check if we are on the search page

    if driver.title.strip() == "Game search results - Games Database":

        if check_exists_by_xpath("//*[@id='GridView1']/tbody"):
            results_table = driver.find_element_by_xpath("//*[@id='GridView1']/tbody")
        else:
            results_table = None

        if results_table is None:
            genre = "-"
        else:
            table_rows = results_table.find_elements_by_xpath("./tr")
            prev_genre = ""
            is_one_genre = True
            for (i, table_row) in enumerate(table_rows):

                if i > 0:
                    try:
                        genre = table_row.find_element_by_xpath("./td[9]/a").text
                    except NoSuchElementException:
                        genre = ""

                    if genre == "":
                        continue
                    else:
                        if prev_genre == "":
                            prev_genre = genre
                        else:
                            if not (prev_genre == genre):
                                is_one_genre = False
                    prev_genre = genre
                    print(i, table_row.text)

            if is_one_genre:
                genre = prev_genre
            else:
                genre = "-"
    else:
        if check_exists_by_xpath("//*[@id='Out']/table[1]/tbody/tr[4]/td[2]/a"):
            genre = driver.find_element_by_xpath("//*[@id='Out']/table[1]/tbody/tr[4]/td[2]/a").text
        else:
            genre = "-"

    print(genre)
    metadata[game]["genre"] = genre

print("\n\n")
print(game_systems)

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
