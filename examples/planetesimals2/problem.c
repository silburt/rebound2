/**
 * Spreading ring
 *
 * A narrow ring of collisional particles is spreading.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "rebound.h"
#include "tools.h"
#include "../examples/planetesimals2/functions.h"

void heartbeat(struct reb_simulation* r);
void planetesimal_forces(struct reb_simulation* r);

double tmax, planetesimal_mass;
int n_output;
char plntdir[200] = "output/planet_", lgnddir[200] = "output/planet_";

int main(int argc, char* argv[]){
	struct reb_simulation* r = reb_create_simulation();
	// Setup constants
	r->integrator	= atoi(argv[3]);    //REB_INTEGRATOR_IAS15 = 0, WHFAST = 1, HYBRID = 5
	r->collision	= REB_COLLISION_NONE;
	r->boundary	= REB_BOUNDARY_OPEN;
    r->additional_forces = planetesimal_forces;
	r->N_active	= 2;
	r->heartbeat	= heartbeat;
    //r->usleep   = 20000; //larger the number, slower OpenGL simulation
    
    // System constants
    tmax = 20;
    double N_Rhill = atof(argv[1]);     //# hill radii for boundary between switch. Try 3?
    double dRHill = atof(argv[2]);      //Number of hill radii buffer. Default = 2?
    double N_planetesimals = 20;
    double M_planetesimals = 3e-6; //Total Mass of all planetesimals (default = Earth mass, 3e-6)
	
    // Other constants
    n_output = 50000;
    double boxsize = 4.8;
    double kicksperorbit = 50.;
	reb_configure_box(r, boxsize, 1, 1, 1);

	// Initial conditions
	struct reb_particle star = {0};
	star.m 		= 1;
	star.r		= 0.01;
	reb_add(r, star);

    //planet 1
    double a1=1, m1=1e-5, e1=0.01;
    struct reb_particle p1 = reb_tools_init_orbit2d(r->G,star.m,m1,a1,e1,0,0);
    reb_add(r, p1);
    
    //calc dt
    double e_max = 0.3;     //max hypothesized eccentricity that the planet/esimals could have
    if(r->integrator == REB_INTEGRATOR_HYBRID){
        r->dt = calc_dt(r, m1, star.m, a1, e_max, N_Rhill, dRHill);
        //dt = sqrt(4.0*M_PI*pow(a,3)/(G*star.m))/kicksperorbit;
    } else { //IAS15, WH
        r->dt = sqrt(4.0*M_PI*pow(a1,3)/(r->G*star.m))/kicksperorbit;
        printf("dt = %f \n",r->dt);
        
        N_Rhill = -1;
        dRHill = -1;
    }
    
    //planetesimals
    double outer = 4, inner = 12, powerlaw = 0.5;  //higher the inner number, closer to the star
    int seed = 13;          //seed was 11
    srand(seed);
    planetesimal_mass = M_planetesimals / N_planetesimals;  //mass of each planetesimal
	while(r->N<N_planetesimals + r->N_active){
		struct reb_particle pt = {0};
		double a	= reb_random_powerlaw(boxsize/outer,boxsize/inner,powerlaw);
		double phi 	= reb_random_uniform(0,2.*M_PI);
		pt.x 		= a*cos(phi);
		pt.y 		= a*sin(phi);
		pt.z 		= a*reb_random_normal(0.0001);
		double vkep 	= sqrt(r->G*star.m/a);
		pt.vx 		= -vkep * sin(phi);
		pt.vy 		= vkep * cos(phi);
		pt.m 		= 0.0001;
		pt.r 		= .3/sqrt((double)N_planetesimals);
		reb_add(r, pt);
	}
    
    //naming stuff
    legend(plntdir, lgnddir, r, tmax, r->N_active, r->N, planetesimal_mass, M_planetesimals, inner, outer, powerlaw, m1, a1, e1, star.m, N_Rhill, dRHill);
    
    // The WH integrator assumes a heliocentric coordinatesystem.
    if(r->integrator != REB_INTEGRATOR_WH) reb_move_to_com(r);
	reb_integrate(r, tmax); //Integrate! INFINITY is a choice
}

void planetesimal_forces(struct reb_simulation *r){
    const double G = r->G;
    const int N = r->N;
    struct reb_particle* const particles = r->particles;
    struct reb_particle com = particles[0];
    struct reb_particle* planet = &(particles[1]);
    const double Gm1 = G*planetesimal_mass;
    const double x = planet->x-com.x;
    const double y = planet->y-com.y;
    const double z = planet->z-com.z;
    for(int i=2;i<N;i++){//add forces to planet
        struct reb_particle* p = &(particles[i]);
        const double xp = p->x-com.x;
        const double yp = p->y-com.y;
        const double zp = p->z-com.z;
        
        const double dx = x - xp;
        const double dy = y - yp;
        const double dz = z - zp;
        const double rinv = 1./sqrt( dx*dx + dy*dy + dz*dz );
        const double ac = Gm1*rinv*rinv*rinv;  //force/mass = acceleration
        
        planet->ax += ac*dx;    //perturbation on planet due to planetesimals
        planet->ay += ac*dy;
        planet->az += ac*dz;
    }
}

void heartbeat(struct reb_simulation* r){
	if (reb_output_check(r, tmax/n_output)){
        double a_p=0, e_p=0, Etot=0, Ltot=0;
        calc_ae(&a_p, &e_p, r);
        calc_ELtot(&Etot, &Ltot, planetesimal_mass, r);
        
        FILE *append;
        append=fopen(plntdir, "a");
        fprintf(append,"%f,%.8f,%.8f,%.15f,%.15f\n",r->t,a_p,e_p,Etot,Ltot);
        fclose(append);
        if(r->integrator != REB_INTEGRATOR_WH) reb_move_to_com(r);
        
		reb_output_timing(r, 0);
	}
}