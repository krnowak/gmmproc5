#!/usr/bin/perl

use strict;
use warnings;
use v5.10;

use IO::File;

if (@ARGV < 1) {
    say 'bla';
    exit 1;
}

my $file = IO::File->new ($ARGV[0], "r");

say '#include "str.hh"';
say '';
say 'namespace Names';
say '{';
say '';

for my $line ($file->getlines ()) {
    chomp ($line);
    my $name = join('', map { ucfirst $_ } split (/[-:]/, $line));
    say 'using ' . $name . " = StaticString<'" . join ("', '", split ('', $line)) . "'>;";
}

say '';
say '} // namespace Names';
