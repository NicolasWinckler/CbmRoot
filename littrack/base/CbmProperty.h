
#ifndef CBMPROPERTY_H
#define CBMPROPERTY_H

#include "TObject.h"

#include <string>
#include <typeinfo>

class CbmPropertyType //: public TObject
{
   
public:
    
   // destructor
   virtual ~CbmPropertyType() { };
   
   const std::string& GetTypeName() {return fTypeName;};
   
protected:
   
   // constructor 
   CbmPropertyType():fTypeName(""){ };
   CbmPropertyType(const std::string& name):fTypeName(name){ };
   
private:
   

   std::string fTypeName;
 
   ClassDef(CbmPropertyType,1);
};

template<class TYPE>
class CbmProperty : public CbmPropertyType
{

public:

  // constructor

  CbmProperty(const std::string& name, const TYPE& value);
  
  // copy constructor 
  //CbmProperty(const CbmProperty<TYPE>& right);
  // assignement operator 
  //CbmProperty<TYPE>& operator=( const CbmProperty<TYPE>& right );

  // destructor 
  ~CbmProperty() ;

  // get property name 
  const std::string& GetPropName() const { return fName; };
  // get property value
  const TYPE& GetPropValue() const { return fValue; };
  
  // set property name 
  void SetPropName(const std::string& name) { fName = name; };
  // set property value
  void SetPropValue(const TYPE& value) { fValue = value; };

protected:  
  
  CbmProperty();
  
private:  
  // property name 
  std::string fName;

  // property value
  TYPE fValue;
  
  ClassDef(CbmProperty,1);
  
};

#endif 
