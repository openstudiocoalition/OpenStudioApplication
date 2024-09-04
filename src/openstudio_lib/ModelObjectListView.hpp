/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTLISTVIEW_HPP
#define OPENSTUDIO_MODELOBJECTLISTVIEW_HPP

#include "OSItemList.hpp"
#include "OSVectorController.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>
#include "../model_editor/QMetaTypes.hpp"

class QMutex;

namespace openstudio {

class ModelObjectListController : public OSVectorController
{
  Q_OBJECT

 public:
  ModelObjectListController(const openstudio::IddObjectType& iddObjectType, const model::Model& model, bool isLibrary);

  virtual ~ModelObjectListController();

  IddObjectType iddObjectType() const;

 private slots:
  void objectAdded(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);
  void objectRemoved(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl>, const openstudio::IddObjectType&, const openstudio::UUID&);

 protected:
  virtual std::vector<OSItemId> makeVector() override;

 private:
  openstudio::IddObjectType m_iddObjectType;
  model::Model m_model;
  bool m_isLibrary;
  bool m_reportScheduled;
  boost::optional<openstudio::UUID> m_selectedHandle;
  QMutex* m_reportItemsMutex;

  void reportItemsImpl();
};

class ModelObjectListView : public OSItemList
{
  Q_OBJECT

 public:
  ModelObjectListView(const openstudio::IddObjectType& iddObjectType, const model::Model& model, bool addScrollArea, bool isLibrary,
                      QWidget* parent = nullptr);

  virtual ~ModelObjectListView() {}

  boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

  IddObjectType iddObjectType() const;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTLISTVIEW_HPP
