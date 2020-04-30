<h1>Life Cycle Cost Analysis</h1>

## Purpose
Life cycle cost analysis is commonly performed by energy modelers in order to determine if energy cost savings due to reduced energy usage will be enough to justify the additional investment required to implement the conservation measure. Different projects are interested in different life cycle metrics (e.g. simple payback, net present value, etc.) and will have different cost assumptions. However, in all cases the modeler needs a way to specify costs associated with the energy conservation measures, enter utility rate tariffs related to energy use, and set life cycle cost parameters such as the analysis period length and discount rate. This document explains how to use the OpenStudio Parametric Analysis Tool (PAT) to perform each of these functions as part of a life cycle cost analysis.

In order to perform a parametric analysis using PAT, a user must first build a baseline OpenStudio model using a combination of the OpenStudio SketchUp plug-in to edit geometry and the OpenStudio application to edit other model components. Once the baseline model has been assembled and tested, users may apply energy conservation measures using the PAT application. Tutorials and documentation on this process are available at http://openstudio.nrel.gov/parametric-analysis-tool-tutorials.

The "OpenStudio Life Cycle Costing Examples" document focuses on how to add cost inputs through measures in order to extend the PAT analysis from one dealing only with energy use to one that includes life cycle cost analysis as well. The first section describes how to set life cycle cost analysis parameters. The second section describes how to input utility rate tariffs. The third section describes how to input costs related to energy conservation measures. Because energy conservation measures may have different cost assumptions for different projects (e.g. retrofit vs. new construction), several examples are given for a variety of measures showing how to input costs in different scenarios. All cost values for measure inputs are entered in constant (year 0) dollars regardless of when the costs are incurred.

## Life Cycle Cost Parameters
The most basic parameters needed for a life cycle cost analysis are the analysis period length and the discount rate. A longer analysis period accumulates more energy cost savings than a shorter period; giving energy conservation measures a better pay back relative to their initial costs. A higher discount rate devalues future energy cost savings relative to money spent on capital improvements in the present; giving energy conservation measures a lower pay back relative to their initial costs. Users may set the "Analysis Length" of their project in the PAT application using the "__Set Lifecycle Cost Parameters__" OpenStudio measure. This measure should be added to the simulation workflow as an always run measure so it is applied to all design alternatives in the analysis. If you don't set OpenStudio will create an object with the default length of 25 years.

## Energy Conservation Measure Costs
Once the life cycle economic parameters and utility rate tariffs have been added to the baseline model (or as always run measures in PAT), the next stage in the analysis is determining which energy conservation measures are available to the project and associated costs for each option. Because there are so many different types of energy efficiency measures (OpenStudio allows users to write their own measures) this document will focus on three measures (written by NREL) which span a range of potential options; "__Replace Exterior Window Constructions with a Different Construction from the Model__", "__Reduce Lighting Loads By Percentage__", and "__Add Daylight Sensors__". Each of these measures takes user input for both cost and performance; this document focuses primarily on the cost parameters. A variety of project scenarios (e.g. retrofit vs. new construction) will be explored to see how the same measure can support each scenario by using different cost inputs. Examples in this document will provide a motivating narrative to give background on why cost inputs were set to the values they are. The document will then show how the cost inputs translate to annual cash flows in the life cycle analysis. No attempt has been made to ensure that the cost inputs provided in these examples are reasonable; they are for illustrative purposes only.

### Case 1 - Replace Exterior Window Constructions, New Construction
#### Narrative:
In this example a new construction project is considering two options for window constructions; the code minimum window construction ("Window A") and a higher performance window construction ("Window B"). The energy modeler has created a baseline model which includes both constructions. All windows in the baseline model reference "Window A"; "Window B" is not referenced by any windows in the baseline model. The energy modeler has received inputs from the cost estimator that "Window A" type windows will cost around $8/ft^2 and the "Window B" type windows will cost around $10/ft^2.

#### Inputs:
The first task the energy modeler needs to accomplish is to set the life cycle cost parameters by adding "Set Lifecycle Cost Parameters" as an always run measure to the simulation workflow with the following inputs:

