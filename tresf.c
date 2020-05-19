// tresf
#include "m_pd.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "shared/rand.h"

#define MAXRAND 268435457
#define DENRAND 268435456

static t_class *tresf_class;

typedef struct _tresf {
  t_object  x_obj;
  unsigned int x_seed;    //random seed for random operations
  t_float   x_proba;      //"proba" parameter
  t_float   x_ampjitter;  //"ampjitter" parameter
  t_int     x_maxfreqs;   //"maxfreqs" parameter

  t_float   *x_afin;      //array de t_floats utilizado, internamente, para armazenar lista de entrada

  t_atom    *x_freqs;     //array de t_atoms utilizado para armazenar lista de todas as tríades de dados f a d (outlet 1)
  t_atom    *x_f_a_d;     //array de t_atoms utilizado para armazenar cada tríade de dado f a d (outlet 2)

  t_atom    *x_listefreq; //array de t_atoms utilizado para armazenar freqs (outlet 3, tendo "freqs" como rótulo)
  //t_atom    *x_listemfreq;
  t_atom    *x_listeamp;  //array de t_atoms utilizado para armazenar amps (outlet 3, tendo "amps" como rótulo)
  t_atom    *x_listedamp; //array de t_atoms utilizado para armazenar damps (outlet 3, tendo "damps" como rótulo)

  //outlets 1, 2 e 3
  t_outlet  *list_freqs_outlet;
  t_outlet  *list_fad_outlet;
  t_outlet  *any_outlet;

} t_tresf;


//FUNÇÕES AUXILIARES
static t_float mapping(t_float f)
{
  if (f == 0 ) {
    return 0;
  } else if (fmod(f,2) == 0) {
    return (-f * 0.5);
  } else {
    return (f + 1) * 0.5; // last else case
  };
}


// static t_float tmtof(t_float f)
// {
//   if (f <= -1500) {
//     return(0);
//   }	else if (f > 1499) {
//     return(tmtof(1499));
//   }	else {
//     return (8.17579891564 * exp(.0577622650 * f));
//   }
// }
//
// static t_float tftom(t_float f)
// {
//   if (f > 0){
//     return 17.3123405046 * log(.12231220585 * f);
//   } else {
//     return -1500;
//   }
// }

static void tresf_ampjitter(t_tresf *x, t_float f) { // amplitude jitter in (no-jitetr)0 - 50 (50db jitter)
  if (f < 0) {
    x->x_ampjitter = 0.;
  } else if (f > 50.) {
    x->x_ampjitter = 50.;
  } else {
    x->x_ampjitter  = f;
  };
}

static void tresf_seed(t_tresf *x, t_floatarg f){
  int i = (int)f;
  if (i < 0)
  i = 1;
  tresf_seed(&x->x_seed, (unsigned int)i);
}

// static t_float tresf_frand(t_tresf *x){
//   return ((t_float)rand_int(&x->x_seed, MAXRAND) / (t_float)DENRAND);
// }

static double doublerand(void){
  return (double)rand()/RAND_MAX;
  //return ((double)rand_int(&x->x_seed,(unsigned int)pow(2,sizeof(double)*8)) / (double)pow(2,20));
}

