# Maintainer: dingjing <dingjing@live.cn>

pkgname=graceful-greeter
pkgver=2.0.0
pkgrel=1
pkgdesc='A beautiful lightDM login interface'
url='https://github.com/graceful-linux/graceful-greeter'
arch=('x86_64')
license=('MIT')
groups=('graceful')
depends=('imlib2' 'lightdm' 'libxrandr' 'libxtst' 'opencv' 'qt5-svg' 'qt5-x11extras')
makedepends=('qt5-tools' 'openmpi' 'cmake')
source=("https://github.com/graceful-linux/udisks/archive/refs/tags/udisks-debug-2.9.4.tar.gz")
sha512sums=('3836f6d2d6fe02eb9cccbb85d940bb5b4a07db1dafe7beba1f2ded110b04de03b1e0b79e11592c3a9b2f0e047ae4fc4284d895a953e1f4dd042e544634e505ca')

prepare() {
  mv udisks-udisks-debug-$pkgver udisks-udisks-$pkgver 
  cd udisks-udisks-$pkgver
  NOCONFIGURE=1 ./autogen.sh
}

build() {
  cd udisks-udisks-$pkgver
  ./configure --prefix=/usr --sysconfdir=/etc \
      --sbindir=/usr/bin --libexecdir=/usr/lib \
      --with-systemdsystemunitdir=/usr/lib/systemd/system \
      --localstatedir=/var --disable-static \
      --enable-gtk-doc enable_available_modules=yes
  make
}

check() {
  cd udisks-udisks-$pkgver
  make check
}

package() {
  cd udisks-udisks-$pkgver
  make DESTDIR="$pkgdir" install
}
