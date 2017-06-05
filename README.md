#AI Tools#

The following repository contains pieces used in an universal AI daemon. The daemon will solve common AI problems. All code is written in C.
Each AI problem can be broken down to the following components:

* SOLUTION: a vector in R<sup>n</sup>, that is expected for a given PROBLEMDATA; SOLUTION packs [ (int) size, (double * ) data ]
* PROBLEMSOLVER: a general wrapper for different types of solvers
* TYPE: the type of PROBLEMSOLVER; TYPE is integer
* STATE: a buffered set of processed data obtained from the given training data
* TRAININGDATA: a matrix, having on each row a pair (x, y) in R<sup>m</sup> &times; R<sup>n</sup>; TRAININGDATA packs [ (long) rows, (int) m, (int) n, (double * ) data ]
* CROSSVALIDATION: a vector of row indices for cross-validation data; CROSSVALIDATION packs [ (int) size, (int * ) indices ]
* TEST: a vector of row indices for test data; TEST packs [ (int) size, (int * ) indices ]
* OPTIMIZATION: the optimization function type; OPTIMIZATION is integer
* PROBLEMDATA: a vector in R<sup>m</sup> for which an output is required; PROBLEMDATA packs [ (int) size, (double * ) data ]

SOLUTION = PROBLEMSOLVER [ TYPE, STATE [ TRAININGDATA, CROSSVALIDATION, TEST, OPTIMIZATION ], PROBLEMDATA ];

##AI problem file structure##

* header: 4 bytes | TYPE (2 bytes) | OPTIMIZATION (2 bytes) |
* data size: 16 bytes | ROWS (8 bytes) | INPUT SIZE (4 bytes) | OUTPUT SIZE (4 bytes) |
* cross validation size: 8 bytes | ROWS (8 bytes) |
* test size: 8 bytes | ROWS (8 bytes) |
* state size: 8 bytes | LENGTH (8 bytes) |
* cross validation: | INDICES (unsigned long, ...) |
* test: | INDICES (unsigned long, ...) |
* data: | DATA (double, ...) |
* state: | STATE (double, ...) |

##Config options##
* port: listening port for problem definition
* path: problems directory, default /var/lib/aiqbd

##Directories##
* /etc/aiqbd: configuration files
* /var/lib/aiqbd: default problem database

##Daemon Behavior##
Listen to connection on TCP/IP port.

##Threads##
* *communication_thread*: handles IPC (registeres problems and answers)
* *train_thread*: trains AI based on problems
* *solve_thread*: solves AI problems
