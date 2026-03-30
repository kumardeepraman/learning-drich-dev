#ifndef G4E_CI_DRICH_MODEL_HH
#define G4E_CI_DRICH_MODEL_HH

/* 
 * g4dRIChOptics class hierarchy
 * -----------------------------
 * original authors: E. Cisbani, A. Del Dotto, C. Fanelli
 * source: git@github.com:cisbani/dRICh.git
 * -> adapted for usage in EPIC
 */

#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include "Geant4/G4Material.hh"
#include "Geant4/G4SystemOfUnits.hh"
#include "Geant4/G4MaterialPropertiesTable.hh"
#include "Geant4/G4OpticalSurface.hh"
#include "Geant4/G4LogicalSkinSurface.hh"
#include "Geant4/G4tgbMaterialMgr.hh"
#include "Geant4/G4tgbVolumeMgr.hh"

/*
 * Service Classes
 */

//
// Generic optical parameters class
//

class g4dRIChOptics {

public:
  
  double *scaledE; // photon energies
  double *scaledN; // real refractive index
  double *scaledA; // absorption length
  double *scaledS; // scattering length

  double *scaledSE; // surface efficiency 
  double *scaledSR;  // surface reflectivity
  double *scaledIN; // imaginary refractive index

  // Add optical properties either to material or volume surface
  // matName: material name
  // logVolName: logical volume name
  // if name is "_NA_", properties are not applied to the corresponding component 
  g4dRIChOptics(const G4String matName, const G4String logVolName) {

    printf("#=======================================================================\n");
    printf("# Set Optical Properties\n");
    
    materialName = matName;
    logicalVName = logVolName;
    
    scaledE=NULL;
    scaledN=NULL;
    scaledA=NULL;
    scaledS=NULL;

    scaledSE = NULL;
    scaledSR = NULL;
    scaledIN = NULL;

    if (matName != "_NA_") {

      printf("# Material %s\n",matName.data());

      mat = G4tgbMaterialMgr::GetInstance()->FindBuiltG4Material(matName);

      if (mat == NULL) {
	pTable = NULL;
	printf("# ERROR: Cannot retrieve %s material in ci_DRICH\n",matName.data());
	// handle error
      } else {
	pTable = mat->GetMaterialPropertiesTable();
      }
      
      if (pTable == NULL) {
	printf("# No properties table available for %s, allocated a new one\n",matName.data());
	pTable = new G4MaterialPropertiesTable();    
      } else {
	pTable->DumpTable();
      }
    }

    if (logVolName != "_NA_") {
      printf("# Logical Volume %s\n",logVolName.data());
      logVolume = G4tgbVolumeMgr::GetInstance()->FindG4LogVol(logVolName, 0);
      if (logVolume == NULL) {
	printf("# ERROR: Cannot retrieve %s logical volume in ci_DRICH\n",logVolName.data());
	// handle error
      }
    }
    
  };
  
  ~g4dRIChOptics() {

    if (scaledE !=NULL) delete[] scaledE;
    if (scaledN !=NULL) delete[] scaledN;
    if (scaledA !=NULL) delete[] scaledA;
    if (scaledS !=NULL) delete[] scaledS;

    if (scaledSE !=NULL) delete[] scaledSE;
    if (scaledSR !=NULL) delete[] scaledSR;
    if (scaledIN !=NULL) delete[] scaledIN;

  };

  // dvalue, ivalue, svalue may represent different quantities depending on implementation
  virtual int setOpticalParams() { return -1; };
  virtual int setOpticalParams(double dvalue) { return -1; };
  virtual int setOpticalParams(int ivalue) { return -1; };
  virtual int setOpticalParams(int ivalue, double dvalue) { return -1; };
  virtual int setOpticalParams(G4String svalue) { return -1; };

  // accessors
  G4String getMaterialName() { return materialName; };
  G4String getLogicalVName() { return logicalVName; };
  G4OpticalSurface *getSurface() { return pOps; };

  // get the appropriate material property table
  G4MaterialPropertiesTable *getMaterialPropertyTable() {
    auto t = pTable;
    if(t==nullptr && pOps!=nullptr) t = pOps->GetMaterialPropertiesTable();
    if(t==nullptr) fmt::print(stderr,"ERROR: cannot find material property table for ({},{})\n",materialName,logicalVName);
    return t;
  }

