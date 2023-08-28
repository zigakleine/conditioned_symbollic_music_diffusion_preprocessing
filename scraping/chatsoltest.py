# import re
#
# def remove_content_inside_parentheses(input_string):
#     # Define a regular expression pattern to match content inside parentheses
#     pattern = r"\([^)]*\)"
#
#     # Use re.sub() to replace matches with an empty string
#     result_string = re.sub(pattern, "", input_string)
#
#     return result_string
#
# # Test examples
# input_string1 = "Capcom (AC)"
# input_string2 = "Namco (AC / FC)"
#
# result1 = remove_content_inside_parentheses(input_string1)
# result2 = remove_content_inside_parentheses(input_string2)
#
# print(result1)  # Output: "Capcom "
# print(result2)  # Output: "Namco "
import pickle

metadata = pickle.load(open("../nesmdb_updated.pkl", "rb"))
base_url = "https://vgmrips.net/packs/pack/"

for game in metadata.keys():
    print(game)
    if game == "246_Mother":
        gamedata = metadata[game]
        print("heheh")