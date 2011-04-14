<?php
class element_SearchResults extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Search Results';
		$this->title = 'Search Results';
		$this->main = 1;
		$this->allowed_tags(array('search' => 'auto'));
		// <id>1</id><title>home page intro</title><score>6.8</score><html>this is a &lt;strong&gt;test&lt;/strong&gt; string</html>

	}
}