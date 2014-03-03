<?php
if (!defined('INDEX')) {
   die('You cannot call this script directly !');
}

class Process
{
	private $pid;
	private $command;
	private $arguments;

	public function __construct($cl, $arg, $run = false)
	{
		$this->command = $cl;
		$this->arguments = $arg;
		$this->status();
		if ($run != false)
		{
			$this->runCom();
		}
	}
	
	private function getPidFile()
	{
		return $this->command.'.pid';
	}
	
	private function runCom()
	{
		if($this->status() == true)
		{
			return;
		}
		$command = 'nohup '.$this->command.' '.$this->arguments.' > '.$this->command.'.log.txt 2>&1 & echo $!';
		exec($command, $op);
		$this->pid = (int)$op[0];
		file_put_contents($this->getPidFile(), $this->pid);
	}

	private function searchPid()
	{
		if(file_exists($this->getPidFile()) == true)
		{
			$this->pid = (int)file_get_contents($this->getPidFile());
		}
		else
		{
			$this->pid = -1;
		}
	}
	
	public function getPid()
	{
		$this->status();
		return $this->pid;
	}

	public function status()
	{
		$this->searchPid();
		if($this->pid == -1)
		{
			return false;
		}
		$command = 'ps -p '.$this->pid;
		exec($command, $op);
		if (!isset($op[1]))
		{
			unlink($this->getPidFile());
			$this->pid = -1;
			return false;
		}
		else
		{
			return true;
		}
	}

	public function start()
	{
		$this->status();
		$this->runCom();
		return $this->status();
	}

	public function stop()
	{
		$this->status();
		$command = 'kill '.$this->pid;
		exec($command);
		if ($this->status() == false)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

function ProcessTest($start = true, $stop = true)
{
	$boolarray = Array(false => 'false', true => 'true');

	$proc = new Process("echo", "\"scale=10000; 4 * a (1)\" | bc -l");
	if($start == true)
	{
		echo "PID   : ".$proc->getPid()."<br>\n";
		echo "Status: ".$boolarray[$proc->status()]."<br>\n";
		echo "Start : ".$boolarray[$proc->start()]."<br>\n";
		echo "<br>\n";
	}
	if($stop == true)
	{
		echo "PID   : ".$proc->getPid()."<br>\n";
		echo "Status: ".$boolarray[$proc->status()]."<br>\n";
		echo "Stop  : ".$boolarray[$proc->stop()]."<br>\n";
		echo "\n";
	}
}

?>