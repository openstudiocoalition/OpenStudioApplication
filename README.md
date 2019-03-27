
For now, to avoid having to push to the internet and retrieve when actually everything is on my disk, I haven't added NREL/OpenStudio as a submodule,
but I have symlinked `OpenStudioApplication/src/openstudiosdk` to point to my local `OpenStudio/` directory
Once we're ready, we'll add an actual submodule:

    git submodule add -b OS_App_Separation git@github.com:jmarrec/OpenStudio.git openstudiosdk


