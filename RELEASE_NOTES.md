# Release notes

## Resonance Audio SDK for Wwise v1.1.0

### Behavioral Changes
* The reverb brightness and time modifiers in `RoomProperties` adjust the room effects more accurately now for long reverb tails.

### Bug fixes
* Fixed an issue where the `Resonance Audio Renderer` and `Resonance Audio Room Effects` plugins would produce artifacts in the output when there are multiple plugin instances. For each plugin, only the most recently activated plugin instance will produce output, while previously-created plugin instances are disabled.
* Fixed an issue where the `Resonance Audio Renderer` plugin would never stop producing an output of silence when the speaker configuration is invalid (i.e., non-Ambisonic), even when there is no input stream to process.

## Resonance Audio SDK for Wwise v1.0.0 (2017-11-06)

This is the initial release of Resonance Audio SDK for Wwise, which includes:
* 3D audio spatialization
* Room effects rendering with custom surface materials