  // get list of material property tables (that are in use)
  std::vector<G4String> getMaterialPropertyNames() {
    std::vector<G4String> validProps;
    auto tab = getMaterialPropertyTable();
    if(tab!=nullptr) {
      for(const auto& propName : tab->GetMaterialPropertyNames()) {
        if(tab->GetProperty(propName)!=nullptr) validProps.push_back(propName);
      }
    }
    return validProps;
  }

  // get the size of the material property table
  int getMaterialPropertyTableSize() { return int(getMaterialPropertyNames().size()); }

  // execute lambda function `block(energy,value)` for each entry (energy,value) in the material property table
  void loopMaterialPropertyTable(G4String propName, std::function<void(G4double,G4double)> block, bool reverseOrder=false) {
    auto tab = getMaterialPropertyTable();
    if(tab==nullptr) return;
    auto prop = tab->GetProperty(propName);
    if(prop==nullptr) return;
    if(reverseOrder) {
      for(int i=prop->GetVectorLength()-1; i>=0; i--) {
        auto energy = prop->Energy(i);
        auto value  = prop->operator[](i);
        block(energy,value);
      }
    } else {
      for(int i=0; i<prop->GetVectorLength(); i++) {
        auto energy = prop->Energy(i);
        auto value  = prop->operator[](i);
        block(energy,value);
      }
    }
  }

  // converters
  static double wl2e(double wl) { // wavelength to energy
    return 1239.84193 * eV / (wl/nm); 
  };

  static double e2wl(double e) { // energy to wavelength
    return 1239.84193 *nm / (e / eV);
  };


protected:

  G4String materialName, logicalVName;
  G4Material *mat;
  G4OpticalSurface *pOps;
  G4LogicalVolume *logVolume; // used for skin surface
    
  G4MaterialPropertiesTable *pTable;
  
  // add properties to the MaterialPropertiesTable and link to material
  void setMatPropTable(int nEntries) {
      
    if (scaledN!=NULL) pTable->AddProperty("RINDEX",    scaledE, scaledN, nEntries, false, true); // `true` replaced `SetSpline(true)`
    if (scaledA!=NULL) pTable->AddProperty("ABSLENGTH", scaledE, scaledA, nEntries, false, true);
    if (scaledS!=NULL) pTable->AddProperty("RAYLEIGH",  scaledE, scaledS, nEntries, false, true);
    //    pTable->AddConstProperty("SCINTILLATIONYIELD", 0. / MeV); // @@@ TBC @@@
    //    pTable->AddConstProperty("RESOLUTIONSCALE", 1.0); // @@@ TBC @@@

    mat->SetMaterialPropertiesTable(pTable);
    printf("# Optical Table for material %s with %d points:\n",materialName.data(),nEntries);
    pTable->DumpTable();
    
  };
  
  // allocate and add properties to the MaterialPropertiesTable
  G4MaterialPropertiesTable *addSkinPropTable(int nE) {
    
    G4MaterialPropertiesTable *pTab = new G4MaterialPropertiesTable();
    if (scaledSE !=NULL) pTab->AddProperty("EFFICIENCY", scaledE, scaledSE, nE);
    if (scaledSR !=NULL) pTab->AddProperty("REFLECTIVITY", scaledE, scaledSR, nE);
    if (scaledN !=NULL) pTab->AddProperty("REALRINDEX", scaledE, scaledN, nE);
    if (scaledIN !=NULL) pTab->AddProperty("IMAGINARYRINDEX", scaledE, scaledIN, nE);
    printf("# Optical Table for volume %s with %d points:\n",logicalVName.data(),nE);
    pTab->DumpTable();
    
    return pTab;

  };
  
  // Linear Interpolation method
  double linint(double val, int n, const double* x, const double* y) {
    if (val<=x[0]) return y[0];
    if (val>=x[n-1]) return y[n-1];
    for (int i=0;i<(n-1);i++) {
      if ((val>=x[i]) && (val<x[i+1])) {
	return (y[i+1]-y[i])/(x[i+1]-x[i])*(val-x[i])+y[i];
      }
    }
    return 0.;
  };
  
};

//
// Aerogel
//
class g4dRIChAerogel : public g4dRIChOptics {

public:

