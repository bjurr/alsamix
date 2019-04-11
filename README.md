# alsamix
NodeJS NAPI module for interfacing alsa mixer.

## Dependencies
This is meant to run on a Linux box, where alsa is available. The build process will require the alsa development headers, as well as pkg-config to actually source the proper paths.

## Installation
Given yu have the required dependencies, you should be able to build alsamix like so:
```
npm install
```

## API
The module is currently limited to:
* set_volume(volume, card, element)
* get_volume(card, element)