Table 1.1: Set Lifecycle Cost Parameters
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Set the length of the Study Period (years)</td>
  <td>10</td>
  <td>The life cycle analysis uses a 10 year analysis period.</td>
</tr>
</tbody>
</table>

Next the modeler inputs utility rate costs by adding "Xcel EDA Tariff Selection and Model Setup" as an always run measure to the simulation workflow with the following inputs:

Table 1.2: Xcel EDA Tariff Selection and Model Setup
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Electricity Tariff</td>
  <td>Commercial</td>
  <td>Select the small commercial electric utility rate.</td>
</tr>
<tr>
  <td>Gas Tariff</td>
  <td>Small CG</td>
  <td>Select the small commercial gas utility rate.</td>
</tr>
</tbody>
</table>

The next task is to add the cost of the "Window A" construction in the baseline model. This cost will be input by adding "__Add Cost per Area to Construction__" as an always run measure to the simulation workflow with the following inputs:

Table 1.3: Add Cost per Area to Construction
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Choose a Construction to Add Costs For.</td>
  <td>Window A</td>
  <td>Name of the construction to add costs to.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this construction.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Construction per Area Used ($/ft^2).</td>
  <td>8</td>
  <td>Estimated cost for the baseline windows.</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No demolition costs are modeled.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

In this example there is just one baseline construction, but often the baseline model will contain multiple constructions that need costing. This can be accomplished by adding multiple always run measures; with each one addressing a particular construction in the baseline model. The final task is to add the "__Replace Exterior Window Constructions__" measure using the "Window B" construction instead of "Window A". This measure will identify the alternate window construction to use as well as the cost for this construction:

Table 1.4: Replace Exterior Window Constructions
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Pick a Window Construction From the Model to Replace Existing Window Constructions.</td>
  <td>Window B</td>
  <td>Name of the alternate construction to use.</td>
</tr>
<tr>
  <td>Change Fixed Windows?</td>
  <td>True</td>
  <td>Use this alternate construction for exterior fixed windows.</td>
</tr>
<tr>
  <td>Change Operable Windows?</td>
  <td>True</td>
  <td>Use this alternate construction for exterior operable windows.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this construction.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Construction per Area Used ($/ft^2).</td>
  <td>10</td>
  <td>Estimated cost for the alternate windows.</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No demolition costs are modeled.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 100 ft^2 of exterior windows in the model the following cash flows will be generated for the two cases:

Table 1.5: Baseline window cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$800</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$800</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

Table 1.6: High performance window cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$1000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$1000</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

### Case 2 - Replace Exterior Window Constructions, Retrofit Construction
#### Narrative:
In this example a retrofit project is considering replacing the existing code minimum windows ("Window A") with a higher performance window construction ("Window B"). The window retrofit is motivated because the existing windows are nearing the end of their life. In the baseline case the current windows are assumed to be replaced by windows with the same energy performance in 5 years. The energy modeler has created a baseline model which includes both constructions. All windows in the baseline model reference "Window A"; "Window B" is not referenced by any windows in the baseline model. The energy modeler has received inputs from the cost estimator that "Window A" type windows will cost around $8/ft^2 and the "Window B" type windows will cost around $10/ft^2. Demolition of the existing windows will be around $3/ft^2 and the existing windows will have a salvage value of $1/ft^2. Discount factors will be applied to the constant dollar cash flows when computing metrics such as net present value.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selection and Model Setup__" as an always run measure. The next task is to add the cost of the "Window A" construction in the baseline model. This cost will be input by adding "__Add Cost per Area to Construction__" as an always run measure to the simulation workflow with the following inputs:

Table 2.1: Add Cost per Area to Construction
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Choose a Construction to Add Costs For.</td>
  <td>Window A</td>
  <td>Name of the construction to add costs to.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this construction.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Construction per Area Used ($/ft^2).</td>
  <td>8</td>
  <td>Estimated cost for the baseline windows.</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>2</td>
  <td>Demolition cost is $3/ft^2 cost to remove windows minus $1/ft^2 residual value.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>5</td>
  <td>Baseline windows would be replaced in year 5.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>True</td>
  <td>Will have to remove current windows when installing new ones.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>


