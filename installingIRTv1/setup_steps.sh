# wget https://raw.githubusercontent.com/kumardeepraman/learning-drich-dev/main/installingIRTv1/setup_steps.sh

#!/bin/bash
set -e
set -o pipefail

# ================================
# === Functions for each step ===
# ================================


install_eic_shell() {
    echo "This is Module A:"
    echo
    echo "=== Installing Epic environment ==="
    curl --location https://get.epic-eic.org | bash


    echo
    echo "eic-shell installed."
    echo
    echo "Next step before Module B is:"
    echo "   Enter the shell:   ./eic-shell"
    echo

}


enter_drich_dev() {
    echo "This is Module B:"
    echo
    echo "=== Cloning the drich-dev repository ===="
    git clone https://github.com/eic/drich-dev.git 
    
    echo
    echo "drich-dev cloned."
    echo
    echo "Next step before Module C is:"
    echo "   Entered drich-dev/: cd drich-dev"
    echo 
}

clone_repos() {
    echo "This is Module C:"
    echo
    echo "=== Cloning drich-dev dependencies ==="
    git clone https://github.com/eic/epic.git
    git clone https://github.com/eic/EICrecon.git 
    git clone --branch v1.0.10 --depth 1 https://github.com/eic/irt.git
    git clone https://github.com/eic/EDM4eic.git
    git clone https://eicweb.phy.anl.gov/EIC/benchmarks/detector_benchmarks.git
    git clone https://github.com/kumardeepraman/learning-drich-dev.git

    echo
    echo "Dependencis cloned & files fixed fixed."
    echo

    cp learning-drich-dev/installingIRTv1/recon_sandbox.yaml config/recon_sandbox.yaml
    cp learning-drich-dev/installingIRTv1/environ.sh .
    cp learning-drich-dev/installingIRTv1/momentum_scan.rb scripts/momentum_scan.rb
    cp learning-drich-dev/installingIRTv1/momentum_scan_2D_draw.C scripts/src/momentum_scan_2D_draw.C
    cp learning-drich-dev/installingIRTv1/g4dRIChOptics.hh src/g4dRIChOptics.hh
    cp learning-drich-dev/installingIRTv1/dconfig-chandra.C irt/delphes/scripts/dconfig-chandra.C
    cp learning-drich-dev/installingIRTv1/dconfig.C irt/delphes/scripts/dconfig.C
    cp learning-drich-dev/installingIRTv1/dloop.C irt/delphes/scripts/dloop.C
    cp learning-drich-dev/installingIRTv1/IrtGeoDRICH.cc EICrecon/src/services/geometry/richgeo/IrtGeoDRICH.cc
    cp learning-drich-dev/installingIRTv1/IrtGeoPFRICH.cc EICrecon/src/services/geometry/richgeo/IrtGeoPFRICH.cc
    cp learning-drich-dev/installingIRTv1/CherenkovPIDAnalysis.cc detector_benchmarks/benchmarks/rich/src/CherenkovPIDAnalysis.cc
    cp learning-drich-dev/installingIRTv1/CMakeLists.txt detector_benchmarks/CMakeLists.txt
    cp learning-drich-dev/installingIRTv1/draw_benchmark.py detector_benchmarks/benchmarks/rich/draw_benchmark.py

    echo "Next step before Module D is:"
    echo "   Source the drich environment: source environ.sh"
    echo

}


build_core() {
    echo "This is Module D:"
    echo
    echo "=== Building core components ==="
    ./build.sh epic
    ./build.sh irt
    ./build.sh EDM4eic
    ./build.sh EICrecon
    ./build.sh detector_benchmarks

    echo
    echo "Next step before Module E is:"
    echo "   source environ.sh"
}


final_build() {
    echo "This is Module E:"
    echo
    #echo "=== Final build ==="
    #make
    ln -s detector_benchmarks/benchmarks/rich/run_benchmark.rb benchmark2.rb
    sed -i "222c outputName = re.sub(r'\\.root$', '', outputFileName)" simulate.py
}

# ================================
# === Command-line argument parser
# ================================

if [ $# -eq 0 ]; then
    echo "Usage: $0 [--all] [--A] [--B] [--C]"
    echo "          [--D] [--E]"
    exit 1
fi

for arg in "$@"; do
    case $arg in
        --all)
	    install_eic_shell
            enter_drich_dev
            clone_repos
            build_core
            final_build
            ;;
	--A)
	    install_eic_shell
	    ;;
        --B)
            enter_drich_dev
            ;;
        --C)
            clone_repos
            ;;
        --D)
            build_core
            ;;
        --E)
            final_build
            ;;
        *)
            echo "❌ Unknown option: $arg"
            exit 1
            ;;
    esac
done

echo "Done. Remember to manually source environ.sh when needed."
