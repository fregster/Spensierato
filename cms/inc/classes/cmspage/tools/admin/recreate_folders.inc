<?php
$folders = array(setup::Singleton()->load('cms_folder_uploads'),
				setup::Singleton()->load('cms_folder_downloads'),
				setup::Singleton()->load('cms_folder_orig_images'),
				setup::Singleton()->load('cms_folder_logs'),
				setup::Singleton()->load('cms_folder_cache'),
				setup::Singleton()->load('cms_folder_search'),
			);		
			
$chmod = 0750;
foreach ($folders as $folder)
{
	if(!file_exists($folder))
	{
		if(@mkdir($folder, $chmod))
		{
			echo('Folder '. $folder .' created<br />');
		}
		else
		{
			echo('<b>Folder '. $folder .' creation failed</b><br />');
		}
	}
}