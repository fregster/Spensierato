<?php
class cmspage_folder extends cmspage
{
	public function run()
	{
		$this->security_element_view();
		
		$element_xml = Content::singleton()->render_element_xml($this->page_details['value']);
		$output_xml = DocumentFolder::convert_xml_to_displayable($element_xml);
		header('Content-Type: text/xml');
		die($output_xml);
		
	}
}