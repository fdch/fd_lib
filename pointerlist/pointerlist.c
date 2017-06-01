/* Maintain a list of gpointers.*/

#include "m_pd.h"

#ifdef _WIN32
# include <malloc.h> /* MSVC or mingw on windows */
#elif defined(__linux__) || defined(__APPLE__)
# include <alloca.h> /* linux, mac, mingw, cygwin */
#else
# include <stdlib.h> /* BSDs for example */
#endif
#ifndef HAVE_ALLOCA     /* can work without alloca() but we never need it */
#define HAVE_ALLOCA 1
#endif

#define PLIST_NGETBYTE 100 

extern t_pd *newest;

typedef struct _listelem
{
	t_atom l_a;
	t_gpointer l_p;
} t_listelem;

typedef struct pointerlist {
    t_pd l_pd;          /* object to point inlets to */
    int l_n;            /* number of items */
    int l_npointer;     /* number of pointers */
    t_listelem *l_vec;  /* pointer to items */
} t_pointerlist;

t_class *pointerlist_class;

static void pointerlist_init(t_pointerlist *x)
{
	x->l_pd = pointerlist_class;
	x->l_n = x->l_npointer = 0;
	x->l_vec = 0;
}

static void pointerlist_clear(t_pointerlist *x)
{
	int i;
	for (i = 0; i < x->l_n; i++)
	{
		if (x->l_vec[i].l_a.a_type == A_POINTER)
		gpointer_unset(x->l_vec[i].l_a.a_w.w_gpointer);
	}
	if (x->l_vec)
		freebytes(x->l_vec, x->l_n * sizeof(*x->l_vec));
}

#if HAVE_ALLOCA
#define ATOMS_ALLOCA(x, n) ((x) = (t_atom *)((n) < PLIST_NGETBYTE ?  \
        alloca((n) * sizeof(t_atom)) : getbytes((n) * sizeof(t_atom))))
#define ATOMS_FREEA(x, n) ( \
    ((n) < PLIST_NGETBYTE || (freebytes((x), (n) * sizeof(t_atom)), 0)))
#else
#define ATOMS_ALLOCA(x, n) ((x) = (t_atom *)getbytes((n) * sizeof(t_atom)))
#define ATOMS_FREEA(x, n) (freebytes((x), (n) * sizeof(t_atom)))
#endif

static void atoms_copy(int argc, t_atom *from, t_atom *to)
{
    int i;
    for (i = 0; i < argc; i++)
        to[i] = from[i];
}

static void pointerlist_anything(t_pointerlist *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    pointerlist_clear(x);
    if (!(x->l_vec = (t_listelem *)getbytes((argc+1) * sizeof(*x->l_vec))))
    {
        x->l_n = 0;
        error("list_alloc: out of memory");
        return;
    }
    x->l_n = argc+1;
    x->l_npointer = 0;
    SETSYMBOL(&x->l_vec[0].l_a, s);
    for (i = 0; i < argc; i++)
    {
        x->l_vec[i+1].l_a = argv[i];
        if (x->l_vec[i+1].l_a.a_type == A_POINTER)
        {
            x->l_npointer++;
            gpointer_copy(x->l_vec[i+1].l_a.a_w.w_gpointer, &x->l_vec[i+1].l_p);
            x->l_vec[i+1].l_a.a_w.w_gpointer = &x->l_vec[i+1].l_p;
        }
    }
}

static void pointerlist_setup(void)
{
    pointerlist_class = class_new(gensym("pointerlist inlet"),
        0, 0, sizeof(t_pointerlist), 0, 0);
    class_addanything(pointerlist_class, pointerlist_anything);
}

static void pointerlist_toatoms(t_pointerlist *x, t_atom *to)
{
    int i;
    for (i = 0; i < x->l_n; i++)
        to[i] = x->l_vec[i].l_a;
}


static void pointerlist_clone(t_pointerlist *x, t_pointerlist *y)
{
    int i;
    y->l_pd = pointerlist_class;
    y->l_n = x->l_n;
    y->l_npointer = x->l_npointer;
    if (!(y->l_vec = (t_listelem *)getbytes(y->l_n * sizeof(*y->l_vec))))
    {
        y->l_n = 0;
        error("list_alloc: out of memory");
    }
    else for (i = 0; i < x->l_n; i++)
    {
        y->l_vec[i].l_a = x->l_vec[i].l_a;
        if (y->l_vec[i].l_a.a_type == A_POINTER)
        {
            gpointer_copy(y->l_vec[i].l_a.a_w.w_gpointer, &y->l_vec[i].l_p);
            y->l_vec[i].l_a.a_w.w_gpointer = &y->l_vec[i].l_p;
        }
    }
}

