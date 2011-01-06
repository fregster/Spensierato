<?php
$folders = array($GLOBALS['cms_folder_uploads'],
				$GLOBALS['cms_folder_downloads'],
				$GLOBALS['cms_folder_orig_images'],
				$GLOBALS['cms_folder_logs'],
				$GLOBALS['cms_folder_cache'],
				$GLOBALS['cms_folder_search'],
			);

foreach ($folders as $folder)
{
	if(!file_exists($folder))
	{
		if(@mkdir($folder, 0750))
		{
			echo('Folder '. $folder .' created<br />');
		}
		else
		{
			echo('<b>Folder '. $folder .' creation failed</b><br />');
		}
	}
}