static void tresf_list(t_tresf *x, t_symbol *s, int argc, t_atom *argv){
  // t_atom *atombuf = (t_atom *)getbytes(sizeof(t_atom) * ac);
  int li;
  int ai;
  int fi;
  int k,l,m,counter=0,freq_counter=0;
  // int max_freqs=16; // commented because declared in the instance variable
  t_int idx_freq=0;
  float p, damping, amp, freq_cl=0;
  t_float thisval;
  t_float amp_max=0, delta_amp=0;


  // float drand;

  // //utilizar em sequencia para mudar seed!!
  // tresf_seed(x, rand());
  // drand = (float)doublerand();


  if (argc == 3){
    for(li = 0; li < 3; ++li){

      thisval = argv->a_w.w_float;
      x->x_afin[li] = thisval;
      //x->x_ain[li].a_type = A_FLOAT;
      //x->x_ain[li].a_w.w_float = thisval;

      // post("afin: %0.3f", thisval);
      // post("teste: %0.9f", drand);
      // outlet_float(x->any_outlet, drand);

      argv++;
    }
  }



  //outlet_list(x->list_freqs_outlet, &s_list, 3, x->x_afin); ///CUIDADO COM ARGUMENTO 3! PRECISA BATER COM O TAMANHO DO ARRAY!

  for ( k = 0; k < 20; ++k)
  {	//post("k"+String.valueOf(k));
  for ( l = 0; l < k+1; ++l)
  {	//post("l"+String.valueOf(l));
  for ( m = 0; m < l+1; ++m)
  { //post("m"+String.valueOf(m));
  // tresf_seed(x, rand());

  tresf_seed(x, rand());
  p = (float)doublerand();
  counter++;
  // post("p: %0.9f", p);


  //Duas condições:
  //p < x->x_proba : só deixará passar um valor de p menor que aquele estabelecido (valores altos "mais permissivos")
  //freq_counter < x->x_maxfreqs : só irá alocar novos números até o limite de freqs estabelecido

  if ((p < x->x_proba) && (freq_counter < x->x_maxfreqs)) { // on continue tant qu'on n'a pas atteint le nombre de frequences

  //soma frequencias
  freq_cl = x->x_afin[0]*mapping(m) + x->x_afin[1]*mapping(l-m) + x->x_afin[2]*mapping(k-l);

  if (freq_cl > 20) { // limit is 20Hz
    tresf_seed(x, rand());
    damping = (99. - (freq_cl * 0.005) * (float)doublerand());
    // init amplitude expr 90-$f1/1000
    amp =  (90. - freq_cl * 0.001); // default is -1dB each 1Khz
    if (x->x_ampjitter > 0) {
      tresf_seed(x, rand());
      amp -= x->x_ampjitter * (t_float)doublerand();
    }



    x->x_freqs[idx_freq].a_type = A_FLOAT;
    x->x_freqs[idx_freq].a_w.w_float = freq_cl;
    idx_freq++;
    x->x_freqs[idx_freq].a_type = A_FLOAT;
    x->x_freqs[idx_freq].a_w.w_float = amp;
    idx_freq++;
    x->x_freqs[idx_freq].a_type = A_FLOAT;
    x->x_freqs[idx_freq].a_w.w_float = damping;
    idx_freq++;

    x->x_listefreq[freq_counter].a_type = A_FLOAT;
    x->x_listefreq[freq_counter].a_w.w_float = freq_cl;

    x->x_listeamp[freq_counter].a_type = A_FLOAT;
    x->x_listeamp[freq_counter].a_w.w_float = amp;

    x->x_listedamp[freq_counter].a_type = A_FLOAT;
    x->x_listedamp[freq_counter].a_w.w_float = damping;


    // VERSAO ANTERIOR UTILIZAVA ARRAYS DE FLOAT...
    // x->x_listefreq[freq_counter] = freq_cl;
    // x->x_listeamp[freq_counter] = amp;
    // x->x_listedamp[freq_counter] = damping;

    freq_counter++;
    // x->x_f_a_d[0].a_type = A_FLOAT;
    // x->x_f_a_d[0].a_w.w_float = freq_cl;
    //
    // x->x_f_a_d[1].a_type = A_FLOAT;
    // x->x_f_a_d[1].a_w.w_float = amp;
    //
    // x->x_f_a_d[2].a_type = A_FLOAT;
    // x->x_f_a_d[2].a_w.w_float = damping;


  }
}
}
}
}

for (ai=0; ai < x->x_maxfreqs; ++ai){ // iteration sur toutes les frequences
  amp_max = (x->x_listeamp[ai].a_w.w_float > amp_max ? x->x_listeamp[ai].a_w.w_float : amp_max );
}

delta_amp = 90. - amp_max; // amp max par rapport � 90 qui est le max

for (fi=0; fi < x->x_maxfreqs; ++fi){
  x->x_f_a_d[0].a_type = A_FLOAT;
  x->x_f_a_d[1].a_type = A_FLOAT;
  x->x_f_a_d[2].a_type = A_FLOAT;

  x->x_listeamp[fi].a_w.w_float += delta_amp ;

  x->x_f_a_d[0].a_w.w_float = x->x_listefreq[fi].a_w.w_float;
  x->x_f_a_d[1].a_w.w_float = x->x_listeamp[fi].a_w.w_float;
  x->x_f_a_d[2].a_w.w_float = x->x_listedamp[fi].a_w.w_float;

  //outlet 2: f_a_d triads
  outlet_list(x->list_fad_outlet, &s_list, 3, x->x_f_a_d);
}
  //outlet 1: dump of f_a_d triads
  outlet_list(x->list_freqs_outlet, &s_list, 3 * x->x_maxfreqs, x->x_freqs);

  //outlet_anything(x->x_nsend_outlet,gensym("connect"),2,connect_values);

  outlet_anything(x->any_outlet, gensym("freqs"), x->x_maxfreqs, x->x_listefreq);
  outlet_anything(x->any_outlet, gensym("amps"), x->x_maxfreqs, x->x_listeamp);
  outlet_anything(x->any_outlet, gensym("damps"), x->x_maxfreqs, x->x_listedamp);

}




