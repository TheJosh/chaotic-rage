<?php
/**
* Should probably be a more CLI-style language like Python or Perl, but I digress
**/

$file = @file_get_contents($argv[1]);
if (! $file) die('Invalid file ' . $argv[1] . "\n");

$lines = explode("\n", $file);
array_shift($lines);

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

foreach ($data as $event => $d) {
	$mean = round($d['total'] / $d['count'], 4);
	
	echo "{$event}\n";
	echo "   Total:  {$d['total']}\n";
	echo "   Min:    {$d['min']}\n";
	echo "   Max:    {$d['max']}\n";
	echo "   Count:  {$d['count']}\n";
	echo "   Mean:   {$mean}\n";
}
