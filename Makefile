.PHONY:deb clean

VERSION=3.2.0
PREFIX = /usr
DESTDIR = dist/
PACKDIR = package
BUILDDIR = build

build: FORCE
	mkdir -p ${BUILDDIR}
	cd ${BUILDDIR}; \
	qmake -o Makefile ../cgtn.pro -spec linux-clang; \
	make

package:
	mkdir -p ${DESTDIR}${PACKDIR}
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/share/applications
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	cp cgtn-live-player.desktop ${DESTDIR}${PACKDIR}${PREFIX}/share/applications
	cp resource/cgtn-live-player.png ${DESTDIR}${PACKDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	cp ${BUILDDIR}/cgtn-live-player ${DESTDIR}${PACKDIR}${PREFIX}/bin
	cp -r DEBIAN ${DESTDIR}${PACKDIR}/

clean:
	rm -rf ${DESTDIR}
	rm -rf ${BUILDDIR}

deb:clean build package
	cd ${DESTDIR};\
	fakeroot dpkg -b ${PACKDIR} cgtn-live-player_${VERSION}.deb

FORCE:
