
import os
import torch
from torch.utils.data import Dataset
from torch.utils.data.dataset import T
import pickle
import numpy as np
import math
import json

metadata_folder = "db_metadata"
database_folder = "nesmdb"

current_dir = os.getcwd()
encoded_dir = "/storage/local/ssd/zigakleine-workspace/"
# encoded_dir = os.getcwd()


metadata_filename = "nesmdb_updated2808.pkl"
nesmdb_metadata_abs_path = os.path.join(current_dir, metadata_folder, database_folder,
                                                  metadata_filename)

metadata = pickle.load(open(nesmdb_metadata_abs_path, "rb"))


all_songs = 0
encodable_songs = 0
sequences = 0

avg_means_b = []
avg_std_devs_b = []

avg_means_a = []
avg_std_devs_a = []

data_min = -14.
data_max = 14.

for game in metadata:

    for song in metadata[game]["songs"]:

        if song["is_encodable"]:
            encodable_songs += 1
            song_rel_urls = song["encoded_song_urls"]

            for song_rel_url in song_rel_urls:
                song_abs_url = os.path.join(encoded_dir, song_rel_url)
                song_encoded = pickle.load(open(song_abs_url, "rb"))

                for seq_idx in song_encoded.shape[0]:
                    seq = song_encoded[seq_idx]
                    sequences += 1
                    mean = np.mean(seq)
                    std_dev = np.std(seq)

                    seq_ = (seq - data_min) / (data_max - data_min)
                    seq_ = 2. * seq_ - 1.

                    mean_ = np.mean(seq_)
                    std_dev_ = np.std(seq_)

                    avg_means_b.append(mean)
                    avg_means_a.append(mean_)

                    avg_std_devs_b.append(std_dev)
                    avg_std_devs_a.append(std_dev_)

                    print(f"mean_b-{mean} mean_a-{mean_} stddev-b-{std_dev} stddev_a-{std_dev_}")


print("mean-before", sum(avg_means_b)/len(avg_means_b))
print("mean-after", sum(avg_means_a)/len(avg_means_a))

print("std-dev-before", sum(avg_std_devs_b)/len(avg_std_devs_b))
print("std-dev-after", sum(avg_std_devs_a)/len(avg_std_devs_a))

