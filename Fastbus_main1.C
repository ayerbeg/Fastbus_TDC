// A simple decoder and histogrammer for fastbus data.
// R. Michaels, Oct 24, 2014.
// developed later by Dasuni, et al.

// MODIFICATIONS BY C. AYERBE:
// Feb 2016:: Histograms saved one per TDC channel only 16) To Be Improved TBI 
// Feb 2016: raw data stored in root leaves
//           * The variable is initialized before it is filled, otherwise it will keep
//             previous value
// Mar 2016: corrected the method to save data to a rootfile. Now it considers the multiplicity
//           of hit on each channel every event/trigger


#define MAXROC     32
#define NUMCHANT   96   // TDC
#define NUMCHANA   64   // ADC
#define MAXHITS    100
#define MAXEVENTS  1000000

#include <iostream>
#include <string>
#include <vector>
#include "THaCodaFile.h"
#include "THaEtClient.h"
#include "TString.h"
#include "TROOT.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TNtuple.h"
#include "TRandom.h"
#include <vector>


typedef struct {
  Int_t EventID;
  Float_t TDCval[1536];//I think being Int could be enough
  Int_t multi[96];
} UTEvent;



using namespace std;

void usage();
void decode(int* data);
void analysis();
void clear();

// Global data 
Int_t evlen, evtype, evnum;
Int_t *irn, *rocpos, *roclen;
Int_t myroc, myslot, mychan;
Int_t *nthits;
Int_t *tdcdat, *adcdat,*tdc2d;
TH1F *htdc1, *htdc2, *htdc3;
Int_t fHasHeader=1;   // =1 for 1877 and 1881, but =0 for 1875
Int_t fWordSeen;
bool dodump=false;
//bool dodump=true; Do not do this! Use the -v runtime flag for verbose output

TH1F *htdc4[96];

UTEvent event; 
TTree *eventtree;

Int_t debug=0; //Do not set to 1! Use the -debug runtime flag for debug output

