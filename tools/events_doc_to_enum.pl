#!/usr/bin/perl

open F, "<../docs/events.txt";

print "{\n";
while (<F>) {
	if (/^([A-Z_]+)$/) { print "\t$1,\n"; }
}
print "}\n";

close F;