  g4dRIChAerogel(const G4String matName) : g4dRIChOptics(matName, "_NA_") {};
  //
  // Compute the refractive index, absorption length, scattering length for different energies points
  // 
  // different methods are available for the refractive index:
  //   0 - Vorobiev
  //   1 - Sellmeier - CLAS12
  //   2 - Sellmeier - LHCB
  //   3 - CLAS12 experimental points rescaled by Alessio/GEMC (the same used for the scattering and absorption length)
  //
  // data are scaled according to the input density of the aerogel
  //
  int setOpticalParams(int mode) {
    
    const double aeroE[]= // energy : wavelenth 660 nm -> 200 nm
      { 1.87855*eV,1.96673*eV,2.05490*eV,2.14308*eV,2.23126*eV, 2.31943*eV,2.40761*eV,2.49579*eV,2.58396*eV,2.67214*eV,
	2.76032*eV,2.84849*eV,2.93667*eV,3.02485*eV,3.11302*eV, 3.20120*eV,3.28938*eV,3.37755*eV,3.46573*eV,3.55391*eV,
	3.64208*eV,3.73026*eV,3.81844*eV,3.90661*eV,3.99479*eV, 4.08297*eV,4.17114*eV,4.25932*eV,4.34750*eV,4.43567*eV,
	4.52385*eV,4.61203*eV,4.70020*eV,4.78838*eV,4.87656*eV, 4.96473*eV,5.05291*eV,5.14109*eV,5.22927*eV,5.31744*eV,
	5.40562*eV,5.49380*eV,5.58197*eV,5.67015*eV,5.75833*eV, 5.84650*eV,5.93468*eV,6.02286*eV,6.11103*eV,6.19921*eV };
    
const double aeroN[] = {
1.02524, 1.02526, 1.02528, 1.02530, 1.02532, 1.02534, 1.02536, 1.02538, 1.02540, 1.02542,
1.02544, 1.02546, 1.02548, 1.02550, 1.02552, 1.02554, 1.02556, 1.02558, 1.02560, 1.02562,
1.02564, 1.02566, 1.02568, 1.02570, 1.02572, 1.02574, 1.02576, 1.02578, 1.02580, 1.02582,
1.02584, 1.02586, 1.02588, 1.02590, 1.02592, 1.02594, 1.02596, 1.02598, 1.02600, 1.02602,
1.02604, 1.02606, 1.02608, 1.02610, 1.02612, 1.02614, 1.02616, 1.02618, 1.02620, 1.02622
};
    
const double aeroA[] = {
150.918*cm, 150.831*cm, 147.447*cm, 140.932*cm, 133.467*cm, 124.969*cm, 115.427*cm, 105.469*cm, 91.714*cm, 81.8737*cm,
71.613*cm, 60.0969*cm, 51.485*cm, 43.9643*cm, 36.5576*cm, 30.6259*cm, 25.5416*cm, 21.3454*cm, 17.7721*cm, 14.9673*cm,
12.5649*cm, 10.5289*cm, 8.86394*cm, 7.49752*cm, 6.35977*cm, 5.33704*cm, 4.51245*cm, 3.83452*cm, 3.28786*cm, 2.81349*cm,
2.44195*cm, 2.10344*cm, 1.8156*cm, 1.55298*cm, 1.3477*cm, 1.14369*cm, 1.00444*cm, 0.854531*cm, 0.730264*cm, 0.602408*cm,
0.444152*cm, 0.398912*cm, 0.330515*cm, 0.297*cm, 0.27*cm, 0.24*cm, 0.22*cm, 0.20*cm, 0.19*cm, 0.194293*cm
};
    
const double aeroS[] = {
43.4498*cm, 36.166*cm, 30.347*cm, 25.6523*cm, 21.8312*cm, 18.6962*cm, 16.104*cm, 13.9459*cm, 12.1377*cm, 10.6131*cm,
9.32055*cm, 8.21903*cm, 7.27543*cm, 6.46344*cm, 5.76171*cm, 5.15262*cm, 4.62193*cm, 4.15788*cm, 3.7506*cm, 3.39198*cm,
3.07526*cm, 2.79462*cm, 2.54528*cm, 2.32316*cm, 2.12473*cm, 1.94704*cm, 1.78756*cm, 1.64406*cm, 1.51468*cm, 1.39779*cm,
1.29195*cm, 1.19594*cm, 1.1087*cm, 1.02926*cm, 0.956808*cm, 0.890629*cm, 0.830067*cm, 0.774566*cm, 0.723627*cm, 0.676813*cm,
0.63372*cm, 0.594002*cm, 0.557352*cm, 0.53*cm, 0.50*cm, 0.48*cm, 0.46*cm, 0.44*cm, 0.42*cm, 0.366376*cm
};

    const int nEntries = sizeof(aeroE)/sizeof(double);

    double density = mat->GetDensity();
    printf("# Aerogel Density : %f g/cm3\n",density/(g/cm3));
    
    double refn = density2refIndex(density); // use a n vs rho formula with provide n at 400 nm
    double refwl = 400*nm;
    
    double refee = wl2e(refwl)/eV; // [eV] reference energy
    double an0 = linint(refee, nEntries, aeroE, aeroN);
    //    double aa0 = linint(refee, nEntries, aeroE, aeroA);
    //    double as0 = linint(refee, nEntries, aeroE, aeroS);

    double aa;
    double nn;
    double ri, a0, wl0, rnscale;
    double rho = 0.0;
    
    if (scaledE==NULL) {
      scaledE = new double[nEntries];
      scaledN = new double[nEntries];
      scaledS = new double[nEntries];
      scaledA = new double[nEntries];
    }
    
    for (int i=0;i<nEntries;i++) {
      double ee = aeroE[i]; 
      double wl = e2wl(ee)/nm; // from Energy to nm

      switch (mode) {
      case 0:     // --- Vorobiev model
	aa = airFraction(refn, refwl);
	nn = aa * riAir(wl) + (1. - aa)*riQuartz(wl);
	break;
      case 1:     // --- Sellmeier, 1 pole from (CLAS12/RICH EPJ A (2016) 52: 23)
	ri = 1.0494; // 400 nm
	rho = 0.230; // g/cm3
	a0 = 0.09683;
	wl0 = 84.13;
	rnscale =  sqrt(1.+ (a0*refwl*refwl)/(refwl*refwl-a0*a0));
	nn = sqrt(1.+ (a0*wl*wl)/(wl*wl-a0*a0))* refn / rnscale;
	break;
      case 2:    // --- Sellmeier, 1 pole from T. Bellunato et al. Eur. Phys. J. C52, 759-764 (2007)
	ri = 1.03; // 400 nm
	rho = 0.149; // g/cm3
	a0 = 0.05639;
	wl0 = 84.22;
	rnscale =  sqrt(1.+ (a0*refwl*refwl)/(refwl*refwl-a0*a0));
	nn = sqrt(1.+ (a0*wl*wl)/(wl*wl-a0*a0)) * refn / rnscale;
	break;
      case 3:    // --- experimental points 
	rho = 0.088; // g/cm3
	nn = aeroN[i] * refn / an0; // scale refractive index
	break;
      default:
	nn = refn;
	break;
      }

      scaledE[i] = ee;
      scaledN[i] = nn;
      scaledA[i] = aeroA[i] * (rho*g/cm3)/density; // approx. larger the density, smaller the abs. length
      scaledS[i] = aeroS[i] * (rho*g/cm3)/density; // approx. larger the density, smaller the abs. length

    }

    printf("# Aerogel Refractive Index, Absorption and Scattering Lengths rescaled to density %.5f g/cm3, method: %d\n", density/g*cm3, mode);

    setMatPropTable(nEntries);
    
    return nEntries;
    
  }
  
private:

