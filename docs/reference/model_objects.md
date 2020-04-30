<h1>OpenStudio Model Objects Overview</h1>
One of the advantages of OpenStudio is that it adds modern object-oriented software concepts such as relationships and inheritance to EnergyPlus. This makes it much easier to add new functionality to tools or write measures that can quickly manipulate energy models. From a practitioner's perspective, inheritance makes it very easy to quickly create new models. __When you are using OpenStudio templates to define building and space types, you are using this capability to avoid specifying the hundreds or thousands of pieces of information EnergyPlus requires to create a detailed simulation.__

## Relationship Diagram
The diagram below shows the OpenStudio 0.6.0 Model Objects Hierarchical Relationships. This is not a comprehensive diagram, but rather focuses on the building envelope (external loads) and building activity (internal loads). It is a good reference to understand where objects reside in the model, and what other objects they impact and are impacted by.

[![Model Object Relationships](img/model/model_object_relationships.jpg "Click to view")](img/model/model_object_relationships.jpg)

*Above: Click to view a larger diagram of model relationships.*

## Inheritance
The diagram below shows the OpenStudio 0.6.0 Model Objects Inheritance Relationships. Again, this is not a comprehensive diagram, but focuses on some of the more commonly used objects. The red dot in each column represents the first place our OSM to IDF translator looks when converting an OSM object to an IDF object or objects. If the field value is blank where the red dot is, the translator looks at the object where the yellow circle is, and continues this until the end of the path. The ability for objects to inherit values from elsewhere in the model allows for quick global changes, and for customization of any individual object in the model.

[![OpenStudio Inheritance](img/model/openstudio_inheritance.jpg "Click to view")](img/model/openstudio_inheritance.jpg)

*Above: Click to view a larger diagram of model inheritance.*
