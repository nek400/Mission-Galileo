/*
    nbody
    Copyright (C) 2020 

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <openmpi/mpi.h>
#include "element.h"
#include "population.h"
#include "recipes.h"
#include "integrator.h"

int main(int argn, char **args){
  int N = 10;
  Element a1,a2;
  Population population;
  double *distance;
  int i,j,k,m;
  int miproc;
  int numproc;
  int begin,end;
  char filenameIn[256],filenameOut[256];
  FILE *in,*out;
  boolean verbose = true;
  boolean output = false;
  MPI_Status status;
  Element *buffer;
  Element *data;
  Element element;
  MPI_Datatype MPI_ELEMENT;
  double dt;
  MPI_Aint baseaddr,addr1,addr2,addr3;
  MPI_Aint displ[4];
  MPI_Datatype dtype[4];
  int blength[4];
  int response;
  MPI_Op resized_new_type;
  int itera;  
  int steps;
  double Ax,Bx,Ay,By,Az,Bz,radii;
  double au = 1.49597870700e8; //km
  double d = 189302400.0; // seg
  Ax= -8*au; //metros
  Bx= 8*au;
  Ay= -15*au;
  By= 15*au;
  Az= -13*au;
  Bz= 0.5*au;
  radii=1.0;
  dt=0.1; //sec
  steps=500;


  
  itera = 0;
  
  MPI_Init (&argn, &args); /* Inicializar MPI */
  MPI_Comm_rank(MPI_COMM_WORLD,&miproc); /* Determinar el rango
 del proceso invocado*/
  MPI_Comm_size(MPI_COMM_WORLD,&numproc); /* Determinar el nume
ro de procesos */
  MPI_Barrier (MPI_COMM_WORLD);



  
  // Initial Conditions
  
  population = new_Population("Earth-Moon",N); // efemeride 1989-10-18
  
  
  /*
  add_element_to_population(&population, new_Element(5.97219e24,0, 0, 0, 0, 0, 0)); //Earth as center
  
  add_element_to_population(&population, new_Element( 7.349e22, (-1.607012772186297e-3)*(au),  (1.889481885884207e-3)*(au),  (1.505067984764226e-4)*(au), (-4.441373820756126e-4)*(au), (-4.204689683825390e-4)*(au),  (3.769551420565769e-5)*(au) ));
  */
  
  
add_element_to_population(&population, new_Element(1.989e30, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));  // sun
  
add_element_to_population(&population, new_Element(  3.302e23, (-2.423837340508285e-1)*(au),  (2.283179366193356e-1)*(au),  (4.090147930328242e-2)*(au), (-2.500533582231846e-2)*(au), (-1.933534335040159e-2)*(au),  (7.165606201190990e-4)*(au) )); //mercury
  
add_element_to_population(&population, new_Element( 48.685e23, (6.279965351339432e-1)*(au), (-3.659074113502193e-1)*(au), (-4.124507417057483e-2)*(au),  (1.006223611787707e-2)*(au),  (1.739227435714184e-2)*(au), (-3.437549474002681e-4)*(au) )); //venus
  
add_element_to_population(&population, new_Element( 5.97219e24, (9.048814763968241e-1)*(au),  (4.168743201018727e-1)*(au),  (1.080349655773159e-5)*(au), (-7.472367882942215e-3)*(au),  (1.555856623260806e-2)*(au), ( 4.788602272729372e-7)*(au) )); // Earth

add_element_to_population(&population, new_Element( 6.4171e23 ,(-1.572843746193111)*(au), (-4.215405696821336e-1)*(au), ( 2.986243952490811e-2)*(au), ( 4.148243175162371e-3)*(au), (-1.232197647750978e-2)*(au), (-3.601215923958582e-4)*(au) )); //mars

add_element_to_population(&population, new_Element( 1898.13e24, (5.523818468809624e-3)*(au),  (5.124075547305725)*(au), (-2.132930486986605e-2)*(au), (-7.650493216941010e-3)*(au),  (3.591116688691631e-4)*(au),  (1.698233530987749e-4)*(au) )); //jupyter

add_element_to_population(&population, new_Element( 5.6834e26, (2.426782948714540)*(au), (-9.733922843102670)*(au),  (7.326795987290702e-2)*(au),  (5.106080342974652e-3)*(au),  (1.334845188513565e-3)*(au), (-2.266378604457396e-4)*(au) )); //saturn

add_element_to_population(&population, new_Element( 86.813e24, (1.623038180331797)*(au), (-1.929828307633671e1)*(au), (-9.266636267812192e-2)*(au),  (3.884247099415706e-3)*(au),  (1.474794321989732e-4)*(au), (-4.974092991353999e-5)*(au) )); //uranus

