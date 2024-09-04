/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODELEDITOR_BRIDGECLASSES_HPP
#define MODELEDITOR_BRIDGECLASSES_HPP

#include "ModelEditorAPI.hpp"

#include <QObject>

class MODELEDITOR_API ComboHighlightBridge : public QObject
{

  Q_OBJECT;

 public:
  explicit ComboHighlightBridge(QObject* parent);

  QObject* bridgeSender();

 public slots:
  void highlighted(const QString& val);
  void activated(const QString& val);
 signals:
  void highlightEmitted(const QString& val);
  void activatedEmitted(const QString& val);

 private:
  QObject* m_source;  //original source object
};

#endif  // MODELEDITOR_BRIDGECLASSES_HPP
