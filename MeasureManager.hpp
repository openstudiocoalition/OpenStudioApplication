/**********************************************************************
 *  Copyright (c) 2008-2016, Alliance for Sustainable Energy.  
 *  All rights reserved.
 *  
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *  
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#ifndef SHAREDGUICOMPONENTS_MEASUREMANAGER_HPP
#define SHAREDGUICOMPONENTS_MEASUREMANAGER_HPP

#include "LocalLibraryController.hpp"
#include "../utilities/bcl/BCLMeasure.hpp"
#include "../utilities/core/Path.hpp"
#include "../utilities/core/UUID.hpp"
#include "../model/Model.hpp"
#include <vector>
#include <map>
#include <QSharedPointer>
#include <QPointer>
#include <QApplication>
#include <QUrl>
class QEvent;
class QNetworkAccessManager;

namespace Json{
  class Value;
}

namespace openstudio {

class BaseApp;
class BCLMeasure;

namespace osversion {
  class VersionTranslator;
}

namespace measure {
  class OSArgument;
  class OSArgumentType;
}
class LocalLibraryController;

/***
* MeasureManager provides a single interface for finding measures on the user's computer in a variety of places.  
*
* It also manages checking if a newer version of a given measure is available on the user's computer and updating
* a SimpleProject to use the updated measure.  Measures directories are searched in the following order 
* 1) pat application measures, 2) my measures, 3) bcl measures.  This supports the following use cases:
*
* A user has started a measure in their my measures directory, then uploaded to BCL, then downloaded that measure then
* a measure with the same uid will exist in both my measures and the bcl measures.  The my measures version is prefered.
* 
* A user has downloaded a measure from the bcl to their local bcl and manually copied the directory to my measures.  
* This use case is not supported but in this case the my measures version is preferred.
*
* A user has manually downloaded a measure from the bcl to their my measures directory as well as downloaded it to their local bcl.  
* In this case the my measures version is preferred.
*
* A user has downloaded a copy of a built in measure from the bcl to their local bcl.  
* In this case the built in version is preferred.
* 
* A user has manually downloaded a copy of a built in measure from the bcl to their my measures directory.  
* In this case the built in version is preferred.
*
* A user has downloaded a copy of a built in measure from the bcl to their local bcl then duplicated it to my measures.  
* In this case there is no uuid conflict.
* 
* A user has manually downloaded a copy of a built in measure from the bcl to their my measures directory then duplicated it to my measures.  
* In this case there is no uuid conflict.
*
* A user has manually copies a measure in their my measures directory to their my measures directory.  
* In this case only one of the measures is displayed.
*
**/
#if defined(openstudio_lib_EXPORTS) || defined(COMPILING_FROM_OSAPP)
#include "../openstudio_lib/OpenStudioAPI.hpp"
class OPENSTUDIO_API MeasureManager : public QObject
#else
class MeasureManager : public QObject
#endif
{
  Q_OBJECT;

  public:
    MeasureManager(BaseApp *t_app);

    virtual ~MeasureManager() {}

    QUrl url() const;

    void setUrl(const QUrl& url);

    void saveTempModel();

    // Returns true if given measure is managed by the application and not the user
    bool isManagedMeasure(const UUID & id) const;

    // Measures shipped with OpenStudio.
    std::vector<BCLMeasure> openstudioMeasures() const;

    // Measures downloaded from the BCL.
    std::vector<BCLMeasure> bclMeasures() const;

    // Measures saved in the user's home directory.
    std::vector<BCLMeasure> myMeasures() const;

    // Get combined list of measures without duplicates, uses same logic as getMeasure
    // If includeOpenStudioMeasures is false then OpenStudio measures will not be included, otherwise they will be.
    std::vector<BCLMeasure> combinedMeasures(bool includeOpenStudioMeasures = true) const;

    // Retrieve a measure from openstudioMeasures, myMeasures, and bclMeasures by id.
    boost::optional<BCLMeasure> getMeasure(const UUID & id);

    ///// Updates an individual measure. Does not ask for user approval, approval is assumed.
    ///// \returns true if the update succeeded.
    std::pair<bool,std::string> updateMeasure(const BCLMeasure &t_measure);

    //// insert / replace a measure by UUID. If the measure already exists in the project
    //// the user is prompted as to how to deal with it. OSArguments are loaded as needed
    BCLMeasure insertReplaceMeasure(const UUID &t_id);

    //// Updates the given set of measures in the current project. Does not ask for user approval. Approval is assumed
    //// when this method is called.
    void updateMeasures(bool t_showMessage=true);

    void setLibraryController(const QSharedPointer<LocalLibraryController> &t_libraryController);

    std::vector<measure::OSArgument> getArguments(const BCLMeasure &t_measure);

    //std::string suggestMeasureGroupName(const BCLMeasure &t_measure);

    //std::string suggestMeasureName(const BCLMeasure &t_measure, bool t_fixed);

    bool isMeasureSelected();

    //QSharedPointer<measure::OSMeasureInfoGetter> infoGetter() const;

  public slots:
    /// Update the UI display for all measures. Does recompute the measure's XML.
    /// Does not update the measures in the project at all
    void updateMeasuresLists();

    ///// For all measures in the "openstudioMeasures" list which have changed relative to the version
    ///// in the project, update the project to the new version
    ///// 
    ///// Does not ask for user approval
    //void updateOpenStudioMeasures(analysisdriver::SimpleProject &t_project);

    ///// Updates the UI for all measures.
    ///// For all measures in the "myMeasures" list which have changed relative to the version
    ///// in the project, update the project to the new version
    ///// 
    ///// Does not ask for user approval
    //void updateMyMeasures(analysisdriver::SimpleProject &t_project);

    ///// Updates the UI for all measures.
    ///// For all measures in the "bclMeasures" list which have changed relative to the version
    ///// in the project, update the project to the new version.
    /////
    ///// Also checks installed measures and updates the project's version of them if 
    ///// appropriate. 
    ///// 
    ///// Does not ask for user approval
    //void updateBCLMeasures(analysisdriver::SimpleProject &t_project);

    /// Checks a BCL measure for updates, returns true if updated
    //bool checkForUpdates(BCLMeasure& measure, bool force=false);

    /// Downloads updated versions of all BCL measures
    void downloadBCLMeasures();

    void addMeasure();

    void duplicateSelectedMeasure();

  signals:
    void newMeasure(BCLMeasure newMeasure);

  private:
    REGISTER_LOGGER("openstudio.MeasureManager");

    void updateMeasuresLists(bool updateUserMeasures);

    bool checkForUpdates(const openstudio::path& measureDir, bool force=false);

    boost::optional<measure::OSArgument> getArgument(const measure::OSArgumentType& type, const Json::Value& jsonArgument);

    BaseApp *m_app;
    openstudio::path m_tempModelPath;
    std::map<UUID,BCLMeasure> m_managedMeasures;
    std::map<UUID,BCLMeasure> m_openstudioMeasures;
    std::map<UUID,BCLMeasure> m_myMeasures;
    std::map<UUID,BCLMeasure> m_bclMeasures;
    QUrl m_url;
    QSharedPointer<LocalLibraryController> m_libraryController;
    QNetworkAccessManager* m_networkAccessManager;
};


} // openstudio

#endif // SHAREDGUICOMPONENTS_MEASUREMANAGER_HPP
