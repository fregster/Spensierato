<?php
element::register('YouTube Embeded Video Channel', 'element_YouTubeChannel', false, 'film', 'film_edit');
class element_YouTubeChannel extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'YouTube Embeded Video Channel';
		$this->allowed_tags(array('load' => 'text'));
	}
	
	protected function add_header()
	{
		content::singleton()->add_custom_head('link', array('rel' => 'stylesheet', 'type' => 'text/css', 'href' => 'http://www.google.com/uds/css/gsearch.css'));
		content::singleton()->add_custom_head('link', array('rel' => 'stylesheet', 'type' => 'text/css', 'href' => 'http://www.google.com/uds/solutions/videobar/gsvideobar.css'));
		content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src' => xml::xmlspecialchars('http://www.google.com/uds/api?file=uds.js&v=1.0', true)));
		content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src' => 'http://www.google.com/uds/solutions/videobar/gsvideobar.js'));
		content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src' => Settings::singleton()->get_setting('document_root').'/js/youtubebar.js'));
	}
}
