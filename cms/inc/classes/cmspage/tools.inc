<?php
class cmspage_tools extends cmspage
{
	protected $sub_folder;
	
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars(2);
		$this->sub_folder = CMS::Singleton()->path_vars(1) . DS;
		switch (CMS::Singleton()->path_vars(1))
		{
			case 'admin':
				return User::singleton()->is_admin();
				break;
				
			case 'loggedin':
				return User::singleton()->is_logged_in();
				break;
				
			case 'approver':
				if(!Security::singleton()->can_approve() || !User::singleton()->is_logged_in()){ return false; }
				break;
				
			default:
				$this->value = CMS::Singleton()->path_vars(1);
				$this->sub_folder = null;
				break;
		}
		return true;
	}
	
	public function run()
	{
		//CMS Included Functions no security checks
		$file = DIR_CLASSES . 'cmspage'. DS . 'tools' . DS . $this->sub_folder . Common::clean_path($this->value) .'.inc'; //FIXME path needs made public, a method writing or ref the GLOBAL
		if(file_exists($file))
		{
			include($file);
			exit;
		}

		error::do404();
	}
}