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
	AnimThing.cpp \
	ball.cpp \
	barrel.cpp \
	bouy.cpp \
	character.cpp \
	chunk.cpp \
	deathWad.cpp \
	demon.cpp \
	dispenser.cpp \
	doofus.cpp \
	dude.cpp \
	explode.cpp \
	fire.cpp \
	fireball.cpp \
	firebomb.cpp \
	flag.cpp \
	flagbase.cpp \
	grenade.cpp \
	heatseeker.cpp \
	hood.cpp \
	item3d.cpp \
	mine.cpp \
	napalm.cpp \
	navnet.cpp \
	ostrich.cpp \
	person.cpp \
	PowerUp.cpp \
	pylon.cpp \
	rocket.cpp \
	sentry.cpp \
	SndRelay.cpp \
	SoundThing.cpp \
	thing.cpp \
	Thing3d.cpp \
	trigger.cpp \
	warp.cpp \
	weapon.cpp \
	alphablitforpostal.cpp \
	Anim3D.cpp \
	BufQ.cpp \
	bulletFest.cpp \
	camera.cpp \
	crawler.cpp \
	cutscene.cpp \
	encrypt.cpp \
	gameedit.cpp \
	GameSettings.cpp \
	grip.cpp \
	IdBank.cpp \
	InputSettings.cpp \
	InputSettingsDlg.cpp \
	keys.cpp \
	Log.cpp \
	logtab.cpp \
	MemFileFest.cpp \
	MenuSettings.cpp \
	MenuTrans.cpp \
	net.cpp \
	NetBrowse.cpp \
	NetClient.cpp \
	NetDlg.cpp \
	netmsgr.cpp \
	NetServer.cpp \
	organ.cpp \
	Personatorium.cpp \
	ProtoBSDIP.cpp \
	realm.cpp \
	scene.cpp \
	score.cpp \
	settings.cpp \
	smash.cpp \
	socket.cpp \
	StockPile.cpp \
	TexEdit.cpp \
	toolbar.cpp \
	TriggerRegions.cpp \
	update.cpp \
	yatime.cpp \
	aivars.cpp \
	band.cpp \
	credits.cpp \
	game.cpp \
	input.cpp \
	localize.cpp \
	main.cpp \
	menus.cpp \
	play.cpp \
	SampleMaster.cpp \
	title.cpp

