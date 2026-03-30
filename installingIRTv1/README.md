wget https://raw.githubusercontent.com/kumardeepraman/learning-drich-dev/main/installingIRTv1/setup_steps.sh

chmod a+x setup_steps.sh

./setup_steps.sh --A

./eic-shell

./setup_steps.sh --B

cd drich-dev/

mv ../setup_steps.sh .

./setup_steps.sh --C

source environ.sh

./setup_steps.sh --D

./setup_steps.sh --E

./simulate.py -t1 -n10

./recon.rb

./benchmark2.rb -b
