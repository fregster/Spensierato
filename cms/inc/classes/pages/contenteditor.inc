<?php
class cmspage_contenteditor extends cmspage
{
	public function run()
	{
		ContentEditor::run($this->value);
	}
}
?>