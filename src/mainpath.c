/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

/* dirent only works for POSIX compliant systems */
#include <dirent.h>



typedef struct mainpath {
	t_object x_obj;
	t_symbol *x_sym;
	t_symbol *x_subdir;
	t_symbol *x_newpath;
} t_mainpath;


static int mainpath_flag(t_mainpath *x, t_symbol *s)
{
	if(!s) 
		return 0;
	else {
		x->x_subdir = s;
		return 1;
	}
	//post("%d", mainpath_flag);
}

static void *mainpath_pathname(t_mainpath *x)
{
	char buf[MAXPDSTRING];
	sprintf(buf, "%s/%s/", x->x_sym->s_name, x->x_subdir->s_name);
	return (x->x_newpath = gensym(buf));
}

static void mainpath_bang(t_mainpath *x)
{
	if(mainpath_flag(x, x->x_subdir)) 
		outlet_symbol(x->x_obj.ob_outlet, mainpath_pathname(x));
	else
		outlet_symbol(x->x_obj.ob_outlet, x->x_sym);
}

static void *filename(t_mainpath *x, t_symbol *s, int f)
{
	t_atom path[2];
	int g;
	if (f == 0) g = 1; else g = 0;
	SETSYMBOL(&path[g], s);
	if(mainpath_flag(x, x->x_subdir))
		SETSYMBOL(&path[f], x->x_newpath);
	else
		SETSYMBOL(&path[f], x->x_sym);
	outlet_list(x->x_obj.ob_outlet, 0, 2, path);
	freebytes(path, sizeof(&path));
	return 0;
}

// void mainpath_prepend(t_mainpath *x, t_symbol *s)
// {
// 	if(mainpath_flag(x, x->x_subdir)) 
// 	{
// 		mainpath_pathname(x);
// 		filename(x, s, 1);
// 	}
// }

// void mainpath_append(t_mainpath *x, t_symbol *s)
// {
// 	if(mainpath_flag(x, x->x_subdir))
// 	{
// 		mainpath_pathname(x);
// 		filename(x, s, 0);
// 	}
// }


static void mainpath_add(t_mainpath *x, t_symbol *s)
{
	char buf[MAXPDSTRING];
	if(mainpath_flag(x, x->x_subdir)) {
		mainpath_pathname(x);
		sprintf(buf, "%s%s", x->x_newpath->s_name, s->s_name);
	} else 
		sprintf(buf, "%s/%s", x->x_sym->s_name, s->s_name);
	outlet_symbol(x->x_obj.ob_outlet, gensym(buf));
}

static void mainpath_dogetfiles(t_mainpath *x, t_int f)
{
	
	DIR *d;
	struct dirent *dir;
	t_atom files[MAXFILEINDEX];
	char buf[MAXPDSTRING];
	int i = 0, pd;
	pd = f == 1 ? 1 : 0;
	int pdlen = strlen(".pd");
	if(mainpath_flag(x, x->x_subdir)) {
		mainpath_pathname(x);
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
				if((strcmp(&dir->d_name[strlen(dir->d_name)-pdlen], ".pd") == 0))
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


static void mainpath_getfiles(t_mainpath *x)
{
	mainpath_dogetfiles(x,0);
}


static void mainpath_getpdfiles(t_mainpath *x)
{
	mainpath_dogetfiles(x,1);
}


static void mainpath_reset(t_mainpath *x)
{
	x->x_newpath = NULL;
	x->x_subdir = NULL;
}



static void mainpath_open(t_mainpath *x, t_symbol *s)
{
	char buf[MAXPDSTRING];
	if(mainpath_flag(x, x->x_subdir)) {
		mainpath_pathname(x);
		sprintf(buf, "open %s%s", x->x_newpath->s_name, s->s_name);
	} else 
		sprintf(buf, "open %s/%s", x->x_sym->s_name, s->s_name);
	if ((system(buf))) pd_error(x,"Couldn't %s", buf);
	else 
		outlet_bang(x->x_obj.ob_outlet);
}


static t_class *mainpath_class;


static void *mainpath_new(t_symbol *s)
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
	// class_addmethod(mainpath_class, (t_method)mainpath_prepend, gensym("prepend"), A_SYMBOL, 0);
	// class_addmethod(mainpath_class, (t_method)mainpath_append, gensym("append"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_add, gensym("add"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_getfiles, gensym("getfiles"), 0);
	class_addmethod(mainpath_class, (t_method)mainpath_getpdfiles, gensym("getpdfiles"), 0);
	class_addmethod(mainpath_class, (t_method)mainpath_open, gensym("open"), A_SYMBOL, 0);
	class_addmethod(mainpath_class, (t_method)mainpath_reset, gensym("reset"), 0, 0);
}