t_class *pointerlist_append_class;

typedef struct _pointerlist_append
{
    t_object x_obj;
    t_pointerlist x_plist;
    t_outlet *x_out1;
    t_outlet *x_out2;
} t_pointerlist_append;

static void *pointerlist_append_new(t_symbol *s, int argc, t_atom *argv)
{
    t_pointerlist_append *x = (t_pointerlist_append *)pd_new(pointerlist_append_class);
    pointerlist_init(&x->x_plist);
    //pointerlist_list(&x->x_alist, 0, argc, argv);
    x->x_out1 = outlet_new(&x->x_obj, &s_list);
    x->x_out2 = outlet_new(&x->x_obj, &s_pointer);
    inlet_new(&x->x_obj, &x->x_plist.l_pd, 0, 0);
    return (x);
}

static void pointerlist_append_anything(t_pointerlist_append *x, t_symbol *s,
    int argc, t_atom *argv)
{
    t_atom *outv;
    int n, outc = x->x_plist.l_n + argc + 1;
    ATOMS_ALLOCA(outv, outc);
    SETSYMBOL(outv, s);
    atoms_copy(argc, argv, outv + 1);
    if (x->x_plist.l_npointer)
    {
        t_pointerlist y;
        pointerlist_clone(&x->x_plist, &y);
        pointerlist_toatoms(&y, outv + 1 + argc);
        outlet_list(x->x_out1, &s_list, outc, outv);
        pointerlist_clear(&y);
    }
    else
    {
        pointerlist_toatoms(&x->x_plist, outv + 1 + argc);
        outlet_list(x->x_out2, &s_list, outc, outv);
    }
    ATOMS_FREEA(outv, outc);
}


void pointerlist_append_bang(t_pointerlist_append *x, t_symbol *s, int argc, t_atom *argv)
{
	outlet_list(x->x_out1, &s_list, argc, argv);
}

void pointerlist_append_float(t_pointerlist_append *x, t_float f, int argc, t_atom *argv)
{
	t_gpointer *gp;
	int n = f;
    if (n < 0)
        n = 0;
    if (argc >= n)
    {
    	gp = (t_gpointer *) argv+n;
    	if (gp)
        outlet_pointer(x->x_out2, gp);
    }
	else
		outlet_list(x->x_out1, &s_list, argc, argv);
}
/*
void pointerlist_symbol(t_pointerlist *x, t_symbol *s)
{
	if (!s) 
		post("No symbol provided.");
	pd_unbind(&x->x_obj.ob_pd, x->x_sym);
	x->x_sym = s;
	pd_bind(&x->x_obj.ob_pd, s);
}

void pointerlist_send(t_pointerlist *x, t_symbol *s)
{
	if (s->s_thing) 
		pd_pointer(s->s_thing, *(t_gpointer **)x->ptrlist);
	else 
		error("Coulndn't send pointerlist");
	
}

static void pointerlist_free(t_pointerlist *x)
{
	pd_unbind(&x->x_obj.ob_pd, x->x_sym);
	t_freebytes((x->ptrlist, sizeof(x->ptrlist);
}
*/


static void pointerlist_append_free(t_pointerlist_append *x)
{
    pointerlist_clear(&x->x_plist);
}

static void pointerlist_append_setup(void)
{
    pointerlist_append_class = class_new(gensym("pointerlist append"),
        (t_newmethod)pointerlist_append_new, (t_method)pointerlist_append_free,
        sizeof(t_pointerlist_append), 0, A_GIMME, 0);
    //class_addlist(list_append_class, list_append_list);
    class_addanything(pointerlist_append_class, pointerlist_append_anything);
    class_sethelpsymbol(pointerlist_append_class, &s_list);
}



static void *pointerlist_new(t_pd *dummy, t_symbol *s, int argc, t_atom *argv)
{
    if (!argc || argv[0].a_type != A_SYMBOL)
        newest = pointerlist_append_new(s, argc, argv);
    else
    {
        t_symbol *s2 = argv[0].a_w.w_symbol;
        if (s2 == gensym("append"))
            newest = pointerlist_append_new(s, argc-1, argv+1);
        else
        {
            error("pointerlist %s: unknown function", s2->s_name);
            newest = 0;
        }
    }
    return (newest);
}

void x_pointerlist_setup(void)
{
    pointerlist_setup();
    pointerlist_append_setup();
    /*
    list_prepend_setup();
    list_split_setup();
    list_trim_setup();
    list_length_setup();
    list_fromsymbol_setup();
    list_tosymbol_setup();
    */
    class_addcreator((t_newmethod)pointerlist_new, &s_list, A_GIMME, 0);
}