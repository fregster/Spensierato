<?php
class robot extends cmspage_html
{	
	public function initalise()
	{
		//TODO: Add some sort of user configurable robot.txt and system protected area's that should not be crawlled as it could messup the reporting tools
		die('User-agent: *
Allow: /');
	}
}