/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_OSQOBJECTCONTROLLER_HPP
#define SHAREDGUICOMPONENTS_OSQOBJECTCONTROLLER_HPP

#include <QObject>
#include <openstudio/nano/nano_signal_slot.hpp>  // Signal-Slot replacement

#include <vector>

namespace openstudio {

class OSQObjectController
  : public QObject
  , public Nano::Observer
{
  Q_OBJECT

 public:
  /** OSQObjectController manages the life cycle of dynamically allocated QObjects when they
   *  are not part of a parent child hierarchy.  This is the case for top level QWidgets
   *  owned by a controller.
   **/
  OSQObjectController();

  virtual ~OSQObjectController();

 protected:
  /** Registers the QObject so that its lifecycle is managed.
   *  When OSQObjectController is deleted the managed QObjects will safely be removed.
   **/
  void addQObject(QObject* object);

 private slots:

  void onObjectDestroyed(QObject* object = nullptr);

 private:
  std::vector<QObject*> m_objects;
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_OSQOBJECTCONTROLLER_HPP
