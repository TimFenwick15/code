fmodstudio is an authoring tool, fmod low level API for playing sounds and managing files, fmodstudio api supports fmod studio and builds on the low level API

FMOD::Debug_Initialize enables logging, dev only.

events - sounds played in game, can have multiple sound files. The game plays the event, the event plays the sound

bank - contains events, sample data, and streaming data. Sample data can either be preloaded or loaded on demand, but it must be in memory to play. Most game sound effects use sample data. Streaming data loads a piece at a time, used for music or dialogue.

EventDescription - class holding meta data
EventInstance - provides control over the event

Load the strings.bank file before the .bank, and you get access to names rather than GUIDs

We need a second class to manage events because storing the EventInstance is a bad idea. Make this a friend of AudioSystem to give it access to the FMOD API

In first person, the camera is the listener, with the same position and direction. In third person, this is more complex.

Note the audio library may use a different coordinate system, and in our case it does

An AudioComponent on an actor means audio is associated with the actor, and dies when the actor dies

## Third Person
Use player position, and camera orientation

Player P, Camera C, Sound S

PlayerToSound = S - P
CameraToSound = S - C

VirtualPosition = ||PlayerToSound|| * CameraToSound / ||CameraToSound||

## Doppler
Use nLowLevelSystem->set3DSettings

## Mixing
Reverb - sound bouncing
Equalisation - keep sound level inside a range

## Buses
Sounds are grouped into buses and can be controlled as a collective

Snapshots are events that control buses

## Occlusion
Muffling sounds passing through an object. Full if the sound source is fully blocked, partial if it is partially blocked. Implemented as a low pass filter
