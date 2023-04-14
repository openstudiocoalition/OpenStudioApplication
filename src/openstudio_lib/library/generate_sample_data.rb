require 'openstudio'

m = OpenStudio::Model::exampleModel

ft = OpenStudio::Model::ThreeJSForwardTranslator.new

# Triangulate
triangulate = true
scene = ft.modelToThreeJS(m, triangulate)

pretty_print = true
File.write('sample_data.json', scene.toJSON(pretty_print))
