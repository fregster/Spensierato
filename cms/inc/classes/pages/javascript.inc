<?php
class cmspage_javascript extends cmspage
{
	public function initalise()
	{
//		parent::initalise(); //The JS class should never need to use the parent initalise as it just dumps the concaternated content to the browser
		$this->value = Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
//		die('hello '.$this->value);
		javascript::run($this->value);
		die();
	}
}
?>