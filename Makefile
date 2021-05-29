.PHONY:deb clean

VERSION=3.2.3
PREFIX = /opt/apps/me.imever.cgtn-live-player
DESTDIR = dist
BUILDDIR = build

build: FORCE
	mkdir -p ${BUILDDIR}
	cd ${BUILDDIR}; \
	qmake -o Makefile ../cgtn.pro -spec linux-clang; \
	make

package:
	strip ${BUILDDIR}/cgtn-live-player
	mkdir -p ${DESTDIR}
	mkdir -p ${DESTDIR}${PREFIX}/files/bin
	mkdir -p ${DESTDIR}${PREFIX}/files/share
	mkdir -p ${DESTDIR}${PREFIX}/entries/applications
	mkdir -p ${DESTDIR}${PREFIX}/entries/icons/hicolor/scalable/apps

	cp cgtn-live-player.desktop ${DESTDIR}${PREFIX}/entries/applications
	cp resource/cgtn-live-player.png ${DESTDIR}${PREFIX}/entries/icons/hicolor/scalable/apps
	cp ${BUILDDIR}/cgtn-live-player ${DESTDIR}${PREFIX}/files/bin
	cp resource/tv.json ${DESTDIR}${PREFIX}/files/share/
	cp info ${DESTDIR}${PREFIX}/

	cp -r DEBIAN ${DESTDIR}/

	sed -i 's/#VERSION#/'${VERSION}'/' ${DESTDIR}${PREFIX}/info
	sed -i 's/#VERSION#/'${VERSION}'/' ${DESTDIR}/DEBIAN/control
	SIZE=`du -sh ${DESTDIR}${PREFIX} | awk '{print $$1}'`; \
	sed -i 's/#SIZE#/'$$SIZE'/' ${DESTDIR}/DEBIAN/control

	cd ${DESTDIR}; \
		for f in `find opt -type f`; do md5sum $$f >> DEBIAN/md5sums; done

clean:
	rm -rf ${DESTDIR}
	rm -rf ${BUILDDIR}

deb: package
	fakeroot dpkg -b ${DESTDIR} me.imever.cgtn-live-player_${VERSION}.deb

FORCE:
.PHONY: FORCE
