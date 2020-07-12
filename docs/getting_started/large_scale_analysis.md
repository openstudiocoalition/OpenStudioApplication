<h1>Large Scale Analysis</h1>
Although building energy modeling has been common for many years, large-scale analyses have more recently become achievable for more users with access to affordable and vast computing power in the cloud. Recently, the OpenStudio Parametric Analysis Tool was enhanced, allowing users to launch their own cloud resources to run a large number of simulations.

The Parametric Analysis Tool (PAT) now exports a spreadsheet format compatible with the OpenStudio-analysis-spreadsheet project for running large parametric analyses using cloud resources.

Go to the [GitHub repository](https://github.com/NREL/OpenStudio-analysis-spreadsheet) for information and code for the analysis spreadsheet. There are instructions and examples on the GitHub site.

## The Workflow
1. Build your baseline model with the OpenStudio Application
2. Manipulate the Model with measures
3. Run basic parametrics via PAT
4. Export a spreadsheet view from PAT
5. Fill in ranges, distributions, defaults, algorithms, objective functions
6. Large Analyses via OpenStudio-Server
7. Start Cluster
8. Upload Data:
    - Measures
    - Variables
    - Seed Model
    - Weather Data
9. Simulation Executive Runs Analysis
10. Browse Results and Download Files
11. Everything has a JSON-based API to access data programmatically
12. Download results in CSV or R dataframe

__________

![Analysis Workflow](img/large_scale/cloud_run_process_diagram.png)

__________

## Starting OpenStudio's Cloud
```bash
gem install bundler
git clone https://github.com/NREL/OpenStudio-analysis-spreadsheet.git
bundle install
```

## Export Spreadsheet
Select the measures you want to use in the analysis and export a spreadsheet by selecting that option under the File menu.
![Export Analysis Spreadsheet Menu](img/large_scale/export_spreadsheet.png)

## Edit the Template Spreadsheet
After exporting the spreadsheet from PAT, open the spreadsheet in Excel to edit fields.

[![Spreadsheet View](img/large_scale/spreadsheet.png "Click to view")](img/large_scale/spreadsheet.png)

*Above: Click to see a larger screenshot of the spreadsheet.*

## Running OpenStudio's Cloud
```bash
rake run
```

__________

### Buildings Specific Chef Cookbooks
Provisioning ensures that the appropriate tools are ready on the server, that there are consistent images across platforms, and makes it easier to test and deploy.

The OpenStudio Server uses Chef to help automate the workflow. [Chef Documentation](https://docs.getchef.com/chef_overview.html)

OpenStudio - [https://github.com/NREL-cookbooks/openstudio](https://github.com/NREL-cookbooks/openstudio)

EnergyPlus - [https://github.com/NREL-cookbooks/energyplus](https://github.com/NREL-cookbooks/energyplus)

Radiance - [https://github.com/NREL-cookbooks/radiance](https://github.com/NREL-cookbooks/radiance)

R - [https://github.com/NREL-cookbooks/cookbook-r](https://github.com/NREL-cookbooks/cookbook-r)

__________

## Server Architecture
![Analysis Architecture](img/large_scale/architecture.png)

__________

## Publications
[A GRAPHICAL TOOL FOR CLOUD-BASED BUILDING ENERGY SIMULATION (PDF)](img/pdfs/graphical-tool-cloud-based.pdf)

[SCALING BUILDING ENERGY MODELING HORIZONTALLY IN THE CLOUD WITH OPENSTUDIO (PDF).](img/pdfs/scaling-building-energy-modeling.pdf)
