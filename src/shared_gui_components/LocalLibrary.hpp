/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_LOCALLIBRARY_HPP
#define SHAREDGUICOMPONENTS_LOCALLIBRARY_HPP

#include <openstudio/utilities/core/Enum.hpp>

namespace openstudio {

struct LocalLibrary
{
  enum LibrarySource
  {
    BCL,       // from the bcl
    USER,      // from the user
    COMBINED,  // from all sources
    OS         // installed with openstudio
  };
};

OPENSTUDIO_ENUM(AspectRatio, ((Square))((Rectangle)));

OPENSTUDIO_ENUM(OSItemType,
                ((CollapsibleListHeader))  // has the arrow
                ((ListItem))               // regular item
                ((LibraryItem))            // library or my model item
                ((DropzoneSquare))         // in scrolling drop zone
                ((DropzoneRectangle))      // for fixed drop zone
);

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_LOCALLIBRARY_HPP
