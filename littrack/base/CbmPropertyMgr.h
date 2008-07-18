

#ifndef CBMPROPERTYMGR_H
#define CBMPROPERTYMGR_H

#include <string>
#include <map>

#include "TObject.h"

class CbmPropertyType;

class CbmPropertyMgr : public TObject
{

public:
  // Constructor
  CbmPropertyMgr();
  // Destructor
  ~CbmPropertyMgr();
  
  // Add a property to the list
  template<class TYPE>
  void AddProperty(const std::string& name, const TYPE& value);

  // Remove a property from the list
  void RemoveProperty(const std::string& name);

  // Set the property value by its name
  template<class TYPE>
  void SetProperty(const std::string& name, const TYPE& value);

  // Get the property by name
  template<class TYPE>
  void GetProperty(const std::string& name, TYPE& value);

private:
  
  // Collection of all declared properties
  // templated CbmProperty class is stored 
  std::map<std::string, CbmPropertyType*> fProperties;
  
  ClassDef(CbmPropertyMgr,1);
  
};

#endif




