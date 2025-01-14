# Microcanonical Ensemble Simulation
# batch-simulate.py:  Batch simulation script
# 
# (c) 2020 Ben Niehoff
# 
# Description:
# This script runs the main program md-simulate over a spread of
# parameters in order to generate data which can then be examined in
# the Mathematica notebook Visualization.nb

import os
import sys
import shutil
import getopt
import ast
import numpy as np
import pandas as pd
import time

start = time.time()

# Get options
opts, args = getopt.getopt(sys.argv[1:], 'rc:d:t:o:',
	['remove-data', 'cellcount=', 'density-linspace=', 'temperature-linspace=',
	'output-directory='])

remove_data = False
cellcount = 6
density_linspace = [0.8, 1, 1]
#density_linspace = [0.2, 1, 5]
# temperature_linspace = [0.9, 0.9, 1]
temperature_linspace = [0.2, 0.8, 1]
#temperature_linspace = [0.2, 0.8, 4]
# temperature_linspace = [0.1, 0.9, 17]
# temperature_linspace = [0.1, 0.9, 33]
# temperature_linspace = [0.125, 0.875, 16]
output_directory = f'test_data_cellcount_{cellcount}'

#print(opts)

for opt, val in opts:
	if opt in ['-r', '--remove-data']:
		remove_data = True
	elif opt in ['-c', '--cellcount']:
		cellcount = int(val)
	elif opt in ['-d', '--density-linspace']:
		density_linspace = ast.literal_eval(val)
	elif opt in ['-t', '--temperature-linspace']:
		temperature_linspace = ast.literal_eval(val)
	elif opt in ['-o', '--output-directory']:
		output_directory = val

# Compute density and temperature values
densities = np.linspace(*density_linspace)
temperatures = np.linspace(*temperature_linspace)

# Check for ranges of density and temperature before proceeding
print('Batch simulate: Ready to run simulation for densities:\n    '
	'{}\nand temperatures :\n    {}'.format(densities, temperatures))
print('This will result in a total of {} runs '
	'with {} particles each'.format(len(densities) * len(temperatures), 4 * cellcount ** 3))
print('All files will be stored in {}'.format(output_directory))
if remove_data:
	print('You have chosen to DELETE all files in {}'.format(output_directory))

confirm = input('Ready to proceed? (y/n): ')

if confirm not in ['y', 'Y']:
	print('Batch simulate: Simulation will NOT proceed')
	print('No files/directories have been created or destroyed')
	exit()
else:
	print('Batch simulate: Simulation WILL proceed')

# First make sure ./data directory exists
if not os.path.exists(output_directory):
	print('Batch simulate:', output_directory, 'does not exist')
	print('Batch simulate: Creating', output_directory)
	if remove_data:
		print('Batch simulate: No need to remove files from', output_directory)
		remove_data = False
	os.mkdir(output_directory)

# Check whether to clean the data directory or not
if remove_data:
	# Clean output directory of all files
	print('Batch simulate: Removing files from', output_directory)
	for root, dirs, files in os.walk(output_directory):
		for f in files:
			os.remove(os.path.join(root, f))
		for d in dirs:
			shutil.rmtree(os.path.join(root, d))

# Prepare initial file
if not os.path.exists(os.path.join(output_directory, 'thermo_measurements.csv')):
	thermo_meas_file = open(os.path.join(output_directory, 'thermo_measurements.csv'), 'w')
	thermo_meas_file.write('Density,Temp,Energy,HeatCapCv,Pressure\n')
	thermo_meas_file.close()

print('Batch simulate: Beginning simulation now...')

#temperatures = np.linspace(0.1,0.9,num=17)
#temperatures = [0.1, 0.3, 0.5, 0.7, 0.9]

# Run the simulations
for rho in densities:
	rho_dirname = 'rho_{:.3f}'.format(rho)
	if not os.path.exists(os.path.join(output_directory, rho_dirname)):
		os.mkdir(os.path.join(output_directory, rho_dirname))
	
	for T in temperatures:
		T_dirname = 'T_{:.3f}'.format(T)
		
		if not os.path.exists(os.path.join(output_directory, rho_dirname, T_dirname)):
			os.mkdir(os.path.join(output_directory, rho_dirname, T_dirname))
		
		if not os.path.exists(os.path.join(output_directory, rho_dirname, T_dirname, 'time_series.csv')):
			time_series_file = open(os.path.join(output_directory, rho_dirname, T_dirname, 'time_series.csv'), 'w')
			time_series_file.write('TimeStep,Temp,PotEnergy,TotEnergy,MeanSqDisp\n')
			time_series_file.close()
		
		if not os.path.exists(os.path.join(output_directory, rho_dirname, T_dirname, 'final_state.csv')):
			final_state_file = open(os.path.join(output_directory, rho_dirname, T_dirname, 'final_state.csv'), 'w')
			final_state_file.write('PosX,PosY,PosZ,VelX,VelY,VelZ,Speed\n')
			final_state_file.close()
		
		if not os.path.exists(os.path.join(output_directory, rho_dirname, T_dirname, 'summary_info.csv')):
			summary_info_file = open(os.path.join(output_directory, rho_dirname, T_dirname, 'summary_info.csv'), 'w')
			summary_info_file.write('CellCount,SideLength,AtomCount,BlockCount,BlocksPerSide,BlockLength\n')
			summary_info_file.close()
		
		cmd = ' '.join(['./md-simulate',
			'--cellcount', str(cellcount),
			'--density', str(rho),
			'--temperature', str(T),
			'--output-directory', output_directory,
			'--prefix', '"' + rho_dirname + '/' + T_dirname + '"'])
		
		print('Batch simulate:  Executing with density {:.3f} and temperature {:.3f}'.format(rho, T))
		#print(cmd)
		os.system(cmd)

# os.system('./md-simulate --cellcount 5 --density 0.8 --temperature 0.9 --output-directory data --prefix T0.9')

print(time.time()-start)
