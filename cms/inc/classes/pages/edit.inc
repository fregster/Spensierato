<?php
class cmspage_edit extends cmspage
{
	public function initalise()
	{
		$this->value = CMS::Singleton()->path_vars[1];
		return true;
	}
	
	public function run()
	{
		//Edit render element
		$this->security_element_edit();
		
		$element = Element::type($this->value);
		$element->build($this->value);
		Render::Singleton()->render_edit(Content::Singleton()->build());
	}
}
?>