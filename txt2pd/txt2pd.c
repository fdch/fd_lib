#include "m_pd.h"
#include "g_canvas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IN 1
#define OUT 0

extern t_pd *newest;

typedef struct txt2pd {
	t_object x_obj;
	t_symbol *x_sym;
	t_symbol *x_filename, *x_newfilename;
	t_int x_yaxis, x_xaxis;
	t_int totwords;
} t_txt2pd;


static int txt2pd_doread(t_txt2pd *x)
{
	FILE *fp, *fp2;
	char c;
	char buf[MAXPDSTRING], buf2[MAXPDSTRING], cbuf[MAXPDSTRING];
	int i = x->totwords;
	int oflag = 0, mflag = 0;
	int connect = 0;
	int state = OUT;
	int to = 1, fro = 0, in = 0, out = 0;

	fp = sys_fopen(x->x_filename->s_name, "r");
	sprintf(buf2, "%s.pd", x->x_filename->s_name);
	x->x_newfilename = gensym(buf2);
	fp2 = sys_fopen(buf2, "w");
	if (!fp || !fp2) return 0;
	post("************ [txt2pd] ************");
	post("source: %s", x->x_filename->s_name);
	post("target: %s", x->x_newfilename->s_name);
	post("------------ begin conversion ------------");
	fputs("#N canvas 0 22 450 350 12;\n", fp2);
	while ( (c = fgetc(fp)) != EOF && i < MAXPDSTRING)
	{
		if(!(c == '\t' || c == '\n' || c == '\r' || c == '(' || c == ')' || c == '[' || c == ']' || c == ',' || c == ';' || c == '|' || c == '$'))
			sprintf(&buf[i++], "%c", c);
		else if (c == '$') sprintf(&buf[i++], "%s", "\\$"), i++;
		else if (c == ';') sprintf(&buf[i++], "%s", "\\;"), i++;
		else if (c == '|') connect++, post("%c", c);
		else if (c == '(') mflag = 1, oflag = 0;
		else if (c == '[') state = IN;
		else if (c == ']') oflag = 1, mflag = 0;
		//print the pd file
		if (state == IN)
		{
			if (oflag == 0 && mflag == 1)
			{
				sprintf(buf2, "#X msg %ld %ld %s;\n",
					x->x_xaxis, x->x_yaxis, buf);
				x->x_yaxis += 30;
				fputs(buf2, fp2);
				post(buf);
				free(buf);
				mflag = oflag = i = 0;
				state = OUT;
			}else if (oflag == 1 && mflag == 0)
			{
				sprintf(buf2, "#X obj %ld %ld %s;\n",
					x->x_xaxis, x->x_yaxis, buf);
				fputs(buf2, fp2);
				x->x_yaxis += 30;
				post(buf);
				free(buf);
				oflag = i = 0;
				state = OUT;
			}
		}
		
	} // finished reading file
	//make connections
	if (connect > 1) {
		for (fro = 0, to = 1; fro < connect; to++, fro++)
		{
			sprintf(cbuf, "#X connect %d %d %d %d;\n", fro, out, to, in);
			fputs(cbuf, fp2);
		}
	}
	sys_fclose(fp);
	sys_fclose(fp2);
	post("------------ conversion ended ------------");
	x->totwords = i;
	return 1;
}

void txt2pd_read(t_txt2pd *x, t_symbol *s)
{
	char buf[MAXPDSTRING], obuf[MAXPDSTRING];
	sprintf(buf, "%s/%s", x->x_sym->s_name, s->s_name);
	x->x_filename = gensym(buf);
	x->totwords = 0;
	x->x_xaxis = x->x_yaxis = 10;
	if (txt2pd_doread(x)) {
		sprintf(obuf, "open %s", x->x_newfilename->s_name);
		system(obuf);
	} else
		bug("txt2pd_doread");
}

static t_class *txt2pd_class;

void *txt2pd_new()
{
	t_txt2pd *x = (t_txt2pd *)pd_new(txt2pd_class);
	x->x_sym = canvas_getcurrentdir();
	x->x_xaxis = 10;
	x->x_yaxis = 10;
	
	return(x);
}

void txt2pd_setup()
{
	txt2pd_class = class_new(gensym("txt2pd"), (t_newmethod)txt2pd_new, 0, sizeof(t_txt2pd), CLASS_DEFAULT, 0, 0);
	class_addmethod(txt2pd_class, (t_method)txt2pd_read, gensym("read"), A_SYMBOL, 0);
}