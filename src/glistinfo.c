/* 

Copyright 2017-2020 Fede Camara Halac - ffddcchh

This file is part of fd_lib.

fd_lib is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

fd_lib is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "fdLib.h"

#define SLEN 33

char vars[][SLEN+1]={ "gl_valid", "gl_pixwidth", "gl_pixheight", "gl_x1", "gl_y1", "gl_x2", "gl_y2", "gl_screenx1", "gl_screeny1", "gl_screenx2", "gl_screeny2", "gl_xmargin", "gl_ymargin", "gl_nxlabels", "gl_xlabely", "gl_nylabels", "gl_ylabelx", "gl_font", "gl_zoom", "gl_havewindow", "gl_mapped", "gl_dirty", "gl_loading", "gl_willvis", "gl_edit", "gl_isdeleting", "gl_goprect", "gl_isgraph", "gl_hidetext", "gl_private", "gl_isclone", "gl_name", "gl_owner", "\0"};

typedef struct _glistinfo {
  t_object  x_obj;
  t_canvas *x_cnv;
  t_atom syms[SLEN];
  t_symbol *x_owner;
} t_glistinfo;

static void glistinfo_load(t_glistinfo *x) {
  SETFLOAT(&x->syms[0],x->x_cnv->gl_valid>1?1:0);
  SETFLOAT(&x->syms[1],x->x_cnv->gl_pixwidth);
  SETFLOAT(&x->syms[2],x->x_cnv->gl_pixheight);
  SETFLOAT(&x->syms[3],x->x_cnv->gl_x1);
  SETFLOAT(&x->syms[4],x->x_cnv->gl_y1);
  SETFLOAT(&x->syms[5],x->x_cnv->gl_x2);
  SETFLOAT(&x->syms[6],x->x_cnv->gl_y2);
  SETFLOAT(&x->syms[7],x->x_cnv->gl_screenx1);
  SETFLOAT(&x->syms[8],x->x_cnv->gl_screeny1);
  SETFLOAT(&x->syms[9],x->x_cnv->gl_screenx2);
  SETFLOAT(&x->syms[10],x->x_cnv->gl_screeny2);
  SETFLOAT(&x->syms[11],x->x_cnv->gl_xmargin);
  SETFLOAT(&x->syms[12],x->x_cnv->gl_ymargin);
  SETFLOAT(&x->syms[13],x->x_cnv->gl_nxlabels);
  SETFLOAT(&x->syms[14], x->x_cnv->gl_xlabely);
  SETFLOAT(&x->syms[15],x->x_cnv->gl_nylabels);
  SETFLOAT(&x->syms[16],x->x_cnv->gl_ylabelx);
  SETFLOAT(&x->syms[17],x->x_cnv->gl_font);
  SETFLOAT(&x->syms[18],x->x_cnv->gl_zoom);
  SETFLOAT(&x->syms[19],x->x_cnv->gl_havewindow);
  SETFLOAT(&x->syms[20],x->x_cnv->gl_mapped);
  SETFLOAT(&x->syms[21],x->x_cnv->gl_dirty);
  SETFLOAT(&x->syms[22],x->x_cnv->gl_loading);
  SETFLOAT(&x->syms[23],x->x_cnv->gl_willvis);
  SETFLOAT(&x->syms[24],x->x_cnv->gl_edit);
  SETFLOAT(&x->syms[25],x->x_cnv->gl_isdeleting);
  SETFLOAT(&x->syms[26],x->x_cnv->gl_goprect);
  SETFLOAT(&x->syms[27],x->x_cnv->gl_isgraph);
  SETFLOAT(&x->syms[28],x->x_cnv->gl_hidetext);
  SETFLOAT(&x->syms[29],x->x_cnv->gl_private);
  SETFLOAT(&x->syms[30],x->x_cnv->gl_isclone);
  if (x->x_cnv->gl_name)
    SETSYMBOL(&x->syms[SLEN-2], x->x_cnv->gl_name);
  else
    SETFLOAT(&x->syms[SLEN-2], 0); 
  if (x->x_cnv->gl_owner)
    SETSYMBOL(&x->syms[SLEN-1], x->x_cnv->gl_owner->gl_name);
  else
    SETFLOAT(&x->syms[SLEN-1], 0); 
}

static void glistinfo_dump(t_glistinfo *x) {
  glistinfo_load(x);
  outlet_list(x->x_obj.ob_outlet, gensym("list"), SLEN, x->syms);
}

static void glistinfo_bang(t_glistinfo *x) {
  glistinfo_load(x);
  t_atom out[1];
  int i;
  for (i = 0; i < SLEN; i++) {
    if (i>=SLEN-2)
      SETSYMBOL(&out[0],atom_getsymbol(&x->syms[i]));
    else
      SETFLOAT(&out[0],atom_getfloat(&x->syms[i]));
    outlet_anything(x->x_obj.ob_outlet,gensym(vars[i]),1,out);
  }
}



static t_class *glistinfo_class;


static void *glistinfo_new(t_symbol *s) {
  t_glistinfo *x = (t_glistinfo *)pd_new(glistinfo_class);
  x->x_cnv = (t_canvas *)canvas_getcurrent();
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void glistinfo_setup(void) {
  glistinfo_class = class_new(gensym("glistinfo"),(t_newmethod)glistinfo_new,0, sizeof(t_glistinfo), CLASS_DEFAULT, A_DEFSYM,0);
  class_addbang(glistinfo_class, glistinfo_bang);
  class_addmethod(glistinfo_class, (t_method)glistinfo_dump, gensym("dump"), 0);
}