  // Quartz (SiO2) Refractive index: https://refractiveindex.info/?shelf=main&book=SiO2&page=Malitson
  double riQuartz(double wl) { // wavelength   
    double x = wl / um;
    double nn = sqrt(1+0.6961663*x*x/(x*x-pow(0.0684043,2))+0.4079426*x*x/(x*x-pow(0.1162414,2))+0.8974794*x*x/(x*x-pow(9.896161,2)));
    if (nn<1.) {
      printf("# WARNING: estimated quartz refractive index is %f at wavelength %f nm -> set to 1\n",nn,x);
      nn = 1.;
    }
    return nn;
  }

  // Air Refractive index: https://refractiveindex.info/?shelf=other&book=air&page=Ciddor
  double riAir(double wl) { // wavelength   
    double x = wl / um;
    double nn = 1.0+(0.05792105/(238.0185-1.0/x/x)+0.00167917/(57.362-1.0/x/x));
    if (nn<1.) {
      printf("# WARNING: estimated air refractive index is %f at wavelength %f nm -> set to 1\n",nn,x);
      nn = 1.;
    }
    return nn;
  }

  /*
   * n_aer = A * n_air + (1-A) * n_quartz  : compute air weight-fraction given a reference n_air,lambda
   *  Vorobiev Model
   *    rn     : reference refractive index of the aerogel
   *    rlambda: wavelength of the reference refractive index
   */
  double airFraction(double rn, double rlambda) {
    double rnq = riQuartz(rlambda);
    double rna = riAir(rlambda);
    double a = (rnq - rn)/(rnq - rna);
    return a;
  }
  
