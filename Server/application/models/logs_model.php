<?php

class Logs_model extends CI_Model {

	public function __construct()
	{
		$this->load->database();
		$this->load->dbutil();
	}
	
	public function get_logs($id = FALSE)
	{
		if ($id === FALSE)
		{
			$query = $this->db->get('Logs');
			return $query->result_array();
		}

		$query = $this->db->get_where('Logs', array('idLogs' => $id));
		return $query->row_array();
	}
}

