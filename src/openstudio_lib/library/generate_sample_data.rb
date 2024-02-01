require 'openstudio'

m = OpenStudio::Model::exampleModel

# Flip a wall
wall = m.getSurfaces.select{|sf| sf.surfaceType.downcase.include?("wall") && sf.outsideBoundaryCondition.downcase == 'outdoors' }.first
wall.setVertices(wall.vertices.reverse)

# Delete a roof
roof = m.getSurfaces.select{|sf| sf.surfaceType.downcase.include?("roof") }.first
roof.remove

ft = OpenStudio::Model::ThreeJSForwardTranslator.new
ft.setIncludeGeometryDiagnostics(true)

# Triangulate
triangulate = true
scene = ft.modelToThreeJS(m, triangulate)

pretty_print = true
File.write('sample_data.json', scene.toJSON(pretty_print))
