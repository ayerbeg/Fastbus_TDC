typedef struct{ 
  Int_t EventId; 
  Float_t TDCval[1536]; 
} EventType; 
 
 
 
void readtree(){ 
   
  EventType Event; 
  TFile *infile=new TFile("test.root"); 
  //  TFile *infile=new TFile("./files/cdet_351_TDC-off.root"); 
  TTree *eventtree=(TTree*)infile->Get("eventtree"); 
 
  Int_t eventEntries=eventtree->GetEntries(); 
  cout<<"eventEntries: "<<eventEntries<<endl;  
   
  eventtree->SetBranchAddress("event",&Event.EventId); 
   
  //  eventtree->SetBranchAddress("event",&Event.TDCval[96]);//USELESS 
   
  for (Int_t i = 0; i < 20; i++)  
    { 
      //  Int_t i = 4000; 
      eventtree->GetEntry(i); 
       
      eventtree->Show(i,300);//Normally, Show() will show only a few entries 
 
      //    cout<<"TDC 5:" <<Event.TDCval[5]<<endl;//For me, it is quite magical that I can read this variable, without a previous declaration. How the struct relates the variables inside the Branch? 
       
    } 
  return 1; 
} 
