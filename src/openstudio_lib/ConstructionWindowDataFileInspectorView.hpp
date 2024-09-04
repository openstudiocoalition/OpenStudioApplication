/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef OPENSTUDIO_CONSTRUCTIONWINDOWDATAFILEINSPECTORVIEW_HPP
#define OPENSTUDIO_CONSTRUCTIONWINDOWDATAFILEINSPECTORVIEW_HPP

#include "ModelObjectInspectorView.hpp"
#include "ModelObjectVectorController.hpp"
#include <openstudio/model/WindowDataFile.hpp>

namespace openstudio {

class OSLineEdit2;

class ConstructionWindowDataFileInspectorView : public ModelObjectInspectorView
{
  Q_OBJECT

 public:
  ConstructionWindowDataFileInspectorView(bool isIP, const openstudio::model::Model& model, QWidget* parent = nullptr);

  virtual ~ConstructionWindowDataFileInspectorView() {}

 protected:
  virtual void onClearSelection() override;

  virtual void onSelectModelObject(const openstudio::model::ModelObject& modelObject) override;

  virtual void onUpdate() override;

 private:
  void createLayout();

  void attach(const openstudio::model::WindowDataFile& windowDataFile);

  void detach();

  bool m_isIP;

  OSLineEdit2* m_nameEdit;

  OSLineEdit2* m_urlEdit;

  boost::optional<model::WindowDataFile> m_windowDataFile;
};

}  // namespace openstudio

#endif  // OPENSTUDIO_CONSTRUCTIONWINDOWDATAFILEINSPECTORVIEW_HPP
