TARGET		:= Postal
TITLE	    := POSTAL001
SRCDIR		:= .
BINDIR  	:= ./bin
INCLUDES	:= .

VITASDK		:= C:\vitasdk

LIBS = -lmikmod -lvorbisfile -lvorbis -logg -lsndfile -lSceLibKernel_stub -lScePvf_stub \
	-lSceAppMgr_stub -lSceCtrl_stub -lSceTouch_stub -lm -lSceAppUtil_stub -lScePgf_stub -ljpeg \
	-lfreetype -lc -lScePower_stub -lSceCommonDialog_stub -lpng16 -lz -lSceAudio_stub -lSceGxm_stub \
	-lSceDisplay_stub -lSceSysmodule_stub -lSDL2 -lvita2d -lSceHid_stub

SRCS := \
	AnimThing.o \
	ball.o \
	barrel.o \
	bouy.o \
	character.o \
	chunk.o \
	deathWad.o \
	demon.o \
	dispenser.o \
	doofus.o \
	dude.o \
	explode.o \
	fire.o \
	fireball.o \
	firebomb.o \
	flag.o \
	flagbase.o \
	grenade.o \
	heatseeker.o \
	hood.o \
	item3d.o \
	mine.o \
	napalm.o \
	navnet.o \
	ostrich.o \
	person.o \
	PowerUp.o \
	pylon.o \
	rocket.o \
	sentry.o \
	SndRelay.o \
	SoundThing.o \
	thing.o \
	Thing3d.o \
	trigger.o \
	warp.o \
	weapon.o \
	alphablitforpostal.o \
	Anim3D.o \
	BufQ.o \
	bulletFest.o \
	camera.o \
	crawler.o \
	cutscene.o \
	encrypt.o \
	gameedit.o \
	GameSettings.o \
	grip.o \
	IdBank.o \
	InputSettings.o \
	InputSettingsDlg.o \
	keys.o \
	Log.o \
	logtab.o \
	MemFileFest.o \
	MenuSettings.o \
	MenuTrans.o \
	net.o \
	NetBrowse.o \
	NetClient.o \
	NetDlg.o \
	netmsgr.o \
	NetServer.o \
	organ.o \
	Personatorium.o \
	ProtoBSDIP.o \
	realm.o \
	scene.o \
	score.o \
	settings.o \
	smash.o \
	socket.o \
	StockPile.o \
	TexEdit.o \
	toolbar.o \
	TriggerRegions.o \
	update.o \
	yatime.o \
	aivars.o \
	band.o \
	credits.o \
	game.o \
	input.o \
	localize.o \
	main.o \
	menus.o \
	play.o \
	SampleMaster.o \
	title.o

RSSRCS := \
	RSPiX/Src/BLUE/unix/Bdebug.o \
	RSPiX/Src/BLUE/unix/Bjoy.o \
	RSPiX/Src/BLUE/unix/Bkey.o \
	RSPiX/Src/BLUE/unix/Bmain.o \
	RSPiX/Src/BLUE/unix/Bmouse.o \
	RSPiX/Src/BLUE/unix/Btime.o \
	RSPiX/Src/BLUE/unix/Bdisp.o \
	RSPiX/Src/BLUE/unix/Bsound.o \
	RSPiX/Src/GREEN/Hot/hot.o \
	RSPiX/Src/GREEN/Image/Image.o \
	RSPiX/Src/GREEN/Image/Imagecon.o \
	RSPiX/Src/GREEN/Image/ImageFile.o \
	RSPiX/Src/GREEN/InputEvent/InputEvent.o \
	RSPiX/Src/GREEN/Mix/mix.o \
	RSPiX/Src/GREEN/Mix/MixBuf.o \
	RSPiX/Src/GREEN/Image/pal.o \
	RSPiX/Src/GREEN/Image/PalFile.o \
	RSPiX/Src/GREEN/Sample/sample.o \
	RSPiX/Src/GREEN/Snd/snd.o \
	RSPiX/Src/GREEN/SndFx/SndFx.o \
	RSPiX/Src/GREEN/Task/task.o \
	RSPiX/Src/GREEN/3D/pipeline.o \
	RSPiX/Src/GREEN/3D/render.o \
	RSPiX/Src/GREEN/3D/types3d.o \
	RSPiX/Src/GREEN/3D/zbuffer.o \
	RSPiX/Src/GREEN/BLiT/alphablit.o \
	RSPiX/Src/GREEN/BLiT/BLIT.o \
	RSPiX/Src/GREEN/BLiT/BLITINIT.o \
	RSPiX/Src/GREEN/BLiT/BLiTT.o \
	RSPiX/Src/GREEN/BLiT/CFNT.o \
	RSPiX/Src/GREEN/BLiT/Fspr1.o \
	RSPiX/Src/GREEN/BLiT/FSPR8.o \
	RSPiX/Src/GREEN/BLiT/line.o \
	RSPiX/Src/GREEN/BLiT/mono.o \
	RSPiX/Src/GREEN/BLiT/Rotate96.o \
	RSPiX/Src/GREEN/BLiT/RPrint.o \
	RSPiX/Src/GREEN/BLiT/ScaleFlat.o \
	RSPiX/Src/ORANGE/GameLib/ANIMSPRT.o \
	RSPiX/Src/ORANGE/Attribute/attribute.o \
	RSPiX/Src/ORANGE/GUI/btn.o \
	RSPiX/Src/ORANGE/Channel/channel.o \
	RSPiX/Src/ORANGE/color/colormatch.o \
	RSPiX/Src/ORANGE/DirtRect/DirtRect.o \
	RSPiX/Src/ORANGE/color/dithermatch.o \
	RSPiX/Src/ORANGE/GUI/dlg.o \
	RSPiX/Src/ORANGE/GUI/edit.o \
	RSPiX/Src/ORANGE/File/file.o \
	RSPiX/Src/ORANGE/QuickMath/FixedPoint.o \
	RSPiX/Src/ORANGE/GUI/guiItem.o \
	RSPiX/Src/ORANGE/IFF/iff.o \
	RSPiX/Src/ORANGE/ImageTools/lasso.o \
	RSPiX/Src/ORANGE/Laymage/laymage.o \
	RSPiX/Src/ORANGE/GUI/ListBox.o \
	RSPiX/Src/ORANGE/GUI/ListContents.o \
	RSPiX/Src/ORANGE/Meter/meter.o \
	RSPiX/Src/ORANGE/MsgBox/MsgBox.o \
	RSPiX/Src/ORANGE/GUI/MultiBtn.o \
	RSPiX/Src/ORANGE/MultiGrid/MultiGrid.o \
	RSPiX/Src/ORANGE/MultiGrid/MultiGridIndirect.o \
	RSPiX/Src/ORANGE/GUI/ProcessGui.o \
	RSPiX/Src/ORANGE/Debug/profile.o \
	RSPiX/Src/ORANGE/GUI/PushBtn.o \
	RSPiX/Src/ORANGE/QuickMath/QuickMath.o \
	RSPiX/Src/ORANGE/GameLib/Region.o \
	RSPiX/Src/ORANGE/RString/rstring.o \
	RSPiX/Src/ORANGE/GUI/scrollbar.o \
	RSPiX/Src/ORANGE/GameLib/SHAPES.o \
	RSPiX/Src/ORANGE/Parse/SimpleBatch.o \
	RSPiX/Src/ORANGE/GameLib/SPRITE.o \
	RSPiX/Src/ORANGE/str/str.o \
	RSPiX/Src/ORANGE/GUI/txt.o \
	RSPiX/Src/CYAN/Unix/uDialog.o \
	RSPiX/Src/CYAN/Unix/uColors.o \
	RSPiX/Src/CYAN/Unix/uPath.o

