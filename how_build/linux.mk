CPUS=$(shell nproc)
CALENDAR=$(shell date '+%Y%m%d')
OSID=$(shell lsb_release -si)
OSRELEASE=$(shell lsb_release -sr)
SUFFIX=
ifneq ("$(OSID)", "")
SUFFIX=_$(OSID)$(OSRELEASE)
endif

PROJECT_NAME=notepad--
PACKAGE_NAME=com.hmja.notepad

all:
	mkdir -p build
	cd build && cmake ..
	cd build && make -j$(CPUS)

run: all
	exec $(shell find build/ -maxdepth 1 -type f -executable | grep $(PROJECT_NAME))

debug:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
	cd build && make -j$(CPUS)

release:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release -DPLUGIN_EN=off -DPACKAGE_SUFFIX="$(SUFFIX)" ..
	cd build && make -j$(CPUS)

package: release
	cd build && make package
	tree build/_CPack_Packages/Linux/DEB/$(PROJECT_NAME)-*
# 	dpkg-deb --contents build/$(PROJECT_NAME)$(SUFFIX).deb

builddeps:
	cd build && make builddeps

cpus:
	@echo "CPU数量: $(CPUS)"


