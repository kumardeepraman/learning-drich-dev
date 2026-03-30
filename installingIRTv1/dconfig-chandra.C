
//
// root -l dconfig.C
//

void dconfig_chandra( void )
{
  auto delphes = new DelphesConfigRICH("pfRICH");

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
  delphes->AddMassHypothesis("pi+");
  delphes->AddMassHypothesis("K+");
  delphes->AddMassHypothesis("proton");

  // Define radiator refractive index;
  delphes->SetRefractiveIndex(1.026);
  // Imitate tracker resolution;
  delphes->SetAdditionalSmearing(0.50);

  // Define eta ranges in ascending order;
  {
    auto eta = delphes->AddEtaRange(-3.8, -2.8);

    // Arguments [0..1]: momentum ranges for this eta in [GeV] in ascending order without gaps, 
    // Arguments [2..N]: sigma values in [mrad] matching the mass hypotheses above, in the same 
    // ascending order used to call delphes->AddMassHypothesis(); 
    eta->AddMomentumRange( 0.9, 2.9, 1.5, 0.0, 0.0);
    eta->AddMomentumRange( 2.9, 3.1, 1.65, 4.83, 0.0);
    eta->AddMomentumRange( 3.1, 3.5, 1.83, 7.72, 0.0);
    eta->AddMomentumRange( 3.5, 4.5, 1.51, 1.63, 0.0);
    eta->AddMomentumRange( 4.5, 5.5, 1.48, 1.60, 0.0);
    eta->AddMomentumRange( 5.5, 6.1, 1.51, 1.63, 5.10);
    eta->AddMomentumRange( 6.1, 6.5, 1.83, 1.5, 4.70);
    eta->AddMomentumRange( 6.5, 7.5, 1.43, 1.76, 6.48);
    eta->AddMomentumRange( 7.5, 8.5, 1.82, 1.81, 4.67);
    eta->AddMomentumRange( 8.5, 9.5, 1.75, 1.58, 3.52);
    eta->AddMomentumRange( 9.5, 10.5, 1.52, 1.60, 1.82);
    eta->AddMomentumRange(10.5, 11.5, 1.75, 1.63, 1.90);
    eta->AddMomentumRange(11.5, 12.5, 1.32, 1.64, 1.54);
    eta->AddMomentumRange(12.5, 13.5, 1.57, 1.58, 1.70);
    eta->AddMomentumRange(13.5, 14.5, 1.49, 1.51, 1.84);
    eta->AddMomentumRange(14.5, 15.5, 1.52, 1.58, 1.73);
  }
  {
    auto eta = delphes->AddEtaRange(-2.8, -1.9);

    // Well, average bin momentum is used to calculate Cherenkov theta, so the bins
    // should not be too wide in the areas where dtheta/dp is high for at least one 
    // of the hypotheses; 
    eta->AddMomentumRange( 0.9, 2.9, 1.5, 0.0, 0.0);
    eta->AddMomentumRange( 2.9, 3.1, 1.87, 5.15, 0.0);
    eta->AddMomentumRange( 3.1, 3.5, 1.63, 6.20, 0.0);
    eta->AddMomentumRange( 3.5, 4.5, 1.73, 1.52, 0.0);
    eta->AddMomentumRange( 4.5, 5.5, 1.71, 1.55, 0.0);
    eta->AddMomentumRange( 5.5, 6.1, 1.48, 1.50, 5.00);
    eta->AddMomentumRange( 6.1, 6.5, 1.83, 1.5, 4.0);   
    eta->AddMomentumRange( 6.5, 7.5, 1.53, 1.78, 5.77);
    eta->AddMomentumRange( 7.5, 8.5, 1.72, 1.67, 4.30);
    eta->AddMomentumRange( 8.5, 9.5, 1.65, 1.47, 2.80);
    eta->AddMomentumRange( 9.5, 10.5, 1.52, 1.48, 1.95);
    eta->AddMomentumRange(10.5, 11.5, 1.43, 1.50, 1.83);
    eta->AddMomentumRange(11.5, 12.5, 1.40, 1.54, 1.73);
    eta->AddMomentumRange(12.5, 13.5, 1.35, 1.45, 1.72);
    eta->AddMomentumRange(13.5, 14.5, 1.53, 1.47, 1.66);
    eta->AddMomentumRange(14.5, 15.5, 1.54, 1.44, 1.54);
  }

  {
    auto eta = delphes->AddEtaRange(-1.9, -1.5);

    eta->AddMomentumRange( 0.9, 2.9, 1.5, 0.0, 0.0);
    eta->AddMomentumRange( 2.9, 3.1, 1.50, 3.8, 0.0);
    eta->AddMomentumRange( 3.1, 3.5, 1.83, 7.8, 0.0);    
    eta->AddMomentumRange( 3.5, 4.5, 1.62, 2.12, 0.0);
    eta->AddMomentumRange( 4.5, 5.5, 1.73, 1.70, 0.0);
    eta->AddMomentumRange( 5.5, 6.1, 1.44, 1.56, 6.5);
    eta->AddMomentumRange( 6.1, 6.5, 1.83, 1.5, 4.0);    
    eta->AddMomentumRange( 6.5, 7.5, 1.53, 1.46, 5.40);
    eta->AddMomentumRange( 7.5, 8.5, 1.65, 1.52, 3.69);
    eta->AddMomentumRange( 8.5, 9.5, 1.41, 1.48, 2.52);
    eta->AddMomentumRange( 9.5, 10.5, 1.32, 1.49, 1.62);
    eta->AddMomentumRange(10.5, 11.5, 1.49, 1.51, 1.80);
    eta->AddMomentumRange(11.5, 12.5, 1.43, 1.48, 1.82);
    eta->AddMomentumRange(12.5, 13.5, 1.54, 1.47, 1.70);
    eta->AddMomentumRange(13.5, 14.5, 1.50, 1.51, 1.66);
    eta->AddMomentumRange(14.5, 15.5, 1.50, 1.49, 1.54); } 

  // This call also makes a guess on the kaon and proton thresholds;
  delphes->AddZeroSigmaEntries();
  delphes->Print();
  delphes->WriteTcl();
  {
    auto fout = new TFile("pfRICH.root", "RECREATE");
    delphes->Write();
    fout->Close();
  }
  exit(0);
} // dconfig()