The final task is to add the "__Replace Exterior Window Constructions__" measure using the "Window B" construction instead of "Window A". This measure will identify the alternate window construction to use as well as the cost for this construction. In this case the existing windows will be demolished and new windows installed in year 0. Therefore, the demolition costs for this measure will be set to match the costs modeled for the baseline case:

Table 2.2: Replace Exterior Window Constructions
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Pick a Window Construction From the Model to Replace Existing Window Constructions.</td>
  <td>Window B</td>
  <td>Name of the alternate construction to use.</td>
</tr>
<tr>
  <td>Change Fixed Windows?</td>
  <td>True</td>
  <td>Use this alternate construction for exterior fixed windows.</td>
</tr>
<tr>
  <td>Change Operable Windows?</td>
  <td>True</td>
  <td>Use this alternate construction for exterior operable windows.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this construction.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Construction per Area Used ($/ft^2).</td>
  <td>10</td>
  <td>Estimated cost for the alternate windows.</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>2</td>
  <td>Same as baseline case.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>True</td>
  <td>Will have to remove current windows when installing new ones.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs should be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 100 ft^2 of exterior windows in the model the following cash flows will be generated for the two cases:

Table 2.3: Baseline window cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$800</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$1000</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

Table 2.4: High performance window cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$1000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$1200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

### Case 3 - Reduce Lighting Load By Percentage, New Construction
#### Narrative:
In this example a new construction project is considering replacing CFL light bulbs in the baseline model with higher performance LED light bulbs. The lighting change is partially motivated because the LED light bulbs have an expected life of 15 years instead of 3 years for the CFLs. The energy modeler has created a baseline model which includes a lighting definition ("Baseline Lighting") that represents the baseline CFLs. The energy modeler estimates that lighting power can be reduced by 30% by moving from CFL to LED light bulbs. The energy modeler has received inputs from the cost estimator that CFL light bulbs will cost around $4/ft^2 and the LED bulbs will cost around $10/ft^2 (150% more than the CFL price). Because the cost for fixtures will be the same in both the baseline and alternate cases, the energy modeler will not include costs for fixtures in the models.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selectionand Model Setup__" as an always run measure. The next task is to add the cost of the CFL bulbs in the baseline model. This cost will be input by adding "__Add Cost per Floor Area to Lights__" as an always run measure to the simulation workflow with the following inputs:

Table 3.1: Add Cost per Floor Area to Lights
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Choose a Watt per Area Lights Definition to Add Costs for.</td>
  <td>Baseline Lighting</td>
  <td>Name of the lighting definition to add costs to.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this definition.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Definition per Floor Area ($/ft^2).</td>
  <td>4</td>
  <td>Estimated cost for the baseline lights.</td>
</tr>
<tr>
  <td>Demolition Costs for Definition per Floor Area ($/ft^2).</td>
  <td>0</td>
  <td>No demolition costs related to removing light bulbs.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new lights.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>4</td>
  <td>The CFL bulbs are expected to last four years.</td>
</tr>
<tr>
  <td>O & M Costs for Definition per Floor Area Used ($/ft^2).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

The final task is to add the "__Reduce Lighting Loads by Percentage__" measure to reduce the lighting power by 30%. This measure is formulated to take arguments in the form of percentage changes from the baseline rather than absolute values:

Table 3.2: Reduce Lighting Loads by Percentage
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Apply the Measure to a Specific Space Type or to the Entire Model.</td>
  <td>Entire Building</td>
  <td>Reduce lighting across the entire building.</td>
</tr>
<tr>
  <td>Lighting Power Reduction Percentage (%).</td>
  <td>30</td>
  <td>Reduce lighting power by 30%.</td>
</tr>
<tr>
  <td>Increase in Material and Installation Cost for Lighting per Floor Area (%).</td>
  <td>150</td>
  <td>Percentage cost increase for the LED bulbs, from $4/ft^2 to $10/ft^2.</td>
</tr>
<tr>
  <td>Increase in Demolition Costs for Lighting per Floor Area (%).</td>
  <td>0</td>
  <td>No increase in demolition costs related to removing light bulbs.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>15</td>
  <td>The LED bulbs are expected to last 15 years. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>Increase in O & M Costs for Lighting per Floor Area (%).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs should be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 100 ft^2 of lighted floor area in the model the following cash flows will be generated for the two cases:

