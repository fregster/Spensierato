<?php
class elementSearchResults extends elementDefault
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Search Results';
		$this->allowed_tags(array('html' => 'auto', 'score' => 'auto', 'result' => 'auto'));
		// <id>1</id><title>home page intro</title><score>6.8</score><html>this is a &lt;strong&gt;test&lt;/strong&gt; string</html>

	}
}
?>