# Must run as root so that we can shutdown backuppc and mount drives 
if [ $(whoami) != "root" ]; then
	echo "Debe ejecutar este guión como «root»."
	echo "Use 'sudo sh instala_paquetes_debian.sh' e introduzca la contraseña cuando se le pida."
	exit 1
fi

apt-get install g++ gfortran cmake libboost-dev libglib2.0-dev libgtk2.0-dev libgtkgl2.0-dev libgtkglext1-dev libgtkglextmm-x11-1.2-dev libgtkmm-2.4-dev libhdf5-mpi-dev libmed-dev libmed-tools libmed1v5 libmedc-dev libmedc1v5 libmedimport-dev libmedimport0v5 libmedimport-dev libsqlite3-dev libvtk5-dev libx11-dev

#libmedimportcxx0 Not found in Ubuntu 14.04