Table 3.3: Baseline CFL bulb cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$400</td>
  <td></td>
  <td></td>
  <td></td>
  <td>$400</td>
  <td></td>
  <td></td>
  <td></td>
  <td>$400</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

Table 3.4: High performance LED bulb cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$1000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$1000</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

### Case 4 - Reduce Lighting Loads By Percentage, Retrofit
#### Narrative:
In this example a retrofit project is considering replacing CFL light bulbs in the baseline model with higher performance linear florescent lighting. The lighting retrofit is more expensive because it requires removing old fixtures and installing new ones. The energy modeler has created a baseline model which includes a lighting definition ("Baseline Lighting") that represents the baseline CFLs. The energy modeler estimates that lighting power can be reduced by 30% by moving from CFL to linear florescent lighting. The energy modeler has received inputs from the cost estimator that CFL light bulbs will cost around $4/ft^2 and have an expected life of 4 years. Removing the current lighting fixtures will cost around $10/ft^2. The new fixtures will cost around $20/ft^2 (400% more than the baseline fixtures) and the linear florescent light bulbs will cost around $3/ft^2 with an expected life of 5 years.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selection and Model Setup__" as an always run measure. The next task is to add the cost of the CFL bulbs in the baseline model. Because the alternate case involves removing and installing lighting fixtures, the modeler chooses to include the cost of light bulbs in the maintenance category in this example. This cost will be input by adding "__Add Cost per Floor Area to Lights__" as an always run measure to the simulation workflow with the following inputs:

Table 4.1: Add Cost per Floor Area to Lights
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Choose a Watt per Area Lights Definition to Add Costs For.</td>
  <td>Baseline Lighting</td>
  <td>Name of the lighting definition to add costs to.</td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any costs previously associated with this definition.</td>
</tr>
<tr>
  <td>Material and Installation Costs for Definition per Floor Area ($/ft^2).</td>
  <td>5</td>
  <td>If the analysis period was long enough this would be incurred.</td>
</tr>
<tr>
  <td>Demolition Costs for Definition  per Floor Area ($/ft^2).</td>
  <td>10</td>
  <td>If the analysis period was long enough this would be incurred.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>15</td>
  <td>In the baseline case the existing lighting fixtures will not be replaced during the analysis period. The bulbs will be changed as an O&M cost.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>True</td>
  <td>Demolition costs will be incurred when construction occurs.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>20</td>
  <td>The expected life of the light fixtures themselves is assumed to be 20 years.</td>
</tr>
<tr>
  <td>O & M Costs for Definition  per Floor Area ($/ft^2).</td>
  <td>4</td>
  <td>Replacing CFL bulbs costs around $4/ft^2.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>4</td>
  <td>CFL bulbs must be replaced every four years.</td>
</tr>
</tbody>
</table>

The final task is to add the "__Reduce Lighting Loads By Percentage__" measure to reduce the lighting power by 30%. This measure will also add costs for removing old fixtures and installing new ones:

Table 4.2: Reduce Lighting Loads By Percentage
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Apply the Measure to a Specific Space Type or to the Entire Model.</td>
  <td>Entire Building</td>
  <td>Reduce lighting across the entire building.</td>
</tr>
<tr>
  <td>Lighting Power Reduction Percentage (%).</td>
  <td>30</td>
  <td>Reduce lighting power by 30%.</td>
</tr>
<tr>
  <td>Increase in Material and Installation Cost for Lighting per Floor Area (%).</td>
  <td>400</td>
  <td>Percentage cost increase of new lighting fixtures relative to existing fixtures.</td>
</tr>
<tr>
  <td>Increase in Demolition Cost for Lighting per Floor Area (%).</td>
  <td>0</td>
  <td>Percentage cost increase of demolition relative to existing fixtures.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>True</td>
  <td>Demolition costs will be incurred when construction occurs.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>20</td>
  <td>The expected life of the light fixtures themselves is assumed to be 20 years.</td>
