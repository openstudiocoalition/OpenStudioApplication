/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#include "OSQObjectController.hpp"

#include <openstudio/utilities/core/Assert.hpp>

#include <algorithm>

namespace openstudio {

OSQObjectController::OSQObjectController() : QObject() {}

OSQObjectController::~OSQObjectController() {
  auto it = m_objects.begin();
  auto itend = m_objects.end();
  for (; it != itend; ++it) {
    if (*it) {
      delete *it;
    }
  }
}

void OSQObjectController::addQObject(QObject* object) {
  m_objects.push_back(object);

  connect(object, &QObject::destroyed, this, &OSQObjectController::onObjectDestroyed);
}

void OSQObjectController::onObjectDestroyed(QObject* object) {
  auto it = find(m_objects.begin(), m_objects.end(), object);

  if (it != m_objects.end()) {
    *it = nullptr;
  }
}

}  // namespace openstudio
