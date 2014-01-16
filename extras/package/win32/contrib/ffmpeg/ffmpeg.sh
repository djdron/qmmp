#!/bin/sh

NAME=ffmpeg
VERSION=2.1.3

case $1 in
  --download)
    mkdir -p temp
    cd temp
    wget -nc http://ffmpeg.org/releases/$NAME-$VERSION.tar.bz2
  ;;
  --install)
    cd temp
    tar xvjf $NAME-$VERSION.tar.bz2
    cd $NAME-$VERSION
    ./configure --prefix=$PREFIX --enable-shared --disable-static --disable-doc --disable-avdevice \
    --disable-network --disable-encoders --enable-small --disable-muxers --disable-indevs --disable-outdevs \
    --disable-iconv --disable-bsfs --disable-programs \
    --disable-muxers --disable-demuxers --disable-protocols --disable-parsers \
    --disable-filter=colorchannelmixer \
    --disable-filter=adelay --disable-filter=compand \
    --disable-filter=aecho --disable-filter=concat \
    --disable-filter=aevalsrc \
    --disable-filter=curves        	--disable-filter=pan \
    --disable-filter=ainterleave   	--disable-filter=dctdnoiz --disable-filter=perms \
    --disable-filter=allpass       	--disable-filter=decimate --disable-filter=pixdesctest \
    --disable-filter=alphaextract  	--disable-filter=deshake --disable-filter=psnr   \
    --disable-filter=alphamerge	   	--disable-filter=drawbox --disable-filter=removelogo \
    --disable-filter=amerge       	--disable-filter=drawgrid   \
    --disable-filter=amix               --disable-filter=earwax --disable-filter=rgbtestsrc \
    --disable-filter=edgedetect	        --disable-filter=rotate \
    --disable-filter=extractplanes \
    --disable-filter=apad		--disable-filter=field			--disable-filter=separatefields \
    --disable-filter=aperms		--disable-filter=fieldmatch		--disable-filter=setdar \
    --disable-filter=aphaser		--disable-filter=fieldorder		--disable-filter=setfield  \
    --disable-filter=setpts             --disable-filter=aselect		--disable-filter=fps	\
    --disable-filter=setsar  \
    --disable-filter=asendcmd		--disable-filter=framestep		--disable-filter=settb   \
    --disable-filter=asetnsamples	--disable-filter=gradfun		--disable-filter=showinfo  \
    --disable-filter=asetpts		--disable-filter=haldclut		--disable-filter=showspectrum  \
    --disable-filter=asetrate		--disable-filter=haldclutsrc		--disable-filter=showwaves    \
    --disable-filter=asettb		--disable-filter=hflip			--disable-filter=silencedetect \
    --disable-filter=ashowinfo		--disable-filter=highpass		--disable-filter=sine    \
    --disable-filter=asplit		--disable-filter=histogram		--disable-filter=smptebars \
    --disable-filter=astats		--disable-filter=hue			--disable-filter=smptehdbars  \
    --disable-filter=astreamsync	--disable-filter=idet			--disable-filter=split  \
    --disable-filter=il			--disable-filter=swapuv   \
    --disable-filter=atrim		--disable-filter=interleave		--disable-filter=telecine \
    --disable-filter=avectorscope	--disable-filter=join			--disable-filter=testsrc   \
    --disable-filter=bandpass		--disable-filter=life			--disable-filter=thumbnail  \
    --disable-filter=bandreject		--disable-filter=lowpass		--disable-filter=tile \
    --disable-filter=bass		--disable-filter=lut			--disable-filter=transpose  \
    --disable-filter=bbox		--disable-filter=lut3d			--disable-filter=treble   \
    --disable-filter=biquad		--disable-filter=lutrgb			--disable-filter=trim    \
    --disable-filter=blackdetect	--disable-filter=lutyuv			--disable-filter=unsharp \
    --disable-filter=blend		--disable-filter=mandelbrot		--disable-filter=vflip \
    --disable-filter=cellauto		--disable-filter=mergeplanes		--disable-filter=vignette \
    --disable-filter=channelmap		--disable-filter=movie \
    --disable-filter=channelsplit	--disable-filter=negate			--disable-filter=volumedetect \
    --disable-filter=color		--disable-filter=noformat		--disable-filter=w3fdif \
    --disable-filter=colorbalance	--disable-filter=noise \
