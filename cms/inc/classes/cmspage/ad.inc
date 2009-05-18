<?php
class cmspage_ad extends cmspage
{
	public function run()
	{
		ad::google();
		exit;
	}
}
?>