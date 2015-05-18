<?php
/**
* This is a simple implementation of an i18n updater tool
* It's not really meant to be final or complete in any way
* It probably should be re-implemented in a different language
* I'm just good (fast) at PHP so I use it for CLI stuff like this occasionally.
*
* TODO: rewrite this tool in perl or python
**/


$base = 'en';

$files = glob('src/i18n/*.txt');
$languages = array();
foreach ($files as $name) {
	$languages[] = basename($name, '.txt');
}

// Parse source string table
function parse_input($lang, $keys = null) {
	$strings = file_get_contents('src/i18n/' . $lang . '.txt');
	$strings = explode("\n", $strings);
	
	$table = array(0);
	foreach ($strings as $ln) {
		$ln = preg_replace('/#.*$/', '', $ln);
		$ln = trim($ln);
		if (! $ln) continue;
		
		$info = preg_split('/\s\s+/', $ln);
		
		if ($keys) {
			if (@$keys[$info[0]]) $table[$keys[$info[0]]] = $info;
		} else {
			$table[] = $info;
		}
	}
	
	unset($table[0]);
	return $table;
}

// Base the base (English) string table
$base = parse_input($base);

// Build a mapping of keys, used for secondary language loading
$keys = array();
foreach ($base as $idx => $info) {
	$keys[$info[0]] = $idx;
}

// Create strings.h
$strings_h  = "// This is a generated file.\n";
$strings_h .= "// Don't include this file; include gettext.h instead.\n";
$strings_h .= "// You can re-generate this file using the tool in tools/i18n\n";
$strings_h .= "\n";

foreach ($base as $idx => $info) {
	$strings_h .= '#define STRING_' . $info[0] . ' ' . $idx . "\n";
}

// Create/Update only if necessary to not trigger compiler
$dest = 'src/i18n/strings.h';
if (!file_exists($dest) || sha1_file($dest) != sha1($strings_h)) {
	echo "Updated strings.h file\n";
	file_put_contents($dest, $strings_h);
} else {
	echo "No changes to strings.h required\n";
}

// Create a data file for each language
foreach ($languages as $lang) {
	$table = parse_input($lang, $keys);
	
	foreach ($base as $idx => $info) {
		if (empty($table[$idx])) {
			$table[$idx] = $info;
		}
	}
	
	ksort($table);
	
	$out = '';
	foreach ($table as $idx => $info) {
		unset ($info[0]);
		$out .= $idx . "\t" . implode("\t", $info) . "\n";
	}
	
	file_put_contents('data/i18n/' . $lang . '.txt', $out);
	
	echo "Saved language pack {$lang}\n";
	
	unset($out);
}
