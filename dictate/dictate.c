#include "m_pd.h"
#include <stdio.h>
#include <pocketsphinx.h>
typedef struct dictate
{
	t_object x_obj;
	t_symbol *x_sym;
	t_symbol *filename;
	t_symbol *x_word;
} t_dictate;

int 
dictate_dodictate(t_dictate *x, t_symbol *filename)
{
	ps_decoder_t *ps = NULL;
	cmd_ln_t *config = NULL;
	FILE *fh;
	char const *hyp, *uttid;
	int16 buf[512];
	int rv;
	int32 score;

	config = cmd_ln_init(NULL, ps_args(), TRUE,
		"-hmm", MODELDIR "/en-us/en-us",
		"-lm", MODELDIR "/en-us/en-us.lm.bin",
		"-dict", MODELDIR "/en-us/cmudict-en-us.dict",
		NULL);
	if (config == NULL) {
		post("Failed to create config object, see log for details");
	}
	
	ps = ps_init(config);
	if (ps == NULL) {
		post("Failed to create recognizer, see log for details");
	}
	
	fh = fopen(filename->s_name, "rb");
	if (fh == NULL) {
		post("Unable to open %s", filename->s_name);
	}
	
	rv = ps_start_utt(ps);
	
	while (!feof(fh)) {
		size_t nsamp;
		nsamp = fread(buf, 2, 512, fh);
		ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
	}
	
	rv = ps_end_utt(ps);
	hyp = ps_get_hyp(ps, &score);
	
	fclose(fh);
	ps_free(ps);
	cmd_ln_free_r(config);
	
	x->x_word = gensym(hyp);
	return 0;
}

void dictate_bang(t_dictate *x)
{
	//dictate_dodictate(x, x->filename);
	if (!x->x_word) post("No hypothesis taken");
	outlet_symbol(x->x_obj.ob_outlet, x->x_word);
}
static t_class *dictate_class;

void *dictate_new(t_symbol *s)
{
	t_dictate *x = (t_dictate *)pd_new(dictate_class);
	outlet_new(&x->x_obj, &s_symbol);
	//x->x_sym = canvas_getcurrentdir();
	x->filename = s;
	return(x);
}

void dictate_setup()
{
	dictate_class = class_new(gensym("dictate"), (t_newmethod)dictate_new, 0, sizeof(t_dictate), 0, 0);
	class_addbang(dictate_class, dictate_bang);
}