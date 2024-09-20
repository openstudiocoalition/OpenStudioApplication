/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_ICONLIBRARY_HPP
#define OPENSTUDIO_ICONLIBRARY_HPP

#include "OpenStudioAPI.hpp"
#include <openstudio/utilities/core/Logger.hpp>

#include <QPixmap>
#include <map>

namespace openstudio {

/*! This class is a simple singleton that maps IddObjectType.value() to QPixmaps
 *
 *
 * Pretty straightforward. The class stores pointers, there is no real advantage to using pointers over by value semantics
 * since Qt is doing internal reference counting in QPixMap anyway. Just check the pointer when you do a find operation on
 * it, because it might return NULL.
 *
 */
class OPENSTUDIO_API IconLibrary
{

 public:
  //! If the class in not instantiated, this call will instantiate it. (So don't ever worry about it, just call Instance when you want to use the class)
  static IconLibrary& Instance();
  /*! gets an Icon
   \param val should be an IddObjectType.value()
   \return either a pointer to the Icon, or NULL if it isn't found
  */
  const QPixmap* findIcon(unsigned int val) const;
  /*! gets a  MiniIcon
   \param val should be an IddObjectType.value()
   \return either a pointer to the MiniIcon, or NULL if it isn't found
  */
  const QPixmap* findMiniIcon(unsigned int val) const;

 protected:
 private:
  REGISTER_LOGGER("openstudio.openstudio_lib.IconLibrary");

  IconLibrary();
  IconLibrary(const IconLibrary&);
  IconLibrary& operator=(const IconLibrary&);

  static IconLibrary* s_instance;

  std::map<unsigned int, QPixmap*> m_icons;
  std::map<unsigned int, QPixmap*> m_miniIcons;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_ICONLIBRARY_HPP
