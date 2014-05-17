<?php

class Logs extends CI_Controller
{
	public function __construct()
	{
		parent::__construct();
		$this->load->model('logs_model');
		$this->load->library('table');
	}
	
	public function index()
	{
		$data['logs'] = $this->logs_model->get_logs();
		$data['title'] = 'Logs';
		
		/*$this->table->set_heading('id', 'Type', 'Title',
			'Start Time', 'Play Time', 'Connections', 'View More', 'Start/Stop');
		
		foreach($data['logs'] as $log_item)
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
		
		$data['data'] = $this->table->generate();*/

		$this->load->view('templates/header', $data);
		$this->load->view('Logs/index', $data);
		$this->load->view('templates/footer');
	}
	
	public function view($id)
	{
		$data['log_item'] = $this->logs_model->get_logs($id);
		
		if(empty($data['log_item']))
		{
			show_404();
		}
		
		$data['title'] = $data['log_item']['Title'];
		
		$this->load->view('templates/header', $data);
		$this->load->view('Logs/view', $data);
		$this->load->view('templates/footer');
	}


}

