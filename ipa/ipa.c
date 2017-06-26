#include "m_pd.h"
#include "stdlib.h"

char *symbols[73] = {"p", "b", "p̪", "b̪", "t̪", "d̪", "t", "d", "ʈ", "ɖ", "c", "ɟ", "k", "ɡ", "q", "ɢ", "ʔ", "m", "ɱ", "n̪", "n", "ɳ", "ɲ", "ŋ", "ɴ", "ʙ", "r̪", "r", "ʀ", "ⱱ̟", "ⱱ", "ɾ̪", "ɾ", "ɽ", "ɢ̆", "ɺ", "ʎ̯", "ʟ̆", "ɸ", "β", "f", "v", "θ", "ð", "s", "z", "ʃ", "ʒ", "ʂ", "ʐ", "ç", "ʝ", "x", "ɣ", "χ", "ʁ", "ħ", "ʕ", "h", "ɦ", "ɬ", "ɮ", "β̞", "ʋ", "ɹ", "ɻ", "j", "ɰ", "l̪", "l", "ɭ", "ʎ", "ʟ", "\0"};

typedef struct _ipa {
  t_object  x_obj;
  t_float  x_a, x_b;
  double x_ox, x_oy;
  int  x_i;
} t_ipa;

static void ipa_bang(t_ipa *x) {
  int i;
  t_atom syms[73];
  for (i = 0; i < 73; i++) {
    SETSYMBOL(&syms[i], gensym(&symbols[i]));
  }
  outlet_list(x->x_obj.ob_outlet, gensym("list"), 73, syms);
}

static void ipa_float(t_ipa *x, t_floatarg f) {
  if(f<73 && f>=0)
  outlet_symbol(x->x_obj.ob_outlet, gensym(&symbols[(int)f])->s_name);
}

static t_class *ipa_class;

void *ipa_new() {
  t_ipa *x = (t_ipa *)pd_new(ipa_class);
  outlet_new(&x->x_obj, &s_list);
  return (void *)x;
}

void ipa_setup(void) {
  ipa_class = class_new(gensym("ipa"),(t_newmethod)ipa_new,0, sizeof(t_ipa), CLASS_DEFAULT, 0);
  class_addbang(ipa_class, ipa_bang);
  class_addfloat(ipa_class, ipa_float);
}