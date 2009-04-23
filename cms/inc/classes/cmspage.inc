<?php
class cmspage //This relies on CurentPage been loaded
{
	protected static $instance;
	protected $type;
	protected $value = NULL;
	protected $class;
	protected $path_vars;
	protected $path;
	protected $page_details;
	protected $id = NULL;

	public static function Singleton()
	{
	   if (!isset(self::$instance))
	   {
			$c = __CLASS__.'_'.CMS::Singleton()->page_details['type'];
			
	   		if(!load_extended_class(__CLASS__, $c))
	   		{
	   			$c = __CLASS__;
	   		}

		   self::$instance = new $c;
	   }

	   return self::$instance;
	}

	protected function __construct()
	{
		$this->type = CMS::Singleton()->page_details['type'];
		$this->class = 'cmspage_'.$this->type; //The class name expected
		$this->path_vars &= CMS::Singleton()->path_vars;
	}

	public function __clone()
	{
	   trigger_error('Clone is not allowed.', E_USER_ERROR);
	}

	public function initalise()
	{
		if($this->type == 'cmspage') { return false; } //If this class has not been extended then the page type does not exsist and so return false
		return true;
	}

	public function run()
	{
		if($this->type == 'cmspage') //The class has not been setup the error has been ignored and run has been called so die
		{
			die('500 Unable in include any content because type (' . $this->type . ') unknown  (' . $this->value.')');
		}
	}

	public function id()
	{
		if (is_null($this->id)) {
		 	return $this->value;
		 }
		return $this->id;
	}

	public function title()
	{
		return null;
	}

	public function page_information()
	{
		return $this->page_details;
	}

	/**
	 * Standard security calls for ease
	 * Just call the function as they all 403 on error
	 * @return nothing
	 */

	protected function security_element_view()
	{
		if(!Security::singleton()->can_view_element($this->value))
		{
			error::do403();
		}
	}

	protected function security_page_view()
	{
		if(!Security::singleton()->can_view($this->value))
		{
			error::do403();
		}
	}

	protected function security_element_edit()
	{
		if(!Security::singleton()->can_edit_element($this->value))
		{
			error::do403();
		}
	}

	protected function security_page_edit()
	{
		if(!Security::singleton()->can_edit_element($this->value))
		{
			error::do403();
		}
	}

	protected function security_element_approve()
	{
		if(!Security::singleton()->can_approve_element($this->value))
		{
			error::do403();
		}
	}

	protected function security_page_approve()
	{
		if(!Security::singleton()->can_approve($this->value))
		{
			error::do403();
		}
	}
}
?>