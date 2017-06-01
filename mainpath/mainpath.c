#include "m_pd.h"
#include "g_canvas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* dirent only works for POSIX compliant systems */
#include <dirent.h>

#define MAXFILEINDEX 8192

typedef struct mainpath {
	t_object x_obj;
	t_symbol *x_sym;
	t_symbol *x_subdir;
	t_symbol *x_newpath;
} t_mainpath;

int flag(t_mainpath *x, t_symbol *s)
{
	if(!s) 
		return 0;
	else {
		x->x_subdir = s;
		return 1;
	}
	//post("%d", flag);
}
void *pathname(t_mainpath *x)
{
	char buf[MAXPDSTRING];
	sprintf(buf, "%s/%s/", x->x_sym->s_name, x->x_subdir->s_name);
	return (x->x_newpath = gensym(buf));
}
void mainpath_bang(t_mainpath *x)
{
	if(flag(x, x->x_subdir)) 
		outlet_symbol(x->x_obj.ob_outlet, pathname(x));
	else
		outlet_symbol(x->x_obj.ob_outlet, x->x_sym);
}

static void *filename(t_mainpath *x, t_symbol *s, int f)
{
	t_atom path[2];
	int g;
	if (f == 0) g = 1; else g = 0;
	SETSYMBOL(&path[g], s);
	if(flag(x, x->x_subdir))
		SETSYMBOL(&path[f], x->x_newpath);
	else
		SETSYMBOL(&path[f], x->x_sym);
	outlet_list(x->x_obj.ob_outlet, 0, 2, path);
	freebytes(path, sizeof(&path));
	return 0;
}

void mainpath_prepend(t_mainpath *x, t_symbol *s)
{
	if(flag(x, x->x_subdir)) pathname(x);
	filename(x, s, 1);
}

void mainpath_append(t_mainpath *x, t_symbol *s)
{
	if(flag(x, x->x_subdir)) pathname(x);
	filename(x, s, 0);
}

void mainpath_add(t_mainpath *x, t_symbol *s)
{
	char buf[MAXPDSTRING];
	if(flag(x, x->x_subdir)) {
		pathname(x);
		sprintf(buf, "%s%s", x->x_newpath->s_name, s->s_name);
	} else 
		sprintf(buf, "%s/%s", x->x_sym->s_name, s->s_name);
	outlet_symbol(x->x_obj.ob_outlet, gensym(buf));
}

void mainpath_dogetfiles(t_mainpath *x, t_int f)
{
	
	DIR *d;
	struct dirent *dir;
	t_atom files[MAXFILEINDEX];
	char buf[MAXPDSTRING];
	int i = 0, pd;
	pd = f == 1 ? 1 : 0;
	int pdlen = strlen(".pd");
	if(flag(x, x->x_subdir)) {
		pathname(x);
		d = opendir(x->x_newpath->s_name);
	} else 
		d = opendir(x->x_sym->s_name);

	if (d) {
		while ((dir = readdir(d)) != NULL || i >= MAXFILEINDEX) {
			if (!pd)
			{
				sprintf(buf, "%s", dir->d_name);
				SETSYMBOL(&files[i], gensym(buf));
				outlet_symbol(x->x_obj.ob_outlet, atom_getsymbol(&files[i]));
			} else 
				if((strcmp(&dir->d_name[dir->d_namlen-pdlen], ".pd") == 0))
				{
				sprintf(buf, "%s.pd", dir->d_name);
				SETSYMBOL(&files[i], gensym(buf));
				outlet_symbol(x->x_obj.ob_outlet, atom_getsymbol(&files[i]));
				}
			i++;
		}
		closedir(d);
	}
}

void mainpath_getfiles(t_mainpath *x)
{
	mainpath_dogetfiles(x,0);
}

void mainpath_getpdfiles(t_mainpath *x)
{
	mainpath_dogetfiles(x,1);
}

void mainpath_reset(t_mainpath *x)
{
	x->x_newpath = NULL;
	x->x_subdir = NULL;
}


void mainpath_open(t_mainpath *x, t_symbol *s)
{
	char buf[MAXPDSTRING];
	if(flag(x, x->x_subdir)) {
		pathname(x);
		sprintf(buf, "open %s%s", x->x_newpath->s_name, s->s_name);
	} else 
		sprintf(buf, "open %s/%s", x->x_sym->s_name, s->s_name);
	if ((system(buf))) post("Couldn't %s", buf);
}


static t_class *mainpath_class;

void *mainpath_new(t_symbol *s)
{
	t_mainpath *x = (t_mainpath *)pd_new(mainpath_class);
	outlet_new(&x->x_obj, &s_symbol);
	x->x_sym = canvas_getcurrentdir();
	if(!*s->s_name)
		symbolinlet_new(&x->x_obj, &x->x_subdir);
	else
		x->x_subdir = s;
	return(x);
}

void mainpath_setup()
{
	mainpath_class = class_new(gensym("mainpath"), (t_newmethod)mainpath_new, 0, sizeof(t_mainpath), CLASS_DEFAULT, A_DEFSYM, 0);
	class_addbang(mainpath_class, mainpath_bang);
	class_addmethod(mainpath_class, (t_method)mainpath_prepend, gensym("prepend"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_append, gensym("append"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_add, gensym("add"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_getfiles, gensym("getfiles"), 0);
	class_addmethod(mainpath_class, (t_method)mainpath_getpdfiles, gensym("getpdfiles"), 0);
	class_addmethod(mainpath_class, (t_method)mainpath_open, gensym("open"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_reset, gensym("reset"), 0, 0);
}