  // density of aerogel from air and quartz densities
  // rho = (n-1)/0.21 g/cm3 (Bellunato) -> rho proportional to (n-1)
  // rho_air    = 0.0011939 g/cm3   T=25 deg
  // rho_quartz = 2.196 g/cm3  (amorphous ?)
  double Density(double rn, double rlambda) {
    double aa = airFraction(rn, rlambda);
    return (aa * 1.1939 * mg/cm3 + (1.- aa ) * 2.32 * g/cm3);
  }

  // at 400 nm - (CLAS12/RICH EPJ A (2016) 52: 23)
  double density2refIndex(double rho) {
    double nn2 = 1.+0.438*rho/g*cm3;
    return sqrt(nn2);
  }
  
};

//
// Acrylic Filter
//

class g4dRIChFilter : public g4dRIChOptics {
  
public:

  g4dRIChFilter(const G4String matName) : g4dRIChOptics(matName, "_NA_") {};

  // wlthr: threshold wavelength for low pass filter
  // mode currently not used
  int setOpticalParams(double wlthr) {

    const double acryE[]= // energy : wavelenth 660 nm -> 200 nm
      { 1.87855*eV,1.96673*eV,2.05490*eV,2.14308*eV,2.23126*eV, 2.31943*eV,2.40761*eV,2.49579*eV,2.58396*eV,2.67214*eV,
	2.76032*eV,2.84849*eV,2.93667*eV,3.02485*eV,3.11302*eV, 3.20120*eV,3.28938*eV,3.37755*eV,3.46573*eV,3.55391*eV,
	3.64208*eV,3.73026*eV,3.81844*eV,3.90661*eV,3.99479*eV, 4.08297*eV,4.17114*eV,4.25932*eV,4.34750*eV,4.43567*eV,
	4.52385*eV,4.61203*eV,4.70020*eV,4.78838*eV,4.87656*eV, 4.96473*eV,5.05291*eV,5.14109*eV,5.22927*eV,5.31744*eV,
	5.40562*eV,5.49380*eV,5.58197*eV,5.67015*eV,5.75833*eV, 5.84650*eV,5.93468*eV,6.02286*eV,6.11103*eV,6.19921*eV };

    const double acryN[]= // refractive index
      { 1.4902, 1.4907, 1.4913, 1.4918, 1.4924, 1.4930,  1.4936,  1.4942,  1.4948,  1.4954,
	1.4960,  1.4965,  1.4971,  1.4977,  1.4983, 1.4991,  1.5002,  1.5017,  1.5017,  1.5017,
	1.5017,  1.5017,  1.5017,  1.5017,  1.5017, 1.5017,  1.5017,  1.5017,  1.5017,  1.5017,
	1.5017,  1.5017,  1.5017,  1.5017,  1.5017, 1.5017,  1.5017,  1.5017,  1.5017,  1.5017,
	1.5017,  1.5017,  1.5017,  1.5017,  1.5017, 1.5017,  1.5017,  1.5017,  1.5017,  1.5017};
    
    const double acryA[]= // absorption length
      { 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm,
	14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8495*cm, 14.8494*cm, 14.8486*cm, 14.844*cm, 
	14.8198*cm, 14.7023*cm, 14.1905*cm, 12.3674*cm, 8.20704*cm, 3.69138*cm, 1.33325*cm, 0.503627*cm, 0.23393*cm, 0.136177*cm,  
	0.0933192*cm, 0.0708268*cm, 0.0573082*cm, 0.0483641*cm, 0.0420282*cm, 0.0373102*cm, 0.033662*cm, 0.0307572*cm, 0.0283899*cm, 0.0264235*cm, 
	0.0247641*cm, 0.0233451*cm, 0.0221177*cm, 0.0210456*cm, 0.0201011*cm, 0.0192627*cm, 0.0185134*cm, 0.0178398*cm, 0.0172309*cm, 0.0166779*cm };  

    const int nEntries = sizeof(acryE)/sizeof(double);
    
    if (scaledE==NULL) {
      scaledE = new double[nEntries];
      scaledN = new double[nEntries];
      scaledS = new double[nEntries];
      scaledA = new double[nEntries];
    }
    
    double e0 = acryE[24]; // wavelength corresponding to the above data threshold at about Half Maximum
    double ethr= wl2e(wlthr);

    int ithr=-1;
    double eshift=0;
    
    for (int i=0;i<(nEntries-1);i++) { // find closest
      double d1 = ethr-acryE[i];
      if (d1>=0) { // good bin
	ithr = i;
	eshift = d1;
      }
      break;
    }
    if (ithr==-1) {
      fprintf(stderr,"# ERROR filter: wavelength threshold %f nm is out of range\n",wlthr/nm);
      return 0;
    }
	
    for (int i=0;i<nEntries;i++) {
      scaledE[i] = acryE[i]+eshift; // to maky ethr corresponds to a sampled point
      scaledN[i] = linint((scaledE[i] - ethr + e0), nEntries, acryE, acryN);
      scaledA[i] = linint((scaledE[i] - ethr + e0), nEntries, acryE, acryA); 
      scaledS[i] = 100000.*cm; // @@@@
    }

    printf("# Acrylic Filter Refractive Index, Absorption and Scattering Lengths rescaled to wavelength threshold %.1f nm\n", wlthr/nm);

    setMatPropTable(nEntries);
    
    return nEntries;
    
  };

private:
  
};

