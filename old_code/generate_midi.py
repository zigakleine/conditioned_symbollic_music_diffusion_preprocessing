import mido
from mido import MidiFile, MidiTrack, Message, MetaMessage, bpm2tempo
import nesmdb

ins_names = ['p1', 'p2', 'tr', 'no']

def convert_to_midi(melodies):
    mid = MidiFile()

    ticks_per_beat = 480  # Standard MIDI ticks per beat

    time_shift_multiplier = int(ticks_per_beat / 24)  # Adjust to match the time-shift event range

    track_metadata = MidiTrack()
    track_metadata.append(MetaMessage('time_signature', numerator=4, denominator=4, time=0))
    track_metadata.append(MetaMessage('set_tempo', tempo=bpm2tempo(120), time=0))
    track_metadata.append(MetaMessage('time_signature', numerator=4, denominator=1, time=(96*time_shift_multiplier)))
    track_metadata.append(MetaMessage('end_of_track', time=0))
    mid.tracks.append(track_metadata)

    for i in range(4):
        track = MidiTrack()
        mid.tracks.append(track)

        track.append(MetaMessage('track_name', name=ins_names[i]))


        time_from_last_event = 0

        for event_type, event_value in melodies[i]:
            if event_type == 1:  # Note-on event
                track.append(Message('note_on', note=event_value, velocity=8, time=time_from_last_event))
                time_from_last_event = 0
            elif event_type == 2:  # Note-off event
                track.append(Message('note_off', note=event_value, velocity=8, time=time_from_last_event))
                time_from_last_event = 0
            elif event_type == 3:  # Time-shift event
                time_shift = int(event_value * time_shift_multiplier)
                time_from_last_event += time_shift
            else:
                raise ValueError("Invalid event type")
        track.append(MetaMessage('end_of_track', time=0))

    return mid

example_melodies = [ [(1, 127), (3, 24), (2, 127), (1, 10), (3, 48), (2, 10), (1, 63), (3, 24), (2, 63)],
                    [(1, 60), (3, 24), (2, 60), (1, 69), (3, 48), (2, 69), (1, 63), (3, 24), (2, 63)],
                    [(1, 60), (3, 24), (2, 60), (1, 69), (3, 48), (2, 69), (1, 63), (3, 24), (2, 63)],
                    [(1, 11), (3, 24), (2, 11), (1, 10), (3, 48), (2, 10), (1, 10), (3, 24), (2, 10)]];

midi_file = convert_to_midi(example_melodies)
midi_file.save('output.mid')