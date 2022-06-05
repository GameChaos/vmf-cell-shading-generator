# vmf-cell-shading-generator
Generates fake cell shaded brushes for the Source engine with a .vmf input. Outputs a vmf that should be used as an instance.

Basically, this is what it does:

![image](https://user-images.githubusercontent.com/25118806/172063323-1e46d7a3-f283-413d-aeea-459fe6a8e856.png)

![image](https://user-images.githubusercontent.com/25118806/172063328-8435cc15-0b9b-45c3-828c-0d65d742d3b7.png)


# Usage

## Setup

1. Download the [latest release](https://github.com/GameChaos/vmf-cell-shading-generator/releases) and put it somewhere on your computer.
2. Open any map Hammer.
3. Open the Run Map window in expert mode.
4. Click on the `New` button.
5. Click `Move Up` until the new run command is at the top of the `Compile/run command` list.
6. On the right side of the window, click on `Cmds` and select `Executable`.
7. Browse to where you saved the `vmf_cell_shading` executable and select it.
8. Copy `-outlinewidth 2 -input $path\$file.vmf -output $path\instances\$file_cellshaded.vmf` into the `Parameters:` text box. This options output the generated instance vmf into the `instances` folder where your vmf is. This folder has to exist. Modify these parameters as you see fit. You can add -help to the list to see more information about the commands.

## Map setup

1. Compile your map (you can compile it by only having the cell shading run command checked and the other run commands unchecked).
2. Create a point entity, **move it to the origin of your map** and name it `func_instance`.
3. Change `VMF Filename` to `instances/<YOUR MAP NAME HERE>_cellshaded.vmf`. Example: `instances/kz_gamechaos_cellshaded.vmf`.
4. Create a visgroup named `Cellshading` and put the entities (no world brushes currently) you want cell shaded in that visgroup.
5. You can now properly compile your map and see the epic cell shading in-game!

# Limitations/Problems

Hammer won't update the instance if you change it outside of it, so you have to reopen your map to see the changes (note: this was tested only on Hammer++).

If two brushes are flush with each other, but have diagonal faces, then sometimes there are gaps in the cell shading between those brushes.

Example:

![image](https://user-images.githubusercontent.com/25118806/172063147-fa9551f4-03d5-4edc-97dd-15db6d2ba0cd.png)

# Building

Compile vmf_cell_shading.cpp with your favourite compiler. This should compile on Linux, haven't tested though.

For windows you can try and use my build.bat and set_up_msvc.bat things. They use msvc (visual studio's compiler), so you need to install that.