//
// gas
//

class g4dRIChGas : public g4dRIChOptics {

public:

  g4dRIChGas(const G4String matName) : g4dRIChOptics(matName, "_NA_") {

    int nel = mat->GetNumberOfElements(); 
    printf("# Gas material number of elements %d\n", nel);

    chemFormula="";
    
    for (int i=0;i<nel;i++) {  // extract chemical formula from gas material
      auto ele = mat->GetElement(i);
      printf("# Element %d : Z %f  Name %s Atoms %d\n",i,ele->GetZ(),ele->GetSymbol().data(),mat->GetAtomsVector()[i]); 
      chemFormula = chemFormula + ele->GetSymbol() + std::to_string(mat->GetAtomsVector()[i]);
    }
    printf("# Chemical Formula : %s\n",chemFormula.data());
    
  };
  
  int setOpticalParams() {

    // different gas types parameters
    G4String gasType[] = { "C2F6", "CF4", "C4F10" };

    // absorption lengths
    // C2F6 and CF4: assumed to be 10m
    // C4F10: 6m, for wavelength 200-700 nm (ATLAS note Simulation of ATLAS Luminosity Monitoring with LUCID)
    const double absLength[] = { 30.*m, 10.*m, 6.*m };

    // A.W. Burner and W. K. Goad - Measurement of the Specific Refractivities of CF4 and C2F6
    // for gases: n-1 = K*rho : K=specific refractivity or Gladstone-Dale constant
    // C2F6: rho = 5.7 kg/m^3, K=0.131 cm^3/g +/- 0.0009 cm^3/g at 300 K, lambda=633 nm
    // CF4:  rho = 7.2 kg/m^3, K=0.122 cm^3/g +/- 0.0009 cm^3/g at 300 K, lambda=633 nm   
    //
    // C4F10: rho = 9.935 kg/m3 at 25°C and 1 atm // NICNAS file NA/317 (1996), perfluorobutane
    //                                            // PFG-5040 3M
    //        K = ??? (TODO; density-dependence disabled for now)
    double Ksr[]={ 0.131*cm3/g, 0.122*cm3/g, 0.000*cm3/g };
    
    // One term Sellmeier formula: n-1 = A*10^-6 / (l0^-2 - l^-2)
    // C2F6: A=0.18994, l0 = 65.47 [nm] (wavelength, E0=18.82 eV)  :  NIMA 354 (1995) 417-418
    // CF4: A=0.124523, l0=61.88 nm (E0=20.04 eV) : NIMA 292 (1990) 593-594
    // C4F10: A=0.2375, l0=73.63 nm (E0=16.84 eV) : NIMA 510 (2003) 262–272
    double Asel[]={0.18994, 0.124523, 0.2375};
    double L0sel[]={65.47*nm, 61.88*nm, 73.63*nm};

    int igas=0;

    for (int i=0;i<3;i++) {
      if (chemFormula == gasType[i]) igas=i;
    }

    printf("# Selected gas index %d for gas %s\n",igas, chemFormula.data());
    
    double density = mat->GetDensity();
    double refn = Ksr[igas] * density + 1.;
    double wlref = 633*nm; // for density vs refractive index
    
    int nEntries = 16;
    double wl0 = 200.*nm;
    double wl1 = 1000.*nm;
    double dwl = (wl1-wl0)/(nEntries-1.);
    
    if (scaledE==NULL) {
      scaledE = new double[nEntries];
      scaledN = new double[nEntries];
      scaledS = new double[nEntries];
      scaledA = new double[nEntries];
    }

    double l02 = 1./(L0sel[igas]/nm)/(L0sel[igas]/nm);
      
    double rnscale = Asel[igas]/1e6/(l02 - 1./(wlref/nm)/(wlref/nm))+1.;
    if(chemFormula=="C4F10") rnscale = refn; // disable density-dependent refractive index
    
    for (int i=0;i<nEntries;i++) {

      double wl = wl1 - i*dwl; // to get increasing energy
      double ee = wl2e(wl);

      scaledE[i]=ee;
      scaledN[i]=(Asel[igas]/1e6/(l02 - 1./(wl/nm)/(wl/nm))+1.) * refn/rnscale;
      scaledA[i]=absLength[igas];    // @@@@
      scaledS[i]=100000.*cm; // @@@@
    }

    printf("# Gas Refractive Index, Absorption and Scattering Lengths rescaled to density %f g/cm3, gas index: %d\n", density/g*cm3, igas);

    setMatPropTable(nEntries);
    
    return nEntries;

  };

private:

