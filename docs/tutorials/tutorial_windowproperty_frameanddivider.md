<h1>Window Property Frame and Divider</h1>
This tutorial explains how to use the OS:WindowProperty:FrameAndDivider object to study the impact of window frame and divider materials as well as the impact of sill and outside reveal surfaces. The OS:WindowProperty:FrameAndDivider object closely follows the EnergyPlus [WindowProperty:FrameAndDivider](http://bigladdersoftware.com/epx/docs/8-2/input-output-reference/page-011.html#windowpropertyframeanddivider
) object with only one difference.  The OpenStudio object includes one additional field, Outside Reveal Depth, which the EnergyPlus object does not.  During translation from OpenStudio to EnergyPlus, OS:SubSurface vertices are offset by this amount in order to model the additional shading provided by the outside reveal surface.

## Create Window Property Frame and Divider
Window Property Frame and Divider objects can be created using the object inspector in the OpenStudio SketchUp plug-in. The inspector can also be used to edit any properties of the Window Property Frame and Divider object.

![Add Window Property Frame and Divider](img/windowproperty_frameanddivider/windowproperty_frameanddivider1.jpg)

### Importing IDF files from the WINDOW program
IDF files exported from [WINDOW](http://windows.lbl.gov/software/window/window.html) containing Window Property Frame and Divider objects can be imported using the OpenStudio SketchUp plug-in via Extensions>OpenStudio>Import>Import EnergyPlus Idf Constructions.  WINDOW exports two IDF files, a *_Avg.idf and a *_Spec.idf file. Only the *_Avg.idf file may be imported as the MaterialProperty:GlazingSpectralData object in the *_Spec.idf file is not yet supported by OpenStudio.

![Import Window Property Frame and Divider](img/windowproperty_frameanddivider/windowproperty_frameanddivider2.jpg)

## Apply Window Property Frame and Divider
Once you have a Window Property Frame and Divider object in your model, you can apply it to sub surfaces using the object inspector.  Only exterior windows and glass doors can reference Window Property Frame and Divider objects.

![Set Window Property Frame and Divider](img/windowproperty_frameanddivider/windowproperty_frameanddivider3.jpg)

The Window Property Frame and Divider object can also be applied to all valid sub surfaces in the current selection using the OpenStudio user script "Set Window Property Frame and Divider" under "Alter or Add Model Elements.

![Add Window Property Frame and Divider Script](img/windowproperty_frameanddivider/windowproperty_frameanddivider4.jpg)


