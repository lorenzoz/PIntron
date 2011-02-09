/**
 *
 *
 *                              PIntron
 *
 * A novel pipeline for computational gene-structure prediction based on
 * spliced alignment of expressed sequences (ESTs and mRNAs).
 *
 * Copyright (C) 2010  Yuri Pirola, Raffaella Rizzi
 *
 * Distributed under the terms of the GNU Affero General Public License (AGPL)
 *
 *
 * This file is part of PIntron.
 *
 * PIntron is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * PIntron is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with PIntron.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
#include "min_factorization.h"
#include "bit_vector.h"
#include "io-factorizations.h"
#include "color_matrix.h"
#include "util.h"
#include "semplify_matrix.h"
#include "my_time.h"
#include "log-build-info.h"

int main(void) {
  INFO("MIN-FACTORIZATION");
  PRINT_LICENSE_INFORMATION;
  PRINT_SYSTEM_INFORMATION;
  pmytime timer= MYTIME_create_with_name("Timer");
  pmytime ttot= MYTIME_create_with_name("Total");
  MYTIME_start(ttot);
  plist p= read_factorizations(stdin);

  //Inizializzo a NULL
  pbit_vect bv=NULL;

  MYTIME_start(timer);

  INFO("Start color_matrix_create");
  bool is_not_window;
  //Colorazione della matrice sulla base degli esoni come fattori unici
  is_not_window=true;
 //Colorazione della matrice sulla base delle finestre di contenimento degli esoni
  is_not_window=false;

  plist unique_factors= color_matrix_create(p, is_not_window);
// color_matrix_print(p);

  INFO("Colored matrix created!");
#if defined (LOG_MSG) && (LOG_LEVEL_DEBUG <= LOG_THRESHOLD)
  print_factors_list(unique_factors,is_not_window);
#endif
  //exit(1);

  /*plistit list_it_est=list_first(p);
   while(listit_has_next(list_it_est)){
	 pEST est=listit_next(list_it_est);
	 plistit temp=list_first(est->bin_factorizations);
	 while(listit_has_next(temp)){
		 fprintf(stdout, ">EST %s\n", est->info->EST_id);
		 pbit_vect ltemp=(pbit_vect)listit_next(temp);
		 unsigned int i;
		 for(i=0; i<ltemp->n; i++){
			 fprintf(stdout, "%d", BV_get(ltemp,i));
		 }
		 fprintf(stdout, "\n");
	 }
	 listit_destroy(temp);
  }
  listit_destroy(list_it_est);
  exit(1);*/

  MYTIME_stop(timer);
  MYTIME_LOG(INFO, timer);
  MYTIME_reset(timer);
  MYTIME_start(timer);
  INFO("Start simplification");
  psempl psemp= semplification(p,unique_factors);
  INFO("Used factors: %d", countTrue(psemp->factors_used));
  INFO("Not-used factors: %d", countTrue(psemp->factors_not_used));
  INFO("EST simplified: %d", countTrue(psemp->ests_ok));

  /*unsigned int j;
   fprintf(stdout, "Used: ");
   for(j=0; j<psemp->factors_used->n; j++){
 	  if(BV_get(psemp->factors_used,j))
 		  fprintf(stdout, "%d-", j);
   }*/
   //exit(1);

  //fprintf(stdout, "ESTs %d factors %d\n", list_size(p), psemp->factors_used->n);
  //exit(1);

  /*unsigned int j;
   fprintf(stdout, "Used: ");
   for(j=0; j<psemp->factors_used->n; j++){
 	  if(BV_get(psemp->factors_used,j))
 		  fprintf(stdout, "%d-", j);
   }
   fprintf(stdout, "\n");
   fprintf(stdout, "ESTs ok: ");
   unsigned int c=0;
   for(j=0; j<psemp->ests_ok->n; j++){
 	  if(BV_get(psemp->ests_ok,j)){
 		  fprintf(stdout, "%d-", j);
 		  c++;
 	  }
   }
   fprintf(stdout, "\nSimplified rows %d", c);
   fprintf(stdout, "\n");
   fprintf(stdout, "Not used: ");
   for(j=0; j<psemp->factors_not_used->n; j++){
 	  if(BV_get(psemp->factors_not_used,j))
 		  fprintf(stdout, "%d-", j);
   }
   fprintf(stdout, "\n");
   exit(1);*/


  //printf("usati: %s", BV_to_string(psemp->ests_ok));
  //exit(1);

  plist pl= color_matrix_semplified_create(p,psemp);
  //color_matrix_print(pl);
  //exit(1);

  //Se pl e' vuota significa che tutti i fattori sono necessari

  INFO("Simplification done!");
  MYTIME_stop(timer);
  MYTIME_LOG(INFO, timer);
  MYTIME_reset(timer);

  MYTIME_start(timer);
  INFO("Start search of the minimum factorization");

  if(!all_true(psemp->ests_ok)){
	 bv= min_fact(pl);
	 INFO("Search of the minimum factorization completed");
  } else {
	 INFO("Nessuna combinazione da ricercare!");
  }

  print_factorizations_result(bv,p,unique_factors,psemp);

  unsigned int q;
  unsigned int count_used_opt=0;
  for(q=0; q<psemp->factors_used->n; q++){
	 if (BV_get(psemp->factors_used,q)){
		  //INFO("U %d ", q);
		count_used_opt++;
	 }
  }
  INFO("Factors used in the optimum: %d", count_used_opt);

  MYTIME_stop(timer);
  MYTIME_LOG(INFO, timer);

  list_destroy(p,(delete_function)EST_destroy);

  //Controllo che pl non sia vuota. Altrimenti potrebbe dare segm. fault!!
  if(!list_is_empty(pl))
	  color_matrix_semplified_destroy(pl);

  list_destroy(pl,(delete_function)noop_free);

  list_destroy(unique_factors,(delete_function)noop_free);

  psempl_destroy(psemp);

  //Controllo!! Altrimenti potrebbe dare segm. fault!!
  if(bv != NULL)
	  BV_destroy(bv);

  MYTIME_stop(ttot);
  MYTIME_LOG(INFO, ttot);
  MYTIME_destroy(ttot);
  MYTIME_destroy(timer);
}