  G4String chemFormula; // chemical formula of the gas material
  
};

//
// Mirror
//

class g4dRIChMirror : public g4dRIChOptics {

public:
  g4dRIChMirror(const G4String logName) : g4dRIChOptics("_NA_", logName) { };

  // pSurfName: prefix used to generate surface names inserted in optical table
  
  int setOpticalParams(G4String pSurfName) {

    G4String surfaceName = pSurfName + "mirrorSurf";
    G4String skinSurfaceName = pSurfName + "mirrorSkinSurf";
    
    const double mirrorE[] =
      { 2.04358*eV, 2.0664*eV, 2.09046*eV, 2.14023*eV, 2.16601*eV, 2.20587*eV, 2.23327*eV, 2.26137*eV, 
	2.31972*eV, 2.35005*eV, 2.38116*eV, 2.41313*eV, 2.44598*eV, 2.47968*eV, 2.53081*eV, 2.58354*eV, 
	2.6194*eV, 2.69589*eV, 2.73515*eV, 2.79685*eV, 2.86139*eV, 2.95271*eV, 3.04884*eV, 3.12665*eV, 
	3.2393*eV, 3.39218*eV, 3.52508*eV, 3.66893*eV, 3.82396*eV, 3.99949*eV, 4.13281*eV, 4.27679*eV, 
	4.48244*eV, 4.65057*eV, 4.89476*eV, 5.02774*eV, 5.16816*eV, 5.31437*eV, 5.63821*eV, 5.90401*eV, 
	6.19921*eV };
    
    const double mirrorR[]= // Reflectivity of AlMgF2 coated on thermally shaped acrylic sheets, measured by AJRP, 10/01/2012:
      { 0.8678125, 0.8651562, 0.8639063, 0.8637500, 0.8640625, 0.8645313, 0.8643750, 0.8656250,
	0.8653125, 0.8650000, 0.8648437, 0.8638281, 0.8635156, 0.8631250, 0.8621875, 0.8617188,
	0.8613281, 0.8610156, 0.8610938, 0.8616016, 0.8623047, 0.8637500, 0.8655859, 0.8673828,
	0.8700586, 0.8741992, 0.8781055, 0.8825195, 0.8876172, 0.8937207, 0.8981836, 0.9027441,
	0.9078369, 0.9102002, 0.9093164, 0.9061743, 0.9004223, 0.8915210, 0.8599536, 0.8208313,
	0.7625024
      };

    const int nEntries = sizeof(mirrorE)/sizeof(double);

    if (scaledE==NULL) {
      scaledE = new double[nEntries];
      scaledSR = new double[nEntries];
    }
    
    for (int i=0;i<nEntries;i++) {
      scaledE[i] = mirrorE[i];
      scaledSR[i] = mirrorR[i];
    }

    G4MaterialPropertiesTable * pT = addSkinPropTable(nEntries);

    pOps = new G4OpticalSurface(surfaceName, unified, polishedfrontpainted, dielectric_dielectric); // to be parametrized
    pOps->SetMaterialPropertiesTable(pT);
    printf("# Surface properties:\n");
    pOps->DumpInfo();
    
    new G4LogicalSkinSurface(skinSurfaceName, logVolume, pOps);

    return nEntries;
    
    /* from original Alessio GEMC code:
      $mir{"name"}         = "spherical_mirror";
        $mir{"description"}  = "reflective mirrors for eic rich";
        $mir{"type"}         = "dielectric_dielectric";
        $mir{"finish"}       = "polishedfrontpainted";
        $mir{"model"}        = "unified";
        $mir{"border"}       = "SkinSurface";
        $mir{"photonEnergy"} = arrayToString(@PhotonEnergyBin1);
        $mir{"reflectivity"} = arrayToString(@Reflectivity1);
        print_mir(\%configuration, \%mir);
    */
    
    
  };

};