RSSRCS := \
	RSPiX/Src/BLUE/unix/Bdebug.cpp \
	RSPiX/Src/BLUE/unix/Bjoy.cpp \
	RSPiX/Src/BLUE/unix/Bkey.cpp \
	RSPiX/Src/BLUE/unix/Bmain.cpp \
	RSPiX/Src/BLUE/unix/Bmouse.cpp \
	RSPiX/Src/BLUE/unix/Btime.cpp \
	RSPiX/Src/BLUE/unix/Bdisp.cpp \
	RSPiX/Src/BLUE/unix/Bsound.cpp \
	RSPiX/Src/GREEN/Hot/hot.cpp \
	RSPiX/Src/GREEN/Image/Image.cpp \
	RSPiX/Src/GREEN/Image/Imagecon.cpp \
	RSPiX/Src/GREEN/Image/ImageFile.cpp \
	RSPiX/Src/GREEN/InputEvent/InputEvent.cpp \
	RSPiX/Src/GREEN/Mix/mix.cpp \
	RSPiX/Src/GREEN/Mix/MixBuf.cpp \
	RSPiX/Src/GREEN/Image/pal.cpp \
	RSPiX/Src/GREEN/Image/PalFile.cpp \
	RSPiX/Src/GREEN/Sample/sample.cpp \
	RSPiX/Src/GREEN/Snd/snd.cpp \
	RSPiX/Src/GREEN/SndFx/SndFx.cpp \
	RSPiX/Src/GREEN/Task/task.cpp \
	RSPiX/Src/GREEN/3D/pipeline.cpp \
	RSPiX/Src/GREEN/3D/render.cpp \
	RSPiX/Src/GREEN/3D/types3d.cpp \
	RSPiX/Src/GREEN/3D/zbuffer.cpp \
	RSPiX/Src/GREEN/BLiT/alphablit.cpp \
	RSPiX/Src/GREEN/BLiT/BLIT.cpp \
	RSPiX/Src/GREEN/BLiT/BLITINIT.cpp \
	RSPiX/Src/GREEN/BLiT/BLiTT.cpp \
	RSPiX/Src/GREEN/BLiT/CFNT.cpp \
	RSPiX/Src/GREEN/BLiT/Fspr1.cpp \
	RSPiX/Src/GREEN/BLiT/FSPR8.cpp \
	RSPiX/Src/GREEN/BLiT/line.cpp \
	RSPiX/Src/GREEN/BLiT/mono.cpp \
	RSPiX/Src/GREEN/BLiT/Rotate96.cpp \
	RSPiX/Src/GREEN/BLiT/RPrint.cpp \
	RSPiX/Src/GREEN/BLiT/ScaleFlat.cpp \
	RSPiX/Src/ORANGE/GameLib/ANIMSPRT.cpp \
	RSPiX/Src/ORANGE/Attribute/attribute.cpp \
	RSPiX/Src/ORANGE/GUI/btn.cpp \
	RSPiX/Src/ORANGE/Channel/channel.cpp \
	RSPiX/Src/ORANGE/color/colormatch.cpp \
	RSPiX/Src/ORANGE/DirtRect/DirtRect.cpp \
	RSPiX/Src/ORANGE/color/dithermatch.cpp \
	RSPiX/Src/ORANGE/GUI/dlg.cpp \
	RSPiX/Src/ORANGE/GUI/edit.cpp \
	RSPiX/Src/ORANGE/File/file.cpp \
	RSPiX/Src/ORANGE/QuickMath/FixedPoint.cpp \
	RSPiX/Src/ORANGE/GUI/guiItem.cpp \
	RSPiX/Src/ORANGE/IFF/iff.cpp \
	RSPiX/Src/ORANGE/ImageTools/lasso.cpp \
	RSPiX/Src/ORANGE/Laymage/laymage.cpp \
	RSPiX/Src/ORANGE/GUI/ListBox.cpp \
	RSPiX/Src/ORANGE/GUI/ListContents.cpp \
	RSPiX/Src/ORANGE/Meter/meter.cpp \
	RSPiX/Src/ORANGE/MsgBox/MsgBox.cpp \
	RSPiX/Src/ORANGE/GUI/MultiBtn.cpp \
	RSPiX/Src/ORANGE/MultiGrid/MultiGrid.cpp \
	RSPiX/Src/ORANGE/MultiGrid/MultiGridIndirect.cpp \
	RSPiX/Src/ORANGE/GUI/ProcessGui.cpp \
	RSPiX/Src/ORANGE/Debug/profile.cpp \
	RSPiX/Src/ORANGE/GUI/PushBtn.cpp \
	RSPiX/Src/ORANGE/QuickMath/QuickMath.cpp \
	RSPiX/Src/ORANGE/GameLib/Region.cpp \
	RSPiX/Src/ORANGE/RString/rstring.cpp \
	RSPiX/Src/ORANGE/GUI/scrollbar.cpp \
	RSPiX/Src/ORANGE/GameLib/SHAPES.cpp \
	RSPiX/Src/ORANGE/Parse/SimpleBatch.cpp \
	RSPiX/Src/ORANGE/GameLib/SPRITE.cpp \
	RSPiX/Src/ORANGE/str/str.cpp \
	RSPiX/Src/ORANGE/GUI/txt.cpp \
	RSPiX/Src/CYAN/Unix/uDialog.cpp \
	RSPiX/Src/CYAN/Unix/uColors.cpp \
	RSPiX/Src/CYAN/Unix/uPath.cpp

WSRCS :=  \
	WishPiX/Menu/menu.cpp \
	WishPiX/Prefs/prefline.cpp \
	WishPiX/Prefs/prefs.cpp \
	WishPiX/ResourceManager/resmgr.cpp \
	WishPiX/Spry/spry.cpp
	
SRCS += $(RSSRCS)
SRCS += $(WSRCS)

RSOBJS := $(RSSRCS:.cpp=.o)
RSOBJS := $(foreach f,$(RSOBJS),$(BINDIR)/$(f))
	
CFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.c))
CPPFILES   := $(foreach dir,$(SOURCES), $(wildcard $(dir)/*.cpp))
BINFILES := $(foreach dir,$(DATA), $(wildcard $(dir)/*.bin))
OBJS     := $(addsuffix .o,$(BINFILES)) $(SRCS:.c=.o) $(CPPFILES:.cpp=.o) 

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir))

PREFIX  = arm-vita-eabi
CC      = $(PREFIX)-gcc
CXX      = $(PREFIX)-g++
CFLAGS  = $(INCLUDE) -g -Wl,-q -O2 -fsigned-char -DPLATFORM_UNIX -DPSP2 -DTARGET=POSTAL_2015 -DLOCALE=US -DALLOW_TWINSTICK -w

CFLAGS += -DDEBUG -D_DEBUG -DRSP_DEBUG_OUT_FILE

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
