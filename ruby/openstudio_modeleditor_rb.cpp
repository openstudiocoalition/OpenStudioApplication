/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include <RubyAPI.hpp>
#include <QtPlugin>
#include <iostream>
#include <ruby.h>

//#if defined(__APPLE__)
//  Q_IMPORT_PLUGIN(QCocoaIntegrationPlugin);
//#elif (defined (_WIN32) || defined (_WIN64))
//  Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin);
//#elif defined(__linux__)
//  Q_IMPORT_PLUGIN(QXcbIntegrationPlugin);
//#endif

inline void initResources() {

#ifndef SHARED_OS_LIBS
  Q_INIT_RESOURCE(modeleditorlib);
#endif  // SHARED_OS_LIBS
}

extern "C"
{

  void Init_openstudiomodeleditor(void);

  RUBY_API void Init_openstudio_modeleditor(void) {

    initResources();

    Init_openstudiomodeleditor();
    rb_provide("openstudiomodeleditor");
    rb_provide("openstudiomodeleditor.so");
  }
}
