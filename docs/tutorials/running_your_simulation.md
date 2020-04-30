<h1>Running Simulations & Viewing Results</h1>
These guides provide a handy reference to the features of the applications that are installed with OpenStudio.

## Weather File and Design Days
Add weather files in the OpenStudio application under the Site tab (first vertical tab) on the left and the "Weather File & Design Days" sub-tab across the top. You can [download weather files](https://www.energyplus.net/weather) on the EnergyPlus site.

[![Open DDY File Dialog](img/run/weather_ddy.png "Click to view")](img/run/weather_ddy.png)

*Above: Screenshot of browsing for design day file.*

## Requesting Output Variables
To select an output variable turn the toggle button to show a blue on. Then set the detail level you want from the drop down menu.

[![Output Variables Tab](img/run/output_variables.png "Click to view")](img/run/output_variables.png)

*Above: Screenshot of selecting output variables.*

You can also choose variables to collect data on by using the "Add Output Variable" measure after downloading it from the [BCL](../reference/openstudio_application_interface.md#find-measures-and-find-components).

[![Measures Tab](img/run/variables_measure.png "Click to view")](img/run/variables_measure.png)

*Above: Screenshot of the variable measure being added to a project. Just type in the variable you want to collect. This will return the variable's value for all associated objects within the model. To get the variable's value for a specific object, enter the full name of this object.*

## Running a Simulation
To run your simulation click the green run button. 

[![Run Simulation Tab](img/run/run.png "Click to view")](img/run/run.png)

*Above: The progress bar displays the status of the run.*

## Looking at Results
Standard OpenStudio results are provided with your run. Learn more about the OpenStudio Standard Results Measure in our [References Section](../reference/openstudio_results_measure.md).

<iframe width="640" height="360" src="https://www.youtube.com/embed/PlrWE3ugcdE" allowfullscreen></iframe>

*Above: This video demonstrates the 1.9.0 Release with the Reporting Measure.*

[![Results Summary Tab](img/run/os_results.png "Click to view")](img/run/os_results.png)

*Above: This screenshot shows and example of the standard OpenStudio results.*

[![Results 2 Summary Tab](img/run/reports2.png "Click to view")](img/run/reports2.png)

*Above: This is another chart available in the standard OpenStudio results.*

You can also use measures to add additional reports. For instance, you can download the calibration report measure from the [BCL](../reference/openstudio_application_interface.md#find-measures-and-find-components). This measure allows you to view simulation results that are associated with any added utility bills. Another option available for adding additional reports is the generation of custom reports. These are created by writing your own measures. Downloaded or created reporting measures are applied within the OpenStudio Measures tab.

[![Calibration Report](img/run/calibration_report.png "Click to view")](img/run/calibration_report.png)

*Above: The calibration report compares utility bills with the model.*

## Using Data Viewer
Data Viewer (DView) displays time series results, which are produced from EnergyPlus report variable requests. It reads the SQLite database of the EnergyPlus engine and presents the time series data using line plots and heat maps. Furthermore, additional graphical/tabular formats can be harnessed to visualize simulation variables that are independent of time. [Learn more about DView using guides created by researchers at the National Renewable Energy Laboratory](https://github.com/NREL/wex/wiki/DView).