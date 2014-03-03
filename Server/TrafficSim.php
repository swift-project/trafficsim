<?php
if (!defined('INDEX')) {
   die('You cannot call this script directly !');
}

include_once("Process.php");

class TrafficSim
{
	private $dir;
	
	public function __construct($dir = "Logs/")
	{
		$this->dir = $dir;
	}

	public function getSceneries()
	{
		$logs = array();
		foreach (glob($this->dir."*.info") as $filename) {
			$filename = substr($filename, strlen($this->dir));
			$logs[] = substr($filename, 0, -5);
		}
		return $logs;
	}
	
	public function getSceneryInfo($log)
	{
		return file_get_contents($this->dir.$log.".info");
	}

	public function startScenery($xml)
	{
		$proc = new Process("./bin/STd", "--xml ".$this->dir.$xml.".xml");
		if($proc->status() == true)
		{
			$proc->stop();
		}
		if($proc->start() == true)
		{
			file_put_contents("scenery.text", $xml);
		}
	}
	
	public function stopScenery()
	{
		$proc = new Process("./bin/STd", "");
		$proc->stop();
		if(file_exists("scenery.text") == true)
		{
			unlink("scenery.text");
		}
	}
	
	public function getRunningScenery()
	{
		$proc = new Process("./bin/STd", "");
		$status = $proc->status();
		if($status == false)
		{
			return false;
		}
		if(file_exists("scenery.text") == true)
		{
			return file_get_contents("scenery.text");
		}
		else
		{
			return false;
		}
	}
}



?>