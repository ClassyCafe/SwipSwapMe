# SwipSwapMe Plugin 

This little plugin can be used to swap the TV and GamePad screen.

## Installation
(`[ENVIRONMENT]` is a placeholder for the actual environment name.)

1. Copy the file `swipswapme.wps` into `sd:/wiiu/environments/[ENVIRONMENT]/plugins`.  
2. Requires the [WiiUPluginLoaderBackend](https://github.com/wiiu-env/WiiUPluginLoaderBackend) in `sd:/wiiu/environments/[ENVIRONMENT]/modules`.

## Usage
Press a button combo (default is the GamePad's TV button) on the GamePad, Pro Controller or Classic Controller to take swap the screens.

Via the plugin config menu (press L, DPAD Down and Minus on the GamePad, Pro Controller or Classic Controller) you can configure the plugin. The available options are the following:
- **Settings**: 
  - Enabled: (Default is true)
    - Enables or disables the SwipSwapMe plugin.
  - Button combo: (Default is the GamePad's TV button)
    - Changes the button combo for swapping the screens.

## Building

For building you need: 
- [wups](https://github.com/wiiu-env/WiiUPluginSystem)
- [wut](https://github.com/devkitPro/wut)

## Building using the Dockerfile

It's possible to use a docker image for building. This way you don't need anything installed on your host system.

```
# Build docker image (only needed once
docker build . -t swipswapme-builder

# make 
docker run -it --rm -v ${PWD}:/project swipswapme-builder make

# make clean
docker run -it --rm -v ${PWD}:/project swipswapme-builder make clean
```

## Format the code via docker

`docker run --rm -v ${PWD}:/src wiiuenv/clang-format:13.0.0-2 -r ./src -i`
