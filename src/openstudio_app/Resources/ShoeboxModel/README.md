# OpenStudio Coalition Example Models

Collection of OpenStudio Coalition Example Models

## ShoeboxExample

A simple model that creates a test building. The OSM only has the weather file and design days, the model is created via a collection of measures.

Third party measures:

- Model Articulation Measures:
    - create_bar_from_space_type_ratios - https://github.com/NREL/openstudio-model-articulation-gem
    - create_typical_building_from_model - https://github.com/NREL/openstudio-model-articulation-gem
- Reporting Measures:
    - openstudio_results - https://github.com/NREL/openstudio-common-measures-gem
    - view_data - https://github.com/NREL/openstudio-common-measures-gem
