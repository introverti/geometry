mamba env create -f smartlabel.yaml
mamba init
source /root/.bashrc
mamba activate smartlabel
mamba install make -y
apt update
apt install libosmesa6-dev libxrandr-dev libxinerama-dev libxcursor-dev libegl1-mesa-dev -y