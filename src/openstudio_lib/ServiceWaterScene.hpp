/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_SERVICEWATERSCENE_HPP
#define OPENSTUDIO_SERVICEWATERSCENE_HPP

#include "GridScene.hpp"
#include "OSItem.hpp"
#include "../model_editor/QMetaTypes.hpp"
#include <openstudio/model/Model.hpp>
#include <openstudio/model/WaterUseConnections.hpp>

namespace openstudio {

namespace model {

class HVACComponent;

}

class ServiceWaterScene : public GridScene
{
  Q_OBJECT

 public:
  explicit ServiceWaterScene(const model::Model& model);

  virtual ~ServiceWaterScene() {}

  model::Model model() const;

 public slots:

  void layout();

 private slots:

  void onAddedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                              const openstudio::UUID& uuid);

  void onRemovedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                const openstudio::UUID& uuid);

 private:
  bool m_dirty;

  model::Model m_model;
};

class WaterUseConnectionsDetailScene : public GridScene
{
  Q_OBJECT

 public:
  explicit WaterUseConnectionsDetailScene(const model::WaterUseConnections& waterUseConnections);

  virtual ~WaterUseConnectionsDetailScene() {}

  model::WaterUseConnections waterUseConnections() const;

 public slots:

  void layout();

 signals:

  void goToServiceWaterSceneClicked();

 private slots:

  void onAddedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                              const openstudio::UUID& uuid);

  void onRemovedWorkspaceObject(std::shared_ptr<openstudio::detail::WorkspaceObject_Impl> wPtr, const openstudio::IddObjectType& type,
                                const openstudio::UUID& uuid);

 private:
  bool m_dirty;

  model::WaterUseConnections m_waterUseConnections;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_SERVICEWATERSCENE_HPP
