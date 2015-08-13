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

void legend(char* planetdir, char* legenddir, struct reb_simulation* r, double tmax, int N_active, int N, double m_planetesimal, double total_planetesimal_mass, double inner, double outer, double powerlaw, double mp, double a, double e, double Ms, double nrhill, double drh);

double calc_dt(struct reb_simulation* r, double mp, double Ms, double a, double e_max, double N_Rhill, double dRHill);

void calc_ELtot(double* Etot, double* Ltot, double planetesimal_mass, struct reb_simulation* r);

void calc_ae(double* a, double* e, struct reb_simulation* r);

void time_init(void);

void time_end(char* dir);

#endif /* defined(____functions__) */