int main(int argc, char* argv[])
{
    
    char c1title[100];
    char h1title[100];
    THaCodaData *coda;      
    char ctitle[100];
    Int_t maxevent = 10000;
    string datafile;
    string outfile;
    Int_t ievent = 0;
    Int_t istatus;
    
  /*if (argc < 2) {
     maxevent = MAXEVENTS;
  }
  if (argc > 2) {
     usage();
     return 1;
  }  */
  
  if (argc > 1) {
	  bool maxeventset=false;
	  bool datafileset=false;
	  bool outfileset=false;
	  bool rocset=false;
	  bool slotset=false;
	  bool chanset=false;

	  for (int i=1;i<argc;i=i+2) {
	    if (strcmp(argv[i],"-maxevents")==0 && !maxeventset) {
	      maxevent = atoi(argv[i+1]);
	      maxeventset=true;
	      continue;
	    }else if (strcmp(argv[i],"-datafile")==0 && !datafileset) {
	      datafile=argv[i+1];
	      datafileset=true;
	      continue;
	    }else if (strcmp(argv[i],"-outfile")==0 && !outfileset) {
	      outfile=argv[i+1];
	      outfileset=true;
	      continue;
	    }else if(strcmp(argv[i],"-roc")==0 && !rocset){
	      myroc=atoi(argv[i+1]);
	      rocset=true;
	      continue;
	    }else if(strcmp(argv[i],"-slot")==0 && !slotset){
	      myslot=atoi(argv[i+1]);
	      slotset=true;
	      continue;
	    }else if(strcmp(argv[i],"-chan")==0 && !chanset){
	      mychan=atoi(argv[i+1]);
	      chanset=true;
	      continue;
	    }else if(strcmp(argv[i],"-v")==0){
	      dodump=true;
	      continue;
	    }else if(strcmp(argv[i],"-debug")==0){
	      debug=1;
	      continue;
	    }else {
	      usage();
	      return 1;
	    }
	  }
	  if (!datafileset||!rocset||!slotset) {
		  usage();
		  return 1;
	  }
	  if (!maxeventset) {
		  maxevent = MAXEVENTS;
	  }
	  if(!chanset){
	    mychan=17;
	  }
	  if (!outfileset) {
	    Int_t s_end =datafile.length();
	    //Int_t s_begin = -1;
	    Int_t count=0;
	    Int_t ext=0;
	    for(string::reverse_iterator rit=datafile.rbegin(); rit!=datafile.rend(); ++rit){
	      if(*rit=='.'){
		//s_end-=count;
		ext=count;
	      }
	      if(*rit=='/'){
		//s_begin=datafile.length()-count;
		break;
	      }
	      count++;
	    }
	    /*if(s_begin==-1){
	      s_begin=0;
	      }*/
	    s_end-=ext;
	    outfile = datafile.substr(0,s_end);
	    outfile.append("root");
	  }
  }else{
    usage();
    return 1;
  }

  int choice1 = 1;
  //myroc=1;  myslot=7;  mychan = 5;   // defaults

  irn = new Int_t[MAXROC];
  rocpos = new Int_t[MAXROC];
  roclen = new Int_t[MAXROC];
 
  //if (argc > 1) maxevent = atoi(argv[2]);
 
  cout << "Fastbus analysis "<<endl;
  cout << "Events to process "<<maxevent<<endl;

  nthits  = new Int_t[NUMCHANT];
  tdcdat  = new Int_t[NUMCHANT*MAXHITS];
  adcdat  = new Int_t[NUMCHANA];

// Initialize root and output
  TROOT fbana("fbroot","Hall A SBS analysis for Fastbus");
  TFile hfile(outfile.c_str(),"RECREATE","SBS data");

  sprintf(ctitle,"TDC hits per channel on slot %d",myslot);
  htdc1 = new TH1F("Hits",ctitle,2*NUMCHANT,0,NUMCHANT);
  sprintf(ctitle,"TDC data on slot %d channel %d",myslot,mychan);
  htdc2 = new TH1F("MyChan",ctitle,200,0,4100.);
  sprintf(ctitle,"TDC data on all channels of slot %d",myslot);
  htdc3 = new TH1F("AllChannels",ctitle,200,0,4100.);

  eventtree = new TTree("eventtree","TDC data");
  eventtree->Branch("event",&event.EventID, "EventID/I:TDCval[1536]/F:multi[96]/I");
   // eventtree->Branch("event",&event.EventID, "EventID/I");
   // eventtree->Branch("TDCval",event.TDCval, "TDCval[96]/I");

  //Declaration of the Histrograms. Note the index is up to 96, and it changes
  // with the index of the TDC readout channel. Thus, if we are using TDC channels
  // 16-31, the index should be modified. (C. Ayerbe)
  
  
  for (int i = 0; i < 96;i++) {
      sprintf(c1title,"TDC data channel %d",i);
      sprintf(h1title,"TDC_data_channel_%d",i);
      htdc4[i] = new TH1F(h1title,c1title,2000,0,41000.);
  }



  if (choice1 == 1) {  // CODA File
    
    // CODA file "run.dat" may be a link to CODA datafile on disk
    TString filename(datafile);

    coda = new THaCodaFile();
    if (coda->codaOpen(filename) != 0) {
      cout << "ERROR:  Cannot open CODA data" << endl;
      goto end1;
    }

    
  } else {         // Online ET connection
    
    int mymode = 1;
    TString mycomputer("sbs1");
    TString mysession("whatever");
    
    coda = new THaEtClient();
    if (coda->codaOpen(mycomputer, mysession, mymode) != 0) {
      cout << "ERROR:  Cannot open ET connection" << endl;
      goto end1;
    }
    
  }

  // Loop over events

  for (int iev = 0; iev < maxevent; iev++)  {//the loop over the events

 
    
    if (iev > 0 && ((iev%1000)==0) ) printf("%d events\n",iev);

    clear();

    istatus = coda->codaRead();  

    if (istatus != 0) {  // EOF or no data
      
      if ( istatus == -1) {
	if (choice1 == 1) {
	  cout << "End of CODA file. Bye bye." << endl;
	}
	if (choice1 == 2) cout << "CODA/ET not running. Bye bye." << endl;
      } else {
	cout << "ERROR: codaRread istatus = " << hex << istatus << endl;
      }
      goto end1;
      
      
    } else {   // we have data ...
      
      ievent++;
      decode( coda->getEvBuffer() );

      // Initialization loop to clean the array
      for (int i = 0; i < 1536; i++)
	{
	  event.TDCval[i] = 0;//array equal 0
	}
      
      analysis();
      event.EventID = iev;
      
  

    }
    eventtree->Fill();	 
  }

end1:
  
  printf("\n");
  printf("Number of events analyzed = %d \n",ievent);

  coda->codaClose();
  eventtree->Write();
  hfile.Write();
  hfile.Close();

  return 0;

}; //end of main function


void usage() {  
  cout << "   Usage:  'fbana -datafile path_to_data -roc roc_number -slot slot_number [-chan channel_number] [-maxevents maxevents] [-outfile path_to_output] [-v]' " << endl;
  cout << "      The location of the data file is non-optional"<<endl;
  cout << "      The ROC and slot in the ROC must be set. The channel will default to 1 if unset." <<endl;
  cout << "      The maximum number of events is an optional parameter" << endl;
  cout << "      The output file path and name is optional. If left unset, it will default to the directory of fbana with the same name as the data file and a .root extension" << endl;
  cout << "      -v turns on verbose output of raw data." << endl;
}

void clear() {
  fWordSeen = 0;
  memset (nthits, 0, NUMCHANT*sizeof(Int_t));
  memset (adcdat, 0, NUMCHANA*sizeof(Int_t));
  memset (tdcdat, 0, NUMCHANT*MAXHITS*sizeof(Int_t));
}

