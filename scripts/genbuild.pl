#!/usr/bin/perl

use Cwd 'abs_path';

# display compiler selection menu
print "Please make a selection:\n";
print "\t1. gcc\n";
print "\t2. clang\n";
print "\n\tq. quit\n\n";
print "Selection: ";

# get the user's input
$selection = <STDIN>;
chomp($selection);

if ($selction eq 'q') {
	exit 0
}

# map selection to compiler string
%compiler_map = (
		'1', '/usr/bin/gcc',
		'2', '/usr/bin/clang' );
$compiler = $compiler_map{$selection};

# display build type selection menu
print "\nPlease make a selection:\n";
print "\t1. Debug\n";
print "\t2. Release\n";
print "\n\tq. quit\n\n";
print "Selection: ";

# get the user's input
$selection = <STDIN>;
chomp($selection);

if ($selection eq 'q') {
	exit 0;
}

# map selection to build type string
%build_map = ( '1', 'Debug', '2', 'Release' );
$build = $build_map{$selection};

# get the location of the genbuild script
$script_path = abs_path($0);

# find the project root by dropping scripts/genbuild.pl from the end
@tokens = split(/\//, $script_path);
@tokens = splice(@tokens, 0, -2);
$proj_root = join('/', @tokens) . '/';

# create output directories, if needed
$bin_dir = $proj_root . 'bin/';
unless (-d $bin_dir) {
	mkdir $bin_dir;
}

$lib_dir = $proj_root . 'lib/';
unless (-d $lib_dir) {
	mkdir $lib_dir;
}

$build_dir = $proj_root . 'build/';
unless (-d $build_dir) {
	mkdir $build_dir;
}

# use m4 template to generate build script
$input_file = $proj_root . 'scripts/templates/gencmake.sh.m4';
$output_file = $build_dir . 'gencmake.sh';
$command = 'm4 -DGEN_COMPILER=' . $compiler;
$command = $command . ' -DGEN_BUILD=' . $build;
$command = $command . ' ' . $input_file . ' > ' . $output_file;
`$command`

