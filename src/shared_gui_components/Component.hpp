/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef SHAREDGUICOMPONENTS_COMPONENT_HPP
#define SHAREDGUICOMPONENTS_COMPONENT_HPP

#include <openstudio/utilities/bcl/BCL.hpp>
#include <openstudio/utilities/data/Attribute.hpp>

#include <QAbstractButton>

class QCheckBox;
class QLabel;

namespace openstudio {

class BCLMeasure;
class BCLSearchResult;

class Component : public QAbstractButton
{
  Q_OBJECT

 public:
  explicit Component(const BCLMeasure& bclMeasure, bool showAbridgedView = true, bool showCheckBox = true, QWidget* parent = nullptr);
  explicit Component(const BCLSearchResult& bclSearchResult, bool showAbridgedView = true, bool showCheckBox = true, QWidget* parent = nullptr);
  explicit Component(bool showAbridgedView = false, bool showCheckBox = false, QWidget* parent = nullptr);
  Component(const Component& other);
  virtual ~Component() = default;
  Component& operator=(const Component& other);
  QLabel* msg();
  std::string uid() const;
  std::string versionId() const;
  void setChecked(bool checked);
  bool checkBoxChecked() const;
  void setCheckBoxChecked(bool checked);
  bool checkBoxEnabled() const;
  void setCheckBoxEnabled(bool enabled);
  void setCheckBoxUpdateAvailable(bool enabled);

 protected:
  void paintEvent(QPaintEvent* event) override;

 private:
  void createAbridgedLayout();
  void createCompleteLayout();
  void parseBCLMeasure(const BCLMeasure& bclMeasure);
  void parseBCLSearchResult(const BCLSearchResult& bclSearchResult);

  QString m_componentType;
  QString m_name;
  QString m_uid;
  QString m_versionId;
  QString m_description;
  QString m_modelerDescription;
  QString m_versionModified;
  QString m_fidelityLevel;
  QString m_org;
  QString m_repo;
  QString m_releaseTag;
  boost::optional<std::string> m_error;
  std::vector<Attribute> m_attributes;
  std::vector<BCLMeasureArgument> m_arguments;
  std::vector<BCLFile> m_files;
  std::vector<BCLFileReference> m_fileReferences;
  std::vector<BCLProvenance> m_provenances;
  std::vector<std::string> m_tags;
  bool m_showAbridgedView;
  bool m_showCheckBox;
  bool m_available;
  bool m_updateAvailable;
  QCheckBox* m_checkBox;
  QLabel* m_msg;

 signals:

 private slots:
};

}  // namespace openstudio

#endif  // SHAREDGUICOMPONENTS_COMPONENT_HPP
