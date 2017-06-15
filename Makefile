
#===============================================================================
# Makefile generated with fdlib.sh for pd lib builder
# Wed Jun 14 21:48:33 EDT 2017
# Author: Fede Camara Halac
# Email: fch226@nyu.edu
#===============================================================================

lib.name = fd_lib

class.sources = ./back/back.c ./cantor/cantor.c ./clifford/clifford.c ./connect/connect.c ./counter/counter.c ./crand/crand.c ./cuadratic/cuadratic.c ./factor/factor.c ./gausstwister/gausstwister.c ./grieta/grieta.c ./halton/halton.c ./henon/henon.c ./henon2/henon2.c ./lor8~/lor.c ./lor8~/lor2~.c ./lor8~/lor3~.c ./lor8~/lor4~.c ./lor8~/lor5~.c ./lor8~/lor6~.c ./lor8~/lor7~.c ./lor8~/lor8~.c ./lor8~/lor~.c ./lorenz/lorenz.c ./lorenz/lorenzc.c ./lorenz/lorenz~.c ./lorsig~/lorsig~.c ./mainpath/mainpath.c ./mainscore/mainscore.c ./mandelbrot/mandelbrot.c ./minimax/minimax.c ./mtwister/mtwister.c ./oil/oil.c ./parabola/parabola.c ./PdCon16~/PdCon16~.c ./pmtwister/pmtwister.c ./pointerlist/pointerlist.c ./prandom/prandom.c ./randy/randy.c ./reflect/reflect.c ./scroll/scroll.c ./siginfo/siginfo.c ./siglor~/siglor.c ./siglor~/siglor~.c ./spline/spline.c ./sradio/sradio.c ./tbessel/tbessel.c ./txt2pd/ruinas.c ./txt2pd/txt2pd.c

