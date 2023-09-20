import numpy as np
import pickle

nesmdb_stacked = pickle.load(open("./nesmdb_single_stacked.pkl", "rb"))

z_stacked_p1 = []
z_stacked_p2 = []
z_stacked_tr = []
z_stacked_no = []

for sequence in nesmdb_stacked:
    track_subsequence = np.split(sequence, 4, axis=0)
    z_stacked_p1.append(track_subsequence[0])
    z_stacked_p2.append(track_subsequence[1])
    z_stacked_tr.append(track_subsequence[2])
    z_stacked_no.append(track_subsequence[3])

z_stacked_all_tracks = [z_stacked_p1, z_stacked_p2, z_stacked_tr, z_stacked_no]

all_tracks_std_devs_indices = []

for z_stacked_track in z_stacked_all_tracks:

    z_stacked_track = np.vstack(z_stacked_track)

    track_std_devs = np.std(z_stacked_track, axis=0)

    track_std_devs_indices = [(i, dev) for i, dev in enumerate(track_std_devs)]
    track_std_devs_indices = sorted(track_std_devs_indices, key=(lambda x: x[1]))
    all_tracks_std_devs_indices.append(track_std_devs_indices)

    # nesmdb_p1_std_devs_indices_less = nesmdb_p1_std_devs_indices[:76]
    # nesmdb_p1_std_devs_indices_less_ixs = [i for i, dev in nesmdb_p1_std_devs_indices_less]
    # nesmdb_p1_std_devs_indices_less_ixs = sorted(nesmdb_p1_std_devs_indices_less_ixs)

file = open('./std_devs_singletrack.pkl', 'wb')
pickle.dump(all_tracks_std_devs_indices, file)
file.close()

# file = pickle.load(open('./fb256_slices_76.pkl', 'rb'))
# print(len(file))