//
//  functions.h
//  
//
//  Created by Ari Silburt on 2015-06-12.
//
//

#ifndef ____functions__
#define ____functions__

#include <stdio.h>
#include "../../src/rebound.h"

//FUNCTIONS******************************
void legend(char* planetdir, char* legenddir, struct reb_simulation* r, double tmax, double m_planetesimal, double total_planetesimal_mass, double inner, double outer, double powerlaw, double mp, double a, double e, double Ms, double drh, int HYBRID_ON);

double calc_dt(struct reb_simulation* r, double mp, double Ms, double a, double dRHill);

double calc_Etot(struct reb_simulation* a);

//void calc_ELtot(double* Etot, double* Ktot, double* Utot, double* Ltot, double planetesimal_mass, struct reb_simulation* r);

void calc_ae(double* a, double* e, double* d, struct reb_simulation* r, int i, double t);

void planetesimal_forces(struct reb_simulation *a);

void check_for_encounter(struct reb_simulation* const r, int* N_encounters);

void ini_mini(struct reb_simulation* const r, struct reb_simulation* s);

void update_global(struct reb_simulation* const s, struct reb_simulation* const r, int N_encounters_previous, int N_encounters);

void add_or_subtract_particles(struct reb_simulation* r, struct reb_simulation* s, int N_encounters,int N_encounters_previous, int dN);

void update_encounter_indices(int* N_encounters, int* N_encounters_previous);

void clock_finish(clock_t timer, int N_encounters, char* legenddir);

struct reb_particle get_com_with_planetesimals(struct reb_simulation* r);

void move_to_com_with_planetesimals(struct reb_simulation* const r);

//EXTERNAL VARIABLES******************************
extern double planetesimal_mass;
extern int* encounter_index;
extern int* previous_encounter_index;
extern double* Hill;
extern int N_encounters_tot;


#endif /* defined(____functions__) */
