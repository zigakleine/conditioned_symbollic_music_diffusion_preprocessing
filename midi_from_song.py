import ctypes
import numpy as np
import mido
from mido import MidiFile, MidiTrack, Message, MetaMessage, bpm2tempo



# Load the shared library
cpp_lib = ctypes.CDLL("/Users/zigakleine/Desktop/scraping_and_cppmidi/ext_nseq_lib.so")

# Define the struct to match the C++ side
class sequence_array(ctypes.Structure):
    _fields_ = [("sequence", ctypes.POINTER(ctypes.POINTER(ctypes.POINTER(ctypes.c_int)))),
                ("dim1", ctypes.c_int),
                ("dim2", ctypes.c_int),
                ("dim3", ctypes.c_int)]

# Define the return type of the function
cpp_lib.extract_note_sequences_from_midi.restype = sequence_array


midifile_loc = ctypes.c_char_p(b"/Users/zigakleine/Desktop/scraping_and_cppmidi/nesdbmario/322_SuperMarioBros__00_01RunningAbout.mid")

# Call the C++ function
sequence_array = cpp_lib.extract_note_sequences_from_midi(midifile_loc)

# Convert the array to a NumPy array
sequences_ = []
for i in range(sequence_array.dim1):
    measure = []
    for j in range(sequence_array.dim2):
        track = []
        for k in range(sequence_array.dim3):
            current_val = sequence_array.sequence[i][j][k]

            if current_val > -1:
                if 0 <= current_val < 128:
                    track.append((1, current_val))
                elif 128 <= current_val < 256:
                    track.append((2, current_val - 128))
                elif 256 <= current_val < 352:
                    track.append((3, current_val - 256 + 1))
                elif 352 <= current_val < 360:
                    track.append((4, current_val - 352 + 1))
                elif 360 <= current_val < 488:
                    track.append((0, current_val - 360 + 1))
                elif current_val == 489:
                    track.append((5, 0))
        measure.append(track)
    sequences_.append(measure)


# Print the elements
for i in range(len(sequences_)):
    print("measure-" + str(i))
    for j in range(len(sequences_[i])):
        for k in range(len(sequences_[i][j])):

            print(str(sequences_[i][j][k]), end=" ")
        print("")
    print("\n")

mid = MidiFile()

measure_ticks = 96
nes_tracks = 4
ticks_per_beat = 480  # Standard MIDI ticks per beat

time_shift_multiplier = ticks_per_beat / (measure_ticks/4) # Adjust to match the time-shift event range

track_metadata = MidiTrack()
mid.tracks.append(track_metadata)
track_metadata.append(MetaMessage('time_signature', numerator=4, denominator=4, time=0))
track_metadata.append(MetaMessage('set_tempo', tempo=bpm2tempo(120), time=0))
track_metadata.append(MetaMessage('time_signature', numerator=4, denominator=1, time=int(len(sequences_)*measure_ticks*time_shift_multiplier)))
track_metadata.append(MetaMessage('end_of_track', time=0))



ins_names = ['p1', 'p2', 'tr', 'no']
ticks_from_last_event = [0, 0, 0, 0, 0]
for i in range(nes_tracks):
    track = MidiTrack()
    mid.tracks.append(track)
    track.append(MetaMessage('track_name', name=ins_names[i]))

for s, sequence in enumerate(sequences_):

    for t, seq_track in enumerate(sequence):
        ticks_passed = 0

        for event_type, event_value in seq_track:

            if event_type == 1:  # Note-on event
                mid.tracks[t].append(Message('note_on', note=event_value, velocity=3, time=ticks_from_last_event[t]))
                ticks_from_last_event[t] = 0

            elif event_type == 2:  # Note-off event
                mid.tracks[t].append(Message('note_off', note=event_value, velocity=3, time=ticks_from_last_event[t]))
                ticks_from_last_event[t] = 0

            elif event_type == 3:  # Time-shift event
                time_shift = int(event_value * time_shift_multiplier)
                ticks_from_last_event[t] += time_shift
                ticks_passed += time_shift

            elif event_type == 5:
                ticks_from_last_event[t] += int(measure_ticks*time_shift_multiplier) - ticks_passed
        # print("")


for i in range(nes_tracks):
    mid.tracks[t].append(MetaMessage('end_of_track', time=0))

mid.save('output.mid')