</tr>
<tr>
  <td>Increase in O & M Costs for Lighting per Floor Area (%).</td>
  <td>-25</td>
  <td>Percentage cost increase of linear fluorescents over CFL bulbs, $3/ft^2-$4/ft^2.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>5</td>
  <td>Linear fluorescents bulbs must be replaced every five years.</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 100 ft^2 of lighted floor area in the model the following cash flows will be generated for the two cases:

Table 4.3: Baseline CFL bulb cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$400</td>
  <td></td>
  <td></td>
  <td></td>
  <td>$400</td>
  <td></td>
  <td></td>
  <td></td>
  <td>$400</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$400</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

Table 4.4: High performance LED bulb cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$2000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td>$1000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$300</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$300</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$300</td>
</tr>
<tr>
  <td>Total</td>
  <td>$3300</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$300</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$300</td>
</tr>
</tbody>
</table>

### Case 5 - Add Daylight Sensors, New Construction
#### Narrative:
In this final example a new construction project is considering adding daylight controls to the baseline model. The energy modeler has created a baseline model which does not include daylight controls. The energy modeler has received inputs from the cost estimator that daylight controls will cost around $500/sensor and have an expected life of 30 years.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selection and Model Setup__" as an always run measure. However, because there are no daylight control sensors in the baseline model there is no need to apply an always run measure to add costs to the baseline model. The final task is to add the "__Add Daylight Sensors__" measure. This measure will add daylight controls to each office space as well as their associated costs:

Table 5.1: Add Daylight Sensors
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Space Type to Add Daylight Sensors in.</td>
  <td>Office</td>
  <td>Add daylight controls to the spaces of Office Space Type</td>
</tr>
<tr>
  <td>Daylighting Setpoint (fc).</td>
  <td>45</td>
  <td>Daylighting control setpoint of 45 footcandles.</td>
</tr>
<tr>
  <td>Daylighting Control Type.</td>
  <td>Continuous/Off</td>
  <td>Daylight controls are allowed to dim lights as well as turn them off.</td>
</tr>
<tr>
  <td>Daylighting Minimum Input Power Fraction (min=0 max=0.6).</td>
  <td>0.3</td>
  <td>Minimum power fraction the daylight controls may dim the lights to.</td>
</tr>
<tr>
  <td>Daylighting Minimum Light Output Fraction (min=0 max=0.6).</td>
  <td>0.2</td>
  <td>Light output fraction at minimum dimming level.</td>
</tr>
<tr>
  <td>Sensor Height (inches).</td>
  <td>30</td>
  <td>Place daylight control sensors at 30 inch workplane.</td>
</tr>
<tr>
  <td>Material and Installation Costs per Sensor ($).</td>
  <td>500</td>
  <td>Cost to install daylight controls</td>
</tr>
<tr>
  <td>Demolition Costs per Sensor ($).</td>
  <td>100</td>
  <td>Demolition costs would be incurred in analysis period was long enough.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The expected life of the daylight controls is assumed to be 30 years.</td>
</tr>
<tr>
  <td>O & M Costs per Sensor ($).</td>
  <td>0</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 10 "Office" spaces in the model the following cash flows will be generated for the two cases:

Table 5.2: Baseline cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

Table 5.3: High performance daylight controls cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$5000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$5000</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
  <td>$0</td>
</tr>
</tbody>
</table>

## Draft examples to serve as design guide for HVAC component costing
### Case 6 - Increase COP of Two Speed DX Cooling Units, New Construction
#### Narrative:
In this example a new construction project is considering two options for two speed dx cooling units; one with the code minimum COP of 3 and a higher performance one with a COP of 4.5. The energy modeler has created a baseline model which includes un-costed two speed dx cooling units. The energy modeler has received inputs from the cost estimator that each baseline DX units will cost around $2000 and the each high performance one will cost around $2500. There are 4 affected units.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selection and Model Setup__" as an always run measure. The next task is to add the cost to the appropriate HVAC components in the baseline model. In this case only two of the three loops are being inspected for two speed DX cooling units. This cost will be input by adding "__Add Cost HVAC Components__" as an always run measure to the simulation workflow with the following inputs:

Table 6.1: Add Cost to baseline two speed DX units.
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Component Type to Add Cost to.</td>
  <td>OS:Coil:Cooling:Dx:</td>
  <td></td>
