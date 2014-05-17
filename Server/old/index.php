<?php
define ('INDEX', true);
include_once("TrafficSim.php");

// Collect the data:
$sim = new TrafficSim();

// Start scenery if needed (by formular):
if(isset($_POST['action']))
{
	if($_POST['action'] == "STOP")
	{
		$sim->stopScenery();
	}
	else
	{
		$sim->startScenery($_POST['action']);
	}
}

// Main Page:
echo "<h1>Hello!</h1>";

// Print running scenery, if there is a running scenery:
$running = $sim->getRunningScenery();
if($running != false)
{
	echo "<h2>Currently a scenery is running:</h2>";
	echo $running;
?>
<form action="index.php" method="post">
<input type="hidden" name="action" value="STOP">
<input type="submit" value="Stop running scenery!">
</form>
<?php
}
else
{
	echo "<h2>No scenery is running!</h2>";
}

// Print numer of found log-files to play:
$sceneries = $sim->getSceneries();
echo "<h2>Found ".count($sceneries)." logs:</h2>";

// Print all log-files and print a button to play the selected scenery:
foreach ($sceneries as $log)
{
	echo "<h3>".$log."</h3>";
	echo "<textarea cols=\"40\" rows=\"10\" readonly>".$sim->getSceneryInfo($log)."</textarea><br>";

?>
<form action="index.php" method="post">
<input type="hidden" name="action" value="<?=$log?>">
<?php
	if($running == $log)
	{
?>
<input type="submit" value="Restart scenery!">
<?php
	}
	else
	{
?>
<input type="submit" value="Start scenery!">
<?php
	}
?>
</form>
<?php
}

?>