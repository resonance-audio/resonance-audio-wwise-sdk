# Release notes

## Resonance Audio SDK for Wwise v1.2.1

### Bug fixes
* Minor bug fixes and performance optimizations.

## Resonance Audio SDK for Wwise v1.2.0 (2018-02-20)

### Behavioral Changes
* Significant CPU performance improvement for reverb times more than 0.6 seconds (thanks to a new spectral reverb implementation under the hood). Also, delivers a slightly brighter sounding reverb.

### Bug fixes
* Static libraries are now linked against gnustl on Android to avoid symbol mismatches.

## Resonance Audio SDK for Wwise v1.1.1 (2017-12-18)

### Bug fixes
* Fixed a bug in stereo deinterleaving input buffer conversion that could lead to a crash.

## Resonance Audio SDK for Wwise v1.1.0 (2017-12-15)

### Behavioral Changes
* The reverb brightness and time modifiers in `RoomProperties` adjust the room effects more accurately now for long reverb tails.

### Bug fixes
* Fixed an issue where the `Resonance Audio Renderer` and `Resonance Audio Room Effects` plugins would produce artifacts in the output when there are multiple plugin instances. For each plugin, only the most recently activated plugin instance will produce output, while previously-created plugin instances are disabled.
* Fixed an issue where the `Resonance Audio Renderer` plugin would never stop producing an output of silence when the speaker configuration is invalid (i.e., non-Ambisonic), even when there is no input stream to process.

## Resonance Audio SDK for Wwise v1.0.0 (2017-11-06)

This is the initial release of Resonance Audio SDK for Wwise, which includes:
* 3D audio spatialization
* Room effects rendering with custom surface materials
