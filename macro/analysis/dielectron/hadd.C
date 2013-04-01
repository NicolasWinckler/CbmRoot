//macro to add histogram files
//NOTE: This macro is kept for back compatibility only.
//Use instead the executable $ROOTSYS/bin/hadd
//
//This macro will add histograms from a list of root files and write them
//to a target root file. The target file is newly created and must not be
//identical to one of the source files.
//
//Author: Sven A. Schmidt, sven.schmidt@cern.ch
//Date:   13.2.2001

//This code is based on the hadd.C example by Rene Brun and Dirk Geppert,
//which had a problem with directories more than one level deep.
//(see macro hadd_old.C for this previous implementation).
//
//The macro from Sven has been enhanced by
//   Anne-Sylvie Nicollerat <Anne-Sylvie.Nicollerat@cern.ch>
// to automatically add Trees (via a chain of trees).
//
//To use this macro, modify the file names in function hadd.
//
//NB: This macro is provided as a tutorial.
//    Use $ROOTSYS/bin/hadd to merge many histogram files


#include <sstream>
#include <string.h>
#include "TChain.h"
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include "Riostream.h"

TList *FileList;
TFile *Target;

void MergeRootfile( TDirectory *target, TList *sourcelist );


void hadd() {
   // in an interactive ROOT session, edit the file names
   // Target and FileList, then
   // root > .L hadd.C
   // root > hadd()

    string particle = "";

    string fileArray[8] = {"analysis.pisupp0.01.25gev.centr.", "analysis.pisupp0.002.25gev.centr.", "analysis.pisupp0.001.25gev.centr.",
    "analysis.pisupp0.0004.25gev.centr.","analysis.pisupp0.0002.25gev.centr.",
    "analysis.pisupp0.0001.25gev.centr.", "analysis.pisupp0.0.25gev.centr.", "reco.auau.8gev.centr."};

    for (int iPs = 7; iPs < 8; iPs++){
       cout << "-I- " << fileArray[iPs] << endl;

   for (int iF = 0; iF < 4; iF++){

      if (iF == 0) {
         particle = "omegaepem";
      } else if (iF == 1) {
         particle = "phi";
      } else if (iF == 2) {
         particle = "omegadalitz";
      } else if (iF == 3) {
         particle = "rho0";
      } else if (iF == 4) {
	  particle = "urqmd";
      }

      std::string dir = "/hera/cbm/users/slebedev/mc/dielectron/jan13/8gev/trd/1.0field/nomvd/" + particle + "/";
      std::string fileName = dir + fileArray[iPs];
      //std::string fileName = dir + "analysis.25gev.centr.";
     // std::string fileName = dir + "reco.auau.8gev.centr.";
     // std::string outputDir = dir + "results/all/";

     // string fileName = "analysis.pi_misid_0.0001.";
     // string fileName = "analysis.";
     // string dir = "/lustre/cbm/user/ebelolap/aug11/sep12/25gev/100field/nomvd/"+particle+"/";
      cout << "-I- " << dir << endl;

      TFile* Target = TFile::Open( string(fileName+"all.root").c_str(), "RECREATE" );

      int count = 0;
      TList* FileList = new TList();
      for (int i = 1; i < 201; i++){
         stringstream ss;
         ss << fileName ;
         ss.fill('0');
         ss.width(5);
         ss  << i << ".root";
         TFile* file = TFile::Open(ss.str().c_str());
         if ( file != NULL && file->GetEND() > 4000){
            FileList->Add( file );
	    count++;
            //cout<< count<<endl;
	 } else {
	     if (file != NULL) {
                 file->Close();
	     }
	 }
      }
      cout << endl<< "-I- number of files to merge = " << count << endl << endl;

      MergeRootfile( Target, FileList );

      Target->Close();
      int nFL = FileList->GetEntries();
      for (int iFL = 0; iFL < nFL; iFL++){
	  TFile* f = (TFile*)FileList->At(iFL);
	  f->Close();
          delete f;
      }
      delete Target;
      delete FileList;
   }//iF


    } //iPs

}

