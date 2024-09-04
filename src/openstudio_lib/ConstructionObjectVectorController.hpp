/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONOBJECTVECTORCONTROLLER_HPP
#define OPENSTUDIO_CONSTRUCTIONOBJECTVECTORCONTROLLER_HPP

#include "ModelObjectVectorController.hpp"
#include <openstudio/utilities/core/Logger.hpp>

namespace openstudio {

class ConstructionObjectVectorController : public ModelObjectVectorController
{
  Q_OBJECT

 public:
  explicit ConstructionObjectVectorController(QWidget* parentWidget);

  // Need to delete the QMutex
  virtual ~ConstructionObjectVectorController();

  void setParentWidget(QWidget* parentWidget);

 protected:
  virtual void onChangeRelationship(const model::ModelObject& modelObject, int index, Handle newHandle, Handle oldHandle) override;

  virtual void onDataChange(const model::ModelObject& modelObject) override;

  virtual void onChange(const model::ModelObject& modelObject) override;

  virtual std::vector<OSItemId> makeVector() override;

  virtual void onRemoveItem(OSItem* item) override;

  virtual void onReplaceItem(OSItem* currentItem, const OSItemId& replacementItemId) override;

  virtual void onDrop(const OSItemId& itemId) override;

 private:
  REGISTER_LOGGER("openstudio::ConstructionObjectVectorController");

  void insert(const OSItemId& itemId, int insertPosition = -1, boost::optional<int> erasePosition_ = boost::none);

  enum LayerType
  {
    FENESTRATION,
    OPAQUE,
    AIRWALL,
    UNKNOWN
  };

  static LayerType getLayerType(IddObjectType iddObjectType);
  QWidget* parentWidget();

  QWidget* m_parentWidget;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONOBJECTVECTORCONTROLLER_HPP