</tr>
<tr>
  <td>TwoSpeed</td>
  <td>List of all types of HVAC components (or we can make multiple measures each with sub-set of objects)</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 1</td>
  <td>True</td>
  <td>The measure will build up list of loops</td>
</tr>
<tr>
  <td>and the modeler can choose which ones to search for two speed dx units in.</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 2</td>
  <td>True</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 3</td>
  <td>False</td>
  <td></td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any cost associated with the affected components</td>
</tr>
<tr>
  <td>Material and Installation Costs for Building ($).</td>
  <td>2000</td>
  <td>Estimated cost each for the baseline two speed dx cooling units</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>200</td>
  <td>This field will not impact results since it will be 30 years before it is incurred.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>50</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

In this example there is just one baseline cost added , but often the baseline model will contain multiple components with different lifecycle cost cycles that need to be modeled. This can be accomplished by adding multiple always run measures; with each one addressing a particular component or component type in the baseline model. The final task is to add the "__Set COP for Two Speed DX Cooling Units__". This measure will adjust the COP of the selected units:

Table 6.2: Set COP for two speed DX Cooling Units
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Rated High Speed COP</td>
  <td>4.5</td>
  <td></td>
</tr>
<tr>
  <td>Rated Low Speed COP</td>
  <td>4.5</td>
  <td></td>
</tr>
<tr>
  <td>Component Type to Add Cost to.</td>
  <td>OS:Coil:Cooling:Dx:</td>
  <td></td>
</tr>
<tr>
  <td>TwoSpeed</td>
  <td>List of all types of HVAC components (or we can make multiple measures each with sub-set of objects)</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 1</td>
  <td>True</td>
  <td>The measure will build up list of loops</td>
</tr>
<tr>
  <td>and the modeler can choose which ones to search for two speed dx units in.</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 2</td>
  <td>True</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 3</td>
  <td>False</td>
  <td></td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any cost associated with the affected components</td>
</tr>
<tr>
  <td>Material and Installation Costs for Building ($).</td>
  <td>2500</td>
  <td>Estimated cost each for the baseline two speed dx cooling units</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>300</td>
  <td>This field will not impact results since it will be 30 years before it is incurred.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>50</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>3</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 4 affected DX units the following cash flows will be generated for the two cases:

Table 6.3: Baseline DX units cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$8000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
</tr>
<tr>
  <td>Total</td>
  <td>$8200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
</tr>
</tbody>
</table>

Table 6.4: High performance DX units cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$10000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$10200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
</tr>
</tbody>
</table>

### Case 7 - Increase COP of Two Speed DX Cooling Units, Retrofit
#### Narrative:
In this example a retrofit project is considering replacing the two speed dx cooling units in part of the building. The baseline units have a code minimum COP of 3. The retrofit would add units 4.5. The energy modeler has created a baseline model which includes un-costed two speed dx cooling units. The energy modeler has received inputs from the cost estimator that each high performance two speed DX units will cost $2500. There are 4 affected units.

#### Inputs:
As in case 1, the energy modeler sets the analysis period length using "__Set Lifecycle Cost Parameters__" as an always run measure and the utility rates using "__Xcel EDA Tariff Selection and Model Setup__" as an always run measure. The next task is to add the cost to the appropriate HVAC components in the baseline model. In this case only two of the three loops are being inspected for two speed DX cooling units. This cost will be input by adding "__Add Cost HVAC Components__" as an always run measure to the simulation workflow with the following inputs:

Table 7.1: Add Cost to baseline two speed DX units.
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Component Type to Add Cost to.</td>
  <td>OS:Coil:Cooling:Dx:</td>
  <td></td>
</tr>
<tr>
  <td>TwoSpeed</td>
  <td>List of all types of HVAC components (or we can make multiple measures each with sub-set of objects)</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 1</td>
  <td>True</td>
  <td>The measure will build up list of loops</td>
</tr>
<tr>
  <td>and the modeler can choose which ones to search for two speed dx units in.</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 2</td>
  <td>True</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 3</td>
  <td>False</td>
  <td></td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any cost associated with the affected components</td>
