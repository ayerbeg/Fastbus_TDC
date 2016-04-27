typedef struct{ 
  Int_t EventId; 
  Float_t TDCval[1536]; 
  Int_t multi[96]; 
} EventType; 
 
 
 
void histos(){ 
     char c1title[100]; 
    char h1title[100]; 
    char htitle[100]; 
    TH1F *htdc4[96]; 
  EventType Event; 
  Int_t EventId; 
  Float_t TDCval[96]; 
  //  TFile *infile=new TFile("./files/cdet_351_TDC-off.root"); 
  TFile *infile=new TFile("test.root"); 
  TTree *eventtree=(TTree*)infile->Get("eventtree"); 
 
  Int_t eventEntries=eventtree->GetEntries(); 
  cout<<"eventEntries: "<<eventEntries<<endl;  
   
  eventtree->SetBranchAddress("event",&Event.EventId); 
   
  //  eventtree->SetBranchAddress("TDCval",&event.TDCval[96]);//USELESS 
   
  for (int i = 0; i < 96;i++) { 
      sprintf(c1title,"TDC data channel %d",i); 
      sprintf(h1title,"TDC_data_channel_%d",i); 
      //      htdc4[i] = new TH1F(h1title,c1title,2000,-330,3300.); 
      htdc4[i] = new TH1F(h1title,c1title,2000,0,41000.); 
  } 
 
   
 
 
  
  for (Int_t i = 0; i < eventEntries; i++)  
    { 
      //  Int_t i = 4000; 
      eventtree->GetEntry(i); 
 
      int k=0; 
      for (int j = 0; j < 32;j++) 
  { 
    Int_t num_hits = Event.multi[j]; 
    if(num_hits!=0) 
      { 
        for (int ch = k; ch < num_hits+k; ch++) 
    { 
      Float_t tdc =Event.TDCval[ch]; 
      htdc4[j] -> Fill(tdc); 
    } 
        k=k+num_hits; 
      } 
  } 
       
      //      eventtree->Show(i,96);//Normally, Show() will show only a few entries 
 
      //      cout<<"TDC 5:" <<Event.TDCval[5]<<endl;//For me, it is quite magical that I can read this variable, without a previous declaration. How the struct relates the variables inside the Branch? 
 
    } 
 
  // c1 = new TCanvas("c1","TDC",1);  
  // c1->Divide(6,6); 
   
  //   for(Int_t i = 0; i<32 ; i++) 
    { 
      //       TPad *pcut1 =  c1->cd(i+1); 
   //     pcut1->SetLogy(); 
 
      Int_t i = 15; 
       
       sprintf(htitle,"CHANNEL_%d (in ns)",i); 
 
    htdc4[i]->SetTitle(htitle); 
    //    htdc4[i]->GetXaxis()->SetRangeUser(0, 600); 
    //  htdc4[i]->GetXaxis()->SetLabelSize(0.08); 
    htdc4[i]->SetXTitle("TDC (ns)");  
    htdc4[i]->SetYTitle("counts"); 
    //  htdc4[i]->Fit("gaus1"); 
    htdc4[i]->Draw(); 
  } 
 
 
   
} 
