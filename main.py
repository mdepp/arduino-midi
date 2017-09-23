from mido import MidiFile

mid = MidiFile('Music.mid')
print(mid)


for i, track in enumerate(mid.tracks):
    print('Track {}: {}'.format(i, track.name))
    for msg in track:
        if not msg.is_meta:
            print(msg)