add_element_to_population(&population, new_Element( 102.413e24, (6.164462103795938)*(au), (-2.957298645388243e1)*(au),  (4.669463041113271e-1)*(au),  (3.047705175550985e-3)*(au),  (6.585262491739477e-4)*(au), (-8.416363874946226e-5)*(au) )); //Neptune


add_element_to_population(&population, new_Element( 2223, (8.908428789496743e-1)*(au) , (4.460649134874483e-1)*(au) , (1.441885322941325e-3)*(au), (6.598375451449180e-3)*(au),  (1.378781163270632e-2)*(au) , (1.238600174019397e-3) *(au) )); //galileo

  
  
  
  
  /*
  add_element_to_population(&population,new_Element(0.5e20, ((-7.145797222659181e-4)*(au)) , ((6.386604312264796e-5)*(au)) ,((3.221609690741755e-4)*(au)) ,((-8.885052733751100e-4)*(au)),((-4.155282710926470e-3)*(au)),((-1.152039370631404e-3)*(au)) )); //proteus
  
  add_element_to_population(&population,new_Element(3.7e18,(-6.566279645817739e-5)*(au),(-3.928878742179119e-4)*(au),(-1.134909268694775e-4)*(au),(5.577594755468575e-3)*(au),(-2.445883633536525e-4)*(au),(-2.377396575170503e-3)*(au))); // galatea
  
  add_element_to_population(&population,new_Element(0.05e20,(-3.011416820451796e-4)*(au),(3.114217986713941e-4)*(au),(2.336058165203136e-4)*(au),(-3.924067413749880e-3)*(au),(-3.939094144862570e-3)*(au),(1.867271701436211e4)*(au))); //larissa
  
  add_element_to_population(&population,new_Element(0.021e20,(1.827481527765589e-4)*(au),(-2.507515784223542e-4)*(au),(-1.642897835765280e-4)*(au),(5.105610349380976e-3)*(au),(4.124547546870767e-3)*(au),(-6.196004128626816e-4)*(au))); //Despina
  
  //add_element_to_population(&population, new_Element(1.989e30, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));  // sol en el centro del sistema
  //add_element_to_population(&population, new_Element(1.898e27, 4.58*au, -2.008*au, -0.094*au, 0.00293*(au), 0.00727*(au), -9.57e-5*(au)));  // jupiter
  */


  //Initial Conditions
  if (miproc==0){
    printf("#%i\t%i\t%le\t%le\t%le\t%le\t%le\t%le\t%le\n",steps,population.n_elements,Ax,Bx,Ay,By,Az,Bz,radii);
    printf("#%i\n",itera);
    print_Population(population);    
  }
  
  
  
  /*  for(i=0; i<N; i++){
    add_element_to_population(&population, new_Element(1.0, (double)i, 0.0,0.0, 0.0, 0.0, 0.0));
  }
  */

  k=0;
  //for(i=0;i<population.n_elements-1;i++){
  begin = (population.n_elements/numproc)*miproc;
  end = (population.n_elements/numproc)*(miproc+1);
  //printf("%i\t%i\n",numproc,miproc);
  if (numproc == (miproc+1)){
    //printf("last processor %i\t%i\n",end, population.n_elements);
    if (end < population.n_elements){
      end = population.n_elements;
    }
  }

  data = malloc(sizeof(Element)*(end-begin));

  //buffer is used for the slaves as the list of updated elements
  if (miproc > 0){
    buffer = malloc(sizeof(Element)*(population.n_elements));
  }
  MPI_Get_address(&element, &baseaddr);
  MPI_Get_address(&element.mass,&addr1);
  MPI_Get_address(&element.position,&addr2);
  MPI_Get_address(&element.velocity,&addr3);

  displ[0] = 0;
  displ[1] = addr1 - baseaddr;
  displ[2] = addr2 - baseaddr;
  displ[3] = addr3 - baseaddr;

  dtype[0] = MPI_INT; blength[0] = 1;
  dtype[1] = MPI_DOUBLE; blength[1] = 1;
  dtype[2] = MPI_DOUBLE; blength[2] = 3;
  dtype[3] = MPI_DOUBLE; blength[3] = 3;

  MPI_Type_create_struct ( 4, blength, displ, dtype, &MPI_ELEMENT );

