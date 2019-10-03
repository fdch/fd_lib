# Makefile for fd_lib - Wed Oct  2 20:18:51 EDT 2019
lib.name=fd_lib

# specify the location of main header file
cflags = -Iinclude
lib.setup.sources = src/$(lib.name).c

class.sources=src/cantor.c src/clifford.c src/colormap.c src/combi.c src/connect.c src/counter.c src/crand.c src/cuadratic.c src/delit.c src/factor.c src/fdm.c src/frand.c src/glistinfo.c src/halton.c src/henon.c src/irlog.c src/irpow.c src/irsqrt.c src/iterate.c src/lorenz.c src/lornorm.c src/lorsig~.c src/lor~.c src/mainpath.c src/mandelbrot.c src/minimax.c src/mtwister.c src/parabola.c src/pmtwister.c src/prandom.c src/randy.c src/reflect.c src/root.c src/scroll.c src/siginfo.c src/sradio.c src/tracks.c 

common.sources=src/fdLib.c

datafiles=help/!msg-help.pd help/__-help.pd help/_bass-help.pd help/_distor-help.pd help/_legato-help.pd help/_mbass-help.pd help/_note-help.pd help/_overview-help.pd help/_snote-help.pd help/_stick-help.pd help/_tom-help.pd help/_wood-help.pd help/cantor-help.pd help/clifford-help.pd help/colormap-help.pd help/combi-help.pd help/connect-help.pd help/counter-help.pd help/crand-help.pd help/cuadratic-help.pd help/delit-help.pd help/factor-help.pd help/fd_2bit-help.pd help/fd_6x6-help.pd help/fd_GEMbuffsize-help.pd help/fd_any2list-help.pd help/fd_arcsin-help.pd help/fd_arcsin~-help.pd help/fd_arg1-help.pd help/fd_argc-help.pd help/fd_argv-help.pd help/fd_array2pix-help.pd help/fd_arrayautotrim-help.pd help/fd_arrayclone-help.pd help/fd_arraycopy-help.pd help/fd_arraymorph-help.pd help/fd_arraysort1-help.pd help/fd_arraysort2-help.pd help/fd_autopatch-help.pd help/fd_autopoly-help.pd help/fd_background-help.pd help/fd_bang-help.pd help/fd_bangem-help.pd help/fd_bark-help.pd help/fd_bark~-help.pd help/fd_bass-help.pd help/fd_bigmyvu-help.pd help/fd_blackman-help.pd help/fd_blackman~-help.pd help/fd_blob-help.pd help/fd_blobtracks-help.pd help/fd_blobvid-help.pd help/fd_bufvid-help.pd help/fd_butterworth-help.pd help/fd_calm-help.pd help/fd_camara-help.pd help/fd_cauchy-help.pd help/fd_cauchy~-help.pd help/fd_ceil-help.pd help/fd_circle-help.pd help/fd_clarg-help.pd help/fd_clicktrack-help.pd help/fd_colorpicker-help.pd help/fd_colortheme-help.pd help/fd_corr-help.pd help/fd_cosline~-help.pd help/fd_countdown-help.pd help/fd_cputime-help.pd help/fd_critical-help.pd help/fd_cuacua-help.pd help/fd_cubic-help.pd help/fd_dacout-help.pd help/fd_dacout_rec-help.pd help/fd_dactest-help.pd help/fd_data-help.pd help/fd_dataDroplet-help.pd help/fd_dataExp-help.pd help/fd_dataGui-help.pd help/fd_dataHann-help.pd help/fd_dataID-help.pd help/fd_dataLine-help.pd help/fd_dataMorph-help.pd help/fd_dataMorphArray-help.pd help/fd_dataMorphGui-help.pd help/fd_dataMorphWidth-help.pd help/fd_dataRand-help.pd help/fd_dataRead-help.pd help/fd_dataRead1-help.pd help/fd_dataRead3-help.pd help/fd_dataStoreGui-help.pd help/fd_dataVis-help.pd help/fd_dataWalk-help.pd help/fd_delit-help.pd help/fd_dels-help.pd help/fd_distor-help.pd help/fd_distributions-help.pd help/fd_dory-help.pd help/fd_dsp-bang-help.pd help/fd_dsp-help.pd help/fd_dspon-help.pd help/fd_emap2-help.pd help/fd_env-help.pd help/fd_etareti-help.pd help/fd_exp-help.pd help/fd_expo~-help.pd help/fd_fftfilt-help.pd help/fd_filt-help.pd help/fd_fk-help.pd help/fd_fletcher-munson-help.pd help/fd_floor-help.pd help/fd_fm-help.pd help/fd_fmosc-help.pd help/fd_fromsig-help.pd help/fd_gate-help.pd help/fd_gauss-help.pd help/fd_gauss~-help.pd help/fd_gem2feat-help.pd help/fd_gembuf-help.pd help/fd_gemloop-help.pd help/fd_gemsphere-help.pd help/fd_geomean-help.pd help/fd_get-element-help.pd help/fd_getcomb-help.pd help/fd_getid-help.pd help/fd_getsize-help.pd help/fd_gr-help.pd help/fd_grain-help.pd help/fd_guiDelta-help.pd help/fd_guiFont-help.pd help/fd_guiTemplate-help.pd help/fd_hann~-help.pd help/fd_hihat-help.pd help/fd_hopmsec-help.pd help/fd_hradio-help.pd help/fd_img2feat-help.pd help/fd_impulse-help.pd help/fd_input-help.pd help/fd_inputGui-help.pd help/fd_iterate-help.pd help/fd_itpoint-help.pd help/fd_jumpv-help.pd help/fd_karplus-help.pd help/fd_key-help.pd help/fd_keycomb-help.pd help/fd_kinetica-help.pd help/fd_kp-help.pd help/fd_legato-help.pd help/fd_lines-help.pd help/fd_list-pointer-help.pd help/fd_listout-help.pd help/fd_log-help.pd help/fd_logistic-help.pd help/fd_logistic~-help.pd help/fd_logo-help.pd help/fd_loop-help.pd help/fd_lorenz-expr-help.pd help/fd_lorenz-help.pd help/fd_lorosc-help.pd help/fd_lorsig-help.pd help/fd_lspace-help.pd help/fd_m-comb-help.pd help/fd_ma-help.pd help/fd_map-help.pd help/fd_map2-help.pd help/fd_mark-help.pd help/fd_markfour-help.pd help/fd_markov-help.pd help/fd_markov3-help.pd help/fd_markov5-help.pd help/fd_matrix-comb-help.pd help/fd_mbass-help.pd help/fd_metro-help.pd help/fd_midiread-help.pd help/fd_miller-gauss-help.pd help/fd_minimax-help.pd help/fd_msecparse-help.pd help/fd_mtxdelit-help.pd help/fd_mtxout-help.pd help/fd_mult-help.pd help/fd_n!-help.pd help/fd_n=n-1-help.pd help/fd_navigator-help.pd help/fd_netreceive-help.pd help/fd_netsend-help.pd help/fd_next-help.pd help/fd_noise~-help.pd help/fd_nonpulmonic-help.pd help/fd_norep-help.pd help/fd_norm-abs-help.pd help/fd_norm-help.pd help/fd_note-help.pd help/fd_nouns-help.pd help/fd_numkey-help.pd help/fd_objcreator-help.pd help/fd_octaver-help.pd help/fd_once-help.pd help/fd_osc-help.pd help/fd_oshift~-help.pd help/fd_out-help.pd help/fd_pan-help.pd help/fd_parsenum-help.pd help/fd_phatty-help.pd help/fd_pix2list-help.pd help/fd_pix_shift-help.pd help/fd_pixcrop-help.pd help/fd_pixrecord-help.pd help/fd_plot-help.pd help/fd_pno-help.pd help/fd_poisson-help.pd help/fd_poisson~-help.pd help/fd_printer-help.pd help/fd_pshift-help.pd help/fd_pshift~-help.pd help/fd_r-help.pd help/fd_range-help.pd help/fd_rbd-help.pd help/fd_read-resize-help.pd help/fd_rec-help.pd help/fd_receive-help.pd help/fd_reflect-help.pd help/fd_relocate-help.pd help/fd_rexpo~-help.pd help/fd_rgb-help.pd help/fd_rgbcol-help.pd help/fd_round-help.pd help/fd_rsdel~-help.pd help/fd_s-prob-5-help.pd help/fd_s-prob-help.pd help/fd_salvo-help.pd help/fd_scales-help.pd help/fd_sdel~-help.pd help/fd_seltab-help.pd help/fd_send-help.pd help/fd_set-element-help.pd help/fd_shader-help.pd help/fd_shift-help.pd help/fd_shuffle-help.pd help/fd_siginfo-help.pd help/fd_sin-help.pd help/fd_sinc-help.pd help/fd_sinc~-help.pd help/fd_sinh-help.pd help/fd_sinh~-help.pd help/fd_smooth-help.pd help/fd_smooth_brute-help.pd help/fd_snote-help.pd help/fd_sp-help.pd help/fd_spat-help.pd help/fd_spigot-help.pd help/fd_spigot3-help.pd help/fd_spline~-help.pd help/fd_step-help.pd help/fd_stick-help.pd help/fd_stopwatch-help.pd help/fd_sum-help.pd help/fd_swap-help.pd help/fd_switch-help.pd help/fd_tempo-msec-help.pd help/fd_textsort2-help.pd help/fd_tglvis-help.pd help/fd_title-help.pd help/fd_tom-help.pd help/fd_toroman-help.pd help/fd_tovowel-help.pd help/fd_tracktopol-help.pd help/fd_trapezoid~-help.pd help/fd_triangle~-help.pd help/fd_trunc2n-help.pd help/fd_untext-help.pd help/fd_updown-help.pd help/fd_vibrato-help.pd help/fd_vidbuf-help.pd help/fd_visera-help.pd help/fd_vu-help.pd help/fd_welch-help.pd help/fd_welch~-help.pd help/fd_wielevy-help.pd help/fd_wielevy2-help.pd help/fd_wincat-help.pd help/fd_winconcat-help.pd help/fd_windows-help.pd help/fd_winreceive~-help.pd help/fd_wood-help.pd help/fd_write-help.pd help/fd_write1-help.pd help/fd_write2-help.pd help/fd_write5-help.pd help/fd_xnote-help.pd help/fd_z12-help.pd help/fd_zero-crossing~-help.pd help/fdm-help.pd help/frand-help.pd help/glistinfo-help.pd help/halton-help.pd help/henon-help.pd help/irlog-help.pd help/irpow-help.pd help/irsqrt-help.pd help/iterate-help.pd help/lorenz-help.pd help/lornorm-help.pd help/lorsig~-help.pd help/lor~-help.pd help/mainpath-help.pd help/mandelbrot-help.pd help/minimax-help.pd help/mtwister-help.pd help/parabola-help.pd help/pmtwister-help.pd help/prandom-help.pd help/randy-help.pd help/reflect-help.pd help/root-help.pd help/scroll-help.pd help/siginfo-help.pd help/sradio-help.pd help/tracks-help.pd abstractions/!msg.pd abstractions/_A.pd abstractions/_alpha.pd abstractions/_butter.pd abstractions/_dummy.pd abstractions/_line.pd abstractions/_omega.pd abstractions/_poly.pd abstractions/_route.pd abstractions/_t-read.pd abstractions/_test-tone.pd abstractions/_test8.pd abstractions/buttercoef.pd abstractions/fd_2bit.pd abstractions/fd_6x6.pd abstractions/fd_GEMbuffsize.pd abstractions/fd_any2list.pd abstractions/fd_arcsin.pd abstractions/fd_arcsin~.pd abstractions/fd_arg1.pd abstractions/fd_argc.pd abstractions/fd_argv.pd abstractions/fd_array2pix.pd abstractions/fd_arrayautotrim.pd abstractions/fd_arrayclone.pd abstractions/fd_arraycopy.pd abstractions/fd_arraymorph.pd abstractions/fd_arraysort1.pd abstractions/fd_arraysort2.pd abstractions/fd_autopatch.pd abstractions/fd_autopoly.pd abstractions/fd_background.pd abstractions/fd_bang.pd abstractions/fd_bangem.pd abstractions/fd_bark.pd abstractions/fd_bark~.pd abstractions/fd_bass.pd abstractions/fd_bigmyvu.pd abstractions/fd_blackman.pd abstractions/fd_blackman~.pd abstractions/fd_blob.pd abstractions/fd_blobtracks.pd abstractions/fd_blobvid.pd abstractions/fd_bufvid.pd abstractions/fd_butterworth.pd abstractions/fd_calm.pd abstractions/fd_camara.pd abstractions/fd_cauchy.pd abstractions/fd_cauchy~.pd abstractions/fd_ceil.pd abstractions/fd_circle.pd abstractions/fd_clarg.pd abstractions/fd_clicktrack.pd abstractions/fd_colorpicker.pd abstractions/fd_colorpicker1.pd abstractions/fd_colortheme.pd abstractions/fd_corr.pd abstractions/fd_cosline~.pd abstractions/fd_countdown.pd abstractions/fd_cputime.pd abstractions/fd_critical.pd abstractions/fd_cuacua.pd abstractions/fd_cubic.pd abstractions/fd_dacout.pd abstractions/fd_dacout_rec.pd abstractions/fd_dactest.pd abstractions/fd_data.pd abstractions/fd_dataDroplet.pd abstractions/fd_dataExp.pd abstractions/fd_dataGui.pd abstractions/fd_dataHann.pd abstractions/fd_dataID.pd abstractions/fd_dataLine.pd abstractions/fd_dataMorph.pd abstractions/fd_dataMorphArray.pd abstractions/fd_dataMorphGui.pd abstractions/fd_dataMorphWidth.pd abstractions/fd_dataRand.pd abstractions/fd_dataRead.pd abstractions/fd_dataRead1.pd abstractions/fd_dataRead3.pd abstractions/fd_dataStoreGui.pd abstractions/fd_dataVis.pd abstractions/fd_dataWalk.pd abstractions/fd_delit.pd abstractions/fd_dels.pd abstractions/fd_distor.pd abstractions/fd_distributions.pd abstractions/fd_dory.pd abstractions/fd_dsp-bang.pd abstractions/fd_dsp.pd abstractions/fd_dspon.pd abstractions/fd_emap2.pd abstractions/fd_env-test.pd abstractions/fd_env.pd abstractions/fd_etareti.pd abstractions/fd_exp.pd abstractions/fd_expo~.pd abstractions/fd_fftfilt.pd abstractions/fd_filt.pd abstractions/fd_fk.pd abstractions/fd_fletcher-munson.pd abstractions/fd_floor.pd abstractions/fd_fm.pd abstractions/fd_fmosc.pd abstractions/fd_fromsig.pd abstractions/fd_gate.pd abstractions/fd_gauss-test.pd abstractions/fd_gauss.pd abstractions/fd_gauss~.pd abstractions/fd_gem2feat.pd abstractions/fd_gembuf.pd abstractions/fd_gemkarplus.pd abstractions/fd_gemloop.pd abstractions/fd_gemsphere.pd abstractions/fd_geomean.pd abstractions/fd_get-element.pd abstractions/fd_getcomb.pd abstractions/fd_getid.pd abstractions/fd_getsize.pd abstractions/fd_gr.pd abstractions/fd_grain.pd abstractions/fd_guiDelta.pd abstractions/fd_guiFont.pd abstractions/fd_guiTemplate-test.pd abstractions/fd_guiTemplate.pd abstractions/fd_hann~.pd abstractions/fd_hihat.pd abstractions/fd_hopmsec.pd abstractions/fd_hradio-status.pd abstractions/fd_hradio-test.pd abstractions/fd_hradio.pd abstractions/fd_img2feat.pd abstractions/fd_impulse.pd abstractions/fd_input.pd abstractions/fd_inputGui.pd abstractions/fd_iterate.pd abstractions/fd_itpoint.pd abstractions/fd_jumpv.pd abstractions/fd_karplus.pd abstractions/fd_key.pd abstractions/fd_keycomb.pd abstractions/fd_kinetica.pd abstractions/fd_kp.pd abstractions/fd_legato.pd abstractions/fd_lines.pd abstractions/fd_list-pointer.pd abstractions/fd_listout.pd abstractions/fd_log.pd abstractions/fd_logistic.pd abstractions/fd_logistic~.pd abstractions/fd_logo.pd abstractions/fd_loop.pd abstractions/fd_lorenz-expr.pd abstractions/fd_lorenz.pd abstractions/fd_lorosc.pd abstractions/fd_lorsig.pd abstractions/fd_lspace.pd abstractions/fd_m-comb.pd abstractions/fd_ma.pd abstractions/fd_map.pd abstractions/fd_map2.pd abstractions/fd_mark.pd abstractions/fd_markfour.pd abstractions/fd_markov.pd abstractions/fd_markov3.pd abstractions/fd_markov5.pd abstractions/fd_matrix-comb.pd abstractions/fd_mbass.pd abstractions/fd_metro.pd abstractions/fd_midiread.pd abstractions/fd_miller-gauss.pd abstractions/fd_minimax.pd abstractions/fd_msecparse.pd abstractions/fd_mtxdelit.pd abstractions/fd_mtxout.pd abstractions/fd_mult.pd abstractions/fd_n!.pd abstractions/fd_n=n-1.pd abstractions/fd_navigator.pd abstractions/fd_netreceive.pd abstractions/fd_netsend.pd abstractions/fd_next.pd abstractions/fd_noise~.pd abstractions/fd_nonpulmonic.pd abstractions/fd_norep.pd abstractions/fd_norm-abs.pd abstractions/fd_norm.pd abstractions/fd_note.pd abstractions/fd_nouns.pd abstractions/fd_numkey.pd abstractions/fd_numword.pd abstractions/fd_objcreator.pd abstractions/fd_octaver.pd abstractions/fd_once.pd abstractions/fd_osc.pd abstractions/fd_oshift~.pd abstractions/fd_out.pd abstractions/fd_pan.pd abstractions/fd_parsenum.pd abstractions/fd_phatty.pd abstractions/fd_pix2list.pd abstractions/fd_pix_shift.pd abstractions/fd_pixcrop.pd abstractions/fd_pixrecord.pd abstractions/fd_plot-test.pd abstractions/fd_plot.pd abstractions/fd_pno.pd abstractions/fd_poisson.pd abstractions/fd_poisson~.pd abstractions/fd_printer.pd abstractions/fd_prob.pd abstractions/fd_pshift.pd abstractions/fd_pshift~.pd abstractions/fd_r.pd abstractions/fd_range.pd abstractions/fd_rbd.pd abstractions/fd_read-resize.pd abstractions/fd_rec.pd abstractions/fd_receive.pd abstractions/fd_reflect.pd abstractions/fd_relocate.pd abstractions/fd_rexpo~.pd abstractions/fd_rgb.pd abstractions/fd_rgbcol.pd abstractions/fd_round.pd abstractions/fd_rsdel~.pd abstractions/fd_s-prob-5.pd abstractions/fd_s-prob.pd abstractions/fd_salvo.pd abstractions/fd_scales.pd abstractions/fd_sdel~.pd abstractions/fd_seltab.pd abstractions/fd_send.pd abstractions/fd_set-element.pd abstractions/fd_shader.pd abstractions/fd_shift.pd abstractions/fd_shuffle.pd abstractions/fd_siginfo.pd abstractions/fd_sin.pd abstractions/fd_sinc.pd abstractions/fd_sinc~.pd abstractions/fd_sinh.pd abstractions/fd_sinh~.pd abstractions/fd_smooth.pd abstractions/fd_smooth_brute.pd abstractions/fd_snote.pd abstractions/fd_sp.pd abstractions/fd_spat.pd abstractions/fd_spigot.pd abstractions/fd_spigot3.pd abstractions/fd_spline~.pd abstractions/fd_step.pd abstractions/fd_stick.pd abstractions/fd_stopwatch.pd abstractions/fd_sum.pd abstractions/fd_swap.pd abstractions/fd_switch.pd abstractions/fd_tempo-msec.pd abstractions/fd_textsort2.pd abstractions/fd_tglvis.pd abstractions/fd_title.pd abstractions/fd_tom.pd abstractions/fd_toroman.pd abstractions/fd_tovowel.pd abstractions/fd_tracktopol.pd abstractions/fd_trapezoid~.pd abstractions/fd_triangle~.pd abstractions/fd_trunc2n.pd abstractions/fd_untext.pd abstractions/fd_updown.pd abstractions/fd_vibrato.pd abstractions/fd_vidbuf.pd abstractions/fd_visera.pd abstractions/fd_vu.pd abstractions/fd_welch.pd abstractions/fd_welch~.pd abstractions/fd_wielevy.pd abstractions/fd_wielevy2.pd abstractions/fd_wincat.pd abstractions/fd_winconcat.pd abstractions/fd_windows.pd abstractions/fd_winreceive~.pd abstractions/fd_wood.pd abstractions/fd_write.pd abstractions/fd_write1.pd abstractions/fd_write2.pd abstractions/fd_write5.pd abstractions/fd_xnote.pd abstractions/fd_z12.pd abstractions/fd_zero-crossing~.pd abstractions/g.pd abstractions/v.pd GnuGPL.LICENSE.txt INSTALL.txt INSTRUCTIONS.txt OVERVIEW.txt README.txt 


define forWindows
  cflags = -D_WIN32 -D_ -Iinclude -march=x86-64
endef
define forDarwin
  cflags += -D__APPLE__ -DHAVE_UNISTD_H
endef
define forLinux
  cflags += -D__linux__ -D__gnu_linux__ -DHAVE_UNISTD_H
endef

# build a multi-object library
make-lib-executable=yes

# provide the path to pd-lib-builder
PDLIBBUILDER_DIR=./pd-lib-builder
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder

