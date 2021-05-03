# Platonica
![Loops!](https://media0.giphy.com/media/VF3rY8aRMSXqSVsN7N/giphy.gif?cid=790b7611f81c06ddc4c8c3d41cebe9cc7e3f51ae7e55d658&rid=giphy.gif&ct=g)
## Requirements!
- [Cmake](http://www.cmake.org/download/) in order to generate platform-specific project files.
- A Gamepad.

## Getting Started!
Clone the repository, using the `--recursive` flag to grab all dependencies. 
If forgotten use `git submodule update --init`.

```bash
git clone --recursive https://github.com/keenaro/Platonica.git
```
I have supplied a batch script in the root folder named `Setup.bat`, running this will setup the project accordingly.

Upon running the script, you will find the project files within `Platonica/build`.

Once the project has been opened in your IDE of choice you will be required to set Platonica as the Startup Project.

## First Steps!
### Hosting
 - Using the World Loader GUI window, dropdown Host World.
 - Load into a previously generated world or create a new world via the dropdown Create New World.
 - If creating a new world, specify a new `World Name`, `Seed`, and `World Length`.
 
 `World length determines how many 16x16 chunks are required till the world wraps`
 - If attempting to play across the internet you will be required to [Port Forward](https://www.noip.com/support/knowledgebase/general-port-forwarding-guide/) the port `25565`.
 
### Joining
 - Using the World Loader GUI window, dropdown Join World.
 - Fill out the hostname text box, with the hostname of the world you would like to connect to.
 
`If the host is on the same machine use localhost`

`If the host is on the same network use their ipv4 ip address`

`If connecting via the internet you will need the hosts external ipv4 ip address` [WhatIsMyIpAddress](https://whatismyipaddress.com/)
 
### Controls
 - Left Stick to move around. ![Left Stick](https://upload.wikimedia.org/wikipedia/commons/4/4f/Xbox_Left_stick_button.svg)
 - Right stick to look around. ![enter image description here](https://upload.wikimedia.org/wikipedia/commons/d/d5/Xbox_Right_stick_button.svg)
 - Bottom face button to place blocks. ![enter image description here](https://upload.wikimedia.org/wikipedia/commons/d/d2/Xbox_button_A.svg)
  - Left face button to remove blocks. ![enter image description here](https://upload.wikimedia.org/wikipedia/commons/8/8c/Xbox_button_X.svg)
  - Hold left bumper to move faster. ![enter image description here](https://upload.wikimedia.org/wikipedia/commons/8/8c/Xbox_Left_Bumper.svg)
  
  ### Additonal Notes
  - The block to place can be changed via the selected block dropdown in the Player GUI window.
  - You leave a world via the World GUI window.
  - The movement speed multiplier can be altered in the Player GUI window via the `Speed Increase Multiplier` slider.
  - The world bend amount can also be tweaked via the `World Spherical Falloff` slider inside of the World GUI window. 
