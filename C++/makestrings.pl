#!/usr/bin/perl

use constant OUT_DIR => $ARGV[0];
use encoding "UTF-8", STDIN => "UTF-16";
use File::Spec;
use strict;

my %formatSpecs =
(
    "" => "s",
    "c" => "c",
    "i" => "d",
    "I64i" => "lld",
);
sub transformat
{
    my $formatSpec = $formatSpecs{$2};
    die "unknown format specifier %$2\n" unless defined $formatSpec;
    return "$1\$$formatSpec";
}

my $key;
my $lang;
my @lines;
my $headerFile;
my $headerFilePath;
my $stringsFile;
my $stringsFilePath;
my %stringsFiles;
my %replace =
(
    "\n" => "\\n",
    "\"" => "\\\"",
    "\\" => "\\\\",
);
$headerFilePath = File::Spec->catfile(OUT_DIR, "messages.h");
open($headerFile, ">", $headerFilePath)
    or warn "can't open \"$headerFilePath\": $!\n";
while (<STDIN>)
{
    if (defined $lang)
    {
        my $end = /^\.\s*$/;
        if ($stringsFile && fileno($stringsFile))
        {
            if ($end)
            {
                my $value = join "\n", @lines;
                eval
                {
                    $value =~
                        s/(?<!%)((?:%%)*%\d+)(?:!(\w+)!)?/transformat()/eg;
                    $value =~ s/(?<!\w)\//:/g;
                    my %replace = ("\\" => "/");
                    $value =~ s/\bMy Music(\\|\b)/~\/Music$replace{$1}/g;
                    $value =~
                        s/\\\\(My-Host)\\(Shared MP3s)\\/\/Volumes\/$1\/$2\//g;
                };
                if ($@)
                {
                    warn $@;
                }
                else
                {
                    $value =~ s/[\n\"\\]/$replace{$&}/g;
                    print $stringsFile "\"$key\"=\"$value\";\n"
                        or warn "can't write to \"$stringsFilePath\": $!\n";
                }
            }
            else
            {
                s/\015?\012//g;
                push @lines, $_;
            }
        }
        if ($end)
        {
            undef $lang;
            undef $stringsFile;
        }
    }
    else
    {
        if (/^\s*SymbolicName\s*=\s*(.*?)\s*$/)
        {
            my $name = $1;
            if ($name =~ /^MSG_(\w+)$/)
            {
                $key = $1;
                if (fileno($headerFile))
                {
                    print $headerFile "#define $name CFSTR(\"$key\")\n"
                        or warn "can't write to \"$headerFilePath\": $!\n";
                }
            }
            else
            {
                undef $key;
            }
        }
        elsif (/^\s*Language\s*=\s*(.*?)\s*$/)
        {
            $lang = $1;
            if (defined $key)
            {
                $stringsFile = $stringsFiles{$lang};
                if (!$stringsFile)
                {
                    my $outDirPath =
                        File::Spec->catdir(OUT_DIR, "$lang.lproj");
                    mkdir $outDirPath;
                    $stringsFilePath =
                        File::Spec->catfile($outDirPath, "Localizable.strings");
                    open $stringsFile, ">:encoding(UTF-16)", $stringsFilePath
                        or warn "can't open \"$stringsFilePath\": $!\n";
                    $stringsFiles{$lang} = $stringsFile;
                }
                undef @lines;
            }
        }
    }
};
