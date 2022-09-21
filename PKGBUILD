# Maintainer: Giulio Girardi <giulio at rapgenic.it>

pkgname=xeus-octave
pkgver=git
pkgrel=0
pkgdesc="A native Octave kernel for jupyter with advanced visualization"
arch=('any')
url="https://github.com/rapgenic/xeus-octave"
license=('GPL3')
source=("${pkgname}::git+https://github.com/rapgenic/${pkgname}#branch=temp/initial")
md5sums=('SKIP')
depends=('xeus' 'octave')

pkgver() {
	cd "$pkgname"
	printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

package() {
	cd "$pkgname"

	cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr  .
	make
	make DESTDIR="$pkgdir/" install
}
