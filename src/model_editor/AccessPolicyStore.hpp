/***********************************************************************************************************************
*  OpenStudio(R), Copyright (c) OpenStudio Coalition and other contributors.
*  See also https://openstudiocoalition.org/about/software_license/
***********************************************************************************************************************/

#ifndef MODEL_ACCESSPOLICYSTORE_HPP
#define MODEL_ACCESSPOLICYSTORE_HPP

#include "ModelEditorAPI.hpp"

#include <map>

#include <QString>
#include <QByteArray>

#include <openstudio/utilities/idd/IddObject.hpp>

#include <openstudio/utilities/core/Path.hpp>

namespace openstudio {
class InspectorView;

namespace model {

class AccessParser;

/*! Access Policy will tell ModelObjects witch fields to expose
     *
     * The Policy will restrict your access to ModelObject data.
     * Workspace objects have fields. you can index into those fields and
     * get the value, change the value, etc. ModelObjects may not want to
     * allow you to change things, or even show them. The policy is used to
     * build GUIs, it is not really something for general use in the backend
     * data manipulation side of things. The idea is that each program might have its
     * own XML file that tells the AccessPolicy how to display fields.
     */
class MODELEDITOR_API AccessPolicy
{
  friend class AccessParser;
  friend class openstudio::InspectorView;  // For overriding via setAccess

 public:
  /*! tells the ModelObect how to sort its fields for display in a GUI*/
  enum ACCESS_LEVEL
  {
    FREE,   /*!< Show this field, allow edits */
    LOCKED, /*!< Show this field, do not allow edits */
    HIDDEN
  }; /*!< Do not show this field*/

  AccessPolicy();

  /*! gets the level of access
        \param index the index into the field vector
        \return defaults to FREE
        *
        * If you send in an erroneous index, or the policy didn't
        * load information for that index, you will get FREE back.
        * (You might get surprising results when you ask a workspace object
        * for that bogus index though. :) )
        */
  ACCESS_LEVEL getAccess(unsigned int index) const;

 protected:
  // For specific overriding of access policies, such as hiding Fan Schedule only on AirLoopHVAC for eg
  bool setAccess(unsigned int index, ACCESS_LEVEL);

 private:
  std::map<unsigned int, ACCESS_LEVEL> m_accessMap;
  unsigned int m_numNormalFields;
  unsigned int m_extensibleSize;
  std::map<unsigned int, ACCESS_LEVEL> m_extensibleAccessMap;
};

/*! This class is a simple singleton that stores AccessPolicy for ModelObjects
   *
   *
   */
class MODELEDITOR_API AccessPolicyStore
{
  friend class AccessParser;

 public:
  static AccessPolicyStore& Instance();

  /*! loads an xml with the policy rules
       */
  static bool loadFile(openstudio::filesystem::ifstream& file);
  static bool loadFile(const openstudio::path& path);
  static bool loadFile(const QByteArray& data);

  /*!Each IddObjectType has a uniqueAccessPolicy. This function will retrieve it*/
  const AccessPolicy* getPolicy(const openstudio::IddObjectType&) const;

  /* clear the map*/
  void clear();

 private:
  AccessPolicyStore();
  ~AccessPolicyStore();
  AccessPolicyStore(const AccessPolicyStore&);
  AccessPolicyStore& operator=(const AccessPolicyStore&);

  std::map<openstudio::IddObjectType, AccessPolicy*> m_policyMap;

  static AccessPolicyStore* s_instance;
  REGISTER_LOGGER("model.AccessPolicyStore");
};

}  // namespace model
}  // namespace openstudio
#endif
