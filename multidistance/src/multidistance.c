/*
code for the "multidistance" pd class. 
[multidistance] runs the equations for the multidistance system as in (https://en.wikipedia.org/wiki/Lorenz_system)
developed by Fede Camara Halac (fch226@nyu.edu)
@ the NYU Waverly Labs in the Music Department - FAS. (nyu-waverlylabs.org)
Released under the GNU General Public License. 
*/
#include "m_pd.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>

#define MAX 100

static t_class *multidistance_class;
typedef struct multidistance {
    t_object x_obj;
    t_symbol *x_objSymbol;
	t_word *x_vec;
    t_uInt x_randState;
	t_symbol *x_arrayName;
	t_bool x_storedFlag;
	t_float *x_originalData;
	t_sampIdx x_arrayPoints;
    t_outlet *x_info;
    t_outlet *x_list;

} t_multidistance;




t_float multidistance_euclid(t_attributeIdx n, t_float *v1, t_float *v2, t_float *weights, t_bool sqroot)
{
	t_float diff, dist;
	t_attributeIdx i;

	diff=dist=0.0;

	for(i=0; i<n; i++)
	{
		diff = v1[i] - v2[i];
		dist += diff*diff*weights[i];
	}

	// if the square root flag is on, take the square root
	if(sqroot)
		dist = sqrt(dist);
	
	return(dist);
}

static void multidistance_doit(t_multidistance *x, t_symbol *array1)
{
	t_garray *a;

	if(!(a = (t_garray *)pd_findbyclass(x->x_arrayName, garray_class)))
        pd_error(x, "%s: no array named %s", x->x_objSymbol->s_name, x->x_arrayName->s_name);
    else if(!garray_getfloatwords(a, (int *)&x->x_arrayPoints, &x->x_vec))
    	pd_error(x, "%s: bad template for %s", x->x_arrayName->s_name, x->x_objSymbol->s_name);
	else
	{
		t_sampIdx i, array1pts, biggestN;
		t_garray *b;
		t_word *vec1;
		t_float dist, *vecBuffer, *vec1Buffer, *vecWeights;

		if(!(b = (t_garray *)pd_findbyclass(array1, garray_class)))
		{
			pd_error(x, "%s: no array named %s", x->x_objSymbol->s_name, array1->s_name);
			return;
		}
		else if(!garray_getfloatwords(b, (int *)&array1pts, &vec1))
		{
			pd_error(x, "%s: bad template for %s", array1->s_name, x->x_objSymbol->s_name);
			return;
		}

		biggestN = 0;
		
		if(array1pts > x->x_arrayPoints)
			biggestN = array1pts;
		else
			biggestN = x->x_arrayPoints;

		vecBuffer = (t_float *)t_getbytes(biggestN*sizeof(t_float));
		vec1Buffer = (t_float *)t_getbytes(biggestN*sizeof(t_float));
		vecWeights = (t_float *)t_getbytes(biggestN*sizeof(t_float));

		for(i=0; i<biggestN; i++)
		{
			vecBuffer[i] = 0.0;
			vec1Buffer[i] = 0.0;
			// these weights aren't used in tabletool really. Just need to set all weights to 1.0 in order to use tIDLib_ distance functions
			vecWeights[i] = 1.0;
		}

		for(i=0; i<biggestN; i++)
		{
			if(i<x->x_arrayPoints)
				vecBuffer[i] = x->x_vec[i].w_float;
			
			if(i<array1pts)
				vec1Buffer[i] = vec1[i].w_float;
		}
			
		dist = multidistance_euclid(biggestN, vecBuffer, vec1Buffer, vecWeights, true);

		// free local memory
		t_freebytes(vecBuffer, biggestN * sizeof(t_float));
		t_freebytes(vec1Buffer, biggestN * sizeof(t_float));
		t_freebytes(vecWeights, biggestN * sizeof(t_float));

		outlet_float(x->x_info, dist);
	// }
}


void *multidistance_new() {
  t_multidistance *x = (t_multidistance *)pd_new(multidistance_class);
  x->x_outlet0 = outlet_new(&x->x_ob, &s_list);
  // multidistance_reset(x);
  return (void *)x;
}
void multidistance_setup(void) {
  multidistance_class = class_new(gensym("multidistance"), (t_newmethod)multidistance_new, 0, sizeof(t_multidistance), CLASS_DEFAULT, 0);
  class_addbang(multidistance_class, multidistance_bang);
  // class_addmethod(multidistance_class, 
  //   (t_method)multidistance_inita, gensym("inita"), A_FLOAT, 0);
  // class_addmethod(multidistance_class, 
  //   (t_method)multidistance_initb, gensym("initb"), A_FLOAT, 0);

}
