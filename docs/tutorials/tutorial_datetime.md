<h1>Date and Time in OpenStudio</h1>

## Dates in OpenStudio

Energy modeling has traditionally been split between applications modeling the operation of buildings during typical years and those modeling buildings during actual years.  Actual year simulation is most commonly used when calibrating simulated energy use against past measured energy use over a period of time.  Typical year simulation is used when making predictions of future energy use.  This all makes sense, we know what past weather and other conditions were but we cannot yet predict the future [reference needed].  The majority of energy modeling applications use the typical year simulation. Because typical year simulations occur in a "typical" year and not an actual year, energy modelers have become accustomed to working with dates that do not specify a year.  This can be seen in the EnergyPlus RunPeriod object (which includes fields 'Begin Month', 'Begin Day of Month', 'End Month', 'End Day of Month', and 'Day of Week for Start Day' but no 'Begin Year' or 'End Year' fields) as well as the EnergyPlus outputs (timestamps in the ESO file do not include years).

This practice leads to problems when trying to leverage Date/Time aware software libraries to process EnergyPlus simulation input and output.  Most software libraries require Date/Times to specify an actual year as well as a time zone.  This is important for real world applications like meeting someone at a particular date and time.  Therefore, in order to use a software calendar widget to create a schedule or to use a flashy html timeseries plot (which we want to do for actual as well as typical year simulations) we have to have a year in addition to the month and day that are required for EnergyPlus simulation (and are available in the EnergyPlus output). OpenStudio has created a scheme to come up with a year for all simulation models (typical or actual) in order to interface with these Date/Time software libraries.

### RunPeriod

The OpenStudio RunPeriod object determines the time period for which to run the simulation.  Like EnergyPlus, the run period is specified with the start day and month and the end day and month for the simulation.  Unlike EnergyPlus, OpenStudio only allows for one unique RunPeriod object in each OpenStudio Model.  The maximum length of a RunPeriod in OpenStudio is one year.  If the start day and month is less than the end day and month, then the start date is assumed to be in the same year as the end date.  If the end day and month is less than the start day and month, then the end date is assumed to be in the year following the start date's year.

### YearDescription

In addition to the RunPeriod object, the OpenStudio Model includes a unique YearDescription object.  This object specifies either an actual year OR the day of the week of the first day in the simulation and whether or not the year of the first day of the simulation is a leap year.  If the user specifies the year in the second format (e.g. simulation starts on a Monday of a year that is not a leap year), OpenStudio searches either forward or backwards in time to find an actual year which meets these criteria.  The default year if no criteria are entered is 2009 which was the birth year of OpenStudio!

<!--- There is a bug, #1895, in OpenStudio here, openstudio::YearDescription uses Jan 1 where we really want to specify the day of week of the day the simulation starts --->

The YearDescription object includes the `makeDate` method which can be used to create a Date object with the same assumed base year as the rest of the OpenStudio model. Any dates returned by the OpenStudio Model API (e.g. from schedules) will be created using the makeDate method of the current YearDescription object.  If the YearDescription object is updated, you will need to recompute any dates for the current model.  Similarly, any dates passed into the OpenStudio Model API (e.g. for schedules) will be checked against the current YearDescription object.  If you did not make these dates using the makeDate method you may see a warning about "Comparing dates with improper base years".

There is currently a deficiency in the OpenStudio SqlFile class.  Timeseries results returned from the SqlFile do not use the YearDescription object.  Therefore, timeseries results returned from the SqlFile will contain dates that appear to be from 2009.  This can be a particular issue if the simulation period includes a leap day. <!--- Issue #817 --->

### Weather Data

The idea of actual vs typical years also appears in the weather files used for simulation.  Typical Meteorological Year (TMY) weather files are assembled by combining weather data from different months across multiple years.  Actual  Meteorological Year (AMY) weather files contain weather data for a contiguous period.  The EpwFile class detects whether a given EPW file is a TMY or AMY when it is read, the `isActual` method returns true if the weather file includes actual year data.  The RunPeriod and YearDescription objects are reset to match the weather file when assigning a new EPW file using the OpenStudio Application.  Any dates returned from the EpwFile class will be constructed using the year specified within the EPW file, the OpenStudio Model's YearDescription object is not used.

### Utility Bills

Utility bill data entered in OpenStudio to support calibration against actual measured energy use is always specified using an actual date.  In order to compare data in the simulation results with measured utility data, the simulation dates (as specified by the RunPeriod and YearDescription objects) must overlap the billing period dates.

## Time in OpenStudio

### Timeseries

Coming soon

