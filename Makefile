.PHONY:deb clean

VERSION=3.2.1
PREFIX = /opt/apps/me.imever.cgtn-live-player
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
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/files/bin
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/files/share
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/entries/applications
	mkdir -p ${DESTDIR}${PACKDIR}${PREFIX}/entries/icons/hicolor/scalable/apps

	cp cgtn-live-player.desktop ${DESTDIR}${PACKDIR}${PREFIX}/entries/applications
	cp resource/cgtn-live-player.png ${DESTDIR}${PACKDIR}${PREFIX}/entries/icons/hicolor/scalable/apps
	cp ${BUILDDIR}/cgtn-live-player ${DESTDIR}${PACKDIR}${PREFIX}/files/bin
	cp resource/tv.json ${DESTDIR}${PACKDIR}${PREFIX}/files/share/
	cp info ${DESTDIR}${PACKDIR}${PREFIX}/

	cp -r DEBIAN ${DESTDIR}${PACKDIR}/

	sed -i 's/#VERSION#/'${VERSION}'/' ${DESTDIR}${PACKDIR}${PREFIX}/info
	sed -i 's/#VERSION#/'${VERSION}'/' ${DESTDIR}${PACKDIR}/DEBIAN/control
	SIZE=`du -sh ${DESTDIR}${PACKDIR}${PREFIX} | awk '{print $$1}'`; \
	sed -i 's/#SIZE#/'$$SIZE'/' ${DESTDIR}${PACKDIR}/DEBIAN/control

	cd ${DESTDIR}${PACKDIR}; \
		for f in `find opt -type f`; do md5sum $$f >> DEBIAN/md5sums; done

clean:
	rm -rf ${DESTDIR}
	rm -rf ${BUILDDIR}

deb: package
	cd ${DESTDIR};\
	fakeroot dpkg -b ${PACKDIR} me.imever.cgtn-live-player_${VERSION}.deb

FORCE:
