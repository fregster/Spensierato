<?php
class cmspage_capthca extends cmspage
{
	public function run()
	{
		Capthca::create_Images();	
	}
}