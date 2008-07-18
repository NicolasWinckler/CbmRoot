 
#include "CbmProperty.h"


template <class TYPE> 
CbmProperty<TYPE>::CbmProperty()
{
   fName = "";
   //fValue = 0;
}

template <class TYPE> 
CbmProperty<TYPE>::CbmProperty(const std::string& name, const TYPE& value)
 : CbmPropertyType(typeid(TYPE).name()),
   fName(name),
   fValue(value)  
{
   
}

// virtual destructor 
template <class TYPE> 
CbmProperty<TYPE>::~CbmProperty()
{

}


/*
template <class TYPE> 
CbmProperty<TYPE>::CbmProperty(const CbmProperty& right)
{
//  fName = right.GetName();
  fValue = right.GetValue();
}

template <class TYPE> 
CbmProperty<TYPE> &CbmProperty<TYPE>::operator=(const CbmProperty& right)
{
//  fName = right.GetName();
  fValue = right.GetValue();
}
*/

template class CbmProperty<double>;
template class CbmProperty<int>;
template class CbmProperty<bool>;


ClassImp(CbmPropertyType)
templateClassImp(CbmProperty)

