<?php
/**
* Should probably be a more CLI-style language like Python or Perl, but I can smash out PHP faster :)
**/

array_shift($argv);
if (count($argv) == 0) {
	die('You must provide at least one argument');
}

foreach ($argv as $filename) {
	process($filename);
}


function process($filename) {
	$file = @file_get_contents($filename);
	if (! $file) {
		echo 'Cannot read file ' . $filename . "\n";
		return;
	}

	$lines = explode("\n", $file);
	array_shift($lines);

	// Aggregate data
	$data = array();
	foreach ($lines as $ln) {
		list($event, $st, $en, $time) = explode("\t", $ln);
		if (! $event) continue;
	
		if (! $data[$event]['min']) $data[$event]['min'] = 9999;
	
		$data[$event]['total'] += $time;
		$data[$event]['min'] = min($data[$event]['min'], $time);
		$data[$event]['max'] = max($data[$event]['max'], $time);
		$data[$event]['count']++;
	}

	// Headings
	echo ' === ', basename($filename), " === \n";
	echo str_pad('Event', 15);
	echo str_pad('Total', 10);
	echo str_pad('Min', 10);
	echo str_pad('Max', 10);
	echo str_pad('Count', 10);
	echo str_pad('Mean', 10);
	echo "\n";

	// Per event
	foreach ($data as $event => $d) {
		$mean = round($d['total'] / $d['count'], 4);
	
		echo str_pad($event, 15);
		echo str_pad("{$d['total']}ms", 10);
		echo str_pad("{$d['min']}ms", 10);
		echo str_pad("{$d['max']}ms", 10);
		echo str_pad("{$d['count']}", 10);
		echo str_pad("{$mean}ms", 10);
		echo "\n";
	}
}

