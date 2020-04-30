<h1>OpenStudio Command Line Interface</h1>

The OpenStudio Command Line Interface (CLI) is a self-contained executable which includes everything needed to apply OpenStudio Measures to an OpenStudio Model and run an EnergyPlus simulation.  The CLI is only avaiable in OpenStudio version 2.0 and higher. The CLI contains a full OpenStudio Ruby environment, the list of Ruby Gems available in each version of OpenStudio can be found [here](https://github.com/NREL/OpenStudio/wiki/OpenStudio-Version-Compatibility-Matrix).

This document provides an overview of the most important features of the CLI, it is not meant to be an exhaustive reference.  For an exhaustive list of features available in the CLI please refer to the command line output of the `--help` command.  For a complete list of the properties available in the OSW file format please refer to the [OSW JSON schema](https://raw.githubusercontent.com/NREL/OpenStudio-workflow-gem/develop/spec/schema/osw_output.json).  For a complete description of the WorkflowJSON class please refer to the [documentation](https://github.com/NREL/OpenStudio/tree/develop/openstudiocore/src/utilities/filetypes/WorkflowJSON.hpp).

# Command Overview

The CLI is executed by calling the OpenStudio executable from the command line with a set of command line arguments.  Calling the CLI with no arguments (such as when double-clicking the executable) causes the CLI to print a help message and then exit.  Several switches which control program behavior may be passed immediately following the OpenStudio executable.  Switches often have both a short form and a long form for convienence. Multiple program level switches may be combined.  Program level switches include:

The `-h` or `--help` switches print the help message:

```
openstudio.exe --help
```

The `-I` or `--include` switches can be used to add additional directories to the Ruby $LOAD_PATH (this switch may be used more than once):

```
openstudio.exe --include /path/to/add/ --include /another/path/to/add
```

The `--gem_path` switch can be used to load gems from a location on the user's disk (this switch may be used more than once):

```
openstudio.exe --gem_path /home/gems1 --include /home/gems2
```

The `--gem_home` switch can be used to set the directory to install gems to (note that the `gem_install` command does not yet work):

```
openstudio.exe --gem_home /home/gems
```

The `--verbose` switch can be used to print additional information for debugging:

```
openstudio.exe --verbose
```

Program level switches (if any) are followed by a subcommand which directs the program to perform a particular function.  These subcommands are explained in further detail below.


# Measure Subcommand

The `measure` subcommand is used to query OpenStudio Measures on disk.  The following arguments may be passed to the `measure` subcommand.

The `-h` switch prints help specific to the `measure` subcommand:

```
openstudio.exe measure -h
```

The `-t` or `--update_all` switches process all measures in a given directory and updates their measure.xml files if updates to their content are detected.  If present, each measure's README.md.erb file will be configured and saved to a README.md file. Information about the measures is printed to standard out in JSON format:

```
openstudio.exe measure --update_all /path/to/measures/
```

The `-u` or `--update` switches process a single measure in a given directory and updates its measure.xml file if updates to the content are detected. If present, the measure's README.md.erb file will be configured and saved to a README.md file. Information about the measure is printed to standard out in JSON format:

```
openstudio.exe measure --update /path/to/measure/
```

The `-a` or `--compute_arguments` switches process a single measure in a given directory and updates its measure.xml file if updates to the content are detected. Information about the measure, including model specific arguments, is printed to standard out in JSON format:

```
openstudio.exe measure --compute_arguments /path/to/model.osm /path/to/measure/
```

or for an EnergyPlus Measure:

```
openstudio.exe measure --compute_arguments /path/to/model.idf /path/to/measure/
```

The `-r` or `--run_tests` will find all measures the given directory, run their associated tests, and save results to a dashboard under a `test_results` directory in the measure directory:

```
openstudio.exe measure --run_tests /path/to/measure_dir/
```


# Run Subcommand

The `run` subcommand is used to load an OpenStudio Model, apply a series of OpenStudio Model Measures, translate to EnergyPlus IDF, apply a series of OpenStudio EnergyPlus Measures, run an EnergyPlus simulation on the resulting model, and finally apply a series of OpenStudio Reporting Measures. The OpenStudio Workflow (OSW) file format is used to describe which OpenStudio Measures to apply to the model and what arguments to pass to each measure.  The OSW format is explained in the following section.

The `-h` switch prints help specific to the `run` subcommand:

```
openstudio.exe run -h
```

The `-w` or `--workflow` switches run the complete simulation workflow as described in an OSW file:

```
openstudio.exe run --workflow /path/to/workflow.osw
```

The `--debug` switch can be used to include additional outputs for debugging failing workflows and does not clean up the run directory:

```
openstudio.exe run --debug --workflow /path/to/workflow.osw
```

The `-m` or `--measures_only` switches run only the OpenStudio Model and EnergyPlus Measures but do not run the EnergyPlus simulation or OpenStudio Reporting Measures in an OSW file:

```
openstudio.exe run --measures_only --workflow /path/to/workflow.osw
```

The `-p` or `--postprocess_only` switches do not run the OpenStudio Model or EnergyPlus Measures, or EnergyPlus simulation in an OSW file.  Existing simulation results are loaded and only OpenStudio Reporting Measures are run, this is useful for generating new reports without re-running simulations:

```
openstudio.exe run --postprocess_only --workflow /path/to/workflow.osw
```

# OSW Structure

The OpenStudio Workflow (OSW) format is a JSON file format that describes a simulation workflow.  In an OpenStudio Workflow, a seed OpenStudio Model is loaded.  OpenStudio Model Measures are applied to the seed model.  After these measures, the OpenStudio Model is translated to EnergyPlus IDF format.  Once in EnergyPlus IDF format, OpenStudio EnergyPlus Measures are applied.  After these measures, the EnergyPlus simulation is executed.  Once the EnergyPlus simulation is complete, OpenStudio Reporting Measures are applied which generate reports.  An error at any point in the workflow will halt the workflow.  Once the workflow is completed (successfully or unsuccessfully) an output OSW file is written which contains output related to running the workflow.

An example OSW project is included in the OpenStudio installer under './Examples/compact_osw'. To run this example, copy it to a user writable location and then:

/path/to/openstudio.exe run -w /path/to/compact.osw

The OSW file format is described in JSON schema format [here](https://raw.githubusercontent.com/NREL/OpenStudio-workflow-gem/develop/spec/schema/osw_output.json).

The OpenStudio API includes the class [WorkflowJSON](https://github.com/NREL/OpenStudio/tree/develop/openstudiocore/src/utilities/filetypes/WorkflowJSON.hpp) which is able read, modify, and write the OSW file format.  The WorkflowJSON class also includes features for finding measures and files associated with a simulation workflow.

The WorkflowJSON associated with an OpenStudio Model can be accessed using `model.workflowJSON`.  The WorkflowJSON object can also be accessed from the OSRunner object within a measure using `runner.workflow`.  Within the context of a measure, the WorkflowJSON object can be used to find files and measures associated with the simulation.  It can also be used to tell which OpenStudio Measures will be run during the workflow, what arguments they will be passed, and capture output from previously run measures.  However, during the course of a simulation workflow the OSW associated with an OpenStudio Model or OSRunner cannot be altered.

An example OSW is shown below, the meaning of key terms is explained in more detail later in this section:

```json
{
  "seed_file": "baseline.osm",
  "weather_file": "USA_CO_Golden-NREL.724666_TMY3.epw",
  "steps": [
    {
      "measure_dir_name": "IncreaseWallRValue",
      "arguments": {}
    },
    {
      "measure_dir_name": "IncreaseRoofRValue",
      "arguments": {
        "r_value": 45
      }
    },
    {
      "measure_dir_name": "SetEplusInfiltration",
      "arguments": {
        "flowPerZoneFloorArea": 10.76
      }
    },
    {
      "measure_dir_name": "DencityReports",
      "arguments": {
        "output_format": "CSV"
      }
    }
  ]
}
```

## Seed File

The seed file is the file name of the OpenStudio Model to be loaded at the beginning of the simulation workflow.  The seed model may be empty or the memeber may be missing, in this case a newly constructed OpenStudio Model is passed to the first OpenStudio Model Measure.  The seed model is found using the logic documented for `WorkflowJSON::findFile`.

## Weather File

The weather file is the file name of the EnergyPlus Weather (EPW) file loaded at the beginning of the simulation workflow.  The weather file may be empty or the memeber may be missing.  The weather file is found using the logic documented for `WorkflowJSON::findFile`.  The following logic applies to the weather file during a simulation workflow:

1. If a weather file is specified in the OSW, that file replaces any weather file specified in the seed OpenStudio Model before measure processing begins.
2. During OpenStudio Model Measure processing, the weather file may be changed by altering the WeatherFile object in the OpenStudio Model.
3. On translation to EnergyPlus, the weather file referenced in the OSM is found using WorkflowJSON.findFile and copied to in.epw in the run directory.
EnergyPlus measures may change the weather file by copying files on top of in.epw.

## Workflow Steps

Each step listed in the OSW file describes an OpenStudio Measure to apply.  Measures are applied in order and must progress from OpenStudio Model Measures to OpenStudio EnergyPlus Measures to OpenStudio Reporting Measures.  Each step lists a `measure_dir_name` which is the directory name of an OpenStudio Measure to apply. Measures are found at run time according to logic in `WorkflowJSON::findMeasure`.  Each step specifies arguments to be passed to the measure, the measure argument's name is the key and the value to pass is the measure.  Optional arguments may be ommitted, default values will be used in this case.  The value passed to choice arguments may be either a valid choice value or a valid choice value display name.

# Environment Variables

The OpenStudio Command Line Interface can be configured with several environment variables.  In cases where a value can be configured with either a command line switch or an environment variable, the command line switch will take precedence. Custom environment variables configurations do not impact local or remote PAT runs.

The `ENERGYPLUS_EXE_PATH` environment variable can be used to configure the CLI to use a different version of the EnergyPlus engine than the one OpenStudio is installed with:

Windows
```
set ENERGYPLUS_EXE_PATH=/path/to/EnergyPlus.exe
```

Unix
```
export ENERGYPLUS_EXE_PATH=/path/to/energyplus
```

The `RUBYLIB` environment variable can be used to configure the CLI to require Ruby files from locations on the user's disk. Locations passed using the `--include` switch take precedence over the paths found in this environment variable:

Windows
```
set RUBYLIB=/home/git/project;/home/git/project/app/helpers
```

Unix
```
export RUBYLIB=/home/git/project:/home/git/project/app/helpers
```

The `GEM_PATH` environment variable can be used to configure the CLI to load gems from a location on the user's disk. Locations passed using the `--gem_path` switch take precedence over the paths found in this environment variable:

Windows
```
set GEM_PATH=/home/gems1;/home/gems2
```

Unix
```
export GEM_PATH=/home/gems1:/home/gems2
```

The `GEM_HOME` environment variable can be used to configure the CLI to install gems to a location on the user's disk. The location passed using the `--gem_home` switch takes precedence over the path found in this environment variable (note that the `gem_install` command does not yet work):

Windows
```
set GEM_HOME=/home/gems
```

Unix
```
export GEM_HOME=/home/gems
```

These same environment variables apply when requiring `openstudio.rb` from system ruby.  Note, that requiring `openstudio.rb` from system ruby does not bring in any of the default gems that are embedded inside the CLI (i.e. the `openstudio-standards` gem).  You must install these gems separately to ensure they are available when using system ruby.  The standard way to do this is with [Bundler](http://bundler.io/).  A Gemfile is included in the Ruby directory of the OpenStudio installation, this can be used as a reference when setting up your own Gemfile to ensure that gems are compatible with the version of OpenStudio.

# Loading Custom Gems

Developers commonly need to be able to override the version of a gem embedded in the OpenStudio CLI with one that they are working on.  Additionally, measure writers may wish to use gems that are not distributed with the OpenStudio CLI.  Neither of these use cases are an issue when using system ruby as the OpenStudio ruby bindings (i.e. `openstudio.rb`) do not have embedded gems, bundler can be used to specify gem versions in this case.  When using the CLI there are two ways to override an embedded gem or provide access to another gem.  The first is to use the `--include` switch to add the directory containing the primary gem file (e.g. the directory containing `openstudio-standards.rb`) to the ruby load path. The second is to install the gem to your system location, then use the `--gem_path` switch to include this location (e.g. `C:\ruby-2.2.4-x64-mingw32\lib\ruby\gems\2.2.0\`). Note that the gem path directory should have `specifications`, `gems`, and other subdirectories in it.  Also, note that when using this second approach, a system gem will only override the embedded gem if the version is equal or greater to the embedded gem and the major version is the same.  It can be useful to use the `--verbose` and `-e` switches to print some information that can verify the correct version of the gem is being loaded:

```
openstudio --verbose -I \openstudio-standards\openstudio-standards\lib\ -e "require 'openstudio-standards'" -e "puts OpenstudioStandards::VERSION"
```

```
openstudio --verbose --gem_path C:\ruby-2.2.4-x64-mingw32\lib\ruby\gems\2.2.0\ -e "require 'openstudio-standards'" -e "puts OpenstudioStandards::VERSION"
```

# Using the Ruby bindings instead

The OpenStudio CLI is suggested for most users.  However, there are some cases where the OpenStudio Ruby bindings can be used directly.  The OpenStudio CLI has a Ruby interpreter built directly into it.  To use the OpenStudio Ruby bindings you must install a Ruby interpreter manually, the version of Ruby installed must match the version the OpenStudio Ruby bindings were built against as described in the [OpenStudio Version Compatibility Matrix](https://github.com/NREL/OpenStudio/wiki/OpenStudio-Version-Compatibility-Matrix). Compatible Ruby versions can be downloaded (for Windows)[https://rubyinstaller.org/downloads/archives/] and (for Mac)[http://rvm.io/]. Ruby can be installed on Ubunutu using `apt-get install ruby2.2`.

*Ruby gems with native MinGW extensions (the default on Windows) are not compatible with the OpenStudio CLI which is built against the MSWin Ruby runtime.  These gems can only be used with the OpenStudio Ruby bindings using the MinGW Ruby runtime.*

When using the OpenStudio CLI, the OpenStudio Ruby bindings and a number of other Ruby Gems are automatically loaded when the CLI is invoked.  When using the OpenStudio Ruby bindings these dependencies must be manually configured and explicitly loaded with require statements.  The best way to do manage your Ruby gems is with a [Gemfile](http://bundler.io/gemfile.html) and [Bundler](http://bundler.io/).  A Gemfile specifies the versions of Ruby gems to use when running your application.  The Bundler gem is used to install and manage these gems, it can also be used to invoke Ruby with the correct path to all gems.  To install Bundler, first install Ruby, make sure the correct version of Ruby is in your path, then:

```
gem install bundler
```

Next, write a Gemfile that specifies all dependencies for your application.  Gemfiles corresponding to the dependencies for OpenStudio releases may be found in the [OpenStudio Version Compatibility Matrix](https://github.com/NREL/OpenStudio/wiki/OpenStudio-Version-Compatibility-Matrix) and used as a starting point.  Note, you may want to relax or remove the Bundler gem dependency if you are using a different version of Bundler.  Place your Gemfile in a directory specific to your application and run:

```
bundle install
bundle update
```

To run a Ruby script with the Ruby gems specified in your Gemfile, call `bundle exec` before invoking Ruby in the same directory as your Gemfile:

```
bundle update ruby my_script.rb
```

If you update your Gemfile simply do:

```
bundle update
```

Now you can run any Ruby script with the OpenStudio Ruby bindings and other Ruby gems of your choice.  Note that the OpenStudio Ruby bindings are not packaged as a Ruby gem so the version of the OpenStudio Ruby bindings is not specified in the Gemfile.  Instead, the version of the OpenStudio Ruby bindings loaded can be controlled using the `-I` Ruby command line option.  You may use the `openstudio_cli.rb` Ruby script with all of the same command line options as the OpenStudio CLI described above.

```
bundle exec ruby -I c:\openstudio-2.4.0\Ruby\ c:\openstudio-2.4.0\Ruby\openstudio_cli.rb run -w in.osw
```

# Loading Custom Gems with Bundler in the CLI

As of OpenStudio 2.7.0, the CLI can load a pre-made gem bundle made with Bundler.  As with the Ruby bindings, a Ruby interpreter must be installed manually along with the Bundler gem.  Once this is done, a bundle can be made by executing the following in a directory with your Gemfile:

```
bundle install --path /path/to/gem/bundle
```

The `--path` argument above tells Bundler where to store your bundled gems.  On Windows, it may also be required to run the following command to install a list of gems compatible with the OpenStudio CLI:

```
bundle lock --add_platform ruby
```

*Ruby gems with native MinGW extensions (the default on Windows) are not compatible with the OpenStudio CLI which is built against the MSWin Ruby runtime.  These gems can only be used with the OpenStudio Ruby bindings using the MinGW Ruby runtime.*

Once these steps are complete, you can execute the OpenStudio CLI with the following command line switches (both switches below must be present when using pre-made gem bundles in the CLI):

The `--bundle` switch is used to point the OpenStudio CLI to your Gemfile, the `BUNDLE_GEMFILE` environment variable may also be set but is overridden by the command line switch.

The `--bundle_path` switch is used to point the OpenStudio CLI to your premade bundle, the `BUNDLE_PATH` environment variable may also be set but is overridden by the command line switch.

The `--bundle_without` switch is used to tell the OpenStudio CLI not to load gems in specific groups, the `BUNDLE_WITHOUT` environment variable may also be set but is overridden by the command line switch.  The argument to this switch is a space separated list of groups for bundler to exclude. Surround multiple groups with quotes like `"test development"`.

For example, the following commands would run the workflow in `in.osw` using the pre-made gem bundle at `/path/to/gem/bundle` described by the Gemfile at `./Gemfile` without the groups `test` and `development`:

```
openstudio.exe --bundle ./Gemfile --bundle_path /path/to/gem/bundle --bundle_without "test development" run -w /path/to/in.osw
```

# Calling the CLI from a separate Ruby application with its own Gemfile

In some cases, the OpenStudio CLI may be called from a separate Ruby application with its own Gemfile.  For example, the OpenStudio Server is a Ruby on Rails application.  The [OpenStudio Server Gemfile](https://github.com/NREL/OpenStudio-server/blob/develop/server/Gemfile) includes Ruby gems that OpenStudio Server needs for talking with clients over http, communicating with the database, and other tasks.

The OpenStudio Server is invoked using a call to `bundle exec`, this call [modifies environment variables](https://bundler.io/man/bundle-exec.1.html#ENVIRONMENT-MODIFICATIONS) in the OpenStudio Server process.  This includes setting the `BUNDLE_GEMFILE` and `BUNDLE_PATH` environment variables.  If the CLI is invoked in this environment, it will read these environment variables and think that it is being requested to load these gems.

If this is not desired, the user can use the command line switches to point to a separate bundle for the CLI.  If the user wants the CLI to run without loading a premade bundle, they must clear these environment variables in the child process that the CLI is launched in.  The [OpenStudio::Extension::Runner::get_clean_env](https://github.com/NREL/openstudio-extension-gem/blob/develop/lib/openstudio/extension/runner.rb) method returns a hash which will clear these environment variables when passed to Ruby's [Process::spawn](https://www.rubydoc.info/stdlib/core/Process.spawn) or [Open3](https://docs.ruby-lang.org/en/2.0.0/Open3.html) methods to launch the CLI in a child process.

For example, to run an OSW in a clean environment (the `--verbose` flag can be useful when diagnosing these issues):

```
def get_clean_env
  # blank out bundler and gem path modifications, will be re-setup by new call
  new_env = {}
  new_env['BUNDLER_ORIG_MANPATH'] = nil
  new_env['BUNDLER_ORIG_PATH'] = nil
  new_env['BUNDLER_VERSION'] = nil
  new_env['BUNDLE_BIN_PATH'] = nil
  new_env['RUBYLIB'] = nil
  new_env['RUBYOPT'] = nil
  new_env['GEM_PATH'] = nil
  new_env['GEM_HOME'] = nil
  new_env['BUNDLE_GEMFILE'] = nil
  new_env['BUNDLE_PATH'] = nil
  new_env['BUNDLE_WITHOUT'] = nil

  return new_env
end

cli = OpenStudio.getOpenStudioCLI

command = "#{cli} --verbose run -w '#{osw_path}'"

stdout_str, stderr_str, status = Open3.capture3(get_clean_env, command)
```
