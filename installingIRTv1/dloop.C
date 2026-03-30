
//
// root -l dloop.C
//

#include <dirent.h>

#define _AEROGEL_

void dloop(const char *fname = 0)
{
#ifdef _AEROGEL_
  auto delphes = new DelphesConfigRICH("dualRICH_aerogel");
  delphes->SetRefractiveIndex(1.0256);
  const char *dname = "EVALUATION.A";
#else
  auto delphes = new DelphesConfigRICH("dualRICH_c2f6");
  delphes->SetRefractiveIndex(1.00076);
  const char *dname = "EVALUATION.G";
#endif

  // Define particle mass hypotheses in ascending mass order; yes, there is no 
  // reason to overcomplicate things;
  delphes->AddMassHypothesis("pi+");
  delphes->AddMassHypothesis("K+");
  delphes->AddMassHypothesis("proton");

  // Imitate tracker resolution;
  delphes->SetAdditionalSmearing(0.50);

  // Loop through all .root file in this directory; the files presumably contain a single
  // tree with a single 'th' variable;
  
  struct dirent **namelist;
  int n = scandir(dname, &namelist, 0, alphasort);

    //while((curr_file = readdir(curr_dir))) {
  for(unsigned iq=0; iq<n; iq++) {
    if (!strcmp(namelist[iq]->d_name, ".") || !strcmp(namelist[iq]->d_name, "..")) continue;
    {
      unsigned len = strlen(namelist[iq]->d_name);
      if (len < 5 || strcmp(namelist[iq]->d_name + len - 5, ".root")) continue;
    }

    if (fname && strcmp(fname, namelist[iq]->d_name)) continue;

    auto ifdata = new TFile((std::string(dname) + "/" + namelist[iq]->d_name).c_str());
    if (!ifdata) {
      printf("failed to open '%s'\n", namelist[iq]->d_name);
      exit(0);
    } //if
    TTree *it = dynamic_cast<TTree*>(ifdata->Get("t"));
    if (!it) {
      printf("input file '%s' does not have \"t\" tree\n", namelist[iq]->d_name);
      exit(0);
    } //if

    int pdg;
    float emin, emax, pmin, pmax;
    const char *format = "drich-data.%d..%f-%f..%f-%f..juggler.evaluation.root";
    sscanf(namelist[iq]->d_name, format, &pdg, &emin, &emax, &pmin, &pmax);

    double par[100]; memset(par, 0x00, sizeof(par));

    const char *var = "1000*th";
    TH1D *dth1 = new TH1D("dth1", "dth1", 50, -10.0, 10.0);
    it->Project("dth1", var);
    TF1 *fq1 = new TF1("fq1", "gaus(0)",    -10.0, 10.0);
    par[0] = 100.0; par[1] = 0.0; par[2] = 1.0;
    fq1->SetParameters(par);
    dth1->Fit("fq1","R");
    fq1->GetParameters(par);

    double mean2 = par[1], sigma2 = fabs(par[2]), min2 = mean2-5*sigma2, max2 = mean2+5*sigma2;

    TH1D *dth2 = new TH1D("dth2", "dth2", 50, min2, max2);
    it->Project("dth2", var);
    TF1 *fq2 = new TF1("fq2", "gaus(0)", min2, max2);
    fq2->SetParameters(par);
    dth2->Fit("fq2","R");
    fq2->GetParameters(par);

    double mean3 = par[1], sigma3 = fabs(par[2]), min3 = mean3-5*sigma3, max3 = mean3+5*sigma3;
    
    TH1D *dth3 = new TH1D("dth3", "dth3", 50, min3, max3);
    it->Project("dth3", var);
    TF1 *fq3 = new TF1("fq3", "gaus(0)", min3, max3);
    fq3->SetParameters(par);
    dth3->Fit("fq3","R");

    printf("@@@ \"%s\" -> %6.2f\n", namelist[iq]->d_name, fabs(par[2]));

    // Shamelessly use the fact that 211/321/2212 are in alphabetic order, and 
    // alphasort takes care about ordering; then all eta ranges and all momentum 
    // ranges will be imported in order, and since 211 files are always present
    // the whole flakey system will seemingly work;
    auto erange = delphes->AddEtaRange    (emin, emax);
    auto mrange = erange->GetMomentumRange(pmin, pmax);
    // FIXME: should check the return code;
    {
      bool ret = delphes->StoreSigmaEntry(mrange, pdg, fabs(par[2]));
      if (!ret) {
	printf("Failed to insert a sigma entry\n");
	exit(0);
      } //if
    } 
  } //for iq
  
  delphes->AddZeroSigmaEntries();
  delphes->Print();
  delphes->WriteTcl();

  if (!fname) exit(0);
} // dconfig()