//
// photo sensor
//
  
class g4dRIChPhotosensor : public g4dRIChOptics {

public:

  g4dRIChPhotosensor(const G4String logName) : g4dRIChOptics("_NA_", logName) { };

  // pSurfName: prefix used to generate surface names inserted in optical table 

  int setOpticalParams(G4String pSurfName) {

    G4String surfaceName = pSurfName + "phseSurf";
    G4String skinSurfaceName = pSurfName + "phseSkinSurf";
    
    // quantum effiency, from SiPM model S13361-3050NE-08
    std::vector<std::pair<double,double>> QE = { // wavelength [nm], quantum efficiency
      {315*nm, 0.00},
      {325*nm, 0.04},
      {340*nm, 0.10},
      {350*nm, 0.20},
      {370*nm, 0.30},
      {400*nm, 0.35},
      {450*nm, 0.40},
      {500*nm, 0.38},
      {550*nm, 0.35},
      {600*nm, 0.27},
      {650*nm, 0.20},
      {700*nm, 0.15},
      {750*nm, 0.12},
      {800*nm, 0.08},
      {850*nm, 0.06},
      {900*nm, 0.04},
      {1000*nm, 0.00}
    };
    std::reverse(QE.begin(), QE.end()); // order in increasing energy
    const int N_POINTS = QE.size();
    double E[N_POINTS], SE[N_POINTS], N[N_POINTS], IN[N_POINTS];
    int i_QE = 0;
    for(auto [w,q] : QE) {
      E[i_QE]  = wl2e(w);
      SE[i_QE] = q;
      N[i_QE]  = 1.92;
      IN[i_QE] = 1.69;
      i_QE++;
    }

    scaledE  = new double[N_POINTS];
    scaledSE = new double[N_POINTS];
    scaledN  = new double[N_POINTS];
    scaledIN = new double[N_POINTS];

    for (int i=0;i<N_POINTS;i++) {
      scaledE[i] = E[i];
      scaledSE[i] = SE[i];
      scaledN[i] = N[i];
      scaledIN[i] = IN[i];
    }
    
    G4MaterialPropertiesTable * pT = addSkinPropTable(N_POINTS);
    
    pOps = new G4OpticalSurface(surfaceName, glisur, polished, dielectric_dielectric);
    pOps->SetMaterialPropertiesTable(pT);
    printf("# Surface properties:\n");
    pOps->DumpInfo();
    
    new G4LogicalSkinSurface(skinSurfaceName, logVolume, pOps); 

    return 2;
    
  };

};

#endif //G4E_CI_DRICH_MODEL_HH
