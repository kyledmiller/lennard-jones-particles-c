/* Microcanonical Ensemble Simulation
 * md-simulate.h:  Function prototypes for main program
 * 
 * (c) 2008
 * Ben Niehoff
 * Jose Gonzalez
 *
 * Description:
 *
 * This program simulates a system of Argon atoms by explicitly
 * integrating the equations of motion of a collection of
 * N atoms.  By measuring energy fluctuations, the program determines
 * the specific heat and pressure of the system.
 */

#define BLOCK_EMPTY (-1)
#define LIST_EMPTY (-1)
#define NULL_PREV (-1)
#define ITERATIONS (100000)
#define SAMPLE_RATE 1
#define SAMPLE_DEPTH 200
#define SAMPLES 5
#define BATCH_RATE 5000

#define STARTUP 0
#define EQUILIBRIUM 1
#define DIVERGE 2

/* Coordinate indices */
#define x 0
#define y 1
#define z 2

/* Handoff sublist indices */
#define first 0
#define last 1

/* Constants */
const double sigma = 3.405e-10; /* m */
const double epsilon = 1.65e-21; /* J per atom */
const double tau = 2.153e-12; /* s */
const double kB = 1.3806503e-23; /* m^2 kg K^-1 s^-2  Boltzmann constant */
const double Mass = 0.03994; /* kg / mol  mass of Ar per mole */
const double Rc = 2.5; /* dimensionless cutoff radius */
const double dt = 0.005; /* time step */
const double Rsmall = 0.01; /* distance (squared) at which to use flat U curve instead of divergent function */

const double PI = 3.14159265358979323846264; /* Archimedes' constant */

/* File paths and prefixes */
char *output_directory; /* where to output files */
char *file_prefix; /* prefix to attach to files unique to this run */

char *time_series_path; /* path to relevant data as a function of time */
char *final_state_path; /* path to final state data */
char *init_state_path; /* path to final state data */
char *thermo_meas_path; /* path to thermo measurements as a function of temperature */
char *summary_info_path; /* basic info like box size and number of particles */

//char *dim_path; /* box dimensions */

/* Global variables */
double L_s; /* dimension of the volume */
double rho_s; /* dimensionless density */
double T_s; /* desired dimensionless Temperature */
double T_c; /* current dimensionless temperature */
int CellCount; /* number of crystal cells per side */
double Rcm[3]; /* center of mass */
double Fsum[3]; /* sum of internal forces (should be 0) */
double Rmin2; /* square of closest approach distance */

double T_array[BATCH_RATE]; /* temperature array */
double U_array[BATCH_RATE]; /* potential array */
double E_array[BATCH_RATE]; /* energy array */
double MSD_array[BATCH_RATE]; /* mean square displacement array */
int batch_index;

long Iteration; /* current iteration in simulation */

int Transfers; /* counts how many atoms had to transfer lists */

double U[SAMPLE_DEPTH]; /* potential energy samples */
double Virial[SAMPLE_DEPTH]; /* virial samples */
double E_sample[SAMPLE_DEPTH]; /* energy samples */
double T_sample[SAMPLE_DEPTH]; /* temperature samples */
int subsample_index; /* index into U[] and Virial[] */

double Cv[SAMPLES]; /* specific heat sample array (heat capacity per particle) */
double P[SAMPLES]; /* pressure samples array */
double E[SAMPLES]; /* energy averages array */
double T[SAMPLES]; /* temperature averages array */
int sample_index; /* index into Cv[] and P[] */


double ** R; /* Positions of the N atoms */
double ** V; /* Velocities of the N atoms */
double ** F; /* forces on the N atoms */

double ** Rinit; /* Initial Positions of the N atoms */
double ** Rtrue; /* distance traveled if PBC were not enforced */
//double MSD; /* mean square displacement */

int *** Blocks; /* Blocks used to divide space */
int *** Handoff[2]; /* points to sublists to be traded between blocks when particles move */

//double * Boundaries; /* boundaries of blocks at multiples of Rc */
int * LinkedList; /* Linked List array */
//double U; /* total potential energy */
double Umax; /* potential energy at small radius */

/* global counts */
int AtomCount;
int BlockCount;
int BlocksPerSide;
double BlockSize;
int HalfKicks;
int FullKicks;


/* Function prototypes */

/* calculates mean square displacement */
double MSD();
/* calculates maximum velocity */
double MaxVelocity();

void MemError();

void Initialize(); /* set up initial arrays */
int AllocateAll(); /* allocate all arrays */
void FreeAll(); /* frees all allocated memory */
void InitialConditions(); /* places atoms in initial R and assigns atoms initial V */
void Recenter(); /* places center of mass at (0,0,0) */
void ZeroP(); /* zeroes the linear momentum */
void ZeroL(); /* zeroes the angular momentum */
void ScaleTemperature(); /* sets temperature to T_s by scaling V */

void PopulateLinkedList(); /* sets up linked list array */
/* appends to linked list */
void AppendBlocks(int new, int i, int j, int k);
/* appends to linked list and updates Handoff */
void AppendHandoff(int new, int i, int j, int k);
/* appends Handoff sublist to Blocks linked list */
void AppendSublistBlocks(int head, int tail, int i, int j, int k);
void Remove(int prev); /* removes from the middle of a list */
/* removes first element from list */
void Tail(int i, int j, int k);

/* Do one iteration of integration algorithm */
void VelocityVerletFirstHalf(int bi, int bj, int bk);
void VelocityVerletSecondHalf(int bi, int bj, int bk);
/* full Velocity-Verlet step on all blocks */
void VelocityVerletFullStep();

/* calculates ALL forces (accelerations) and potential */
void AllForcesAndPotential();
/* calculate the forces (accelerations) and potential within a block */
void InternalForcesAndPotential(int bi, int bj, int bk);
/* calculate the forces and potential between two blocks */
void ExternalForcesAndPotential(int ai, int aj, int ak, int bi, int bj, int bk);

/* calculates current kinetic energy and adds to K_avg array */
void KineticEnergy();
/* calculates current virial and adds to Virial_avg array */
//void Virial(double fij, double rij2);
/* calculates specific heat and pressure, records to arrays */
void ThermoQuantities();
/* prints out pressure, specific heat */
void PrintThermoQuantities();

/* calculates sum of internal forces */
void ForceCheck();

/* calculate the force as a function of r^2 */
double Force(double r2);

/* calculate the potential as a function of r^2 */
double Potential(double r2);

void CenterOfMass(double [3]); /* calculates CoM */
void TotalP(double [3]); /* calculates total linear momentum */
void TotalL(double [3]); /* calculates total angular momentum */
void MomentOfInertia(double [3][3]); /* calculates moment of inertia */
double Temperature(); /* calculates temperature */

/* print out CoM, P, and L for diagnostic */
void Diagnostic();
/* list what atoms are in a handoff block */
void HandoffCheck(int bi, int bj, int bk);
/* list what atoms are in a block */
void BlockCheck(int bi, int bj, int bk);
/* print current total energy */
void EnergyCheck();
/* checks to see if all lists are terminated */
void ListCheck();
/* prints out all positions */
void PositionCheck();
/* prints out all velocities */
void VelocityCheck();
/* prints out closest approach */
void RminCheck();
/* prints out all speeds */
void SpeedCheck();

/* prints out final state to file */
void FinalStateToFile();
void InitStateToFile();

/* print summary info to file */
void SummaryInfoToFile();

/* prints out all positions to file */
//void PositionsToFile();
/* prints out all speeds to file */
//void SpeedsToFile();

/* writes all collected data to files in batches */
void BatchFileWrite();