////  MPI_Type_create_resized(MPI_ELEMENT,
////                            // lower bound == min(indices) == indices[0]
////                            displ[0],
////                            (MPI_Aint)sizeof(Element),
////                            &resized_new_type);
////    MPI_Type_commit(&resized_new_type);
////    // Free new_type as it is no longer needed
////    MPI_Type_free(&MPI_ELEMENT);


  MPI_Type_commit ( &MPI_ELEMENT );
  
  //m=0;
  //printf("Segmento %i: %i\t%i\n",miproc,begin,end);

  for(itera=1;itera<=steps;itera++){

    /* Note: begin and end are updated for the process 0, we need to be 
       carefoul and in the following cycle of development move the name
       of the variable to improve the computations*/

    if (miproc==0){
      begin = (population.n_elements/numproc)*miproc;
      end = (population.n_elements/numproc)*(miproc+1);
    }
    
    //printf("%i\t%i\t%i\t%i\n",miproc,itera,begin,end);
    response = integrate(population,&data,begin,end,dt);
    //use proc 0
    if (miproc==0){
      for(i=0;i<end;i++){
	      population.element[i] = data[i];
	//print_Element(data[j]);
      }
    }
    /*
      for(i=0;i<population.n_elements-1;i++){
      //for(j=i+1 ;j<population.n_elements;j++){
      for(j=i+1 ;j<population.n_elements;j++){
      if ((begin<=k) && (k<end)){
      //printf("%i,%i\n",i,j);
      population.distance[k] = compute_distance(population.element[i],population.element[j]);
      data[m] = population.distance[k];
      m++;
      }
      k++;
      }
      }
    */
  
    
    
    if (miproc > 0){
      //for(j=0;j<end-begin;j++){ 
      //	printf("before[%i]=%lf\n",j,data[j]);
      //  }
      //data[end-begin-1].position[0] = (double)miproc;
      
      //Sending subset of position to master
      //for(i=begin;i<end; i++){
      //  print_Element(data[i]);
      //}
    
      MPI_Send(data, end-begin, MPI_ELEMENT, 0, 99, MPI_COMM_WORLD);
      //receiving the new list of position of the system from master.
      //MPI_Barrier (MPI_COMM_WORLD);

      MPI_Recv(buffer, population.n_elements , MPI_ELEMENT, 0, 99, MPI_COMM_WORLD, &status);

      for(i=0;i<population.n_elements; i++){
	      population.element[i] = buffer[i];
	//printf("updating info ");
	//print_Element(buffer[i]);
      }
    
    }
    
    else{ // MASTER PROCESS (0)
      for (i = 1; i < numproc; i++){
	      begin = (population.n_elements/numproc)*i;
	      end = (population.n_elements/numproc)*(i+1);
	
	      if (numproc == (i+1)){
	  //printf("last processor %i\t%i\n",end, population.n_elements);
	        if (end < population.n_elements){
	          end = population.n_elements;
	        }
	      }
	      buffer = malloc(sizeof(Element)*(end-begin));
	//Receiving subset of data from slave i
	      MPI_Recv(buffer, end-begin , MPI_ELEMENT, i, 99, MPI_COMM_WORLD, &status);
	
	      k=0;
	      for(j=begin;j<end;j++){ 
	        population.element[j] = buffer[k];
        //printf("receiving elements ");
        //print_Element(population.element[j]);
        //population.distance[begin+j] = buffer[j];
        //printf("distance[%i]=%lf\n",begin+j,population.distance[begin+j]);
        //printf("Recibido [%i] = %lf\n",i,buffer[end-begin-1].position[0]);
	        k++;
	      }
	      free(buffer);
	
      }

      //MPI_Barrier (MPI_COMM_WORLD);

      //Sending updated list of elements to the slaves.
      for (i=1; i < numproc; i++){
	      MPI_Send((&population)->element, population.n_elements, MPI_ELEMENT, i, 99, MPI_COMM_WORLD);
      }
      
      //Print results of the iterations
      printf("#%i\n",itera);
      print_Population(population);    
      
    } //process 0
    
    //  population.distance[0] = compute_distance(population.element[0],population.element[1]);
    
    if(output){
      // write_distances("output/",population,begin,end,miproc);
    }
    
    MPI_Barrier (MPI_COMM_WORLD);
    
    // if (miproc==0){
       //merge all the ouput archives
    
      // if(output){
      //   printf("Entró\n");
      //   sprintf(filenameOut,"%s/distances.dat","output/");
      //   out = fopen(filenameOut, "w"); //opening the file for writing.
      //   for(i=0;i<numproc ;i++){
      //     sprintf(filenameIn,"%s/distances-%i.dat","output/",i);
      //     in = fopen(filenameIn, "r"); //opening the file for reading.
      //     write_block(in,out);
      //     fclose(in);
      //   }
      //   fclose(out);
      //   }
    ////    if (verbose){
    ////      print_Population(population);
    ////      print_distances(population,0, population.n_distance);
    ////    }
    ////
    ////
    ////    //printf("List of distances %i\n",population.n_distance);
    // }
      
  }//END ITERA
  
  
  MPI_Barrier (MPI_COMM_WORLD);
  //print_distances(population,begin,end);

  if (miproc > 0){
    free(buffer);
  }
  
  MPI_Finalize();
  return 1;
}
