.PHONY:deb clean

VERSION=3.1.0
PREFIX = /usr
DESTDIR = dist/
BUILDDIR = build

install:
	mkdir -p ${DESTDIR}${BUILDDIR}
	mkdir -p ${DESTDIR}${BUILDDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${BUILDDIR}${PREFIX}/share/applications
	mkdir -p ${DESTDIR}${BUILDDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	#mkdir -p ${DESTDIR}${BUILDDIR}${PREFIX}/share/bingwallpaper
	#mkdir -p ${DESTDIR}${BUILDDIR}${PREFIX}/share/doc/bingwallpaper
	cp cgtn-live-player.desktop ${DESTDIR}${BUILDDIR}${PREFIX}/share/applications
	cp resource/cgtn-live-player.png ${DESTDIR}${BUILDDIR}${PREFIX}/share/icons/hicolor/scalable/apps
	cp cgtn-live-player ${DESTDIR}${BUILDDIR}${PREFIX}/bin
	cp -r DEBIAN ${DESTDIR}${BUILDDIR}/

clean:
	rm -rf ${DESTDIR}

deb:clean install
	cd ${DESTDIR};\
	fakeroot dpkg -b ${BUILDDIR} cgtn-live-player_${VERSION}.deb
