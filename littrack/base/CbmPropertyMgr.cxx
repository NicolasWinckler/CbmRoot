 

#include "CbmPropertyMgr.h" 
#include "CbmProperty.h"

#include "TObject.h"

#include <iostream>

//#include <TArrayI.h>
//#include <TArrayD.h>


// Constructor
CbmPropertyMgr::CbmPropertyMgr()
{

}

// Destructor
CbmPropertyMgr::~CbmPropertyMgr()
{
   //TODO is it neccessary to cas to actual type before deleting the pointer?!?!
   
   for(std::map<std::string, CbmPropertyType*>::iterator i = fProperties.begin();
         i != fProperties.end(); i++) delete (*i).second;
   fProperties.clear();
}

// Add a property
template<class TYPE>
void CbmPropertyMgr::AddProperty(const std::string& name, const TYPE& value)
{
   if (fProperties.find(name) == fProperties.end()) {
      fProperties[name] = new CbmProperty<TYPE>(name, value); 
   } else {
      std::cout << "-W- CbmPropertyMgr::AddProperty: "
                << "property with name - " << name << " already exists" << std::endl;
      
      // this is just for debuging, to prevent possible errors in the program
      TObject::Fatal("CbmPropertyMgr::AddProperty", 
    		  "property with this name already exists, please choose another name");
   }
}
 
// Remove a property
void CbmPropertyMgr::RemoveProperty(const std::string& name)
{
   if (fProperties.find(name) == fProperties.end()) {
      std::cout << "-W- CbmPropertyMgr::RemoveProperty: "
                << "property with name - " << name << " doesn't exist" << std::endl;
      
      // this is just for debuging, to prevent possible errors in the program
      TObject::Fatal("CbmPropertyMgr::RemoveProperty", 
    		  "property with this name doesn't exist, please check the name in your code");
      
   } else {
     delete fProperties[name];
     fProperties.erase(name);
   }
}

// Set the property
template<class TYPE>
void CbmPropertyMgr::SetProperty(const std::string& name, const TYPE& value)
{
   if (fProperties.find(name) == fProperties.end()) {
      std::cout << "-W- CbmPropertyMgr::SetProperty: "
                << "property with name - " << name << " doesn't exist" << std::endl;
      
      // this is just for debuging, to prevent possible errors in the program
      TObject::Fatal("CbmPropertyMgr::SetProperty", "property with this name doesn't exist, please check the name in your code");          
   } else {
      CbmPropertyType* propertyType = fProperties[name];
      
      if (typeid(TYPE).name() ==  propertyType->GetTypeName()) {
         //std::cout << "types are the same" << std::endl;
         CbmProperty<TYPE>* property = static_cast<CbmProperty<TYPE>* > (fProperties[name]);
         property->SetPropValue(value);
      } else {
         //std::cout << "types are different" << std::endl;
         // crash the program here :)))
         std::cout << "-W- CbmPropertyMgr::SetProperty: "
                << " property with this name has another type, "
                << "and will not be set" << std::endl;
                
         // this is just for debuging, to prevent possible errors in the program
         TObject::Fatal("CbmPropertyMgr::SetProperty", "property with this name has another type, and will not be set, please check the name and corresponding data type in your code, note that there is NO implicit data types conversion !!!"); 
      }
   }
}

// Get the property 
template<class TYPE>
void CbmPropertyMgr::GetProperty(const std::string& name, TYPE& value) 
{
   if (fProperties.find(name) == fProperties.end()) {
      std::cout << "-W- CbmPropertyMgr::GetProperty: "
                << "property with name - " << name << " doesn't exist" << std::endl;
                
      // this is just for debuging, to prevent possible errors in the program
     TObject::Fatal("CbmPropertyMgr::GetProperty", "property with this name doesn't exist, please check the name in your code"); 
      
   } else {
      CbmPropertyType* propertyType = fProperties[name];
      //std::cout << "proprtyType->GetTypeName() = " << propertyType->GetTypeName() << std::endl;
      
      if (typeid(TYPE).name() ==  propertyType->GetTypeName()) {
         //std::cout << "types are the same" << std::endl;
         CbmProperty<TYPE>* property = static_cast<CbmProperty<TYPE>* > (fProperties[name]);
         value = property->GetPropValue();
      } else {
         //std::cout << "types are different" << std::endl;
         // crash the program here :)))
         std::cout << "-W- CbmPropertyMgr::GetProperty: "
                << " property with this name has another type, "
                << "and will not be set" << std::endl;
                
         TObject::Fatal("CbmPropertyMgr::GetProperty", "property with this name has another type, please check the name and corresponding data type in your code, note that there is NO implicit data types conversion !!!"); 
      }
   }
}

// for the ROOT one has explicitly instantiate template with all used type

template void CbmPropertyMgr::AddProperty<double>(const std::string&, const double&);
template void CbmPropertyMgr::AddProperty<int>(const std::string&, const int&);
template void CbmPropertyMgr::AddProperty<bool>(const std::string&, const bool&);



template void CbmPropertyMgr::SetProperty<double>(const std::string&, const double&);
template void CbmPropertyMgr::SetProperty<int>(const std::string&, const int&);
template void CbmPropertyMgr::SetProperty<bool>(const std::string&, const bool&);



template void CbmPropertyMgr::GetProperty<double>(const std::string&, double&);
template void CbmPropertyMgr::GetProperty<int>(const std::string&, int&);
template void CbmPropertyMgr::GetProperty<bool>(const std::string&, bool&);



ClassImp(CbmPropertyMgr)



  
  
