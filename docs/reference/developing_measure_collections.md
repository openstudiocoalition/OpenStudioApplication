#Developing Measure Collections

When writing a collection of measures meant to be used in conjunction with each other, it's a good idea to develop an overall plan for the workflow. This page provides guidance on best practices to manage coordination between measures.

## Identify Measure Use Cases

Think about what you are trying to accomplish and how the measures will be used; this will greatly impact the design process. Below are some example use cases for a measure collection:

- Fully automated model generation from an empty seed model
- Using a generic completed model as a "seed model", altering it to follow a set of prescribed guidelines such as the Advanced Energy Design Guides
- Running a collection of input diagnostics such as a collection of "pre-flight" measures

## Define Result Requirements

- What should the resulting model look like? Can it be simple, with power per area loads or do you need to model individual pieces of equipment?
- Is the model being used in a controlled environment where you know what the seed models will look like, or is it meant for public consumption where it needs to gracefully handle any model or inputs thrown at it?
- What kinds of variables will you want to change?
- Will a single person write all of the measures, or will a team be developing them?

## Outline the Workflow

Start with what the seed model looks like, and then move into what will be added or altered in the model. Think about how much modularity you want in your measures, there is a "sweet spot"; too much or too little modularity will make more work for the team. Think about where you can use (or adapt) off-the-shelf measures vs. writing your own.

Generally speaking, think about the order in which you would add to, and modify, your model manually. A modeler typically would not add an HVAC system before there are thermal zones in the model, for example. Here we present a basic example workflow:

- seed model will be pre-loaded with weather file and spaces with stub space types assigned
- based on stub space types, add real space types for a user selected standard
- add a construction set for the building based on a user selected standard and climate zone. Optionally allow user to pick the type of exterior wall (mass, steel frame, etc.).
- rotate building
- add fenestration and overhangs based on user specified targets (use multiple instances of the same measure with different input azimuths (and overhang depths) for each instance)
- create thermal zones. Need to decide if one zone per space, or if space should be merged. Maybe define this and stories in the seed model?
- add HVAC systems
- add service water heating
- add exterior lights
- add reporting measures

## Create Design Documents

Write a basic design document for each measure describing the general intent of the measure and how it will work. It should list descriptions, the arguments, and the log messages that will be reported when the measure is run. It should also include an outline of the code from the run section of the measure. It can also include information on testing. You can use [this example design document](https://docs.google.com/document/d/1TlFS-uvRTDkT26uU49sJGAyxhSCu6efo56X8nZMqe7I/edit#) for the "Hello World" measure as a starting point.

## Writing the Measures

Once you have written the design documents and reviewed them as a collection to make sure they will function together correctly, you are ready to write the measures. Some measures are simple and can be written in less than an hour, but sometimes they are more complex and may take a number of days or more to write. When the measure is going to take a long time, try not to get lost in the details initially. Make a simplified first pass that runs, even if it is with limited functionality. This allows you to start to test the measures in a full workflow early on. The workflow testing is an important milestone as it validates that the overall logic of the process is working. It also allows multiple people to add detail and functionality on individual measures while maintaining a test of the full workflow.

Having a version control system like Git or SVN is very useful for managing the coding process, in particular if a team will be working on the project. Once the workflow testing is passing you should try not to break it on the main branch of the repository. When breaking a measure in the process of adding more functionality; use your local checkout on your computer or on a branch in a repository. This isolates everyone else and the workflow testing from any instability.

## Testing the Entire Workflow

Workflow testing shouldn't begin after all the code is written, it should be up and running as soon as possible. Each measure already has its own unit test, but _workflow testing_ can provide a complete test of all of the measures used, in their intended combinations. Below are few options for workflow testing.

- Use the "Measures" section of the OpenStudio application. (This isn't recommended because measure reporting in the OpenStudio application isn't as accessible as it is in PAT)
- Use PAT with only "Always Run" Measures (this will produce only a single model and simulation. This is ok, but having multiple Paths for argument values is ideal
- Use PAT with a combination of "Always Run" and "Measure Group" measures. This will allow you to test more scenarios.
- Use the OpenStudio Analysis spreadsheet [described here](https://github.com/NREL/OpenStudio-analysis-spreadsheet). You can export a PAT project to the spreadsheet as a starting point.
- The OpenStudio developers are working on a scripted workflow testing solution that allows many tests to be run without having to trigger simulations. This is very useful for times when you want to inspect the model or the measure logging to see if it looks like it should. Because this is text based it is more manageable in version control than OpenStudio projects or Excel spreadsheets. When this is ready for use we will link to it from this documentation.

