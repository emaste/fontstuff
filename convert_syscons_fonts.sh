#!/bin/sh

srcdir=$1
dstdir=$2

if [ ! -d "$srcdir" -o -z "$dstdir" ]; then
    echo "usage: $0 <path to syscons fonts dir> <output hex font dir>" >&2
    exit 1
fi

mkdir -p $dstdir
for f in $srcdir/*.fnt; do
    name=$(basename $f)
    name=${name%.fnt}

    dim=$(echo $name | sed -E -n 's/^.*-([0-9]*)x([0-9]*)/\1 \2/p')
    set -- $dim
    dim_args=
    if [ $# -eq 2 ]; then
        dim_args="-x $1 -y $2"
    fi
    echo $name $dimension
    uudecode -p $f > $dstdir/$name
    ./bin2hex $dim_args $dstdir/$name > $dstdir/$name.hex
    rm $dstdir/$name
done
