<?php
class cmspage_robots extends cmspage_html
{	
	public function initalise()
	{
		//TODO: Add some sort of user configurable robot.txt and system protected area's that should not be crawlled as it could messup the reporting tools
		die('User-agent: *
Allow: '.DOCUMENT_PATH.'/
Disallow: '.DOCUMENT_PATH.'/cart'.PHP_EOL.
'Disallow: '.DOCUMENT_PATH.'/toggle'.PHP_EOL.
'Disallow: '.DOCUMENT_PATH.'/options'.PHP_EOL
		);
	}
}