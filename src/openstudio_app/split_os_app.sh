src/model_editor src/openstudio_app src/openstudio_lib src/qtwinmigrate src/shared_gui_components

Source: https://stackoverflow.com/questions/2982055/detach-many-subdirectories-into-a-new-separate-git-repository

    git filter-branch --index-filter 'git rm --cached -qr --ignore-unmatch -- . && git reset -q $GIT_COMMIT -- src/model_editor src/openstudio_app src/openstudio_lib src/qtwinmigrate src/shared_gui_components' --prune-empty -- --all
