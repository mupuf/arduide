#!/bin/sh

set -e -x

base_dir=$1
shift
build_dir=$1
shift
release_dir=$1
shift

[ -z "${build_dir}" -o -z "${release_dir}" -o ! -d "${base_dir}" -o ! -d "${build_dir}" ] && exit 1
[ -d "${release_dir}" ] && rm -r "${release_dir}"
install -d "${release_dir}"

function to_absolute_path()
{
  cd "$1"
  pwd
  cd - > /dev/null
}

base_dir=`to_absolute_path "${base_dir}"`
build_dir=`to_absolute_path "${build_dir}"`
release_dir=`to_absolute_path "${release_dir}"`

install -m644 "${build_dir}/arduino-ide.exe" "${release_dir}/"
cd "${build_dir}"
find plugins -type f -name '*.dll' -print | while read plugin; do
  install -Dm644 "${plugin}" "${release_dir}/${plugin}"
done

cd /usr/local/i486-mingw32/grantlee/lib
find grantlee -type f -name '*.dll' -print | while read plugin; do
  install -Dm644 "${plugin}" "${release_dir}/${plugin}"
done

cp -dpr "${base_dir}/data" "${release_dir}/data"
cp -dpr "${base_dir}/*.qm" "${release_dir}/translations"

for dll in mingwm10.dll libgcc_s_dw2-1.dll QtCore4.dll QtGui4.dll QtNetwork4.dll QtWebKit4.dll QtScript4.dll QtXmlPatterns4.dll phonon4.dll; do
  install -m644 "/usr/local/i486-mingw32/qt/bin/${dll}" "${release_dir}/"
done

install -m644 /usr/local/i486-mingw32/qscintilla/bin/qscintilla2.dll "${release_dir}/"
install -m644 /usr/local/i486-mingw32/grantlee/bin/libgrantlee_core.dll "${release_dir}/"
install -m644 /usr/local/i486-mingw32/grantlee/bin/libgrantlee_gui.dll "${release_dir}/"