</tr>
<tr>
  <td>Material and Installation Costs for Building ($).</td>
  <td>2000</td>
  <td>Estimated cost each for the baseline two speed dx cooling units</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>200</td>
  <td>This field will not impact results since it will be 30 years before it is incurred.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>5</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>False</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>50</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>1</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

In this example there is just one baseline cost added , but often the baseline model will contain multiple components with different lifecycle cost cycles that need to be modeled. This can be accomplished by adding multiple always run measures; with each one addressing a particular component or component type in the baseline model. The final task is to add the "Set COP for Two Speed DX Cooling Units". This measure will adjust the COP of the selected units:

Table 7.2: Set COP for two speed DX Cooling Units
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Argument</th>
  <th>Value</th>
  <th>Note</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Rated High Speed COP</td>
  <td>4.5</td>
  <td></td>
</tr>
<tr>
  <td>Rated Low Speed COP</td>
  <td>4.5</td>
  <td></td>
</tr>
<tr>
  <td>Component Type to Add Cost to.</td>
  <td>OS:Coil:Cooling:Dx:</td>
  <td></td>
</tr>
<tr>
  <td>TwoSpeed</td>
  <td>List of all types of HVAC components (or we can make multiple measures each with sub-set of objects)</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 1</td>
  <td>True</td>
  <td>The measure will build up list of loops</td>
</tr>
<tr>
  <td>and the modeler can choose which ones to search for two speed dx units in.</td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 2</td>
  <td>True</td>
  <td></td>
</tr>
<tr>
  <td>Search in Loop 3</td>
  <td>False</td>
  <td></td>
</tr>
<tr>
  <td>Remove Existing Costs?</td>
  <td>True</td>
  <td>Remove any cost associated with the affected components</td>
</tr>
<tr>
  <td>Material and Installation Costs for Building ($).</td>
  <td>2500</td>
  <td>Estimated cost each for the baseline two speed dx cooling units</td>
</tr>
<tr>
  <td>Demolition Costs for Construction per Area Used ($/ft^2).</td>
  <td>300</td>
  <td>This field will not impact results since it will be 30 years before it is incurred.</td>
</tr>
<tr>
  <td>Years Until Costs Start (whole years).</td>
  <td>0</td>
  <td>Construction costs will be incurred in year 0.</td>
</tr>
<tr>
  <td>Demolition Costs Occur During Initial Construction?</td>
  <td>True</td>
  <td>No demolition required for new construction.</td>
</tr>
<tr>
  <td>Expected Life (whole years).</td>
  <td>30</td>
  <td>The windows are expected to last for 30 years after installation. This will be beyond the analysis period.</td>
</tr>
<tr>
  <td>O & M Costs for Construction per Area Used ($/ft^2).</td>
  <td>50</td>
  <td>No maintenance costs are modeled.</td>
</tr>
<tr>
  <td>O & M Frequency (whole years).</td>
  <td>3</td>
  <td>Maintenance costs would be incurred annually (if there were any).</td>
</tr>
</tbody>
</table>

#### Output:
Assuming that there are 4 affected DX units the following cash flows will be generated for the two cases:

Table 6.3: Baseline DX units cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$8000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
</tr>
<tr>
  <td>Total</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$8400</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
  <td>$200</td>
</tr>
</tbody>
</table>

Table 6.4: High performance DX units cash flows (constant dollar)
<table class="table table-striped table-bordered">
<thead>
<tr>
  <th>Year</th>
  <th>0</th>
  <th>1</th>
  <th>2</th>
  <th>3</th>
  <th>4</th>
  <th>5</th>
  <th>6</th>
  <th>7</th>
  <th>8</th>
  <th>9</th>
  <th>10</th>
</tr>
</thead>
<tbody>
<tr>
  <td>Mat & Inst</td>
  <td>$10000</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>Demo</td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
  <td></td>
</tr>
<tr>
  <td>O&M</td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
  <td></td>
  <td>$200</td>
  <td></td>
</tr>
<tr>
  <td>Total</td>
  <td>$10400</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
  <td>$0</td>
  <td>$200</td>
  <td>$0</td>
</tr>
</tbody>
</table>
