
//
// root -l dconfig.C
//

void dconfig( void )
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
  delphes->SetAdditionalSmearing(0.5);

  // Define eta ranges in ascending order;
  {
    auto eta = delphes->AddEtaRange(-3.5, -3.0);

    // Arguments [0..1]: momentum ranges for this eta in [GeV] in ascending order without gaps, 
    // Arguments [2..N]: sigma values in [mrad] matching the mass hypotheses above, in the same 
    // ascending order used to call delphes->AddMassHypothesis(); 
    eta->AddMomentumRange( 8., 10., 1.5, 1.5, 1.0);
    eta->AddMomentumRange(10., 11., 1.0, 1.0, 1.5);
  }
  {
    auto eta = delphes->AddEtaRange(-3.0, -1.7);

    // Well, average bin momentum is used to calculate Cherenkov theta, so the bins
    // should not be too wide in the areas where dtheta/dp is high for at least one 
    // of the hypotheses; 
    eta->AddMomentumRange( 8., 10., 1.1, 1.2, 1.3);
    eta->AddMomentumRange(10., 11., 1.0, 1.0, 1.5);
  }

  {
    auto eta = delphes->AddEtaRange(-1.7, -1.5);

    eta->AddMomentumRange( 8.,  9., 1.1, 1.2, 1.3);
    eta->AddMomentumRange( 9., 10., 1.1, 1.2, 1.3);
    eta->AddMomentumRange(10., 11., 1.0, 1.0, 1.5);
  } 

  delphes->WriteTcl();
  exit(0);
} // dconfig()
