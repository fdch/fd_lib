#include "m_pd.h"
#include "g_canvas.h"
#include "m_imp.h"

typedef struct connect {
	t_object x_obj;
	t_symbol *x_who;
	t_canvas *canvas;
	t_canvas *x_parent;
	t_object *x_object;
} t_connect;

int setcanvassym(t_connect *x, t_symbol *s)
{
	x->x_who = s;
	x->canvas = glist_getcanvas(((t_canvas *)pd_findbyclass(x->x_who, canvas_class)));
	
	if (x->canvas) {
		x->x_object = pd_checkobject((t_pd*)x->canvas);
		x->x_parent = x->canvas->gl_owner;
		return 1;
	} else {
		bug("setcanvassym");
		return 0;
	}
}

void connect_doit(t_connect *x, t_int f)
{
	t_gobj *y;
	int ninlets = 0, fro = 0, in = 0, out = 0;
	int to = 1;
	int connid = 0;
	//ninlets = obj_ninlets(x->x_object);
	
	
	for (y = x->canvas->gl_list; y; y = y->g_next)
	{
		t_object*obj=(t_object*)y;
		connid = glist_getindex(x->canvas, (t_gobj*)obj);
	}
	if (!connid)
		post("Found no objects in %s", x->x_who->s_name);
	else
		for (to = 0, fro = 1; to < connid; fro++, to++) 
			if (f == 1)
				canvas_connect(x->canvas, to, out, fro, in);
			else
				canvas_disconnect(x->canvas, to, out, fro, in);

}

void connect_bang(t_connect *x)
{
	if (!x->x_who) {
		post("No canvas to point to...");
		return;
	} else
		connect_doit(x, 1);
}

void connect_symbol(t_connect *x, t_symbol *s)
{
	setcanvassym(x,s);
	connect_bang(x);
}

void connect_disconnect(t_connect *x, t_symbol *s)
{
	setcanvassym(x,s);
	if (!x->x_who) {
		post("No canvas to point to...");
		return;
	} else
		connect_doit(x, 0);
}

static t_class *connect_class;

void *connect_new(t_symbol *s)
{
	t_connect *x = (t_connect *)pd_new(connect_class);
	if (*s->s_name)
		setcanvassym(x,s);
	return (x);
}

void connect_setup() {
	connect_class = class_new(gensym("connect"), (t_newmethod)connect_new, 0, sizeof(t_connect), CLASS_DEFAULT, A_DEFSYM, 0);
	class_addbang(connect_class, connect_bang);
	class_addsymbol(connect_class, connect_symbol);
	class_addmethod(connect_class, (t_method)connect_disconnect, gensym("disconnect"), A_SYMBOL, 0);
}