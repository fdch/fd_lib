#include "m_pd.h"
#include <string.h>

static t_class *timid2json_class;

typedef struct _timid2json {
	t_object x_obj;
	t_int x_sense;
} t_timid2json;

void timid2json(t_timid2json *x, t_int argc, t_atom *argv)
{
	post("timid2json: %d arguments found",argc);
}

void json2timid(t_timid2json *x, t_int argc, t_atom *argv)
{
	post("json2timid: %d arguments found",argc);
}

void timid2json_sense(t_timid2json *x, t_floatarg f)
{
	x->x_sense=f>=1?1:0;
	post("[timid2json]: sense = %d",x->x_sense);
}

void timid2json_list(t_timid2json *x, t_symbol *s, t_int argc, t_atom *argv)
{
	if (!x->x_sense) {
		timid2json(x,argc,argv);
	} else {
		json2timid(x,argc,argv);
	}

}

void *timid2json_new(void)
{
	t_timid2json *x = (t_timid2json *)pd_new(timid2json_class);
	
	outlet_new(&x->x_obj, gensym("bang"));
	return (void *)x;
}


void timid2json_setup(void)
{
	timid2json_class = class_new(gensym("timid2json"), (t_newmethod)timid2json_new, 0, sizeof(t_timid2json), CLASS_DEFAULT, 0);

	class_addlist(timid2json_class, timid2json_list);
	class_addfloat(timid2json_class, timid2json_sense);
	post("timid to json converter");
}