void MergeRootfile( TDirectory *target, TList *sourcelist ) {

   //  cout << "Target path: " << target->GetPath() << endl;
   TString path( (char*)strstr( target->GetPath(), ":" ) );
   path.Remove( 0, 2 );

   TFile *first_source = (TFile*)sourcelist->First();
   first_source->cd( path );
   TDirectory *current_sourcedir = gDirectory;
   //gain time, do not add the objects in the list in memory
   Bool_t status = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);

   // loop over all keys in this directory
   TChain *globChain = 0;
   TIter nextkey( current_sourcedir->GetListOfKeys() );
   TKey *key, *oldkey=0;
   while ( (key = (TKey*)nextkey())) {

      //keep only the highest cycle number for each key
      if (oldkey && !strcmp(oldkey->GetName(),key->GetName())) continue;

      // read object from first source file
      first_source->cd( path );
      TObject *obj = key->ReadObj();

      if ( obj->IsA()->InheritsFrom( TH1::Class() ) ) {
         // descendant of TH1 -> merge it

         //      cout << "Merging histogram " << obj->GetName() << endl;
         TH1 *h1 = (TH1*)obj;

         // loop over all source files and add the content of the
         // correspondant histogram to the one pointed to by "h1"
         TFile *nextsource = (TFile*)sourcelist->After( first_source );
         while ( nextsource ) {

            // make sure we are at the correct directory level by cd'ing to path
            nextsource->cd( path );
            TKey *key2 = (TKey*)gDirectory->GetListOfKeys()->FindObject(h1->GetName());
            if (key2) {
               TH1 *h2 = (TH1*)key2->ReadObj();
               h1->Add( h2 );
               delete h2;
            }
           // cout << nextsource->GetPath() << endl;
            nextsource = (TFile*)sourcelist->After( nextsource );
         }
      }
//      else if ( obj->IsA()->InheritsFrom( TTree::Class() ) ) {
//
//         // loop over all source files create a chain of Trees "globChain"
//         const char* obj_name= obj->GetName();
//
//         globChain = new TChain(obj_name);
//         globChain->Add(first_source->GetName());
//         TFile *nextsource = (TFile*)sourcelist->After( first_source );
//         //      const char* file_name = nextsource->GetName();
//         // cout << "file name  " << file_name << endl;
//         while ( nextsource ) {
//
//            globChain->Add(nextsource->GetName());
//            nextsource = (TFile*)sourcelist->After( nextsource );
//         }
//
//      } else if ( obj->IsA()->InheritsFrom( TDirectory::Class() ) ) {
//         // it's a subdirectory
//
//         cout << "Found subdirectory " << obj->GetName() << endl;
//
//         // create a new subdir of same name and title in the target file
//         target->cd();
//         TDirectory *newdir = target->mkdir( obj->GetName(), obj->GetTitle() );
//
//         // newdir is now the starting point of another round of merging
//         // newdir still knows its depth within the target file via
//         // GetPath(), so we can still figure out where we are in the recursion
//         MergeRootfile( newdir, sourcelist );
//
//      } else {
//
//         // object is of no type that we know or can handle
//         cout << "Unknown object type, name: "
//         << obj->GetName() << " title: " << obj->GetTitle() << endl;
//      }

      // now write the merged histogram (which is "in" obj) to the target file
      // note that this will just store obj in the current directory level,
      // which is not persistent until the complete directory itself is stored
      // by "target->Write()" below
      if ( obj ) {
         target->cd();

         //!!if the object is a tree, it is stored in globChain...
//         if(obj->IsA()->InheritsFrom( TTree::Class() ))
//            globChain->Merge(target->GetFile(),0,"keep");
//         else
            obj->Write( key->GetName() );
      }

   } // while ( ( TKey *key = (TKey*)nextkey() ) )

   // save modifications to target file
   target->SaveSelf(kTRUE);
   TH1::AddDirectory(status);
}
