/* 

Copyright 2019 Fede Camara Halac

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"
 
typedef struct root
{
  t_object x_ob;
  t_glist *x_glist;
} t_root;


static t_class *root_class;

static void root_bang(t_root *x) {
  char buf[MAXPDSTRING];
  if(x->x_glist->gl_owner)
    snprintf(buf, MAXPDSTRING-1, ".x%lx", (t_int)canvas_getrootfor(x->x_glist->gl_owner));
  else
    snprintf(buf, MAXPDSTRING-1, ".x%lx", (t_int)x->x_glist->gl_name);
  buf[MAXPDSTRING-1]=0;
  outlet_symbol(x->x_ob.ob_outlet, gensym(buf));
}


static void *root_new() {
  t_root *x = (t_root *)pd_new(root_class);
  x->x_glist = canvas_getcurrent();
  outlet_new(&x->x_ob, &s_symbol);
  return (void *)x;
}

void root_setup(void) {
    root_class = class_new(gensym("root"), (t_newmethod)root_new, 0, sizeof(t_root), 0, 0);
    class_addbang(root_class, root_bang);
}