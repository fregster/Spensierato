<?php
class Timer {
	var $start_time = 0;
	var $end_time = 0;
	function Timer($start=0) {
		if($start) $this->start();
	}
	function start() {
		$timeparts = explode(' ',microtime());
		$this->start_time = $timeparts[1].substr($timeparts[0],1);
	}
	function stop($show=1,$decimal_place=8) {
		$timeparts = explode(" ",microtime());
		$this->end_time = $timeparts[1].substr($timeparts[0],1);
		
		if($show) $this->display($decimal_place);
	}
	function display($decimal_place=8) {
		echo number_format(bcsub($this->end_time,$this->start_time,6),$decimal_place);
		echo "s";
	}
}