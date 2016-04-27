{ 
  gROOT->Reset(); 
   
   TFile *f1 = new TFile("./files/cdet_211_TDC.root"); 
  // TFile *f2 = new TFile("./files/cdet_212_TDC.root"); 
  // TFile *f3 = new TFile("./files/cdet_213_TDC.root"); 
  // TFile *f4 = new TFile("./files/cdet_216_TDC.root"); 
  // TFile *f5 = new TFile("./files/cdet_107_ADC.root"); 
  // TFile *f6 = new TFile("./files/cdet_108_ADC.root"); 
   
  // TFile *f7 = new TFile("./files/cdet_121_ADC.root"); 
  
 
 char c1title[100]; 
 char htitle[100]; 
 Double_t ped[16]; 
 
  TH1F *hEne01[32]; 
  TH1F *hEne02[32]; 
  TH1F *hEne03[32]; 
  // TH1F *hEne04[16]; 
  // TH1F *hEne05[16]; 
  // TH1F *hEne06[16]; 
 
  for(Int_t i = 0; i<96 ; i++) 
  { 
    sprintf(c1title,"TDC_data_channel_%d",i); 
     //   f1->GetObject(c1title,hEne[i]);  
 
    if(i < 32) 
      TH1F *hEne01[i]  = (TH1F*)f1->Get(c1title); 
 
     
    // if(i>31 && i< 64) 
    //   TH1F *hEne02[i-32]  = (TH1F*)f2->Get(c1title); 
    // if(i > 63 && i<81) 
    //   TH1F *hEne03[i-64]  = (TH1F*)f3->Get(c1title); 
  //    TH1F *hEne04[i]  = (TH1F*)f4->Get(c1title); 
  //    TH1F *hEne05[i]  = (TH1F*)f5->Get(c1title); 
  //    TH1F *hEne06[i]  = (TH1F*)f6->Get(c1title); 
  // 
  } 
 
   TF1 *gaus1 = new TF1("gaus1","gaus"); 
    
   //   gStyle->SetCanvasColor(kGray+2); 
   c1 = new TCanvas("c1","TDC",1);  
   c1->Divide(9,9); 
 
   // line = TLine(300, 0, 300, 10000); 
 
   // line ->SetLineColor(kWhite); 
 
   //gStyle->SetOptStat(0); 
    gStyle->SetStatW(0.3); gStyle->SetStatH(0.5);  
   gStyle->SetStatFontSize(0.1); 
 
   for(Int_t i = 0; i<96 ; i++) 
     { 
       TPad *pcut1 =  c1->cd(i+1); 
       //      pcut1->SetLogy(); 
        
       sprintf(htitle,"CHANNEL_%d (in ns)",i); 
        
       if (i<32) 
  { 
    hEne01[i]->SetTitle(htitle); 
    hEne01[i]->GetXaxis()->SetRangeUser(0, 600); 
    hEne01[i]->GetXaxis()->SetLabelSize(0.08); 
    hEne01[i]->SetXTitle("TDC (ns)");  
    hEne01[i]->SetYTitle("counts"); 
    //  hEne01[i]->Fit("gaus1"); 
    hEne01[i]->Draw(); 
  } 
       /*        
      if (i>=32 && i<64) 
  { 
    hEne02[i-32]->SetTitle(htitle); 
    hEne02[i-32]->GetXaxis()->SetRangeUser(0, 600); 
    hEne02[i-32]->GetXaxis()->SetLabelSize(0.08); 
    hEne02[i-32]->SetXTitle("TDC (ns)");  
    hEne02[i-32]->SetYTitle("counts"); 
    hEne02[i-32]->Draw(); 
  } 
 
      if (i>=64 && i<81) 
  { 
    hEne03[i-64]->SetTitle(htitle); 
    hEne03[i-64]->GetXaxis()->SetRangeUser(0, 600); 
    hEne03[i-64]->GetXaxis()->SetLabelSize(0.08); 
    hEne03[i-64]->SetXTitle("TDC (ns)");  
    hEne03[i-64]->SetYTitle("counts"); 
    hEne03[i-64]->Draw(); 
  } 
       */ 
      /* 
      if (i>=48 && i<64) 
  { 
    hEne04[i-48]->SetTitle(htitle);  
    hEne04[i-48]->SetXTitle("ADC (a.u.)");  
    hEne04[i-48]->SetYTitle("counts"); 
    hEne04[i-48]->Draw(); 
  } 
 
      if (i>=64 && i<80) 
  { 
    hEne05[i-64]->SetTitle(htitle);  
    hEne05[i-64]->SetXTitle("ADC (a.u.)");  
    hEne05[i-64]->SetYTitle("counts"); 
    hEne05[i-64]->Draw(); 
  } 
       
      if (i==80) 
  { 
    hEne06[i-80]->SetTitle(htitle);  
    hEne06[i-80]->SetXTitle("ADC (a.u.)");  
    hEne06[i-80]->SetYTitle("counts"); 
    hEne06[i-80]->Draw(); 
  } 
       
//      ped[i] =  gaus1->GetParameter(1); 
*/ 
  } 
 
  
  // gStyle->SetOptStat(0); 
  
  // c2 = new TCanvas("c2","TDC",1);  
 
 
   
  // c2->cd(); 
   
  // hEne06[0]->SetTitle("ALL CHANNELS (Gain Match)");  
  // hEne06[0]->SetXTitle("ADC (a.u.)");  
  // hEne06[0]->SetYTitle("counts"); 
  // hEne06[0]->Draw(); 
   
 
  // for(Int_t i = 0; i<80 ; i++) 
  //   { 
  //     // TPad *pcut1 = c1->cd(i+1); 
  //     // pcut1->SetLogy(); 
       
  //     sprintf(htitle,"CHANNEL_%d",i); 
 
       
  //     if (i<16) 
  //   { 
  //     hEne01[i]->SetTitle(htitle);  
  //     hEne01[i]->SetXTitle("ADC (a.u.)");  
  //     hEne01[i]->SetYTitle("counts"); 
  //     hEne01[i]->SetLineColor(kRed); 
  //     //  hEne01[i]->Fit("gaus1"); 
 
  //     hEne01[i]->Draw("same"); 
  //   } 
       
  //     if (i>=16 && i<32) 
  //   { 
  //     hEne02[i-16]->SetTitle(htitle);  
  //     hEne02[i-16]->SetXTitle("ADC (a.u.)");  
  //     hEne02[i-16]->SetYTitle("counts"); 
  //     hEne02[i-16]->SetLineColor(kGreen); 
     
  //     hEne02[i-16]->Draw("same"); 
  //   } 
 
  //     if (i>=32 && i<48) 
  //   { 
  //     hEne03[i-32]->SetTitle(htitle);  
  //     hEne03[i-32]->SetXTitle("ADC (a.u.)");  
  //     hEne03[i-32]->SetYTitle("counts"); 
  //     hEne03[i-32]->SetLineColor(kMagenta); 
  //     hEne03[i-32]->Draw("same"); 
  //   } 
 
  //     if (i>=48 && i<64) 
  //   { 
  //     hEne04[i-48]->SetTitle(htitle);  
  //     hEne04[i-48]->SetXTitle("ADC (a.u.)");  
  //     hEne04[i-48]->SetYTitle("counts"); 
  //     hEne04[i-48]->SetLineColor(kYellow); 
  //     hEne04[i-48]->Draw("same"); 
  //   } 
 
  //     if (i>=64 && i<80) 
  //   { 
  //     hEne05[i-64]->SetTitle(htitle);  
  //     hEne05[i-64]->SetXTitle("ADC (a.u.)");  
  //     hEne05[i-64]->SetYTitle("counts"); 
  //     hEne05[i-64]->SetLineColor(kRed); 
  //     hEne05[i-64]->Draw("same"); 
  //   } 
       
 
       
//      ped[i] =  gaus1->GetParameter(1); 
 
  } 
 
   
 
  // for(Int_t i = 0; i<16 ; i++) 
  // { 
  //     cout<<"Pedestal mean: "<<ped[i]<<endl; 
  // } 
} 
