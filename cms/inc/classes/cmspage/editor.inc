<?php
class cmspage_editor extends cmspage
{
	public function initalise()
	{
		$this->value = Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
		ContentEditor::run($this->value);
	}
}