datafiles = ./!msg/!msg-help.pd ./back/back-help.pd ./back/back-object-help.pd ./cantor/cantor-help.pd ./clifford/clifford-help.pd ./connect/connect-help.pd ./counter/counter-help.pd ./crand/crand-help.pd ./cuadratic/cuadratic-help.pd ./factor/factor-help.pd ./fd_2bit/fd_2bit-help.pd ./fd_6x6/fd_6x6-help.pd ./fd_ain/fd_ain-help.pd ./fd_arcsin/fd_arcsin-help.pd ./fd_arcsin~/fd_arcsin~-help.pd ./fd_arg1/fd_arg1-help.pd ./fd_argc/fd_argc-help.pd ./fd_argv/fd_argv-help.pd ./fd_arrayautotrim/fd_arrayautotrim-help.pd ./fd_arraymorph/fd_arraymorph-help.pd ./fd_arraysort1/fd_arraysort1-help.pd ./fd_arraysort2/fd_arraysort2-help.pd ./fd_arraytrim/fd_arraytrim-help.pd ./fd_autopatch/fd_autopatch-help.pd ./fd_autopoly/fd_autopoly-help.pd ./fd_bang/fd_bang-help.pd ./fd_bark/fd_bark-help.pd ./fd_bark~/fd_bark~-help.pd ./fd_bigmyvu/fd_bigmyvu-help.pd ./fd_blackman/fd_blackman-help.pd ./fd_blackman~/fd_blackman~-help.pd ./fd_calm/fd_calm-help.pd ./fd_camara/fd_camara-help.pd ./fd_cauchy/fd_cauchy-help.pd ./fd_cauchy~/fd_cauchy~-help.pd ./fd_ceil/fd_ceil-help.pd ./fd_circle/fd_circle-help.pd ./fd_clarg/fd_clarg-help.pd ./fd_clicktrack/fd_clicktrack-help.pd ./fd_colorpicker/fd_colorpicker-help.pd ./fd_colortheme/fd_colortheme-help.pd ./fd_cosline~/fd_cosline~-help.pd ./fd_countdown/fd_countdown-help.pd ./fd_cputime/fd_cputime-help.pd ./fd_critical/fd_critical-help.pd ./fd_cuacua/fd_cuacua-help.pd ./fd_cubic/fd_cubic-help.pd ./fd_dacout/fd_dacout-help.pd ./fd_dataDroplet/fd_dataDroplet-help.pd ./fd_dataExp/fd_dataExp-help.pd ./fd_dataGui/fd_dataGui-help.pd ./fd_dataHann/fd_dataHann-help.pd ./fd_dataID/fd_dataID-help.pd ./fd_dataLine/fd_dataLine-help.pd ./fd_dataMorph/fd_dataMorph-help.pd ./fd_dataMorphArray/fd_dataMorphArray-help.pd ./fd_dataMorphGui/fd_dataMorphGui-help.pd ./fd_dataMorphWidth/fd_dataMorphWidth-help.pd ./fd_dataRand/fd_dataRand-help.pd ./fd_dataRead/fd_dataRead-help.pd ./fd_dataRead1/fd_dataRead1-help.pd ./fd_dataRead3/fd_dataRead3-help.pd ./fd_dataStoreGui/fd_dataStoreGui-help.pd ./fd_dataVis/fd_dataVis-help.pd ./fd_dataWalk/fd_dataWalk-help.pd ./fd_delit/fd_delit-help.pd ./fd_distributions/fd_distributions-help.pd ./fd_dory/fd_dory-help.pd ./fd_draw/fd_draw-help.pd ./fd_dsp/fd_dsp-help.pd ./fd_dspon/fd_dspon-help.pd ./fd_dummy/fd_dummy-help.pd ./fd_env/fd_env-help.pd ./fd_etareti/fd_etareti-help.pd ./fd_exp/fd_exp-help.pd ./fd_expo~/fd_expo~-help.pd ./fd_factor/fd_factor-help.pd ./fd_fftbark~/fd_fftbark~-help.pd ./fd_fftbark~-graph/fd_fftbark~-graph-help.pd ./fd_fftfilt/fd_fftfilt-help.pd ./fd_filewindow/fd_filewindow-help.pd ./fd_flip/fd_flip-help.pd ./fd_floor/fd_floor-help.pd ./fd_fm/fd_fm-help.pd ./fd_fmosc/fd_fmosc-help.pd ./fd_fromsig/fd_fromsig-help.pd ./fd_g-reader/fd_g-reader-help.pd ./fd_gauss/fd_gauss-help.pd ./fd_gauss-t/fd_gauss-t-help.pd ./fd_gauss-test/fd_gauss-test-help.pd ./fd_gaussian-distribution/fd_gaussian-distribution-help.pd ./fd_gauss~/fd_gauss~-help.pd ./fd_GEMbuffsize/fd_GEMbuffsize-help.pd ./fd_gemcontext/fd_gemcontext-help.pd ./fd_gemset/fd_gemset-help.pd ./fd_gemsphere/fd_gemsphere-help.pd ./fd_geometric-mean/fd_geometric-mean-help.pd ./fd_get/fd_get-help.pd ./fd_get-element/fd_get-element-help.pd ./fd_getfiles/fd_getfiles-help.pd ./fd_getid/fd_getid-help.pd ./fd_getsize/fd_getsize-help.pd ./fd_gr/fd_gr-help.pd ./fd_grain/fd_grain-help.pd ./fd_gt/fd_gt-help.pd ./fd_hann0/fd_hann0-help.pd ./fd_hann~/fd_hann~-help.pd ./fd_hopmsec/fd_hopmsec-help.pd ./fd_hradio/fd_hradio-help.pd ./fd_impulse/fd_impulse-help.pd ./fd_input/fd_input-help.pd ./fd_interp~/fd_interp~-help.pd ./fd_iterate/fd_iterate-help.pd ./fd_itpoint/fd_itpoint-help.pd ./fd_key/fd_key-help.pd ./fd_keycomb/fd_keycomb-help.pd ./fd_kinetica/fd_kinetica-help.pd ./fd_list-pointer/fd_list-pointer-help.pd ./fd_listout/fd_listout-help.pd ./fd_log/fd_log-help.pd ./fd_logistic/fd_logistic-help.pd ./fd_logistic~/fd_logistic~-help.pd ./fd_logo/fd_logo-help.pd ./fd_lorenz/fd_lorenz-help.pd ./fd_lorsig/fd_lorsig-help.pd ./fd_lspace/fd_lspace-help.pd ./fd_m-comb/fd_m-comb-help.pd ./fd_markfour/fd_markfour-help.pd ./fd_markov/fd_markov-help.pd ./fd_markov-5/fd_markov-5-help.pd ./fd_markov-5-help-2/fd_markov-5-help-2-help.pd ./fd_markov-test/fd_markov-test-help.pd ./fd_markov3/fd_markov3-help.pd ./fd_matrix-comb/fd_matrix-comb-help.pd ./fd_maudio/fd_maudio-help.pd ./fd_maudio-assign/fd_maudio-assign-help.pd ./fd_maudio-receive/fd_maudio-receive-help.pd ./fd_metro/fd_metro-help.pd ./fd_metronomo/fd_metronomo-help.pd ./fd_midinote/fd_midinote-help.pd ./fd_midiread/fd_midiread-help.pd ./fd_miller-gauss/fd_miller-gauss-help.pd ./fd_milliseconds-tempo/fd_milliseconds-tempo-help.pd ./fd_minimax/fd_minimax-help.pd ./fd_msecparse/fd_msecparse-help.pd ./fd_mtxdelit/fd_mtxdelit-help.pd ./fd_mtxout/fd_mtxout-help.pd ./fd_mult/fd_mult-help.pd ./fd_n!/fd_n!-help.pd ./fd_n=n-1/fd_n=n-1-help.pd ./fd_netreceive/fd_netreceive-help.pd ./fd_netsend/fd_netsend-help.pd ./fd_next/fd_next-help.pd ./fd_nonpulmonic/fd_nonpulmonic-help.pd ./fd_norep/fd_norep-help.pd ./fd_norm/fd_norm-help.pd ./fd_norm-abs/fd_norm-abs-help.pd ./fd_not/fd_not-help.pd ./fd_nouns/fd_nouns-help.pd ./fd_nountxt/fd_nountxt-help.pd ./fd_numkey/fd_numkey-help.pd ./fd_objcreator/fd_objcreator-help.pd ./fd_octaver/fd_octaver-help.pd ./fd_once/fd_once-help.pd ./fd_osc/fd_osc-help.pd ./fd_oscil/fd_oscil-help.pd ./fd_out/fd_out-help.pd ./fd_own/fd_own-help.pd ./fd_oxy/fd_oxy-help.pd ./fd_oxy-ctlin/fd_oxy-ctlin-help.pd ./fd_oxy-fad/fd_oxy-fad-help.pd ./fd_oxy-toggle/fd_oxy-toggle-help.pd ./fd_oxygen-49/fd_oxygen-49-help.pd ./fd_pan/fd_pan-help.pd ./fd_parsenum/fd_parsenum-help.pd ./fd_phasor16~/fd_phasor16~-help.pd ./fd_phatty/fd_phatty-help.pd ./fd_phrase/fd_phrase-help.pd ./fd_pixrecord/fd_pixrecord-help.pd ./fd_plot/fd_plot-help.pd ./fd_pno/fd_pno-help.pd ./fd_pno-12/fd_pno-12-help.pd ./fd_pno-24/fd_pno-24-help.pd ./fd_poisson/fd_poisson-help.pd ./fd_poisson~/fd_poisson~-help.pd ./fd_polea/fd_polea-help.pd ./fd_preset/fd_preset-help.pd ./fd_printer/fd_printer-help.pd ./fd_pshift/fd_pshift-help.pd ./fd_r/fd_r-help.pd ./fd_random-bar-distribution/fd_random-bar-distribution-help.pd ./fd_range/fd_range-help.pd ./fd_range-copy2/fd_range-help.pd ./fd_rec/fd_rec-help.pd ./fd_rec1/fd_rec1-help.pd ./fd_receive/fd_receive-help.pd ./fd_receive-b/fd_receive-b-help.pd ./fd_reflect/fd_reflect-help.pd ./fd_relocate/fd_relocate-help.pd ./fd_rest/fd_rest-help.pd ./fd_rexpo~/fd_rexpo~-help.pd ./fd_rgb/fd_rgb-help.pd ./fd_rgbcol/fd_rgbcol-help.pd ./fd_round/fd_round-help.pd ./fd_rsf-out/fd_rsf-out-help.pd ./fd_rspline/fd_rspline-help.pd ./fd_s-grain-envelope/fd_s-grain-envelope-help.pd ./fd_s-prob/fd_s-prob-help.pd ./fd_s-prob-5/fd_s-prob-5-help.pd ./fd_salvo/fd_salvo-help.pd ./fd_scales/fd_scales-help.pd ./fd_send/fd_send-help.pd ./fd_send-b/fd_send-b-help.pd ./fd_set-element/fd_set-element-help.pd ./fd_sgrain/fd_sgrain-help.pd ./fd_shader/fd_shader-help.pd ./fd_shift/fd_shift-help.pd ./fd_shuffle/fd_shuffle-help.pd ./fd_siginfo/fd_siginfo-help.pd ./fd_sin/fd_sin-help.pd ./fd_sinc/fd_sinc-help.pd ./fd_sinc~/fd_sinc~-help.pd ./fd_sinh/fd_sinh-help.pd ./fd_sinline/fd_sinline-help.pd ./fd_sinline~/fd_sinline~-help.pd ./fd_sp/fd_sp-help.pd ./fd_spat/fd_spat-help.pd ./fd_spigot/fd_spigot-help.pd ./fd_spigot3/fd_spigot3-help.pd ./fd_spigot5/fd_spigot5-help.pd ./fd_spigot6/fd_spigot6-help.pd ./fd_spline~/fd_spline~-help.pd ./fd_splittone/fd_splittone-help.pd ./fd_step/fd_step-help.pd ./fd_stepper/fd_stepper-help.pd ./fd_stopwatch/fd_stopwatch-help.pd ./fd_stp/fd_stp-help.pd ./fd_stream-onoff/fd_stream-onoff-help.pd ./fd_sum/fd_sum-help.pd ./fd_swap/fd_swap-help.pd ./fd_switch/fd_switch-help.pd ./fd_t-read/fd_t-read-help.pd ./fd_tab/fd_tab-help.pd ./fd_tabout/fd_tabout-help.pd ./fd_tabswitch/fd_tabswitch-help.pd ./fd_test-12ch/fd_test-12ch-help.pd ./fd_test-buffer-limit/fd_test-buffer-limit-help.pd ./fd_test-maudio-video/fd_test-maudio-video-help.pd ./fd_test-plott/fd_test-plott-help.pd ./fd_test-plott2/fd_test-plott2-help.pd ./fd_test-plott3/fd_test-plott3-help.pd ./fd_test-plott4/fd_test-plott4-help.pd ./fd_test-tone/fd_test-tone-help.pd ./fd_test8/fd_test8-help.pd ./fd_text-read/fd_text-read-help.pd ./fd_textsort2/fd_textsort2-help.pd ./fd_tglvis/fd_tglvis-help.pd ./fd_title/fd_title-help.pd ./fd_toroman/fd_toroman-help.pd ./fd_tovowel/fd_tovowel-help.pd ./fd_tracktopol/fd_tracktopol-help.pd ./fd_trapezoid~/fd_trapezoid~-help.pd ./fd_traverse/fd_traverse-help.pd ./fd_trb-ranges/fd_trb-ranges-help.pd ./fd_tri/fd_tri-help.pd ./fd_triangle~/fd_triangle~-help.pd ./fd_uexpo/fd_uexpo-help.pd ./fd_untext/fd_untext-help.pd ./fd_updown/fd_updown-help.pd ./fd_urn/fd_urn-help.pd ./fd_vibrato/fd_vibrato-help.pd ./fd_vidfft/fd_vidfft-help.pd ./fd_visera/fd_visera-help.pd ./fd_vu/fd_vu-help.pd ./fd_weightor-test/fd_weightor-test-help.pd ./fd_weinlevy2-test/fd_weinlevy2-test-help.pd ./fd_weinlevy2-test-audio/fd_weinlevy2-test-audio-help.pd ./fd_welch/fd_welch-help.pd ./fd_wetdry/fd_wetdry-help.pd ./fd_wetdry~/fd_wetdry~-help.pd ./fd_wielevy/fd_wielevy-help.pd ./fd_wielevy-test/fd_wielevy-test-help.pd ./fd_wielevy2/fd_wielevy2-help.pd ./fd_wind/fd_wind-help.pd ./fd_windows/fd_windows-help.pd ./fd_write/fd_write-help.pd ./fd_write1/fd_write1-help.pd ./fd_write2/fd_write2-help.pd ./fd_write5/fd_write5-help.pd ./fd_z12/fd_z12-help.pd ./fd_zero-crossing~/fd_zero-crossing~-help.pd ./fd_zero-crossing~-help2/fd_zero-crossing~-help2-help.pd ./gausstwister/gausstwister-help.pd ./grieta/grieta-help.pd ./halton/halton-help.pd ./henon/henon-help.pd ./henon2/henon2-help.pd ./lor8~/lor2~-help.pd ./lor8~/lor3~-help.pd ./lor8~/lor4~-help.pd ./lor8~/lor5~-help.pd ./lor8~/lor6~-help.pd ./lor8~/lor7~-help.pd ./lor8~/lor8~-help.pd ./lor8~/lor~-help.pd ./lorenz/lorsig-help.pd ./lorsig~/lorsig~-help.pd ./mainpath/mainpath-help.pd ./mandelbrot/mandelbrot-help.pd ./minimax/minimax-help.pd ./mtwister/mtwister-help.pd ./oil/oil-help.pd ./parabola/parabola-help.pd ./PdCon16~/PdCon16~-help.pd ./pmtwister/pmtwister-help.pd ./pointerlist/pointerlist-help.pd ./prandom/prandom-help.pd ./reflect/reflect-help.pd ./scroll/scroll-help.pd ./siginfo/siginfo-help.pd ./siglor~/siglor~-help.pd ./spline/spline-help.pd ./sradio/sradio-help.pd ./txt2pd/txt2pd-help.pd ./clifford/clifford-test.pd ./counter/counter-test.pd ./cuadratic/cuadratic-test.pd ./fd_gauss-test/fd_gauss-test.pd ./fd_hradio/fd_hradio-test.pd ./fd_impulse/fd_impulse-test.pd ./fd_markov-test/fd_markov-test.pd ./fd_plot/fd_plot-test.pd ./fd_weightor-test/fd_weightor-test.pd ./fd_weinlevy2-test/fd_weinlevy2-test.pd ./fd_wielevy-test/fd_wielevy-test.pd ./henon/henon-test.pd ./lorenz/lorenz-test.pd ./lorenz/lorenzc-test.pd ./minimax/minimax-test.pd ./reflect/reflect-test.pd ./sradio/sradio-test.pd 

include pd-lib-builder/Makefile.pdlibbuilder

#===============================================================================

