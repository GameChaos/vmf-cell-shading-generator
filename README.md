# vmf-cell-shading-generator
Generates fake cell shaded brushes for the Source engine with a .vmf input. Outputs a vmf that should be used as an instance.

Basically, this is what it does:

![image](https://user-images.githubusercontent.com/25118806/172063323-1e46d7a3-f283-413d-aeea-459fe6a8e856.png)

![image](https://user-images.githubusercontent.com/25118806/172063328-8435cc15-0b9b-45c3-828c-0d65d742d3b7.png)


# Usage

## Hammer setup

1. Download the [latest release](https://github.com/GameChaos/vmf-cell-shading-generator/releases) and put it somewhere on your computer.
2. Open any map in Hammer.
3. Open the Run Map window in expert mode.
4. Click on the `New` button.
5. Click `Move Up` until the new run command is at the top of the `Compile/run command` list.
6. Click on the checkbox next to the new empty command to enable it.
7. On the right side of the window, click on `Cmds` and select `Executable`.
8. Browse to where you saved the `vmf_cell_shading` executable and select it.
9. Copy `-outlinewidth 2 -input $path\$file.vmf -output $path\instances\$file_cellshaded.vmf` into the `Parameters:` text box. This options output the generated instance vmf into the `instances` folder where your vmf is. This folder has to exist. Modify these parameters as you see fit. You can add -help to the list to see more information about the commands.

[These instructions, but in video form](https://youtu.be/JhnN4ZsGzhE)

## Map setup

1. You have to complete the hammer setup first!
3. Create a point entity **at the exact origin of the hammer grid** and name it `func_instance`.
4. Change `VMF Filename` to `instances/<YOUR MAP NAME HERE>_cellshaded.vmf`. For example if your map name is test.vmf, then you type `instances/test_cellshaded.vmf`.
5. If the `instances` folder doesn't exist in the folder where your vmf is, then create it!
6. Create a visgroup named `Cellshade` and put the entities (no world brushes currently) you want cell shaded in that visgroup. Faces that are textured with `tools/toolsnodraw` won't have an outline generated.
7. You can now compile your map and see the epic cell shading in-game!
8. To see the cell shading update in hammer you have to close and reopen the map unfortunately.

[These instructions, but in video form](https://youtu.be/PboGaKNj2mY)

# Limitations/Problems

Hammer won't update the instance if you change it outside of it, so you have to reopen your map to see the changes (note: this was tested only on Hammer++).

If two brushes are flush with each other, but have diagonal faces, then sometimes there are gaps in the cell shading between those brushes.

Example:

![image](https://user-images.githubusercontent.com/25118806/172063147-fa9551f4-03d5-4edc-97dd-15db6d2ba0cd.png)

# Building

Compile vmf_cell_shading.cpp with your favourite compiler. This should compile on Linux, haven't tested though.

For windows you can try and use my build.bat and set_up_msvc.bat things. They use msvc (visual studio's compiler), so you need to install that.
