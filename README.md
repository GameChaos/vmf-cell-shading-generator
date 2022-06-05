# vmf-cell-shading-generator
Generates fake cell shaded brushes for the Source engine with a .vmf input. Outputs a vmf that should be used as an instance.

Basically, this is what it does:

![image](https://user-images.githubusercontent.com/25118806/172063323-1e46d7a3-f283-413d-aeea-459fe6a8e856.png)

![image](https://user-images.githubusercontent.com/25118806/172063328-8435cc15-0b9b-45c3-828c-0d65d742d3b7.png)


# Usage



# Limitations/Problems

Hammer won't update the instance if you change it outside of it, so you have to reopen your map to see the changes (note: this was tested only on Hammer++).

If two brushes are flush with each other, but have diagonal faces, then sometimes there are gaps in the cell shading between those brushes.

Example:

![image](https://user-images.githubusercontent.com/25118806/172063147-fa9551f4-03d5-4edc-97dd-15db6d2ba0cd.png)

# Building

Compile vmf_cell_shading.cpp with your favourite compiler. This should compile on Linux, haven't tested though.

For windows you can try and use my build.bat and set_up_msvc.bat things. They use msvc (visual studio's compiler), so you need to install that.