void decode (int* data) {

  Int_t found_dat;
  Int_t ichan, rdata;
  evlen = data[0] + 1;
  evtype = data[1]>>16;
  evnum = data[4];
  //static int dodump = 1;  // dump the raw data

  if (dodump) { // Event dump
    cout << "\n\n Event number " << dec << evnum;
    cout << " length " << evlen << " type " << evtype << endl;
    int ipt = 0;
    for (int j = 0; j < (evlen/5); j++) {
	cout << dec << "\n evbuffer[" << ipt << "] = ";
          for (int k=j; k<j+5; k++) {
    	 cout << hex << data[ipt++] << " ";
	  }
	  cout << endl;
    }
    if (ipt < evlen) {
        cout << dec << "\n evbuffer[" << ipt << "] = ";
        for (int k=ipt; k<evlen; k++) {
	    cout << hex << data[ipt++] << " ";
	 
	}
        cout << endl;
    }
  }
  
  // n1 = pointer to first word of ROC
  int pos = data[2]+3;
  int nroc = 0;
  while( pos+1 < evlen && nroc < MAXROC ) {
     int len  = data[pos];
     int iroc = (data[pos+1]&0xff0000)>>16;
     if(iroc>=MAXROC || nroc >= MAXROC) {
  	 cout << "Decoder error:  ";
	 cout << "  ROC num " <<dec<<iroc;
         cout << "  nroc "<<nroc<<endl;
         return;
       }
// Save position and length of each found ROC data block
       rocpos[iroc]  = pos;
       roclen[iroc]  = len;
       irn[nroc++] = iroc;
       pos += len+1;
  }
  Int_t found = 0;
  for (int j=0; j<nroc; j++) {
     if(myroc == irn[j]) found=1;
  }
  if (!found && debug) {
       cout << "ROC "<<dec<<myroc<<" not in datastream ";
       cout << "  for event "<<evnum<<"  evtype "<<evtype<<endl;
       return;
  }

  if (debug) {
     cout << "Roc info "<<nroc<<endl;
     for (int j=0; j<nroc; j++) {
       Int_t iroc = irn[j];
       cout << "irn "<<dec<<iroc<<"   pos "<<rocpos[iroc];
       cout <<"   len "<<roclen[iroc]<<endl;
     }
  }    

 // Go over the data in myroc

  found_dat = 0;
 
  for (int j = rocpos[myroc]; j < rocpos[myroc]+roclen[myroc]; j++) {

    if (debug) printf("data[%d] = 0x%x = (dec) %d \n",j,data[j],data[j]);

    Int_t slot = (data[j] &0xF8000000)>> 27;
    if (debug) cout << "slot "<<slot<<endl;

    if (slot == myslot) {

      if (fWordSeen > 0 || !fHasHeader) {  // skip header if there is one

         ichan = (data[j]&0xfe0000)>>17; // 1877 
     //  chan = (data[j]&0x7e0000)>>17;    // 1881

         rdata = data[j]&0xffff;  // 1877 
       // rdata = data[j]&0x3fff;  // 1881
	 //	 if (debug) cout << "ichan "<<ichan<<" rdata:"<<rdata<<endl;
           if (ichan >= 0 && ichan < NUMCHANT && rdata > 0) {
	     if (nthits[ichan] < MAXHITS-1) {

	        tdcdat[nthits[ichan]*NUMCHANT+ichan] = rdata;
 
		nthits[ichan]++;
                if (nthits[ichan] > 100) cout << "HUH ?"<<endl;

	     }
	   }
	}
      fWordSeen++;
 
    }
  }
}



void analysis()
{
  
  Float_t offset[32]={333.6,330.9,329.4,328.6,331,331.4,329.6,329.7,329.4,330.5,332.4,331.3,333.6,334.5,335.5,335.6,329.7,329,326.9,327.2,327.6,328.4,326.3,327.6,326.8,327.2,326.5,326.6,327.2,324,324.7,325.4};
  

  
  Int_t ichan, ihit, rdata;

  int k=0;
  
  for (ichan = 0; ichan < NUMCHANT; ichan++)
    {
      
      if (debug && (ichan == mychan)) cout << "Hits on mychan "<<myslot<<"  "<<mychan<<"  "<<nthits[ichan]<<endl;
      
      if (debug && nthits[ichan]>0)      cout << "chan "<<ichan<<"  nthits "<<nthits[ichan]<<endl;
      
      htdc1->Fill(ichan, nthits[ichan]);
      
      for (ihit = 0; ihit < nthits[ichan]; ihit++)
	{
	  
	  rdata = tdcdat[ihit*NUMCHANT+ichan];
	  
	  if (debug)	    cout << "data on myslot "<<rdata<<endl;
	  
	  if (ichan == mychan)
	    {
	      htdc2->Fill(rdata);
	      //  	      cout<<"ichan: " <<ichan<<" ihit: "<<ihit<<" rdata: "<<rdata <<endl;
	      
	    }
	  htdc3->Fill(rdata); // all channels of this slot
	  
	  
	  //The index of the histograms array and the root variable should be arranged acording the channels used. 
	  //	  if (ichan>15 )
	  // 	  if ( ichan<16)
	  
	  htdc4[ichan]->Fill(rdata);//-offset[ichan]);//to convert in ns
	  
	  event.TDCval[k] = (rdata);//*0.5)-offset[ichan];
	  k++;
	}
      
      event.multi[ichan] = nthits[ichan];
      //      cout<<"HELLO!"<<endl;	  
      
    }
}




