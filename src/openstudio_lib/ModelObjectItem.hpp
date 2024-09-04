/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTITEM_HPP
#define OPENSTUDIO_MODELOBJECTITEM_HPP

#include "OSItem.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <QWidget>

class QPushButton;
class QLabel;

namespace openstudio {

OSItemId modelObjectToItemId(const openstudio::model::ModelObject& modelObject, bool isDefaulted);

QString modelToSourceId(const openstudio::model::Model& model);

class ModelObjectItem : public OSItem
{
  Q_OBJECT

 public:
  ModelObjectItem(const openstudio::model::ModelObject& modelObject, bool isDefaulted, OSItemType type, QWidget* parent = nullptr);

  virtual ~ModelObjectItem() {}

  openstudio::Handle handle() const;

  openstudio::model::ModelObject modelObject() const;

  virtual bool equal(const openstudio::OSItem*) const override;

 private slots:

  void onObjectChanged();

 private:
  openstudio::Handle m_handle;

  openstudio::model::ModelObject m_modelObject;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTITEM_HPP
