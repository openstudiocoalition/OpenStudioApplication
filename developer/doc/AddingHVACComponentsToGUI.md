# Adding HVAC Component to the OpenStudio Application

This documents the process for adding HVAC components to the GUI.

This assumes that the model object(s) already exist NREL/OpenStudio (core SDK)

## Add icon:

You need two icons:

* a Full size icon that is 100px x 100px: it will be displayed when you drag the HVAC component into the HVAC scene (grid layout)
* a mini icon that is 24px x 24px: it will be used in the Main Right Column Controller ("Library" pane)

Copy the full size icon to `./src/openstudio_lib/images/`, and the mini icon to `./src/openstudio_lib/images/mini_icons/`.

Add two entries to the file `./src/openstudio_lib/openstudio.qrc`.
This file registers the image files so that they become resources in the executable.
Add one entry for the full size icon and one for the mini icon.

Add two entries to the file `./src/openstudio_lib/IconLibrary.cpp`.
This file contains a big map of the new OS type to the proper icon.

## Make the library aware of the new type:

Tell the library UI to look for the new type in the library and
display any instances of the type when the HVAC tab is active.

Do this by editing `./src/openstudio_lib/MainRightColumnController.cpp`.

In the function `MainRightColumnController::configureForHVACSystemsSubTab(int subTabID)`,
add an entry to `libraryWidget->addModelObjectType(...`

This will make a new category in the library chooser on the right so that you
can drag and drop instances of the new type into the HVAC scene.

## Add one or more instances of the new model object to the HVAC library:

  The HVAC Library is located at: `./src/openstudio_app/Resources/default/hvac_library.osm`.
  Use the ruby bindings to add an instance of your object to the HVAC library.

  Assuming you have built ruby bindings for the new type fire up irb and point it to the
  ruby bindings in your build directory.  Something similar to this...

```ruby
irb -I path/to/openstudio/build/ruby

require 'openstudio'


# Helper to load a model in one line
# It will raise if the path (or the model) isn't valid
#
# @param path [String] The path to the osm
# @return [OpenStudio::Model::Model] the resulting model.
def osload(path)
  translator = OpenStudio::OSVersion::VersionTranslator.new
  ospath = OpenStudio::Path.new(path)
  model = translator.loadModel(ospath)
  if model.empty?
      raise "Path '#{path}' is not a valid path to an OpenStudio Model"
  else
      model = model.get
  end
  return model
end

lib_path = '/path/to/OpenStudioApplication/src/openstudio_app/Resources/default/hvac_library.osm'
lib= osload(lib_path)

# Create an instance of your component and configure it
c = OpenStudio::Model::NewComponentFoo.new(lib)
[...]

# Save the library
lib.save(lib_path, true)
```

**Note**: One thing to be mindful of is that the above command will also do Version Translation of the hvac_library to the version you used to build your ruby bindings.
In some cases, such as when you expect **other resquired version translation rules to be written after you've added your component** (= when IDD Freeze hasn't happened yet), this could cause problems.
You might just use the git diff to pull the changes related to your component only.
