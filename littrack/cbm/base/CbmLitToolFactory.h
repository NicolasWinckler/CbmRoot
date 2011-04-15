/** CbmLitToolFactory.h
 *@author A.Lebedev <andrey.lebedev@gsi.de>
 *@since 2008
 *
 * Littrack tool factory. Most of the tools
 * are created using this factory.
 **/

#ifndef CBMLITTOOLFACTORY_H_
#define CBMLITTOOLFACTORY_H_

#include "base/CbmLitPtrTypes.h"

#include <string>
#include <iostream>

class CbmLitToolFactory
{
public:
   /* Constructor */
   virtual ~CbmLitToolFactory();

   /* Returns pointer to the singleton object instance. */
   static CbmLitToolFactory* Instance();

   /* Creates track extrapolator tool by name. */
   TrackExtrapolatorPtr CreateTrackExtrapolator(
      const std::string& name);

   /* Creates track propagator tool by name. */
   TrackPropagatorPtr CreateTrackPropagator(
      const std::string& name);

   /* Creates track update tool by name. */
   TrackUpdatePtr CreateTrackUpdate(
      const std::string& name);

   /* Creates track fitter tool by name. */
   TrackFitterPtr CreateTrackFitter(
      const std::string& name);

   /* Creates track selection tool by name. */
   TrackSelectionPtr CreateTrackSelection(
      const std::string& name);

   /* Creates track finder tool by name. */
   TrackFinderPtr CreateTrackFinder(
      const std::string& name);

   /* Creates hit to track merger tool by name. */
   HitToTrackMergerPtr CreateHitToTrackMerger(
      const std::string& name);

protected:
   /* Constructor is protected since singleton pattern is used.
    * Pointer to the object is returned by static Instance() method. */
   CbmLitToolFactory();

private:
   static CbmLitToolFactory* fInstance; // static instance
};

#endif /*CBMLITTOOLFACTORY_H_*/
