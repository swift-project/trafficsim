<!-- LOGS_VIEW_START -->
<table border="1">
	<tr>
		<th>Type</th>
		<td><?php echo $log_item['LogTypes_idLogTypes']?></td>
	</tr>
	<tr>
		<th>Title</th>
		<td><?php echo $log_item['Title']?></td>
	</tr>
	<tr>
		<th>FileName</th>
		<td><?php echo $log_item['FileName']?></td>
	</tr>
	<tr>
		<th>Description</th>
		<td><?php echo nl2br($log_item['Description'])?></td>
	</tr>
</table>
<!-- LOGS_VIEW_START -->