WSRCS :=  \
	WishPiX/Menu/menu.o \
	WishPiX/Prefs/prefline.o \
	WishPiX/Prefs/prefs.o \
	WishPiX/ResourceManager/resmgr.o \
	WishPiX/Spry/spry.o
	
SRCS += $(RSSRCS)
SRCS += $(WSRCS)

RSOBJS := $(RSSRCS:.cpp=.o)
RSOBJS := $(foreach f,$(RSOBJS),$(BINDIR)/$(f))
	
CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(SRCS)
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) 

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O2 -fsigned-char -DPLATFORM_UNIX -DPSP2 -DTARGET=POSTAL_2015 -DLOCALE=US -DALLOW_TWINSTICK -w

#CFLAGS += -DDEBUG -D_DEBUG -DRSP_DEBUG_OUT_FILE

# includes ...
CFLAGS += -I$(SRCDIR)
CFLAGS += -I$(VITASDK)/$(PREFIX)/include/SDL2
CFLAGS += -I$(SRCDIR)/RSPiX
CFLAGS += -I$(SRCDIR)/RSPiX/Inc
CFLAGS += -I$(SRCDIR)/RSPiX/Src
CFLAGS += -I$(SRCDIR)/RSPiX/Src/BLUE
CFLAGS += -I$(SRCDIR)/RSPiX/Src/BLUE/unix
CFLAGS += -I$(SRCDIR)/RSPiX/Src/CYAN
CFLAGS += -I$(SRCDIR)/RSPiX/Src/CYAN/Unix
CFLAGS += -I$(SRCDIR)/RSPiX/Src/ORANGE
CFLAGS += -I$(SRCDIR)/RSPiX/Src/ORANGE/CDT
CFLAGS += -I$(SRCDIR)/RSPiX/Src/ORANGE/GameLib
CFLAGS += -I$(SRCDIR)/RSPiX/Src/ORANGE/File
CFLAGS += -I$(SRCDIR)/RSPiX/Src/ORANGE/Multigrid
CFLAGS += -I$(SRCDIR)/RSPiX/Src/GREEN/Image
CFLAGS += -I$(SRCDIR)/WishPiX
CFLAGS += -I$(SRCDIR)/WishPiX/Spry


CXXFLAGS  = $(CFLAGS) -fno-exceptions -std=gnu++11 -fpermissive
ASFLAGS = $(CFLAGS)

all: $(TARGET).vpk

$(TARGET).vpk: $(TARGET).velf
	vita-make-fself -s $< build/eboot.bin
	vita-mksfoex -s TITLE_ID=$(TITLE) "$(TARGET)" param.sfo
	cp -f param.sfo build/sce_sys/param.sfo
	
	#------------ Comment this if you don't have 7zip ------------------
	7z a -tzip ./$(TARGET).vpk -r ./build/sce_sys ./build/eboot.bin
	#-------------------------------------------------------------------

%.velf: %.elf
	cp $< $<.unstripped.elf
	$(PREFIX)-strip -g $<
	vita-elf-create $< $@

$(TARGET).elf: $(OBJS)
	$(CXX) $(CXXFLAGS) $^ $(LIBS) -o $@

clean:
	@rm -rf $(TARGET).velf $(TARGET).elf
