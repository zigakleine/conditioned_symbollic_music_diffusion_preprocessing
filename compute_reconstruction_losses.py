import numpy as np
import os

import numpy as np
import pickle
import torch.nn.functional as F
import torch

from singletrack_VAE import check_gpus, db_processing, singletrack_vae

mario_file_path = "/Users/zigakleine/Desktop/conditioned_symbollic_music_diffusion_preprocessing/nesmdb_flat/322_SuperMarioBros__00_01RunningAbout.mid"

batch_size = 64
temperature = 0.0002
total_steps = 32

current_dir = os.getcwd()
model_rel_path = "cat-mel_2bar_big.tar"
model_path = os.path.join(current_dir, model_rel_path)
db_type = "nesmdb_singletrack"
nesmdb_shared_library_rel_path = "ext_nseq_nesmdb_single_lib.so"

transposition = 0
transposition_plus = True

model_path = os.path.join(current_dir, model_rel_path)
nesmdb_shared_library_path = os.path.join(current_dir, nesmdb_shared_library_rel_path)

db_proc = db_processing(nesmdb_shared_library_path, db_type)
vae = singletrack_vae(model_path, batch_size)
#
# db_metadata_pkl_rel_path = "db_metadata/nesmdb/nesmdb_updated2808.pkl"
# db_metadata_pkl_abs_path = os.path.join(current_dir, db_metadata_pkl_rel_path)
# metadata = pickle.load(open(db_metadata_pkl_abs_path, "rb"))
#
# songs = []
# for game in metadata:
#     for song in metadata[game]["songs"]:
#         if song["is_encodable"]:
#             song_url = song["url"]
#             song_url_abs = os.path.join(current_dir, song_url)
#             song = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)
#             if song.shape[1] >= 32*16:
#                 songs.append(song_url)
#
#
# valid_songs = len(songs)
# songs_to_sample = 100
#
# sample_songs_idxs = np.random.choice(valid_songs, songs_to_sample, replace=False).tolist()
#
#
# reconstruction_sample_songs = [songs[idx] for idx in sample_songs_idxs]
#
# file = open('./vae_reconstruction_songs.pkl', 'wb')
# pickle.dump(reconstruction_sample_songs, file)
# file.close()


num_songs = 10
reconstruction_sample_songs = pickle.load(open("./vae_reconstruction_songs.pkl", "rb"))
reconstruction_sample_songs = reconstruction_sample_songs[:num_songs]
num_latents = 512

std_devs_tracks = pickle.load(open("./std_devs_singletrack.pkl", "rb"))
std_devs_masks = []

for std_dev_track in std_devs_tracks:
    std_dev_track = std_dev_track[:num_latents]
    std_dev_idx_track = [i for i, dev in std_dev_track]
    std_dev_idx_track = np.array(std_dev_idx_track)

    std_dev_mask = np.zeros((512,), dtype=bool)
    std_dev_mask[std_dev_idx_track] = True

    std_devs_masks.append(std_dev_mask)


# temperatures = [0.0005, 0.0009, 0.001, 0.002, 0.003, 0.004, 0.005, 0.006, 0.007, 0.008, 0.009, 0.01, 0.015, 0.03, 0.09, 0.1, 0.5, 1.0, 1.5 ,2.0]
# temperatures = [0.00001, 0.00005, 0.0001, 0.0002, 0.0003, 0.0004, 0.0005]
temperatures =  [1.0, 2.0, 5.0, 10.0, 20.0, 30.0, 50.0]
# temperatures = [0.0002]
accuracies = []
losses = []

for temperature in temperatures:
    accuracy_sum = 0
    avg_accuracy = 0

    for i, reconstruction_sample_song in enumerate(reconstruction_sample_songs):

        song_url_abs = os.path.join(current_dir, reconstruction_sample_song)
        song_data = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)
        song_data = song_data[:, :32*16]

        z = vae.encode_sequence(song_data)
        z_tracks = np.split(z, 4, axis=0)
        z_tracks_reconstructed = []

        for z_track, std_devs_mask in zip(z_tracks, std_devs_masks):

            z_track_reduced = z_track[:, std_devs_mask]
            z_track_reconstructed = np.random.randn(*z_track_reduced.shape[:-1], 512)
            z_track_reconstructed[..., std_devs_mask] = z_track_reduced

            z_tracks_reconstructed.append(z_track_reconstructed)

        z_tracks_reconstructed = np.vstack(z_tracks_reconstructed)

        song_data_, song_tensors, logits = vae.decode_sequence_full_results(z_tracks_reconstructed, total_steps, temperature)

        song_data_flat = song_data.ravel()
        logits_vertical = logits.reshape((logits.shape[0]*logits.shape[1], logits.shape[-1]))

        loss = F.cross_entropy(torch.tensor(logits_vertical), torch.tensor(song_data_flat).long())

        cross_entropy_loss = loss.item()
        losses.append(cross_entropy_loss)

        compare_encoded_decoded = (song_data == song_data_)
        true_count = np.count_nonzero(compare_encoded_decoded)

        total_elements = compare_encoded_decoded.size

        accuracy = true_count / total_elements
        accuracy_sum += accuracy
        print(f"accuracy-{i}-{accuracy}_loss-{cross_entropy_loss}")

    avg_accuracy = accuracy_sum/num_songs
    print(f"avg_accuracy-{avg_accuracy}")
    accuracies.append(avg_accuracy)

