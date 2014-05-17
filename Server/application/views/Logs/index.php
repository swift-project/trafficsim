<!-- LOGS_INDEX_START -->
<?php /*<table border="1">
	<thead>
		<tr>
			<th>Type</th>
			<th>Title</th>
			<th>FileName</th>
			<th>View</th>
		</tr>
	</thead>
<?php foreach ($logs as $log_item): ?>
	<tbody>
		<tr>
			<td><?php echo $log_item['LogTypes_idLogTypes']?></td>
			<td><?php echo $log_item['Title']?></td>
			<td><?php echo $log_item['FileName']?></td>
			<td><a href="Logs/view/<?php echo $log_item['idLogs'] ?>">View Log</a></td>
		</tr>
	</tbody>
<?php endforeach ?>
</table>*/ ?>
<?php
$this->load->library('table');

$this->table->set_heading('id', 'Type', 'Title',
	'Start Time', 'Play Time', 'Connections', 'View More', 'Start/Stop');

foreach($logs as $log_item)
{
	$this->table->add_row($log_item['idLogs'], $log_item['LogTypes_idLogTypes'], $log_item['Title'],
		$log_item['StartTime'], $log_item['PlayTime'], $log_item['Connections'], '<a href="Logs/view/'.$log_item['idLogs'].'">View Log</a>', '');
}

$tmpl = array (
			'table_open'          => '<table border="1" cellpadding="4" cellspacing="0">',

			'heading_row_start'   => '<tr>',
			'heading_row_end'     => '</tr>',
			'heading_cell_start'  => '<th>',
			'heading_cell_end'    => '</th>',

			'row_start'           => '<tr>',
			'row_end'             => '</tr>',
			'cell_start'          => '<td>',
			'cell_end'            => '</td>',

			'row_alt_start'       => '<tr>',
			'row_alt_end'         => '</tr>',
			'cell_alt_start'      => '<td>',
			'cell_alt_end'        => '</td>',

			'table_close'         => '</table>'
		);
$this->table->set_template($tmpl);
		
echo $this->table->generate();
?>
<!-- LOGS_INDEX_START -->