void tresf_float(t_tresf *x, t_floatarg f)//método
{
  t_float proba;
  // int i;

  if (f < 0.03) {
    proba = 0.03;
  } else if (f > 1.0) {
    proba = 1.0;
  } else {
    proba = f;
  };

  x->x_proba = proba;

  // x->x_fvec[0].a_type = A_FLOAT;
  // x->x_fvec[0].a_w.w_float = proba; //a_w :
  //
  // x->x_fvec[1].a_type = A_FLOAT;
  // //x->x_fvec[1].a_w.w_float = (float)rand_int(&x->x_seed, pow(2,sizeof(float)*7 + 1));// / (float)sizeof(float);
  // x->x_fvec[1].a_w.w_float = tresf_frand(x);
  //
  // x->x_fvec[2].a_type = A_FLOAT;
  // x->x_fvec[2].a_w.w_float = (t_float)doublerand();
  //
  // x->x_fvec[3].a_type = A_FLOAT;
  // x->x_fvec[3].a_w.w_float = (t_float)sizeof(t_float);
  //
  //
  // x->x_fvec[4].a_type = A_FLOAT;
  // x->x_fvec[4].a_w.w_float = (t_float)sizeof(double);
  //
  // x->x_fvec[5].a_type = A_FLOAT;
  // x->x_fvec[5].a_w.w_float = (t_float)sizeof(t_atom);
  //
  //
  // outlet_list(x->list_freqs_outlet, &s_list, x->x_n, x->x_fvec);
}


static void max_freqs(t_tresf *x, int mf)
{ // set max_freqs limit
  t_int  maxf;
  if (mf < 1) {
    maxf = 1;
  } else if (mf > 32) {
    maxf = 32;
  } else {
    maxf = mf;
  };
  x->x_maxfreqs = (int)maxf;

  // x->x_fvec = (t_atom *)getbytes(x->x_n * sizeof(t_atom));
  // x->x_freqout = (t_atom *)getbytes(x->x_n * sizeof(t_atom));

  x->x_afin = (t_float *)getbytes(3 * sizeof(t_float));

  //ATOMS!
  x->x_freqs = (t_atom *)getbytes(3 * x->x_maxfreqs * sizeof(t_atom));
  x->x_f_a_d = (t_atom *)getbytes(3 * sizeof(t_atom));
  x->x_listefreq = (t_atom *)getbytes(x->x_maxfreqs * sizeof(t_atom));
  //x->x_listemfreq = (t_atom *)getbytes(x->x_maxfreqs * sizeof(t_atom));
  x->x_listeamp = (t_atom *)getbytes(x->x_maxfreqs * sizeof(t_atom));
  x->x_listedamp = (t_atom *)getbytes(x->x_maxfreqs * sizeof(t_atom));

}

static void tresf_free(t_tresf *x)//declara um argumento
{

  freebytes(x->x_afin, 3 * sizeof(t_float));
  freebytes(x->x_freqs, 3 * x->x_maxfreqs * sizeof(t_atom));
  freebytes(x->x_f_a_d, 3 * sizeof(t_atom));
  freebytes(x->x_listefreq, x->x_maxfreqs * sizeof(t_atom));
  //freebytes(x->x_listemfreq, x->x_maxfreqs * sizeof(t_atom));
  freebytes(x->x_listeamp, x->x_maxfreqs * sizeof(t_atom));
  freebytes(x->x_listedamp, x->x_maxfreqs * sizeof(t_atom));
}

//constructor
static void *tresf_new(void)
{
  t_tresf *x = (t_tresf *)pd_new(tresf_class);

  max_freqs(x,16); //bytes são alocados aqui

  srand((unsigned int)sys_getrealtime() * 1000000.);
  tresf_seed(x, rand());

  x->x_proba = 0.5;
  x->x_ampjitter = 0.;
  x->list_freqs_outlet=outlet_new(&x->x_obj, &s_list);
  x->list_fad_outlet=outlet_new(&x->x_obj, &s_list);
  x->any_outlet=outlet_new(&x->x_obj, &s_anything);

  // outlet_new(&x->x_obj, &s_list);
  // outlet_new(&x->x_obj, &s_anything);
  return (void *)x;
}

void tresf_setup(void) {
  tresf_class = class_new(gensym("tresf"),
  (t_newmethod)tresf_new,
  (t_method)tresf_free, sizeof(t_tresf),
  CLASS_DEFAULT,
  A_DEFFLOAT, 0);

  class_addlist(tresf_class, (t_method)tresf_list);
  class_addmethod(tresf_class, (t_method)tresf_seed, gensym("seed"), A_FLOAT, 0);
  class_addmethod(tresf_class, (t_method)tresf_ampjitter, gensym("ampjitter"), A_FLOAT, 0);
  class_addfloat(tresf_class, (t_method)tresf_float);
}
