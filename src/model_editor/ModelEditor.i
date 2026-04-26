#ifndef MODELEDITOR_LIB_I
#define MODELEDITOR_LIB_I

#ifdef SWIGPYTHON
%module openstudiomodeleditor
#endif

#define UTILITIES_API
#define MODEL_API
#define MODELEDITOR_API

%include <openstudio/utilities/core/CommonInclude.i>
%import <openstudio/utilities/core/CommonImport.i>
%import <openstudio/utilities/Utilities.i>

#if defined(SWIGCSHARP) || defined(SWIGJAVA)
%import <openstudio/model/Model.i>
#else
%import(module="openstudiomodel") <openstudio/model/Model.hpp>
%import(module="openstudiomodel") <openstudio/model/ModelObject.hpp>
#endif

%{
  #include <model_editor/OSSimpleProgressBar.hpp>
  #include <openstudio_qt_utils/Utilities.hpp>

  #include <openstudio/model/Model.hpp>
  #include <openstudio/model/ModelObject.hpp>

  using namespace openstudio;
  using namespace openstudio::model;

  // to be ignored
  namespace openstudio{
    class ProgressBar;
    class UpdateManager;
    class IdfObjectWatcher;
    class BCL;
    class RemoteBCL;
    class LocalBCL;
    class WorkspaceObjectWatcher;
    class WorkspaceWatcher;
  }
%}

%feature("director") OSSimpleProgressBar;
%include <model_editor/OSSimpleProgressBar.hpp>

#endif //MODELEDITOR_LIB_I