print("finished")
for temp, acc, loss in zip(temperatures, accuracies, losses):
    print(f"temp-{temp} acc-{acc} loss{loss}")


# reconstructed_dir = os.path.join(current_dir, "reconstructed")
# if not os.path.exists(reconstructed_dir):
#     os.mkdir(reconstructed_dir)
#
#
# possible_num_latents = [512, 511, 510, 505, 502, 500, 450, 400, 300, 200, 100, 80, 60, 40, 30, 20]
# accuracies = []
# losses = []
#
# for num_latents in possible_num_latents:
#
#     std_devs_tracks = pickle.load(open("./std_devs_singletrack.pkl", "rb"))
#     std_devs_masks = []
#
#     for std_dev_track in std_devs_tracks:
#         std_dev_track = std_dev_track[:num_latents]
#         std_dev_idx_track = [i for i, dev in std_dev_track]
#         std_dev_idx_track = np.array(std_dev_idx_track)
#
#         std_dev_mask = np.zeros((512,), dtype=bool)
#         std_dev_mask[std_dev_idx_track] = True
#
#         std_devs_masks.append(std_dev_mask)
#
#     song_url_abs = os.path.join(current_dir, mario_file_path)
#     song_data = db_proc.song_from_midi_nesmdb(song_url_abs, 0, True)
#     song_data = song_data[:, :32 * 16]
#
#     z = vae.encode_sequence(song_data)
#     z_tracks = np.split(z, 4, axis=0)
#     z_tracks_reconstructed = []
#
#     for z_track, std_devs_mask in zip(z_tracks, std_devs_masks):
#         z_track_reduced = z_track[:, std_devs_mask]
#         z_track_reconstructed = np.random.randn(*z_track_reduced.shape[:-1], 512)
#         z_track_reconstructed[..., std_devs_mask] = z_track_reduced
#
#         z_tracks_reconstructed.append(z_track_reconstructed)
#
#     z_tracks_reconstructed = np.vstack(z_tracks_reconstructed)
#
#     song_data_, song_tensors, logits = vae.decode_sequence_full_results(z_tracks_reconstructed, total_steps, temperature)
#
#     song_data_flat = song_data.ravel()
#     logits_vertical = logits.reshape((logits.shape[0]*logits.shape[1], logits.shape[-1]))
#
#     loss = F.cross_entropy(torch.tensor(logits_vertical), torch.tensor(song_data_flat).long())
#
#     cross_entropy_loss = loss.item()
#
#     compare_encoded_decoded = (song_data == song_data_)
#     true_count = np.count_nonzero(compare_encoded_decoded)
#
#     total_elements = compare_encoded_decoded.size
#
#     accuracy = true_count / total_elements
#     accuracies.append(accuracy)
#     losses.append(cross_entropy_loss)
#     print(f"num_latents-{num_latents}, accuracy-{accuracy}, cross_entropy_loss-{cross_entropy_loss}")
#
#     midi = db_proc.midi_from_song(song_data_)
#     reconstructed_midi_filename = f"mario_rec_latents_{num_latents}.mid"
#     reconstructed_midi_abs_path = os.path.join(current_dir, reconstructed_dir,  reconstructed_midi_filename)
#     midi.save(reconstructed_midi_abs_path)
#
# import matplotlib.pyplot as plt
#
# plt.plot(possible_num_latents, accuracies, 'b', label='accuracy')
# plt.xlabel('num_latents')
# plt.ylabel('accuracy')
# plt.title('322_SuperMarioBros__00_01RunningAbout.mid')
# plt.legend()
# plt.savefig("./mario_accuracy.png")
# plt.clf()
#
# plt.plot(possible_num_latents, losses, 'r', label='cross_entropy_loss')
# plt.xlabel('num_latents')
# plt.ylabel('cross_entropy_loss')
# plt.title('322_SuperMarioBros__00_01RunningAbout.mid')
# plt.legend()
# plt.savefig("./mario_loss.png")
# plt.clf()