import os
import torch
from torch.utils.data import Dataset
from torch.utils.data.dataset import T
import pickle

subdirectories = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f"]
metadata_folder = "db_metadata"
database_folder = "lakh"
#
# class LakhMidiDataset(Dataset):
#
#     def __init__(self):
#
#         current_dir = os.getcwd()
#         all_lakh_metadata = []
#
#         for subdir_name in subdirectories:
#
#             current_metadata_filename = "lakh_2908_" + subdir_name + ".pkl"
#             current_lakh_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder, current_metadata_filename)
#
#             metadata = pickle.load(open(current_lakh_metadata_abs_path, "rb"))
#             all_lakh_metadata.append(metadata)
#     def __getitem__(self, index):
#         pass

current_dir = os.getcwd()
all_lakh_metadata = []

for subdir_name in subdirectories:
    current_metadata_filename = "lakh_2908_" + subdir_name + ".pkl"
    current_lakh_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  current_metadata_filename)

    metadata = pickle.load(open(current_lakh_metadata_abs_path, "rb"))



print(all_lakh_metadata)