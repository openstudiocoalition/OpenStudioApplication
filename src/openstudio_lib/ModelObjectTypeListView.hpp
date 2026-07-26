/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_MODELOBJECTTYPELISTVIEW_HPP
#define OPENSTUDIO_MODELOBJECTTYPELISTVIEW_HPP

#include "OSCollapsibleItemList.hpp"
#include "OSCategoryPlaceholder.hpp"

#include <openstudio/model/Model.hpp>
#include <openstudio/model/ModelObject.hpp>

#include <boost/optional.hpp>

#include <QString>

#include <tuple>

class QVBoxLayout;

class QHBoxLayout;

namespace openstudio {

class ModelObjectTypeListView : public OSCollapsibleItemList
{
  Q_OBJECT

 public:
  ModelObjectTypeListView(const model::Model& model, bool addScrollArea, OSItemType headerType, bool isLibrary, QWidget* parent = nullptr);

  ModelObjectTypeListView(const std::vector<std::pair<IddObjectType, QString>>& modelObjectTypesAndNames, const model::Model& model,
                          bool addScrollArea, OSItemType headerType, bool isLibrary, QWidget* parent = nullptr);

  ModelObjectTypeListView(const std::vector<std::tuple<IddObjectType, QString, QString>>& modelObjectTypesNamesAndUrls,
                          const model::Model& model, bool addScrollArea, OSItemType headerType, bool isLibrary, QWidget* parent = nullptr);

  virtual ~ModelObjectTypeListView() {}

  void addModelObjectType(const IddObjectType& iddObjectType, const QString& name, const QString& url = {});

  void addModelObjectCategoryPlaceholder(const QString& name);

  virtual IddObjectType currentIddObjectType() const;

  virtual boost::optional<openstudio::model::ModelObject> selectedModelObject() const;

 private:
  std::vector<std::pair<IddObjectType, QString>> m_modelObjectTypesAndNames;

  model::Model m_model;
  OSItemType m_headerType;
  bool m_isLibrary;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_MODELOBJECTTYPELISTVIEW_HPP
