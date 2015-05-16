#!/usr/bin/perl

use constant IN_DIR => $ARGV[0];
use constant OUT_DIR => $ARGV[1];
use File::Basename;
use File::Copy;
use File::Path;
use strict;

my $inSubPath = File::Spec->catdir(IN_DIR, "??.lproj");
my @inSubDirs = glob($inSubPath);
foreach my $inSubDir (@inSubDirs)
{
    my $inFile = File::Spec->catfile($inSubDir, "MP3epoc.1");
    my $lang = fileparse($inSubDir, ".lproj");
    my $outSubPath = $lang eq "en" ? "man1" : File::Spec->catdir($lang, "man1");
    my $outSubDir = File::Spec->catdir(OUT_DIR, "man", $outSubPath);
    mkpath $outSubDir;
    copy $inFile, $outSubDir or die "can't copy \"$inFile\": $!\n";
}