--disable-decoder=aac			--disable-decoder=bmp			--disable-decoder=idcin  \
--disable-decoder=aac_latm		--disable-decoder=bmv_audio		--disable-decoder=idf  \
--disable-decoder=aasc			--disable-decoder=bmv_video		--disable-decoder=iff_byterun1  \
--disable-decoder=ac3			--disable-decoder=brender_pix		--disable-decoder=iff_ilbm  \
--disable-decoder=adpcm_4xm		--disable-decoder=c93			--disable-decoder=imc    \
--disable-decoder=adpcm_adx		--disable-decoder=cavs			--disable-decoder=indeo2  \
--disable-decoder=adpcm_afc		--disable-decoder=cdgraphics		--disable-decoder=indeo3   \
--disable-decoder=adpcm_ct		--disable-decoder=cdxl			--disable-decoder=indeo4    \
--disable-decoder=adpcm_dtk		--disable-decoder=cinepak		--disable-decoder=indeo5     \
--disable-decoder=adpcm_ea		--disable-decoder=cljr			--disable-decoder=interplay_dpcm \
--disable-decoder=adpcm_ea_maxis_xa	--disable-decoder=cllc			--disable-decoder=interplay_video \
--disable-decoder=adpcm_ea_r1		--disable-decoder=comfortnoise		--disable-decoder=jacosub \
--disable-decoder=adpcm_ea_r2		--disable-decoder=cook			--disable-decoder=jpeg2000 \
--disable-decoder=adpcm_ea_r3		--disable-decoder=cpia			--disable-decoder=jpegls \
--disable-decoder=adpcm_ea_xas		--disable-decoder=cscd			--disable-decoder=jv     \
--disable-decoder=adpcm_g722		--disable-decoder=cyuv			--disable-decoder=kgv1    \
--disable-decoder=adpcm_g726		--disable-decoder=dca			--disable-decoder=kmvc     \
--disable-decoder=adpcm_g726le		--disable-decoder=dfa			--disable-decoder=lagarith  \
--disable-decoder=adpcm_ima_amv		--disable-decoder=dirac			--disable-decoder=loco       \
--disable-decoder=adpcm_ima_apc		--disable-decoder=dnxhd			--disable-decoder=mace3       \
--disable-decoder=adpcm_ima_dk3		--disable-decoder=dpx			--disable-decoder=mace6        \
--disable-decoder=adpcm_ima_dk4		--disable-decoder=dsicinaudio		--disable-decoder=mdec          \
--disable-decoder=adpcm_ima_ea_eacs	--disable-decoder=dsicinvideo		--disable-decoder=metasound      \
--disable-decoder=adpcm_ima_ea_sead	--disable-decoder=dvbsub		--disable-decoder=microdvd        \
--disable-decoder=adpcm_ima_iss		--disable-decoder=dvdsub		--disable-decoder=mimic            \
--disable-decoder=adpcm_ima_oki		--disable-decoder=dvvideo		--disable-decoder=mjpeg            \
--disable-decoder=adpcm_ima_qt		--disable-decoder=dxa			--disable-decoder=mjpegb            \
--disable-decoder=adpcm_ima_rad		--disable-decoder=dxtory		--disable-decoder=mlp                \
--disable-decoder=adpcm_ima_smjpeg	--disable-decoder=eac3			--disable-decoder=mmvideo             \
--disable-decoder=adpcm_ima_wav		--disable-decoder=eacmv			--disable-decoder=motionpixels         \
--disable-decoder=adpcm_ima_ws		--disable-decoder=eamad			--disable-decoder=movtext               \
--disable-decoder=adpcm_ms		--disable-decoder=eatgq			--disable-decoder=mp1                    \
--disable-decoder=adpcm_sbpro_2		--disable-decoder=eatgv			--disable-decoder=mp1float                \
--disable-decoder=adpcm_sbpro_3		--disable-decoder=eatqi			--disable-decoder=mp2                      \
--disable-decoder=adpcm_sbpro_4		--disable-decoder=eightbps		--disable-decoder=mp2float   \
--disable-decoder=adpcm_swf		--disable-decoder=eightsvx_exp		--disable-decoder=mp3         \
--disable-decoder=adpcm_thp		--disable-decoder=eightsvx_fib		--disable-decoder=mp3adu       \
--disable-decoder=adpcm_xa		--disable-decoder=escape124		--disable-decoder=mp3adufloat   \
--disable-decoder=adpcm_yamaha		--disable-decoder=escape130		--disable-decoder=mp3float       \
--disable-decoder=aic			--disable-decoder=evrc			--disable-decoder=mp3on4          \
--disable-decoder=alac			--disable-decoder=exr			--disable-decoder=mp3on4float      \
--disable-decoder=als			--disable-decoder=ffv1			--disable-decoder=mpc7              \
--disable-decoder=amrnb			--disable-decoder=ffvhuff		--disable-decoder=mpc8               \
--disable-decoder=amrwb			--disable-decoder=ffwavesynth		--disable-decoder=mpeg1video          \
--disable-decoder=amv			--disable-decoder=flac			--disable-decoder=mpeg2video           \
--disable-decoder=anm			--disable-decoder=flashsv		--disable-decoder=mpeg4                 \
--disable-decoder=ansi			--disable-decoder=flashsv2		--disable-decoder=mpegvideo              \
--disable-decoder=ape			--disable-decoder=flic			--disable-decoder=mpl2                    \
--disable-decoder=ass			--disable-decoder=flv			--disable-decoder=msa1                     \
--disable-decoder=asv1			--disable-decoder=fourxm		--disable-decoder=msmpeg4v1  \
--disable-decoder=asv2			--disable-decoder=fraps			--disable-decoder=msmpeg4v2   \
--disable-decoder=atrac1		--disable-decoder=frwu			--disable-decoder=msmpeg4v3    \
--disable-decoder=atrac3		--disable-decoder=g2m			--disable-decoder=msrle         \
--disable-decoder=aura			--disable-decoder=g723_1		--disable-decoder=mss1           \
--disable-decoder=aura2			--disable-decoder=g729			--disable-decoder=mss2            \
--disable-decoder=avrn			--disable-decoder=gif			--disable-decoder=msvideo1         \
--disable-decoder=avrp			--disable-decoder=gsm			--disable-decoder=mszh              \
--disable-decoder=avs			--disable-decoder=gsm_ms		--disable-decoder=mts2               \
--disable-decoder=avui			--disable-decoder=h261			--disable-decoder=mvc1                \
--disable-decoder=ayuv			--disable-decoder=h263			--disable-decoder=mvc2                 \
--disable-decoder=bethsoftvid		--disable-decoder=h263i			--disable-decoder=mxpeg
--disable-decoder=bfi			--disable-decoder=h263p			--disable-decoder=nellymoser   \
--disable-decoder=bink			--disable-decoder=h264			--disable-decoder=nuv           \
--disable-decoder=binkaudio_dct		--disable-decoder=hevc			--disable-decoder=paf_audio      \
--disable-decoder=binkaudio_rdft	--disable-decoder=huffyuv		--disable-decoder=paf_video      \
--disable-decoder=bintext		--disable-decoder=iac			--disable-decoder=pam             \
--disable-decoder=pbm			--disable-decoder=realtext		--disable-decoder=v210x            \
--disable-decoder=pcm_alaw		--disable-decoder=rl2			--disable-decoder=v308              \
--disable-decoder=pcm_bluray		--disable-decoder=roq			--disable-decoder=v408               \
--disable-decoder=pcm_dvd		--disable-decoder=roq_dpcm		--disable-decoder=v410                \
--disable-decoder=pcm_f32be		--disable-decoder=rpza			--disable-decoder=vb                   \
--disable-decoder=pcm_f32le		--disable-decoder=rv10			--disable-decoder=vble                  \
--disable-decoder=pcm_f64be		--disable-decoder=rv20			--disable-decoder=vc1                    \
--disable-decoder=pcm_f64le		--disable-decoder=rv30			--disable-decoder=vc1image                \
--disable-decoder=pcm_lxf		--disable-decoder=rv40			--disable-decoder=vcr1                     \
--disable-decoder=pcm_mulaw		--disable-decoder=s302m			--disable-decoder=vima      \
--disable-decoder=pcm_s16be		--disable-decoder=sami			--disable-decoder=vmdaudio   \
--disable-decoder=pcm_s16be_planar	--disable-decoder=sanm			--disable-decoder=vmdvideo    \
--disable-decoder=pcm_s16le		--disable-decoder=sgi			--disable-decoder=vmnc         \
--disable-decoder=pcm_s16le_planar	--disable-decoder=sgirle		--disable-decoder=vorbis        \
--disable-decoder=pcm_s24be		--disable-decoder=shorten		--disable-decoder=vp3            \
--disable-decoder=pcm_s24daud		--disable-decoder=sipr			--disable-decoder=vp5             \
--disable-decoder=pcm_s24le		--disable-decoder=smackaud		--disable-decoder=vp6              \
--disable-decoder=pcm_s24le_planar	--disable-decoder=smacker		--disable-decoder=vp6a              \
--disable-decoder=pcm_s32be		--disable-decoder=smc			--disable-decoder=vp6f               \
--disable-decoder=pcm_s32le		--disable-decoder=smvjpeg		--disable-decoder=vp8   \
--disable-decoder=pcm_s32le_planar	--disable-decoder=snow			--disable-decoder=vp9    \
--disable-decoder=pcm_s8		--disable-decoder=sol_dpcm		--disable-decoder=vplayer \
--disable-decoder=pcm_s8_planar		--disable-decoder=sonic			--disable-decoder=vqa      \
--disable-decoder=pcm_u16be		--disable-decoder=sp5x			--disable-decoder=wavpack   \
--disable-decoder=pcm_u16le		--disable-decoder=srt			--disable-decoder=webp       \
--disable-decoder=pcm_u24be		--disable-decoder=ssa			--disable-decoder=webvtt      \
--disable-decoder=pcm_u24le		--disable-decoder=subrip		--disable-decoder=wmalossless  \
--disable-decoder=pcm_u32be		--disable-decoder=subviewer		--disable-decoder=wmapro        \
--disable-decoder=pcm_u32le		--disable-decoder=subviewer1		--disable-decoder=wmav1          \
--disable-decoder=pcm_u8		--disable-decoder=sunrast		--disable-decoder=wmav2           \
--disable-decoder=pcm_zork		--disable-decoder=svq1			--disable-decoder=wmavoice         \
--disable-decoder=pcx			--disable-decoder=svq3			--disable-decoder=wmv1              \
--disable-decoder=pgm			--disable-decoder=tak			--disable-decoder=wmv2               \
--disable-decoder=pgmyuv		--disable-decoder=targa			--disable-decoder=wmv3                \
--disable-decoder=pgssub		--disable-decoder=targa_y216		--disable-decoder=wmv3image            \
--disable-decoder=pictor		--disable-decoder=text			--disable-decoder=wnv1                  \
--disable-decoder=pjs			--disable-decoder=theora		--disable-decoder=ws_snd1     \
--disable-decoder=png			--disable-decoder=thp			--disable-decoder=xan_dpcm     \
--disable-decoder=ppm			--disable-decoder=tiertexseqvideo	--disable-decoder=xan_wc3       \
--disable-decoder=prores		--disable-decoder=tiff			--disable-decoder=xan_wc4        \
--disable-decoder=prores_lgpl		--disable-decoder=tmv			--disable-decoder=xbin            \
--disable-decoder=ptx			--disable-decoder=truehd		--disable-decoder=xbm              \
--disable-decoder=qcelp			--disable-decoder=truemotion1		--disable-decoder=xface             \
--disable-decoder=qdm2			--disable-decoder=truemotion2		--disable-decoder=xl       \
--disable-decoder=qdraw			--disable-decoder=truespeech		--disable-decoder=xsub      \
--disable-decoder=qpeg			--disable-decoder=tscc			--disable-decoder=xwd        \
--disable-decoder=qtrle			--disable-decoder=tscc2			--disable-decoder=y41p        \
--disable-decoder=r10k			--disable-decoder=tta			--disable-decoder=yop          \
--disable-decoder=r210			--disable-decoder=twinvq		--disable-decoder=yuv4          \
--disable-decoder=ra_144		--disable-decoder=txd			--disable-decoder=zero12v        \
--disable-decoder=ra_288		--disable-decoder=ulti			--disable-decoder=zerocodec       \
--disable-decoder=ralf			--disable-decoder=utvideo		--disable-decoder=zlib  \
--disable-decoder=rawvideo		--disable-decoder=v210			--disable-decoder=zmbv

    make -j${JOBS}
    make install

  ;;
  --clean)
    cd temp
    rm -rf $NAME-$VERSION
    cd ..
  ;;
esac
